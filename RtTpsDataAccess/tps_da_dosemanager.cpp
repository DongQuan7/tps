////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_da_tpsdosegridmanager.cpp
/// 
///  \brief class TPS dose grid manager declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/11
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/recursive_mutex.hpp"

#include <limits>

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_contour_set.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"

#ifndef TPS_FLOOR
#define TPS_FLOOR(a) ((ceil(a) - a) <= 1e-3 ? ceil(a) : floor(a))
#endif

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
TpsDoseManager::TpsDoseManager()
{
    mReMutex = new boost::recursive_mutex();
    mMutex = new boost::mutex();
}

TpsDoseManager::~TpsDoseManager()
{
    delete mReMutex; mReMutex = nullptr;
    delete mMutex; mMutex = nullptr;
    Dispose();
}

void TpsDoseManager::Dispose() 
{
    auto itDoselineList = mDoselineCollection.begin();
    while (itDoselineList != mDoselineCollection.end()) {

        auto itDoseLine = itDoselineList->second.begin();
        while(itDoseLine != itDoselineList->second.end()) {
            delete *itDoseLine;
            *itDoseLine = nullptr;
            ++itDoseLine;
        }

        itDoselineList->second.clear();
        ++itDoselineList;
    }

    mDoselineCollection.clear();
}

void TpsDoseManager::ClearPatientData() 
{
    Dispose();
}

bool TpsDoseManager::InitializeDosegrid(const DosegridInfo& dosegridInfo, 
    const TpsImage3DEntity* image3DEntity,
    const ROIEntity* skinVoi, 
    RtDosegrid& aDosegrid,
    bool bCreatBuffer)
{
    RtImage3DHeader* imgHdr = nullptr;
    if (!image3DEntity->GetImage3DHeader(imgHdr)){
        TPS_LOG_DEV_ERROR<<"Failed to get the image header.";
        return TPS_ER_FAILURE;
    }

    Mcsf::Matrix4f matVolume2Pat;
    if (!image3DEntity->GetVolumeToPatientMatrix(matVolume2Pat))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get the matVolume2Pat.";
        return TPS_ER_FAILURE;
    }

    float realSpacing[3];
    realSpacing[0] = dosegridInfo.spacingx;
    realSpacing[1] = dosegridInfo.spacingy;
    realSpacing[2] = dosegridInfo.spacingz;

    // realspacing has 0 element, will cause divide by zero exception
    if (fabs(realSpacing[0]*realSpacing[1]*realSpacing[2])<1e-6) {
        TPS_LOG_DEV_ERROR<<"realspacing("<<realSpacing[0]<<","<<realSpacing[1]
        <<","<<realSpacing[2]<<") has 0 element, will cause divide by zero exception";
        return false;
    }

    Mcsf::Vector3f axis[3] =
    {
        matVolume2Pat.Transform(Mcsf::Vector3f(1, 0, 0)),
        matVolume2Pat.Transform(Mcsf::Vector3f(0, 1, 0)),
        matVolume2Pat.Transform(Mcsf::Vector3f(0, 0, 1)),
    };
    float spacing[3] = 
    {
        axis[0].Magnitude(), axis[1].Magnitude(), axis[2].Magnitude()
    };
    for (int i = 0; i < 3; ++i)
    {
        axis[i].Normalize();
    }

    float dim[3];
    Mcsf::Vector3f vecZ;
    Mcsf::Point3f start, end;
    if (skinVoi == nullptr || skinVoi->IsEmpty())
    {
        start = Mcsf::Point3f(0, 0, 0);
        dim[0] = imgHdr->m_iXDim;
        dim[1] = imgHdr->m_iYDim;
        dim[2] = imgHdr->m_iSliceCount;
    }
    else
    {
        float bdryBox[6] = 
        {
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::lowest(),
        };

        auto cs = skinVoi->GetContourSet();
        if (cs == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"can't get contour set from skin voi";
            return false;
        }

        PT3 pt;
        auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matVolume2Pat.Inverse());
        for (auto it = cs->page_contours.begin(); it != cs->page_contours.end(); ++it)
        {
            for (int i = 0; i < it->second->contour_list.size(); ++i)
            {
                for (int j = 0; j < it->second->contour_list[i].pt.size(); ++j)
                {
                    pt = matP2V.Transform(it->second->contour_list[i].pt[j]);
                    bdryBox[0] = TPS_MIN(bdryBox[0], pt.x);
                    bdryBox[1] = TPS_MAX(bdryBox[1], pt.x);
                    bdryBox[2] = TPS_MIN(bdryBox[2], pt.y);
                    bdryBox[3] = TPS_MAX(bdryBox[3], pt.y);
                    bdryBox[4] = TPS_MIN(bdryBox[4], pt.z);
                    bdryBox[5] = TPS_MAX(bdryBox[5], pt.z);
                }
            }
        }

        dim[0] = bdryBox[1] - bdryBox[0];
        dim[1] = bdryBox[3] - bdryBox[2];
        dim[2] = bdryBox[5] - bdryBox[4];
        start = Mcsf::Point3f(bdryBox[0], bdryBox[2],bdryBox[4]);
    }
    start = matVolume2Pat.Transform(start);

    //这里不需要dim[i] - 1， 因为本身层也是有厚度的，所以series的厚度是spacing[i] * dim[i]
    int realDim[3];
    realDim[0] = TPS_FLOOR((spacing[0] * dim[0] / realSpacing[0] + 1)); //expand
    realDim[1] = TPS_FLOOR((spacing[1] * dim[1] / realSpacing[1] + 1));
    realDim[2] = TPS_FLOOR((spacing[2] * dim[2] / realSpacing[2] + 1));

    // 填充dosegrid对象
    // rcc: 这里需要确认，SetupGridToPatient_i里面的spacing，到底是图像的spacing，
    //      还是dosegrid的spacing
    if (TPS_ER_SUCCESS != UpdateDoseGrid_i(&aDosegrid, start, axis, realSpacing, realDim, bCreatBuffer)){
        TPS_LOG_DEV_ERROR<<" failed to update dosegrid.";
        return false;
    }

    return true;
}

