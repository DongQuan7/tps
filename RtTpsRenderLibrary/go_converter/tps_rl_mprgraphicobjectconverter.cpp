////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprgraphicobjectconverter.cpp
/// 
///  \brief class TpsMPRGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobjectconverter.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsMPRGOConverter::TpsMPRGOConverter(GO_TYPE goType)
: TpsGOConverterBase(goType), mVolume(nullptr)
{
    //default view settings
    mGOSectionToPatienCSAxisMap[AXIAL] = H2F;
    mGOSectionToPatienCSAxisMap[CORONAL] = A2P;
    mGOSectionToPatienCSAxisMap[SAGITTAL] = L2R;

    mVecRightMap[F2H] = R2L;
    mVecRightMap[H2F] = L2R;
    mVecRightMap[L2R] = A2P;
    mVecRightMap[R2L] = P2A;
    mVecRightMap[A2P] = R2L;
    mVecRightMap[P2A] = L2R;

    mVecUpMap[F2H] = P2A;
    mVecUpMap[H2F] = P2A;
    mVecUpMap[L2R] = F2H;
    mVecUpMap[R2L] = F2H;
    mVecUpMap[A2P] = F2H;
    mVecUpMap[P2A] = F2H;
}

TpsMPRGOConverter::~TpsMPRGOConverter()
{

}

GraphicObjectPtr TpsMPRGOConverter::CreateGraphicObject_i()
{
    return std::shared_ptr<MPRGraphicObject>(new MPRGraphicObject());
}

bool TpsMPRGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mVolume = nullptr;
    std::string volumeKey = it->second + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetModelObject(volumeKey);
    mVolume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (mVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][PrepareSourceData_i]: "
            <<"The volume data with UID: "<<it->second<<" does not exist.";
        return false;
    }

    TpsImage3DEntity* imageData = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(it->second, &imageData)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data which UID: "<<it->second;
        return false;
    }
    RtSeries* series = imageData->GetRtSeries();
    if (series == nullptr){
        TPS_LOG_DEV_ERROR<<"The series of image 3d is empty.";
        return false;
    }

    auto pp = (PatientPosition)ToPatientPosition_i(
        series->get_patientposition());
    switch(pp)
    {
    case HFS:
        mGOSectionToPatienCSAxisMap[AXIAL] = F2H;
        mVecRightMap[F2H] = R2L;
        mVecUpMap[F2H] = P2A;
        break;
    case HFP:
        mGOSectionToPatienCSAxisMap[AXIAL] = F2H;
        mVecRightMap[F2H] = L2R;
        mVecUpMap[F2H] = A2P;
        break;
    case HFDR:
        mGOSectionToPatienCSAxisMap[AXIAL] = F2H;
        mVecRightMap[F2H] = A2P;
        mVecUpMap[F2H] = R2L;
        break;
    case HFDL:
        mGOSectionToPatienCSAxisMap[AXIAL] = F2H;
        mVecRightMap[F2H] = P2A;
        mVecUpMap[F2H] = L2R;
        break;
    case FFS:
        mGOSectionToPatienCSAxisMap[AXIAL] = H2F;
        mVecRightMap[H2F] = L2R;
        mVecUpMap[H2F] = P2A;
        break;
    case FFP:
        mGOSectionToPatienCSAxisMap[AXIAL] = H2F;
        mVecRightMap[H2F] = R2L;
        mVecUpMap[H2F] = A2P;
        break;
    case FFDR:
        mGOSectionToPatienCSAxisMap[AXIAL] = H2F;
        mVecRightMap[H2F] = P2A;
        mVecUpMap[H2F] = R2L;
        break;
    case FFDL:
        mGOSectionToPatienCSAxisMap[AXIAL] = H2F;
        mVecRightMap[H2F] = A2P;
        mVecUpMap[H2F] = L2R;
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unknown patient position.";
        return false;
    }
    return true;
}

bool TpsMPRGOConverter::Update_i(GraphicObjectPtr go)
{
    auto mprGo = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mVolume == nullptr || mprGo == nullptr) return false;

    if (!mprGo->Initialize()){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][Update_i]: "
            <<"Failed to initialize MPR graphic object.";
        return false;
    }

    unsigned char colorArray[4 * 256];
    for (int i = 0; i < 256; i += 4){
        colorArray[i] = 162;          //r
        colorArray[i + 1] = 35;       //g
        colorArray[i + 2] = 221;      //b
        colorArray[i + 3] = 255;      //a
    }
    mprGo->AddMaskLut(1, colorArray, 256);
    mprGo->SetRenderAlgorithmType(ClsGraphicObjectMPR::MPR_ALGO_MIP);

    auto sectionType = GOTypeHelper::GetFirstPostfixComponent(mGOType);
    auto location = GOTypeHelper::GetSecondPostfixComponent(mGOType);
    mprGo->SetSectionType(sectionType);
    mprGo->SetLocation(location);

    //set window level
    double ww, wc;
    mVolume->GetDefaultWindowing(&ww, &wc);
    mprGo->SetWindowing(ww, wc);
    mprGo->SetColorInverted(mVolume->GetModality() == "PT");
    if (!PositionMPRPlane_i(mVolume, sectionType, location, mprGo)){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][Update_i]: "
            <<"Failed to position MPR plane.";
        return false;
    }
    return true;
}

