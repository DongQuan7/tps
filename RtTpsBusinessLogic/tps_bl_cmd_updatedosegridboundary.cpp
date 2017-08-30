//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_pca_updategridboundarycmd.cpp
///  \brief   update gridboundary command
///
///  \version 1.0
///  \date    Apr. 10, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_updatedosegridboundary.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
//Rt
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

TPS_BEGIN_NAMESPACE
using namespace Mcsf::MedViewer3D;

TpsUpdateGridBoundaryCMD::TpsUpdateGridBoundaryCMD(CertifiedGridBoundary *pocoGridBoundary) 
    : mPocoGridBoundary(pocoGridBoundary) {
}


TpsUpdateGridBoundaryCMD::~TpsUpdateGridBoundaryCMD(void) {
}

int TpsUpdateGridBoundaryCMD::Execute() {
    //Get RenderProxy and RenderController
    Mcsf::Point2f screenPoint;
    Mcsf::Point3f patientPoint;
    screenPoint.SetX(mPocoGridBoundary->mX);
    screenPoint.SetY(mPocoGridBoundary->mY);
    mRenderProxy->TransformPoint((WINDOW_TYPE)mPocoGridBoundary->mWindowType,
        mPocoGridBoundary->mWindowUid, screenPoint, patientPoint);
    TpsDoseManager* doseManager = mDataRepository->GetDoseManager();
    RtDosegrid* dosegrid = 
        doseManager->GetDoseGrid(mPocoGridBoundary->mPlanUid,DOSE_GRID_PLAN);
    dosegrid->set_isdosevalid(false);
    float xCount = dosegrid->get_xcount() - 0.5;
    float yCount = dosegrid->get_ycount() - 0.5;
    float zCount = dosegrid->get_zcount() - 0.5;
    Mcsf::Point3f increase = Mcsf::Point3f(dosegrid->get_xcoordinate3dinc(), dosegrid->get_ycoordinate3dinc(), dosegrid->get_zcoordinate3dinc());
    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid->get_grid_to_pat_t());
    Mcsf::Point3f mRightTopPoint = matG2P.Transform(
        Mcsf::Point3f(xCount, yCount, zCount));
    Mcsf::Point3f mLeftBottomPoint = matG2P.Transform(Mcsf::Point3f(-0.5f, -0.5f, -0.5f));
    bool updateDoseGridBoundarySuccess = true;
    if (mPocoGridBoundary->mActionType == DOWN){
        bool hitTestSuccess = false;
        int planes[3];
        hitTestSuccess = HitTest(planes, dosegrid);
        doseManager->SetMovingPlanes(planes);
        if (hitTestSuccess == true){
            doseManager->SetHitTestSuccess(true);
        }
        else{
            doseManager->SetHitTestSuccess(false);
        }
    }
    else if(mPocoGridBoundary->mActionType == DRAG){
        if (doseManager->IsHitTestSuccess()){
            updateDoseGridBoundarySuccess = UpdateDoseGridBoundary();
        }
    }
    else if (mPocoGridBoundary->mActionType == UP){
        doseManager->FinishMovingBoundary();
        Mcsf::Point3f start;
        start = Mcsf::Point3f(dosegrid->get_xcoordinate3dstart(), dosegrid->get_ycoordinate3dstart(), dosegrid->get_zcoordinate3dstart());
        float spacing[3];
        spacing[0] = dosegrid->get_xcoordinate3dinc();
        spacing[1] = dosegrid->get_ycoordinate3dinc();
        spacing[2] = dosegrid->get_zcoordinate3dinc();
        int dim[3];
        dim[0] = dosegrid->get_xcount();
        dim[1] = dosegrid->get_ycount();
        dim[2] = dosegrid->get_zcount();

        std::vector<RtDosegrid*> vDosegrids;
        std::vector<std::string> ngUids = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(mPocoGridBoundary->mPlanUid);
        std::vector<std::string> beamUids;
        vDosegrids.push_back(dosegrid);
        for (std::vector<std::string>::iterator ngUidIt = ngUids.begin(); ngUidIt != ngUids.end(); ++ngUidIt)
        {
            beamUids = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(*ngUidIt);
            for (auto beamUidIt = beamUids.begin(); beamUidIt != beamUids.end(); ++beamUidIt) {
                RtDosegrid* beamDoseGrid = doseManager->GetDoseGrid(*beamUidIt, DOSE_GRID_BEAM);
                vDosegrids.push_back(beamDoseGrid);
            }
            RtDosegrid* ngDoseGrid = doseManager->GetDoseGrid(*ngUidIt, DOSE_GRID_NORMGROUP);
            vDosegrids.push_back(ngDoseGrid);
        }
        Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(
            vDosegrids[0]->get_grid_to_pat_t());
        Mcsf::Vector3f axis[3];
        GetAxis(matG2P, axis);
        for (std::vector<RtDosegrid*>::iterator dosegridIt = vDosegrids.begin();
            dosegridIt != vDosegrids.end(); ++dosegridIt){
                doseManager->UpdateDoseGrid(*dosegridIt,start,axis,spacing,dim);
        }
        mSaveObjects->SaveDosegridsForPlan(mPocoGridBoundary->mPlanUid);
        //RtPlan* plan = nullptr;
        //mDataRepository->GetPatientManager()->GetPlan(mPocoGridBoundary->mPlanUid, &plan);
        //std::vector<RtPlan*> plans;
        //plans.push_back(plan);
        //RepositoryDispatcher::GetInstance()->UpdatePlanToFE(plans);

        // 更新plan图像相关voi的in grid信息，目前只在鼠标弹起时更新
        UpdateRelatedVOIInGrid(mPocoGridBoundary->mPlanUid, dosegrid);
    }
    if (updateDoseGridBoundarySuccess){
        Mcsf::Point3f spacing;
        spacing = Mcsf::Point3f(dosegrid->get_xcoordinate3dinc(), dosegrid->get_ycoordinate3dinc(), dosegrid->get_zcoordinate3dinc());
        mRenderProxy->EditDoseGridBoundary(mPocoGridBoundary->mPlanUid, 
            mLeftBottomPoint, mRightTopPoint, spacing);
        mRenderProxy->Render();
    }
    return TPS_ER_SUCCESS;
}