int TpsDoseManager::CreateDoseGrid(const Mcsf::Point3f& start, 
    Mcsf::Vector3f* axis, float* dosegridSpacing, int* dosegridDim,
    RtDosegrid*& dosegrid)
{
    boost::lock_guard<boost::mutex> lg(*mMutex);

    // 40 * 40 * 40
    //float realSpacing[3];
    //realSpacing[0] = spacing[0] * (dim[0]-1) / 39.0;
    //realSpacing[1] = spacing[1] * (dim[1]-1) / 39.0;
    //realSpacing[2] = spacing[2] * (dim[2]-1) / 39.0;

    // 注意:这里的策略是，内部申请，外部释放
    dosegrid = new RtDosegrid(true);
    dosegrid->set_bitsallocated(32);//for default 
    if (TPS_ER_SUCCESS != UpdateDoseGrid_i(dosegrid, start, axis, 
        dosegridSpacing, dosegridDim)){
        delete dosegrid; dosegrid = nullptr;
        TPS_LOG_DEV_ERROR<<" failed to update dosegrid.";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

Mcsf::Matrix4f 
TpsDoseManager::SetupGridToPatient(const RtImage3DHeader& imgHdr,
    const RtDosegrid& dosegrid)
{
    int dim[3];
    dim[0] = imgHdr.m_iXDim;
    dim[1] = imgHdr.m_iYDim;
    dim[2] = imgHdr.m_iSliceCount;

    Mcsf::Vector3f axis[3];
    axis[0].SetX(imgHdr.m_ImageXOrientation.x);
    axis[0].SetY(imgHdr.m_ImageXOrientation.y);
    axis[0].SetZ(imgHdr.m_ImageXOrientation.z);
    axis[1].SetX(imgHdr.m_ImageYOrientation.x);
    axis[1].SetY(imgHdr.m_ImageYOrientation.y);
    axis[1].SetZ(imgHdr.m_ImageYOrientation.z);
    auto scanInfoList = imgHdr.m_vScanInfoList;
    Mcsf::Point3f start(scanInfoList[0].m_ImagePosition.x,
        scanInfoList[0].m_ImagePosition.y,scanInfoList[0].m_ImagePosition.z);
    Mcsf::Point3f tmpPt(scanInfoList[dim[2] - 1].m_ImagePosition.x,
        scanInfoList[dim[2] - 1].m_ImagePosition.y, scanInfoList[dim[2] - 1].m_ImagePosition.z);
    Mcsf::Vector3f vecZ = tmpPt - start;
    axis[2] = vecZ.GetNormalize();

    float realSpacing[3];
    realSpacing[0] = dosegrid.get_xcoordinate3dinc();
    realSpacing[1] = dosegrid.get_ycoordinate3dinc();
    realSpacing[2] = dosegrid.get_zcoordinate3dinc();

    Mcsf::Matrix4f matGrid2Patient = SetupGridToPatient_i(start, axis, realSpacing);
    return matGrid2Patient;
}

// rcc: 将来考虑将这堆零碎的入参，统一到DosegridInfo的结构体里面去
int TpsDoseManager::UpdateDoseGrid_i(RtDosegrid* grid, 
    const Mcsf::Point3f& start, Mcsf::Vector3f* axis, 
    float* spacing, int* dim, bool bCreatBuffer)
{
    grid->set_xcoordinate3dstart(start.GetX());
    grid->set_ycoordinate3dstart(start.GetY());
    grid->set_zcoordinate3dstart(start.GetZ());
    grid->set_xcoordinate3dinc(spacing[0]);
    grid->set_ycoordinate3dinc(spacing[1]);
    grid->set_zcoordinate3dinc(spacing[2]);
    grid->set_xcount(dim[0]);
    grid->set_ycount(dim[1]);
    grid->set_zcount(dim[2]);
    grid->set_accuracy(0.00001);

    Mcsf::Matrix4f matGrid2Patient = SetupGridToPatient_i(start, axis, spacing);
    float matElems[16]; memset(matElems, 0, sizeof(float)*16);
    matGrid2Patient.GetElements(matElems);
    grid->set_grid_to_pat_t(matElems);

    if (bCreatBuffer)
    {
        grid->create_dosegrid_buffer();
    }
    //const float* gridBuffer = grid->get_dosegrid_buffer();
    //memset(gridBuffer, 0, sizeof(float) * dim[0] * dim[1] * dim[2]);

    grid->set_bitsallocated(32);//for default
    grid->set_bitsstored(32);
    grid->set_highbits(31);
    return TPS_ER_SUCCESS;
}

void TpsDoseManager::ClearDosegridForPlan(const std::string& planUID)
{
    TpsPatientManager& patMgr = *mDataRepository->GetPatientManager();
    TpsNormGroupManager& ngMgr = *GetNormgroupManager();

    RtPlan* pPlan = nullptr;
    if (!patMgr.GetPlan(planUID, &pPlan) || pPlan==nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get plan["<<planUID<<"] from pat manager";
        return;
    }
    pPlan->get_dosegrid()->clear_dosegrid_buffer();
    pPlan->get_dosegrid()->set_isdosevalid(false);

    std::vector<std::string> ngUids = ngMgr.GetNormGroupUIDListByPlan(planUID);
    for (int n=0; n<ngUids.size(); n++) {
        ClearDosegridForNormgroup_i(ngUids[n]);
    }
}

void TpsDoseManager::ClearDosegridForNormgroup(const std::string& ngUID)
{
    std::string planUID = GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUID);

    RtPlan* pPlan = nullptr;
    GetPatientManager()->GetPlan(planUID, &pPlan);
    if (pPlan == nullptr)
    {
        return;
    }

    pPlan->get_dosegrid()->clear_dosegrid_buffer();
    pPlan->get_dosegrid()->set_isdosevalid(false);
    ClearDosegridForNormgroup_i(ngUID);
}

void TpsDoseManager::ClearDosegridForNormgroup_i(const std::string& ngUID)
{
    TpsNormGroupManager& ngMgr = *GetNormgroupManager();

    RtNormgroup* pNormgroup = nullptr;
    if (!ngMgr.GetNormGroup(ngUID, &pNormgroup) || pNormgroup==nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get normgroup["<<ngUID<<"] from normgroup manager";
        return;
    }
    pNormgroup->get_dosegrid()->clear_dosegrid_buffer();
    pNormgroup->get_dosegrid()->set_isdosevalid(false);
    //pNormgroup->set_isjoined(false);

    std::vector<std::string> beamUids = GetBeamManager()->GetBeamUIDListByNormgroupUID(ngUID);
    for (int n=0; n<beamUids.size(); n++) {
        ClearDosegridForBeam_i(beamUids[n]);
    }
}

void TpsDoseManager::ClearDosegridForBeam_i(const std::string& beamUID)
{
    TpsBeamManager& beamMgr = *GetBeamManager();
    RtBeam* pBeam = beamMgr.GetBeam(beamUID);
    if (nullptr == pBeam) {
        TPS_LOG_DEV_ERROR<<"Failed to get beam["<<beamUID<<"] from beam manager";
        return;
    }
    if (nullptr != pBeam->get_dosegrid()){
        pBeam->get_dosegrid()->clear_dosegrid_buffer();
        pBeam->get_dosegrid()->set_isdosevalid(false);
    }

    //pBeam->set_isjoincalc(false);
}

bool TpsDoseManager::IsTwoDoseGridConsistent(const RtDosegrid* grid1, 
    const RtDosegrid* grid2)
{
    if (nullptr==grid1 || nullptr==grid2) {
        TPS_LOG_DEV_ERROR<<"nullptr: grid1 or grid2";
        return false;
    }

    bool bRet = true;
    bRet &= grid1->get_xcount()==grid2->get_xcount();
    bRet &= grid1->get_ycount()==grid2->get_ycount();
    bRet &= grid1->get_zcount()==grid2->get_zcount();

    return bRet;
}

bool TpsDoseManager::UpdateDoseGridBuffer(RtDosegrid* target, 
    RtDosegrid* source)
{
    if (target==nullptr || source==nullptr) {
        TPS_LOG_DEV_ERROR<<"Invalid input (null pointer) for target or source";
        return false;
    }

    if (!IsTwoDoseGridConsistent(target, source)) {
        TPS_LOG_DEV_ERROR<<"Target and source dosegrid are not consistent. Can NOT update.";
        return false;
    }

    int buffersize = sizeof(float)*target->get_xcount()*target->get_ycount()*target->get_zcount();
    memcpy(target->get_dosegrid_buffer(), source->get_dosegrid_buffer(), buffersize);
    return true;
}

int TpsDoseManager::UpdateDoseGrid(RtDosegrid* grid, const Mcsf::Point3f& start, 
    Mcsf::Vector3f* axis, float* spacing, int* dim, bool bCreatBuffer){
    boost::lock_guard<boost::mutex> lg(*mMutex);
    return UpdateDoseGrid_i(grid, start, axis, spacing, dim, bCreatBuffer);
}

RtDosegrid* TpsDoseManager::GetDoseGrid(const std::string& uid
    , DoseGridType type/* = DOSE_GRID_NORMGROUP*/)
{
    //boost::lock_guard<boost::mutex> lg(mMutex);
    boost::recursive_mutex::scoped_lock lock(*mReMutex);
    RtDosegrid* dosegrid = nullptr;

    switch (type)
    {
    case DOSE_GRID_PLAN:
        {
            RtPlan* pPlan = nullptr;
            if (!mDataRepository->GetPatientManager()->GetPlan(uid, &pPlan)) {
                TPS_LOG_DEV_ERROR<<"Failed to get plan from patient manager";
                return nullptr;
            }
            dosegrid = pPlan->get_dosegrid();
        }
    	break;
    case DOSE_GRID_NORMGROUP:
        {
            RtNormgroup* pNg = nullptr;
            if (!GetNormgroupManager()->GetNormGroup(uid, &pNg)) {
                TPS_LOG_DEV_ERROR<<"Failed to get normgroup from normgroup manager";
                return nullptr;
            }
            dosegrid = pNg->get_dosegrid();
        }
        break;
    case DOSE_GRID_BEAM:
        {
            RtBeam* pBeam = GetBeamManager()->GetBeam(uid);
            if (nullptr==pBeam) {
                TPS_LOG_DEV_ERROR<<"Failed to get beam from beam manager.";
                return nullptr;
            }
            dosegrid = pBeam->get_dosegrid();
        }
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unknown type. Can not get dosegrid";
        return nullptr;
    }

    return dosegrid;
}

void TpsDoseManager::SetPickCoordinate(double x, double y) {
    mPickCoordinateX = x;
    mPickCoordinateY = y;
    mSelectionMode = 1;  // selecting
}
bool TpsDoseManager::GetPickCoordinate(double& x, double& y) {
    x = mPickCoordinateX;
    y = mPickCoordinateY;
    return true;
}
bool TpsDoseManager::GetTargetCoordinate(double& x, double& y) {
    x = mTargetX;
    y = mTargetY;
    return true;
}
void TpsDoseManager::SetTargetCoordinate(double x, double y) {
    mTargetX = x;
    mTargetY = y;
}
void TpsDoseManager::MovingBoundary(double targetX, double targetY) {
    mPickCoordinateX = mTargetX;
    mPickCoordinateY = mTargetY;
    mTargetX = targetX;
    mTargetY = targetY;

    mSelectionMode = 2;  // dragging
}
void TpsDoseManager::FinishMovingBoundary() {
    mSelectedBoundary = -1;
    mSelectionMode = 3; // finished
}

int TpsDoseManager::GetSelectionMode() {
    return mSelectionMode;
}

bool TpsDoseManager::IsHitTestSuccess() {
    return mIsHitTestSuccess;
}
void TpsDoseManager::SetHitTestSuccess(bool flag) {
    mIsHitTestSuccess = flag;
}

void TpsDoseManager::SetMovingPlanes(int* plane) {
    mPlanes[0] = plane[0];
    mPlanes[1] = plane[1];
    mPlanes[2] = plane[2];
}

bool TpsDoseManager::GetMovingPlanes(int* plane) {
    plane[0] = mPlanes[0];
    plane[1] = mPlanes[1];
    plane[2] = mPlanes[2];
    return true;
}

void TpsDoseManager::SetDoseGridSpacing(float* spacing) {
    mSpacing[0] = spacing[0];
    mSpacing[1] = spacing[1];
    mSpacing[2] = spacing[2];
}
void TpsDoseManager::GetDoseGridSpacing(float* spacing) {
    spacing[0] = mSpacing[0];
    spacing[1] = mSpacing[1];
    spacing[2] = mSpacing[2];
}

Mcsf::Matrix4f TpsDoseManager::SetupGridToPatient_i(
    const Mcsf::Point3f& start, Mcsf::Vector3f* axis, float* spacing){

    Mcsf::Vector4f col0(spacing[0] * axis[0].GetX(), spacing[0] * axis[0].GetY(), 
        spacing[0] * axis[0].GetZ(), 0.0f);
    Mcsf::Vector4f col1(spacing[1] * axis[1].GetX(), spacing[1] * axis[1].GetY(), 
        spacing[1] * axis[1].GetZ(), 0.0f);
    Mcsf::Vector4f col2(spacing[2] * axis[2].GetX(), spacing[2] * axis[2].GetY(), 
        spacing[2] * axis[2].GetZ(), 0.0f);
    Mcsf::Vector4f col3(start.GetX(), start.GetY(), start.GetZ(), 1.0f);
    return Mcsf::Matrix4f(col0, col1, col2, col3);
}

int TpsDoseManager::GetPointDoseValue(RtDosegrid& dosegrid, 
    double xInPatient, double yInPatient, double zInPatient, float& dosevalue)
{
    const float* pDoseGridData = dosegrid.get_dosegrid_buffer();
    if (pDoseGridData == nullptr) {
        TPS_LOG_DEV_ERROR<<"Invalid dosegrid buffer in dosegrid object["
            <<dosegrid.get_uid()<<"].";
        return -1;
    }

    // Point in patient to dose grid
    Mcsf::Point3f ptInPat3f;
    ptInPat3f.SetX(xInPatient);
    ptInPat3f.SetY(yInPatient);
    ptInPat3f.SetZ(zInPatient);
    Mcsf::Matrix4f matGrid2Pat = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid.get_grid_to_pat_t());
    Mcsf::Point3f ptInDoseGrid = matGrid2Pat.Inverse().Transform(ptInPat3f);

    float fX = ptInDoseGrid.GetX();
    float fY = ptInDoseGrid.GetY();
    float fZ = ptInDoseGrid.GetZ();

    int xDim = dosegrid.get_xcount();
    int yDim = dosegrid.get_ycount();
    int zDim = dosegrid.get_zcount();
    if (fX < -0.5f || fX > xDim - 0.5f || 
        fY < -0.5f || fY > yDim - 0.5f || 
        fZ < -0.5f || fZ > zDim - 0.5f) 
    {
        //TPS_LOG_DEV_ERROR<<"Failed because point is out of dose grid range. uid["<<
        //    dosegrid.get_uid()<<"]";
        return -1;
    }

    if (!dosegrid.get_isdosevalid())
    {
        dosevalue = -1.0;
        return 0;
    }
    
    // Get dose value
    bool bTrilinear = true;
    if (bTrilinear)
    {
        int x = floor(fX);
        int y = floor(fY);
        int z = floor(fZ);
        int x1 = x + 1;
        int y1 = y + 1;
        int z1 = z + 1;

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (z < 0) z = 0;
        if (x1 >= xDim) x1 = xDim - 1;
        if (y1 >= yDim) y1 = yDim - 1;
        if (z1 >= zDim) z1 = zDim - 1;

        float dx = fX - x;
        float dy = fY - y;
        float dz = fZ - z;

        float f000 = pDoseGridData[z * xDim * yDim + y * xDim + x];
        float f100 = pDoseGridData[z * xDim * yDim + y * xDim + x1];;
        float f010 = pDoseGridData[z * xDim * yDim + y1 * xDim + x];
        float f110 = pDoseGridData[z * xDim * yDim + y1 * xDim + x1];
        float f001 = pDoseGridData[z1 * xDim * yDim + y * xDim + x];
        float f101 = pDoseGridData[z1 * xDim * yDim + y * xDim + x1];
        float f011 = pDoseGridData[z1 * xDim * yDim + y1 * xDim + x];
        float f111 = pDoseGridData[z1 * xDim * yDim + y1 * xDim + x1];
        dosevalue = (1 - dx) * (1 - dy) * (1 - dz) * f000 + 
            dx * (1 - dy) * (1 - dz) * f100 + 
            (1 - dx) * dy * (1 - dz) * f010 + 
            dx * dy * (1 - dz) * f110 +
            (1 - dx) * (1 - dy) * dz * f001 + 
            dx * (1 - dy) * dz * f101 + 
            (1 - dx) * dy * dz * f011 + 
            dx * dy * dz * f111;
    }
    else
    {
        int x = fX;
        int y = fY;//static_cast<int>( + 0.5);
        int z = static_cast<int>(fZ + 0.5);
        float f000 = pDoseGridData[z * xDim * yDim + y * xDim + x];
        float f100 = f000;
        float f010 = f000;
        float f110 = f000;

        if ((x + 1) < xDim)
        {
            f100 = pDoseGridData[z * xDim * yDim + y * xDim + x + 1];
            f110 = f100;
        }

        if ((y + 1) < yDim)
        {
            f010 = pDoseGridData[z * xDim * yDim + y * xDim + x + xDim];
            f110 = f010;
        }

        if (((x + 1) < xDim) && ((y + 1) < yDim))
        {
            f110 = pDoseGridData[z * xDim * yDim + y * xDim + x + xDim + 1];
        }

        float dx = fX - x;
        float dy = fY - y;
        dosevalue = (1 - dx) * (1 - dy) * f000 + dx * (1 - dy) * f100 + (1 - dx) * dy * f010 + dx * dy * f110;
    }

    return 0;
}

