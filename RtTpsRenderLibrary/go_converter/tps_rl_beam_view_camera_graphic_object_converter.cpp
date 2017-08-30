////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_beam_view_camera_graphic_object_converter.cpp
/// 
///  \brief class BeamViewCameraGoConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/29
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_beam_view_camera_graphic_object_converter.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

using namespace Mcsf::MedViewer3D;
#define DEFAULT_FOV_SIZE 400 //mm
#define DEFAULT_SAD 1000 //mm

TPS_BEGIN_NAMESPACE   // begin namespace tps

BeamViewCameraGoConverter::BeamViewCameraGoConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mPlanUID(""), mImageUID(""), mViewType(BEV_VIEW){

}

BeamViewCameraGoConverter::~BeamViewCameraGoConverter(){

}

GraphicObjectPtr BeamViewCameraGoConverter::CreateGraphicObject_i(){

    return std::shared_ptr<CameraGraphicObject>(new CameraGraphicObject());
}

bool BeamViewCameraGoConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end())
    {
        TPS_LOG_DEV_ERROR<<"Can not find the PLAN data source.";
        return false;
    }

    mImageUID = "";
    mPlanUID = it->second;
    if (mPlanUID.empty()) return false;
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(mPlanUID, &plan) || plan == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<mPlanUID;
        return false;
    }
    mImageUID = plan->get_imageseriesuid();
    if (mImageUID.empty()){
        TPS_LOG_DEV_ERROR<<"The plan has no connected series.";
        return false;
    }

    mViewType = mDataRepository->GetDisplayStateManager()->GetBevViewType();
    return true;
}

bool BeamViewCameraGoConverter::Update_i(GraphicObjectPtr go)
{
    auto bevCamera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (bevCamera == nullptr) return false;

    auto volumeGO = mModelWarehouse->GetModelObject(
        mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(volumeGO);
    if(nullptr == volume) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go";
        return false;
    }

    SetupDefaultCameraLocation_i(volume, bevCamera);
    if (mPlanUID.empty()) return true;

    auto goTmp = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(goTmp);
    if(nullptr == goNGCollection) {
        TPS_LOG_DEV_WARNING<<"Failed to get normal group collection";
        return true;
    }
    std::string activeNGUID = goNGCollection->GetActiveNormalGroupUID();
    if (activeNGUID.empty()) return true;
    auto activeNG = goNGCollection->GetNormalGroup(activeNGUID);
    if(nullptr == activeNG) {
        TPS_LOG_DEV_ERROR<<"Failed to get normal group!";
        return false;
    }
    const double* fieldLimits = activeNG->GetFieldLimits();
    std::shared_ptr<TpsBeamGraphicObject> beamGo = activeNG->GetActiveBeamGo();
    if(nullptr == beamGo) return true;
    auto activeSegment = beamGo->GetActiveSegmentGo();
    if (activeSegment == nullptr) return true;

    Mcsf::MedViewer3D::Matrix4x4 matWorld2Pat = volume->GetWorld2Patient();
    Mcsf::MedViewer3D::Matrix4x4 matBeam2Pat;
    activeSegment->GetStartCollimatorToPatMatrix(&matBeam2Pat);
    Mcsf::MedViewer3D::Matrix4x4 matB2W = matBeam2Pat;
    matB2W.Prepend(matWorld2Pat.Inverse());

    Mcsf::MedViewer3D::Point3D lookAtBeam(0.0,0.0,-1000.0);
    Mcsf::MedViewer3D::Point3D eyeBeam(0.0,0.0,0.0);
    Mcsf::MedViewer3D::Point3D upBeam(0.0,1.0,-1000.0);

    Mcsf::MedViewer3D::Point3D eyeWorld = matB2W.Transform(eyeBeam);
    Mcsf::MedViewer3D::Point3D lookAtWorld = matB2W.Transform(lookAtBeam);
    Mcsf::MedViewer3D::Vector3D upWorld = matB2W.Transform(upBeam - lookAtBeam);
    upWorld.Normalize();
    bevCamera->SetEye(eyeWorld);
    bevCamera->SetLookAt(lookAtWorld);
    bevCamera->SetUpDirection(upWorld);

    Mcsf::MedViewer3D::Quaternion myQuaternion;
    //BEV时摄像机位于Gantry上，在Source点，不随Collimator而改变
    if (BEV_VIEW == mViewType){
        double collimateAngle = activeSegment->GetCollimatorAngle() * PI / 180;
        myQuaternion = Mcsf::MedViewer3D::Quaternion(collimateAngle, 
            Mcsf::MedViewer3D::Vector3D(0, 0, -1));
        bevCamera->Rotate(myQuaternion);
    }

    double distance = (eyeWorld - lookAtWorld).Magnitude();
    bevCamera->SetProjectionType(Perspective);
    bevCamera->SetNearClipDistance(0.1 * distance);
    bevCamera->SetFarClipDistance(2 * distance + 3.464);
    double fieldSize = TPS_MAX(fieldLimits[1] - fieldLimits[0], 
        fieldLimits[3] - fieldLimits[2]);
    fieldSize = matB2W.Transform(Vector3D(fieldSize, 0.0, 0.0)).Magnitude();
    double aspect = bevCamera->GetAspectRatio();
    double height = aspect >= 1.0 ? fieldSize : fieldSize / aspect;
    bevCamera->SetFovY(atan(height * 0.5 / distance) * 2);
    return true;
}

void BeamViewCameraGoConverter::SetupDefaultCameraLocation_i(
    const std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>& volume,
    const std::shared_ptr<CameraGraphicObject>& camera)
{
    Matrix4x4 matPatient2World = volume->GetWorld2Patient().Inverse();
    double ratio = matPatient2World.Transform(Vector3D(1.0, 0.0, 0.0)).Magnitude();

    double distance = ratio * DEFAULT_SAD;
    camera->SetEye(Point3D(0.0, -distance, 0.0));
    camera->SetLookAt(Point3D(0.0, 0.0, 0.0));
    camera->SetUpDirection(Vector3D(0.0, 0.0, 1.0));

    camera->SetProjectionType(Perspective);
    camera->SetNearClipDistance(0.1 * distance);
    camera->SetFarClipDistance(2 * distance + 3.464);
    double field = ratio * DEFAULT_FOV_SIZE;
    double aspect = camera->GetAspectRatio();
    double height = aspect >= 1.0 ? field : field / aspect;
    camera->SetFovY(atan(height * 0.5 / distance) * 2);
}
TPS_END_NAMESPACE  // end namespace tps