bool TpsUpdateGridBoundaryCMD::HitTest(int *plane, RtDosegrid* grid) {

    Mcsf::Point2f screenPoint;
    Mcsf::Point3f doseGridPoint;
    screenPoint.SetX(mPocoGridBoundary->mX);
    screenPoint.SetY(mPocoGridBoundary->mY);
    if (!mRenderProxy->TransformPoint((WINDOW_TYPE)mPocoGridBoundary->mWindowType, 
        mPocoGridBoundary->mWindowUid, screenPoint, doseGridPoint)){
        TPS_LOG_DEV_ERROR<<"Failed to transform point.";
        return false;
    }
    Mcsf::Matrix4f matP2G = TpsArithmeticConverter::ConvertToMatrix4f(grid->get_grid_to_pat_t()).Inverse();
    doseGridPoint = matP2G.Transform(doseGridPoint);
    doseGridPoint.SetX(doseGridPoint.GetX() * grid->get_xcoordinate3dinc());
    doseGridPoint.SetY(doseGridPoint.GetY() * grid->get_ycoordinate3dinc());
    doseGridPoint.SetZ(doseGridPoint.GetZ() * grid->get_zcoordinate3dinc());
    //Transform 5 pixel length in screen to patient coordinate,
    //Get HitTest radius
    DISPLAY_SIZE displaySize;
    mRenderProxy->GetDisplaySize((WINDOW_TYPE)mPocoGridBoundary->mWindowType, 
        mPocoGridBoundary->mWindowUid, displaySize);
    Mcsf::Point2f point1(0.0, 0.0);
    Mcsf::Point2f point2(0.0, 5.0 / displaySize.height);
    Mcsf::Point2f point3(5.0 / displaySize.width, 0.0);
    Mcsf::Point3f doseGridPoint1;
    Mcsf::Point3f doseGridPoint2;
    Mcsf::Point3f doseGridPoint3;
    mRenderProxy->TransformPoint((WINDOW_TYPE)mPocoGridBoundary->mWindowType, 
        mPocoGridBoundary->mWindowUid, point1, doseGridPoint1);
    mRenderProxy->TransformPoint((WINDOW_TYPE)mPocoGridBoundary->mWindowType, 
        mPocoGridBoundary->mWindowUid, point2, doseGridPoint2);
    mRenderProxy->TransformPoint((WINDOW_TYPE)mPocoGridBoundary->mWindowType, 
        mPocoGridBoundary->mWindowUid, point3, doseGridPoint3);

    doseGridPoint1 = matP2G.Transform(doseGridPoint1);
    doseGridPoint2 = matP2G.Transform(doseGridPoint2);
    doseGridPoint3 = matP2G.Transform(doseGridPoint3);

    doseGridPoint1.SetX(doseGridPoint1.GetX() * grid->get_xcoordinate3dinc());
    doseGridPoint1.SetY(doseGridPoint1.GetY() * grid->get_ycoordinate3dinc());
    doseGridPoint1.SetZ(doseGridPoint1.GetZ() * grid->get_zcoordinate3dinc());
    doseGridPoint2.SetX(doseGridPoint2.GetX() * grid->get_xcoordinate3dinc());
    doseGridPoint2.SetY(doseGridPoint2.GetY() * grid->get_ycoordinate3dinc());
    doseGridPoint2.SetZ(doseGridPoint2.GetZ() * grid->get_zcoordinate3dinc());
    doseGridPoint3.SetX(doseGridPoint3.GetX() * grid->get_xcoordinate3dinc());
    doseGridPoint3.SetY(doseGridPoint3.GetY() * grid->get_ycoordinate3dinc());
    doseGridPoint3.SetZ(doseGridPoint3.GetZ() * grid->get_zcoordinate3dinc());

    double hitTestSpacing = sqrt((doseGridPoint2.GetX() - doseGridPoint1.GetX()) * 
        (doseGridPoint2.GetX() - doseGridPoint1.GetX()) + 
        (doseGridPoint2.GetY() - doseGridPoint1.GetY()) * 
        (doseGridPoint2.GetY() - doseGridPoint1.GetY()) + 
        (doseGridPoint2.GetZ() - doseGridPoint1.GetZ()) * 
        (doseGridPoint2.GetZ() - doseGridPoint1.GetZ()));
    //Get HitTest Plane
    GetHitTestPlane(doseGridPoint, hitTestSpacing, plane);
    GetMovingPlane(plane, doseGridPoint1, doseGridPoint2, doseGridPoint3);
    if (plane[0] != -1 || plane[1] != -1 || plane[2] != -1){
        return true;
    }
    return false;
}