// return -1 if invalid
// return 0  if dose returned is valid
int TpsDoseManager::GetPointDoseValueofPlan(const std::string& strPlanUID, 
    double xInPatient, double yInPatient, double zInPatient, float& doseValue) 
{
    // doseValue默认先赋无效值
    doseValue = INVALID_DOSE_VALUE;
    if (strPlanUID.empty()) return TPS_ER_SUCCESS;

    // Get dose grid buffer
    RtPlan* pPlan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(strPlanUID, &pPlan)) {
        TPS_LOG_DEV_INFO<<"Failed to get plan["<<strPlanUID<<"] from patient manager";
        return -1;
    }

    return GetPointDoseValue(*pPlan->get_dosegrid(), xInPatient, yInPatient, zInPatient, doseValue);
}

int TpsDoseManager::GetPointDoseValueofNormgroup(const std::string& strNgUid, 
    double xInPatient, double yInPatient, double zInPatient, float& doseValue) 
{
    doseValue = 0.0f;

    // Get dose grid buffer
    RtNormgroup* pNg = nullptr;
    if (!GetNormgroupManager()->GetNormGroup(strNgUid, &pNg) || 
        pNg==nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get normgroup["<<strNgUid<<"] from manager";
            return -1;
    }

    return GetPointDoseValue(*pNg->get_dosegrid(), 
        xInPatient, yInPatient, zInPatient, doseValue);
}

