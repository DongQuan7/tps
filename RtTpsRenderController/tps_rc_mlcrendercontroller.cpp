//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rc_mlcrendercontroller.cpp 
///  \brief   render mlc only
///
///  \version 1.0
///  \date  2015/02/04  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_mlcrendercontroller.h"

//ZhengHe
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_simulator_mlcrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"

#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"


using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE
TpsMLCRenderController::TpsMLCRenderController(WINDOW_TYPE type) :
TpsRenderController(type){

}

TpsMLCRenderController::~TpsMLCRenderController() {
}

bool TpsMLCRenderController::SetupScene_i() {
    try 
    {
        mMLCRenderStrategy.reset(new TpsSimulatorMLCRenderStategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mMLCRenderStrategy);

        return true;
    }
    catch(std::exception &ex) {
        TPS_LOG_DEV_ERROR<<"Failed to setup scene:" <<ex.what();
        return false;
    }
}

bool TpsMLCRenderController::ResetDataSource_i(DATA_SOURCE_ID id){

    bool bRes = false;
    switch (id) {
    case IMAGE:
        //bRes = ResetImage_i();
        break;
    case PLAN:
        bRes = ResetPlan_i();
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unrecognized data source id: "<<id;
        break;
    }
    return bRes;
}

//bool TpsMLCRenderController::ResetImage_i() {
//    //std::string imageUID = mDataSourceMap[IMAGE];
//
//    return true;
//}

bool TpsMLCRenderController::ResetPlan_i(){
    std::string planUID = mDataSourceMap[PLAN];

    //check normal group collection graphic object
    IGraphicObjectPtr go = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_NORMALGROUP_COLLECTION, go)){
        TPS_LOG_DEV_ERROR<<"Failed to check normal group collection of plan: "<<planUID;
        return false;
    }

    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);

    if (goNGCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    mMLCRenderStrategy->SetNormalGroupCollection(goNGCollection);
    return true;
}

void TpsMLCRenderController::RemoveDataSource_i(DATA_SOURCE_ID id){

    switch (id) {
    case IMAGE:
        break;
    case PLAN:
        mMLCRenderStrategy->SetNormalGroupCollection(nullptr);
        break;
    default:
        break;
    }
}

bool TpsMLCRenderController::GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY) {

    //临时方法，待修改
    spacingX = 10;
    spacingY = 10;
    return true;
    //check data source
    /*if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The image data source is not ready.";
        return false;
    }
    if (mDataSourceMap.find(PLAN) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The plan data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    std::string planUID = mDataSourceMap[PLAN];
    //lock model warehouse
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    //get volume graphic object, get world 2 patient TM
    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if(volume == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go!"<<imageUID;
        return false;
    }

    Matrix4x4 matW2P = volume->GetWorld2Patient();
    //get active segment graphic object, get collimator 2 patient TM
    go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if(nullptr == goNGCollection) {
        TPS_LOG_DEV_ERROR<<"Failed to get normal group collection";
        return false;
    }

    std::string activeNGUID = goNGCollection->GetActiveNormalGroupUID();
    if (activeNGUID.empty()) return true;
    auto activeNG = goNGCollection->GetNormalGroup(activeNGUID);
    if(nullptr == activeNG) {
        TPS_LOG_DEV_ERROR<<"There is no active normal group graphic object.";
        return false;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGo = activeNG->GetActiveBeamGo();
    if(nullptr == beamGo){
        TPS_LOG_DEV_ERROR<<"There is no active beam graphic object.";
        return false;
    }
    std::shared_ptr<TpsSegmentGraphicObject> activeSegment = beamGo->GetActiveSegmentGo();
    if (nullptr == activeSegment){
        TPS_LOG_DEV_ERROR<<"There is no active segment graphic object.";
        return false;
    }
    Mcsf::MedViewer3D::Matrix4x4 matC2P;
    activeSegment->GetStartCollimatorToPatMatrix(&matC2P);
    //get camera graphic object, translate screen point 2 view c.s. & get world 2 view TM
    go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera){
        TPS_LOG_DEV_ERROR<<"The BEV camera does not exist.";
        return false;
    }

    //transform point from camera model to collimator c.s.
    int width, height;
    mView->GetDisplaySize(width, height);
    camera->AdaptFovViaAspect((double)width / height);
    const Mcsf::MedViewer3D::Point3D* corners = camera->GetWorldFrustumCorners();
    double widthInMM = matW2P.Transform(corners[1] - corners[0]).Magnitude();
    Point3D eye = camera->GetEye();
    Point3D lookAt = camera->GetLookAt();
    double isoDist = (eye - lookAt).Magnitude();
    double nearDist = camera->GetNearClipDistance();
    widthInMM *= isoDist / nearDist;
    widthInMM = widthInMM <= 1e-6 ? 1e-6 : widthInMM;
    spacingY = spacingX = width / widthInMM * 10;

    ///handle zoom factor
    double zoomFactor = camera->GetZoomFactor();
    spacingX /= zoomFactor;
    spacingY /= zoomFactor;
    return true;*/
}

void TpsMLCRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == PLAN){
        RemoveGraphicObject_i(uid, GO_TYPE_NORMALGROUP_COLLECTION);
    }
}
TPS_END_NAMESPACE