bool TpsUpdateGridBoundaryCMD::GetHitTestPlane(Mcsf::Point3f& point, double radius, int *plane) {
    plane[0] = -1;
    plane[1] = -1;
    plane[2] = -1;
    //Get DoseGrid and the leftBottom Point and rightTop Point
    RtDosegrid* dosegrid = 
        mDataRepository->GetDoseManager()->GetDoseGrid(mPocoGridBoundary->mPlanUid,DOSE_GRID_PLAN);
    float xCount = dosegrid->get_xcount() - 0.5f;
    float yCount = dosegrid->get_ycount() - 0.5f;
    float zCount = dosegrid->get_zcount() - 0.5f;
    Mcsf::Point3f leftBottomPoint(-0.5f * dosegrid->get_xcoordinate3dinc(), 
        -0.5f * dosegrid->get_xcoordinate3dinc(), 
        -0.5f * dosegrid->get_xcoordinate3dinc());
    Mcsf::Point3f rightTopPoint(xCount * dosegrid->get_xcoordinate3dinc(), 
        yCount * dosegrid->get_ycoordinate3dinc(), 
        zCount * dosegrid->get_zcoordinate3dinc());
    //First, if the point is out of the doseGridBoundary, return
    if (abs(point.GetX() - rightTopPoint.GetX()) > radius &&
        abs(point.GetY() - rightTopPoint.GetY()) > radius &&
        abs(point.GetZ() - rightTopPoint.GetZ()) > radius &&
        abs(leftBottomPoint.GetX() - point.GetX()) > radius &&
        abs(leftBottomPoint.GetY() - point.GetY()) > radius &&
        abs(leftBottomPoint.GetZ() - point.GetZ()) > radius) {
            return false;
    }
    //when the opposite planes is very close, I must choose one of them. Here I 
    //have two ways, the first way is choose the closer one, and the other method
    //is choose the plane according to the moving direction of the mouse move.
    //In this function I choose the first way.
    //the order of plane in volume, left(0), right(1), top(2), bottom(3), front(4), back(5)
    //0):judge weather the point is on the left plane
    bool hasOneHitTestPlane = false;
    bool hasTwoHitTestPlane = false;

    float plx = abs(point.GetX() - leftBottomPoint.GetX());
    float prx = abs(point.GetX() - rightTopPoint.GetX());
    float ply = abs(point.GetY() - leftBottomPoint.GetY());
    float pry = abs(point.GetY() - rightTopPoint.GetY());
    float plz = abs(point.GetZ() - leftBottomPoint.GetZ());
    float prz = abs(point.GetZ() - rightTopPoint.GetZ());
    if (plx < radius) {
        plane[0] = 0;
        hasOneHitTestPlane = true;
    }
    //1):judge weather the point is on the right plane
    if (prx < radius) {
        if (hasOneHitTestPlane == true) {
            if(prx < plx) {
                plane[0] = 1;
            }
        }
        else{
            plane[0] = 1;
            hasOneHitTestPlane = true;
        }
    }
    //2):judge weather the point is on the top plane
    if (prz < radius) {
        if (hasOneHitTestPlane == true) {
            plane[1] = 2;
            hasTwoHitTestPlane = true;
        }
        else {
            plane[0] = 2;
            hasOneHitTestPlane = true;
        }
    }
    //3):judge weather the point is on the bottom plane
    if (plz < radius) {
        if (hasTwoHitTestPlane == true) {
            if(plz < prz) {
                plane[1] = 3;
            }
        }
        else if(hasTwoHitTestPlane == false && hasOneHitTestPlane == true){
            if(plane[0] == 2) {
                if(plz < prz) {
                    plane[0] = 3;
                }
            }
            else {
                plane[1] = 3;
                hasTwoHitTestPlane = true;
            }
        }
        else if (hasOneHitTestPlane == false) {
            plane[0] = 3;
            hasOneHitTestPlane = true;
        }
    }
    //4):judge weather the point is on the front plane
    if (pry < radius) {
        if (hasTwoHitTestPlane == true) {
            plane[2] = 4;
        }
        else if(hasTwoHitTestPlane == false && hasOneHitTestPlane == true){
            plane[1] = 4;
            hasTwoHitTestPlane = true;
        }
        else if (hasOneHitTestPlane == false) {
            plane[0] = 4;
            hasOneHitTestPlane = true;
        }
    }
    //5):judge weather the point is on the back plane
    if (ply < radius) {
        if (hasTwoHitTestPlane == true) {
            if(ply < pry) {
                if(plane[1] == 4) {
                    plane[1] = 5;
                }
                else if(plane[2] == 4) {
                    plane[2] = 5;
                }
            }
            return true;
        }
        else if(hasTwoHitTestPlane == false && hasOneHitTestPlane == true){
            if(ply < pry) {
                if(plane[0] == 4) {
                    plane[0] = 5;
                }
                else {
                    plane[1] = 5;
                    hasTwoHitTestPlane = true;
                }
            }
            return true;
        }
        else if (hasOneHitTestPlane == false) {
            plane[0] = 5;
            hasOneHitTestPlane = true;
        }
    }
    if (hasOneHitTestPlane == true) {
        return true;
    }
    else {
        return false;
    }
}