int TpsDoseManager::GetPointDoseValueofBeam(const std::string& strBeamUid, 
    double xInPatient, double yInPatient, double zInPatient, float& doseValue) 
{
    doseValue = 0.0f;

    // Get dose grid buffer
    RtBeam* pBeam = GetBeamManager()->GetBeam(strBeamUid);
    if (pBeam==nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get beam["<<strBeamUid<<"] from manager";
        return -1;
    }

    return GetPointDoseValue(*pBeam->get_dosegrid(), xInPatient, yInPatient, zInPatient, doseValue);
}

int TpsDoseManager::GetMaxDose_i(RtDosegrid& dosegrid,
    float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue)
{
    const float* pDoseGridData = dosegrid.get_dosegrid_buffer();
    if (pDoseGridData == nullptr) {
        TPS_LOG_DEV_ERROR<<"Invalid dosegrid buffer in dosegrid object["
            <<dosegrid.get_uid()<<"].";
        return -1;
    }

    int xInGrid = 0;
    int yInGrid = 0;
    int zInGrid = 0;
    int xDim = dosegrid.get_xcount();
    int yDim = dosegrid.get_ycount();
    int zDim = dosegrid.get_zcount();
    doseValue = pDoseGridData[0];
    for (int z = 0; z < zDim; z++) {
        for (int y = 0; y < yDim; y++) {
            for (int x = 0; x < xDim; x++) {
                if (pDoseGridData[z * xDim * yDim + y * xDim + x] > doseValue) {
                    doseValue = pDoseGridData[z * xDim * yDim + y * xDim + x];
                    xInGrid = x;
                    yInGrid = y;
                    zInGrid = z;
                }
            } // x loop
        } // y loop
    } // z loop

    // Point in patient to dose grid
    Mcsf::Point3f ptInGrid;
    ptInGrid.SetX(xInGrid);
    ptInGrid.SetY(yInGrid);
    ptInGrid.SetZ(zInGrid);

    Mcsf::Matrix4f matGrid2Pat = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid.get_grid_to_pat_t());
    Mcsf::Point3f ptInPat = matGrid2Pat.Transform(ptInGrid);

    pXInPat = ptInPat.GetX();

    pYInPat = ptInPat.GetY();

    pZInPat = ptInPat.GetZ();

    return 0;
}