//modify MPRAPI::PositionMPRPlane method in 3D render engine
bool TpsMPRGOConverter::PositionMPRPlane_i(
    std::shared_ptr<GraphicObjVolume> volume, 
    FIRST_POSTFIX_COMPONENT section, SECOND_POSTFIX_COMPONENT location,
    std::shared_ptr<MPRGraphicObject> mpr)
{
    try{
        size_t dim[3];
        volume->GetDimension(dim);
        double dDim[3] = {
            static_cast<double>(dim[0]),
            static_cast<double>(dim[1]),
            static_cast<double>(dim[2])};
        Vector3D vModelDeltaX(0.5 / dDim[0], 0.0, 0.0);
        Vector3D vModelDeltaY(0.0, 0.5 / dDim[1], 0.0);
        Vector3D vModelDeltaZ(0.0, 0.0, 0.5 / dDim[2]);

        // Transform the axis of model coordinate to world coordinate
        //model origin
        Matrix4x4 matM2W = volume->GetModel2World();
        //X
        Vector3D vModelX2World = matM2W.Transform(vModelDeltaX);
        double dModelDeltaX = vModelX2World.Magnitude();
        vModelX2World.Normalize();
        //Y
        Vector3D vModelY2World = matM2W.Transform(vModelDeltaY);
        double dModelDeltaY = vModelY2World.Magnitude();
        vModelY2World.Normalize();
        //Z
        Vector3D vModelZ2World = vModelX2World.CrossProduct(vModelY2World);
        Vector3D vDircZWorld = matM2W.Transform(Vector3D(0, 0, 1));
        if (vDircZWorld.DotProduct(vModelZ2World) < -1e10){
            vModelZ2World *= -1;
        }
        Vector3D vWorldDeltaZ = matM2W.Transform(vModelDeltaZ);
        double dModelDeltaZ = vWorldDeltaZ.Magnitude();

        // push z first because we should refer to the plane on the x-y plane of volume
        Vector3D vecModelAxis[3] = {
            vModelZ2World, vModelY2World, vModelX2World};
        double dModelAxisDelta[3] = {
            dModelDeltaZ, dModelDeltaY, dModelDeltaX};
        bool bNeedShift[3] = {dim[2] % 2 == 0, 
            dim[1] % 2 == 0, dim[0] % 2 == 0};

        // The vector representation of image orientation
        // Tag 0020,0037 Image Orientation
        // Refer to Dicom Standard Part 3: C.7.6.2.1.1
        std::map<MPR_VIEW_DIRECTION, Vector3D> mapPatientCSVector;
        mapPatientCSVector[R2L] = Vector3D(1.0, 0.0, 0.0);
        mapPatientCSVector[A2P] = Vector3D(0.0, 1.0, 0.0);
        mapPatientCSVector[F2H] = Vector3D(0.0, 0.0, 1.0);
        
        std::map<MPR_VIEW_DIRECTION, Vector3D> mapDestinationVector;
        std::map<MPR_VIEW_DIRECTION, double> mapDestinationDelta;
        std::map<MPR_VIEW_DIRECTION, bool> mapNeedShift;
        MPR_VIEW_DIRECTION dir, op_dir;
        double dotPrd, currentPrd;
        Vector3D modelAxis;
        for (int i=0; i<3; i++){
            dotPrd = 0.0;
            currentPrd = 0.0;
            auto itrMap = mapPatientCSVector.begin();
            for(; itrMap != mapPatientCSVector.end(); ++itrMap){
                currentPrd = itrMap->second.DotProduct(vecModelAxis[i]);
                if (std::abs(currentPrd) > std::abs(dotPrd)){
                    dotPrd = currentPrd;
                    dir = itrMap->first;
                }
            }
            //mapDefaultThickness[sDir] = aThickness[i];
            modelAxis = (dotPrd > 0.0) ? (vecModelAxis[i]) : -(vecModelAxis[i]);
            mapDestinationVector[dir] = modelAxis;
            mapDestinationDelta[dir] = dModelAxisDelta[i];
            mapNeedShift[dir] = bNeedShift[i];

            op_dir = (MPR_VIEW_DIRECTION)(dir ^ (0x01 << 2));
            mapDestinationVector[op_dir] = -1 * modelAxis;
            mapDestinationDelta[op_dir] = dModelAxisDelta[i];
            mapNeedShift[op_dir] = bNeedShift[i];
            // remove the position from the candidates in case
            // more than one planes correspond to the same position
            mapPatientCSVector.erase(dir);
        }

        auto it = mGOSectionToPatienCSAxisMap.find(section);
        if (it == mGOSectionToPatienCSAxisMap.end()){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][PositionMPRPlane_i]: "
                <<"MPR section type: "<<section<<" does not exist.";
            return false;
        }
        MPR_VIEW_DIRECTION viewDirt = it->second;
        Vector3D vecRight = mapDestinationVector[mVecRightMap[viewDirt]];
        Vector3D vecUp = mapDestinationVector[mVecUpMap[viewDirt]];
        double dDeltaGenerial = mapDestinationDelta[viewDirt];
        Vector3D vNormal = vecRight.CrossProduct(vecUp);
        if(!vNormal.Normalize()){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][PositionMPRPlane_i]: "
                <<"Failed to calculate normal vector of mpr section: "<<section;
            return false;
        }
        Vector3D vDelta = vNormal * (mapNeedShift[viewDirt] ? dDeltaGenerial : 0);

        Point3D ptDelta(vDelta.x, vDelta.y, vDelta.z);
        Point3D corner1(-vecRight.x - vecUp.x, -vecRight.y - vecUp.y, -vecRight.z - vecUp.z);
        corner1 += ptDelta;
        Point3D corner2(vecRight.x - vecUp.x, vecRight.y - vecUp.y, vecRight.z - vecUp.z);
        corner2 += ptDelta;
        Point3D corner3(vecRight.x + vecUp.x, vecRight.y + vecUp.y, vecRight.z + vecUp.z);
        corner3 += ptDelta;
        Point3D corner4(-vecRight.x + vecUp.x, -vecRight.y + vecUp.y, -vecRight.z + vecUp.z);
        corner4 += ptDelta;
        Point3D aPlane[4] = {corner1, corner2, corner3, corner4};

        double volumeRangeInModel[6] = { 
            0.5 / dDim[0], 1 - 0.5 / dDim[0], 
            0.5 / dDim[1], 1 - 0.5 / dDim[1], 
            0.5 / dDim[2], 1 - 0.5 / dDim[2]};

        Vector3D vecX = 0.5 * (aPlane[2] - aPlane[3]);
        Vector3D vecY = 0.5 * (aPlane[0] - aPlane[3]);
        Point3D centerPoint = aPlane[3] + vecX + vecY;
        Point3D middlePt = matM2W.Inverse().Transform(centerPoint);
        if (middlePt.x < volumeRangeInModel[0] || middlePt.x > volumeRangeInModel[1] || 
            middlePt.y < volumeRangeInModel[2] || middlePt.y > volumeRangeInModel[3] || 
            middlePt.z < volumeRangeInModel[4] || middlePt.z > volumeRangeInModel[5] ){
            TPS_LOG_DEV_WARNING<<"[WARNING][TpsMPRGOConverter][PositionMPRPlane_i]: "
                <<"The center point of the mpr plane is out of the range!";
            return false;
        }

        ////////////////////////////////////////////////////////////
        if(location != EMPTY_COMPONENT){
            auto matP2M = volume->GetModel2PatientMat().Inverse();
            auto matM2W = volume->GetModel2World();
            matP2M.Prepend(matM2W);

            int pageTurn = location == PREV ? 1: -1;
            for (int j=0; j<4; ++j){
                aPlane[j] = aPlane[j] + pageTurn * vNormal * dDeltaGenerial * -2;
            }

            Vector3D vecX = 0.5 * (aPlane[2] - aPlane[3]);
            Vector3D vecY = 0.5 * (aPlane[0] - aPlane[3]);
            Point3D ptCenter = aPlane[3] + vecX + vecY;
            Point3D ptCenterM = matM2W.Inverse().Transform(ptCenter);
            if (ptCenterM.x < volumeRangeInModel[0] || ptCenterM.x > volumeRangeInModel[1] || 
                ptCenterM.y < volumeRangeInModel[2] || ptCenterM.y > volumeRangeInModel[3] || 
                ptCenterM.z < volumeRangeInModel[4] || ptCenterM.z > volumeRangeInModel[5] ){
                TPS_LOG_DEV_WARNING<<"[WARNING][TpsMPRGOConverter][PositionMPRPlane_i]: "
                    <<"The center point of the mpr plane is out of the range!";
                return false;
            }
        }

        MPRAPI::CalculateMPRMatrix(mpr, aPlane);

        double dMinThicknessW;
        TpsMprHelper::GetMinThickness(volume, mpr, dMinThicknessW);
        mpr->SetThickness(dMinThicknessW);
        return true;
    }
    catch (const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsMPRGOConverter][PositionMPRPlane_i]: "
            <<"Exception happens when position MPR section: "<<section
            <<". Information: "<<ex.what();
        return false;
    }
}

int TpsMPRGOConverter::ToPatientPosition_i(std::string sPP)
{
    boost::to_upper(sPP);
    PatientPosition pp = UNKNOWN_POSITION;
    if(sPP == "HFS")       { pp = HFS;  }
    else if(sPP == "HFP" ) { pp = HFP;  }
    else if(sPP == "HFP" ) { pp = HFP;  }
    else if(sPP == "HFDR") { pp = HFDR; }
    else if(sPP == "HFDL") { pp = HFDL; }
    else if(sPP == "FFS" ) { pp = FFS;  }
    else if(sPP == "FFP" ) { pp = FFP;  }
    else if(sPP == "FFDR") { pp = FFDR; }
    else if(sPP == "FFDL") { pp = FFDL; }
    return pp;
}
TPS_END_NAMESPACE