bool TpsUpdateGridBoundaryCMD::GetMovingPlane(int *plane, Mcsf::Point3f& point1, 
    Mcsf::Point3f& point2, Mcsf::Point3f& point3) {
        //Get DoseGrid and the leftBottom Point and rightTop Point
        RtDosegrid* dosegrid = 
            mDataRepository->GetDoseManager()->GetDoseGrid(mPocoGridBoundary->mPlanUid,DOSE_GRID_PLAN);
        float xCount = dosegrid->get_xcount() - 0.5f;
        float yCount = dosegrid->get_ycount() - 0.5f;
        float zCount = dosegrid->get_zcount() - 0.5f;
        Mcsf::Point3f leftBottomPoint(-0.5f * dosegrid->get_xcoordinate3dinc(), 
            -0.5f * dosegrid->get_xcoordinate3dinc(),
            -0.5f * dosegrid->get_xcoordinate3dinc());
        Mcsf::Point3f rightTopPoint(xCount * dosegrid->get_xcoordinate3dinc(), 
            yCount * dosegrid->get_ycoordinate3dinc(), 
            zCount * dosegrid->get_zcoordinate3dinc());

        Mcsf::Matrix4f matPat2Grid = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid->get_grid_to_pat_t()).Inverse();

        for (int i = 0; i < 3; ++i) {
            if (plane[i] == 0){
                if (abs(point1.GetX() - leftBottomPoint.GetX()) < 1e-6 &&
                    abs(point2.GetX() - leftBottomPoint.GetX()) < 1e-6 &&
                    abs(point3.GetX() - leftBottomPoint.GetX()) < 1e-6 ) {
                        plane[i] = -1;
                }
            }
            if (plane[i] == 1){
                if (abs(point1.GetX() - rightTopPoint.GetX()) < 1e-6 &&
                    abs(point2.GetX() - rightTopPoint.GetX()) < 1e-6 &&
                    abs(point3.GetX() - rightTopPoint.GetX()) < 1e-6 ) {
                        plane[i] = -1;
                }
            }
            if (plane[i] == 2){
                if (abs(point1.GetZ() - rightTopPoint.GetZ()) < 1e-6 &&
                    abs(point2.GetZ() - rightTopPoint.GetZ()) < 1e-6 &&
                    abs(point3.GetZ() - rightTopPoint.GetZ()) < 1e-6 ) {
                        plane[i] = -1;
                }
            }
            if (plane[i] == 3){
                if (abs(point1.GetZ() - leftBottomPoint.GetZ()) < 1e-6 &&
                    abs(point2.GetZ() - leftBottomPoint.GetZ()) < 1e-6 &&
                    abs(point3.GetZ() - leftBottomPoint.GetZ()) < 1e-6 ) {
                        plane[i] = -1;
                }
            }
            if (plane[i] == 4){
                if (abs(point1.GetY() - rightTopPoint.GetY()) < 1e-6 &&
                    abs(point2.GetY() - rightTopPoint.GetY()) < 1e-6 &&
                    abs(point3.GetY() - rightTopPoint.GetY()) < 1e-6 ) {
                        plane[i] = -1;
                }
            }
            if (plane[i] == 5){
                if (abs(point1.GetY() - leftBottomPoint.GetY()) < 1e-6 &&
                    abs(point2.GetY() - leftBottomPoint.GetY()) < 1e-6 &&
                    abs(point3.GetY() - leftBottomPoint.GetY()) < 1e-6 ) {
                        plane[i] = -1;
                }
            }
        }
        return true;
}