bool TpsDoseManager::AddDoseLineList(std::string planUID, const std::vector<RtDoseline*>& doseLineList) 
{
    auto itDoseLineList = mDoselineCollection.find(planUID);
    if (itDoseLineList != mDoselineCollection.end()) {
        return false;
    }

    mDoselineCollection[planUID] = doseLineList;

    return true;
}

bool TpsDoseManager::RemoveDoseLineList(std::string planUID) {
    auto itDoseLineList = mDoselineCollection.find(planUID);
    if (itDoseLineList == mDoselineCollection.end()) {
        return false;
    }

    std::vector<RtDoseline*> delDoseLineList = itDoseLineList->second;
    for (auto itDoseLine = delDoseLineList.begin(); itDoseLine != delDoseLineList.end(); ++itDoseLine) {
        delete *itDoseLine;
        *itDoseLine = nullptr;
    }

    delDoseLineList.clear();
    mDoselineCollection.erase(itDoseLineList);

    return true;
}

bool TpsDoseManager::UpdateDoseLineList(std::string planUID, std::vector<RtDoseline*>& doseLineList) {

    return RemoveDoseLineList(planUID) && AddDoseLineList(planUID, doseLineList);
}

bool TpsDoseManager::GetDoseLineList(std::string planUID, std::vector<RtDoseline*>& doseLineList) {
    auto itDoseLineList = mDoselineCollection.find(planUID);
    if (itDoseLineList == mDoselineCollection.end()) {
        return false;
    }

    doseLineList = itDoseLineList->second;

    return true;
}