bool TpsUpdateGridBoundaryCMD::UpdateDoseGridBoundary() {

    //Get DoseGrid and the leftBottom Point and rightTop Point
    TpsDoseManager * doseManager = mDataRepository->GetDoseManager();
    RtDosegrid* dosegrid = 
        doseManager->GetDoseGrid(mPocoGridBoundary->mPlanUid,DOSE_GRID_PLAN);
    float xCount = dosegrid->get_xcount() - 0.5f;
    float yCount = dosegrid->get_ycount() - 0.5f;
    float zCount = dosegrid->get_zcount() - 0.5f;
    Mcsf::Point3f leftBottomPoint(-0.5f, -0.5f, -0.5f);
    Mcsf::Point3f rightTopPoint(xCount, yCount, zCount);
    Mcsf::Point2f targetPoint(mPocoGridBoundary->mX, mPocoGridBoundary->mY);
    Mcsf::Point3f targetPointInDoseGrid;
    mRenderProxy->TransformPoint((WINDOW_TYPE)mPocoGridBoundary->mWindowType, 
        mPocoGridBoundary->mWindowUid, targetPoint, targetPointInDoseGrid);
    targetPointInDoseGrid = TpsArithmeticConverter::ConvertToMatrix4f(
        dosegrid->get_grid_to_pat_t()).Inverse().Transform(targetPointInDoseGrid);
    int plane[3];
    doseManager->GetMovingPlanes(plane);
    for (int i = 0; i < 3; ++i) {
        if (plane[i] != -1){
            if (!UpdateDoseGridBoundaryByMovingOnePlane(plane[i], leftBottomPoint,
                rightTopPoint, targetPointInDoseGrid)){
                    return false;
            }
        }
    }
    return true;
}

bool TpsUpdateGridBoundaryCMD::UpdateDoseGridBoundaryByMovingOnePlane(int plane, 
    Mcsf::Point3f lbPt,Mcsf::Point3f rtPt, 
    Mcsf::Point3f targetPt) 
{
    std::string planUID = mPocoGridBoundary->mPlanUid;
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan) 
        || plan == nullptr){
            TPS_LOG_DEV_ERROR<<"The plan does not exist, uid: "<<planUID;
            return false;
    }

    std::string seriesUid = plan->get_imageseriesuid();

    TpsImage3DEntity *imageEntity = nullptr;
    if(!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &imageEntity) || imageEntity == nullptr) {
        TPS_LOG_DEV_ERROR<<"The plan does not exist, uid: "<<planUID;
        return false;
    }


    RtDosegrid* dosegrid = plan->get_dosegrid();
    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid->get_grid_to_pat_t());

    Mcsf::Vector3f axis[3];
    GetAxis(matG2P, axis);

    Mcsf::Matrix4f matP2G = matG2P.Inverse();

    float newTarget = 0;
    int newCountx = 0;
    int newCounty = 0;
    int newCountz = 0;

    int lbx, lby, lbz, rtx, rty, rtz;

    imageEntity->GetBoundaryInDosegrid(matP2G, lbx, lby, lbz, rtx, rty,rtz);

    //边界向外扩一格
    //lbx -= 1; lby -= 1; lbz -= 1;
    //rtx += 1; rty += 1; rtz += 1;

    Mcsf::Point3f innerLbPt, innerRtPt;
    switch (plane)
    {
    case 0:
        newTarget = static_cast<int>(targetPt.GetX());
        newTarget -= 0.5f;
        newCountx = (rtPt.GetX() - newTarget) + 0.5f;
        if (newCountx <= 1){
            newCountx = 1;
			lbPt.SetX(rtPt.GetX() - 1.0f);
			MovePoint(lbPt, 0.5f);
        }
        if(newCountx != 1)
        {
			lbPt.SetX(newTarget);
			MovePoint(lbPt, 0.5f);
            if(lbPt.GetX() < lbx)
            {
                lbPt.SetX(lbx);
                newCountx = (rtPt.GetX() - lbPt.GetX()) + 1.0f;
            }
        }
        lbPt = matG2P.Transform(lbPt);
        dosegrid->set_xcoordinate3dstart(lbPt.GetX());
        dosegrid->set_ycoordinate3dstart(lbPt.GetY());
        dosegrid->set_zcoordinate3dstart(lbPt.GetZ());
        dosegrid->set_xcount(newCountx);
        break;
    case 1:
        newTarget = static_cast<int>(targetPt.GetX());
        newTarget -= 0.5f;
        newCountx = (newTarget - lbPt.GetX()) + 0.5;
        if (newCountx < 1){
            newCountx = 1;
        }
        if(newCountx != 1)
        {
            if(newTarget > rtx)
            {
                newTarget = rtx;
                newCountx = (newTarget - lbPt.GetX()) + 1.0f;
            }
        }
        dosegrid->set_xcount(newCountx);
        break;
    case 2:
        newTarget = static_cast<int>(targetPt.GetZ());
        newTarget -= 0.5f;

        newCountz = (newTarget - lbPt.GetZ()) + 0.5;
        if (newCountz < 1){
            newCountz = 1;
        }
        if(newCountz != 1)
        {
            if(newTarget > rtz)
            {
                newTarget = rtz;
                newCountz = (newTarget - lbPt.GetZ()) + 1.0f;
            }
        }
        dosegrid->set_zcount(newCountz);
        break;
    case 3:
        newTarget = static_cast<int>(targetPt.GetZ());
        newTarget -= 0.5f;
        newCountz = (rtPt.GetZ() - newTarget) + 0.5;
        if (newCountz <= 1){
            newCountz = 1;
			lbPt.SetZ(rtPt.GetZ() - 1.0f);
			MovePoint(lbPt, 0.5f);
        }
        if(newCountz != 1)
        {
			lbPt.SetZ(newTarget);
			MovePoint(lbPt, 0.5f);
            if(lbPt.GetZ() < lbz)
            {
                lbPt.SetZ(lbz);
                newCountz = (rtPt.GetZ() - lbPt.GetZ()) + 1.0f;
            }
        }
        lbPt =  matG2P.Transform(lbPt);
        dosegrid->set_xcoordinate3dstart(lbPt.GetX());
        dosegrid->set_ycoordinate3dstart(lbPt.GetY());
        dosegrid->set_zcoordinate3dstart(lbPt.GetZ());
        dosegrid->set_zcount(newCountz);
        break;
    case 4:
        newTarget = static_cast<int>(targetPt.GetY());
        newTarget -= 0.5f;
        newCounty = (newTarget - lbPt.GetY()) + 0.5;
        if (newCounty < 1){
            newCounty = 1;
        }
        if(newCounty != 1)
        {
            if(newTarget > rty)
            {
                newTarget = rty;
                newCounty = (newTarget - lbPt.GetY()) + 1.0f;
            }
        }
        dosegrid->set_ycount(newCounty);
        break;
    case 5:
        newTarget = static_cast<int>(targetPt.GetY());
        newTarget -= 0.5f;
        newCounty = (rtPt.GetY() - newTarget) + 0.5;
        if (newCounty <= 1){
            newCounty = 1;
			lbPt.SetY(rtPt.GetY() - 1.0f);
			MovePoint(lbPt, 0.5f);
        }
        if(newCounty != 1)
        {
			lbPt.SetY(newTarget);
			MovePoint(lbPt, 0.5f);
            if(lbPt.GetY() < lby)
            {
                lbPt.SetY(lby);
                newCounty = (rtPt.GetY() - lbPt.GetY()) + 1.0f;
            }
        }
        lbPt =  matG2P.Transform(lbPt);
        dosegrid->set_xcoordinate3dstart(lbPt.GetX());
        dosegrid->set_ycoordinate3dstart(lbPt.GetY());
        dosegrid->set_zcoordinate3dstart(lbPt.GetZ());
        dosegrid->set_ycount(newCounty);
        break;
    default:
        break;
    }
    UpdateDoseGrid(dosegrid, axis, false);

    return true;
}