int TpsDoseManager::GetMaxDosePointOfPlan(const std::string& strPlanUID,
    float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue)
{
    // Get dose grid buffer
    RtPlan* pPlan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(strPlanUID, &pPlan)) {
        TPS_LOG_DEV_ERROR<<"Failed to get plan object from patient manager";
        return -1;
    }

    return GetMaxDose_i(*pPlan->get_dosegrid(), pXInPat, pYInPat, pZInPat, doseValue);
}

int TpsDoseManager::GetMaxDosePointOfNormgroup(const std::string& strNgUid,
    float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue)
{
    // Get dose grid buffer
    RtNormgroup* pNg = nullptr;
    if (!GetNormgroupManager()->GetNormGroup(strNgUid, &pNg)) {
        TPS_LOG_DEV_ERROR<<"Failed to get normgroup object from manager";
        return -1;
    }
    
    return GetMaxDose_i(*pNg->get_dosegrid(), pXInPat, pYInPat, pZInPat, doseValue);
}

int TpsDoseManager::GetMaxDosePointOfBeam(const std::string& strBeamUid,
    float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue)
{
    RtBeam* pBeam = GetBeamManager()->GetBeam(strBeamUid);
    if (pBeam==nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get beam object from manager";
        return -1;
    }

    return GetMaxDose_i(*pBeam->get_dosegrid(), pXInPat, pYInPat, pZInPat, doseValue);
}

TPS_END_NAMESPACE