bool TpsUpdateGridBoundaryCMD::GetAxis(const Mcsf::Matrix4f& matrix, Mcsf::Vector3f* axis) 
{
    axis[0] = matrix.Transform(Mcsf::Vector3f(1.f, 0.f, 0.f));
    axis[1] = matrix.Transform(Mcsf::Vector3f(0.f, 1.f, 0.f));
    axis[2] = matrix.Transform(Mcsf::Vector3f(0.f, 0.f, 1.f));
    axis[0].Normalize();
    axis[1].Normalize();
    axis[2].Normalize();

    //std::string planUID = mPocoGridBoundary->mPlanUid;
    //RtPlan* plan = nullptr;
    //if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan) && plan == nullptr) {
    //    TPS_LOG_DEV_ERROR<<"The plan does not exist, uid: "<<planUID;
    //    return false;
    //}

    //std::string imageUID = plan->get_imageseriesuid();
    //TpsImage3DEntity* image = nullptr;
    //if (!mDataRepository->GetImageDataManager()->GetImage3DData(
    //    imageUID, &image)){
    //        TPS_LOG_DEV_ERROR<<"The image entity with UID: "<<
    //            imageUID<<" does not exist.";
    //        return false;
    //}

    //RtImage3DHeader* header = nullptr;
    //if (!image->GetImage3DHeader(header)){
    //    TPS_LOG_DEV_ERROR<<"Failed to get the image header.";
    //    return false;
    //}

    //int dim[3] = {header->m_iXDim, header->m_iYDim, header->m_iSliceCount};
    //axis[0] = Mcsf::Point3f(header->m_ImageXOrientation.x, header->m_ImageXOrientation.y,header->m_ImageXOrientation.z);
    //axis[1] = Mcsf::Point3f(header->m_ImageYOrientation.x, header->m_ImageYOrientation.y,header->m_ImageYOrientation.z);

    //auto scanInfoList = header->m_vScanInfoList;
    //Mcsf::Point3f org = Mcsf::Point3f(scanInfoList[0].m_ImagePosition.x,scanInfoList[0].m_ImagePosition.y,scanInfoList[0].m_ImagePosition.z);
    //Mcsf::Point3f last = Mcsf::Point3f(scanInfoList[dim[2] - 1].m_ImagePosition.x,scanInfoList[dim[2] - 1].m_ImagePosition.y,scanInfoList[dim[2] - 1].m_ImagePosition.z);
    //Mcsf::Vector3f vecZ = last - org;
    //axis[2] = vecZ.GetNormalize();
    return true;
}

bool TpsUpdateGridBoundaryCMD::UpdateDoseGrid(RtDosegrid* grid, 
    Mcsf::Vector3f* axis, bool bCreatBuffer) {
        Mcsf::Point3f start;
        start = Mcsf::Point3f(grid->get_xcoordinate3dstart(), grid->get_ycoordinate3dstart(), grid->get_zcoordinate3dstart());
        float spacing[3];
        spacing[0] = grid->get_xcoordinate3dinc();
        spacing[1] = grid->get_ycoordinate3dinc();
        spacing[2] = grid->get_zcoordinate3dinc();
        int dim[3];
        dim[0] = grid->get_xcount();
        dim[1] = grid->get_ycount();
        dim[2] = grid->get_zcount();
        mDataRepository->GetDoseManager()->UpdateDoseGrid(grid, start, axis, spacing, dim, bCreatBuffer);
        return true;
}

void TpsUpdateGridBoundaryCMD::MovePoint(Mcsf::Point3f& point, float offset) {
    point.SetX(point.GetX() + offset);
    point.SetY(point.GetY() + offset);
    point.SetZ(point.GetZ() + offset);
}

void TpsUpdateGridBoundaryCMD::UpdateRelatedVOIInGrid( const std::string &planUid, RtDosegrid* dosegrid )
{
    TpsPatientManager   *patientManager  = mDataRepository->GetPatientManager();
    TpsVOIManager       *voiManager      = mDataRepository->GetVoiManager();
    TpsImageDataManager *imageManager    = mDataRepository->GetImageDataManager();

    RtPlan* pPlan = nullptr;
    if(!patientManager->GetPlan(planUid, &pPlan) || pPlan == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The plan does not exist, uid: "<<planUid;
        return;
    }

    std::string seriesUid = pPlan->get_imageseriesuid();
    TpsImage3DEntity *image3DEntity = nullptr; 
    if (!imageManager->GetImage3DData(seriesUid, &image3DEntity)
        || image3DEntity == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get related image 3d data of plan: "<<planUid;
        return;
    }
    
    std::vector<ROIEntity*> rois = voiManager->GetROIBySeries(seriesUid);
    for(int i = 0; i < rois.size(); ++i)
    {
        if (rois[i]->IsEmpty()) continue;

        float inGrid = SubVolumeHelperAPI::CalculateInGridPercentage(*image3DEntity, rois[i], dosegrid);
        rois[i]->SetInGrid(inGrid);
    }
}

TPS_END_NAMESPACE