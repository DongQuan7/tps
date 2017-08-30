////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_bevdisplayoperation.cpp
/// 
///  \brief class BEVWindowOperation
/// 
///  \version 1.0
/// 
///  \date    2014/5/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_bevdisplayoperation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"

#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE

BEVPanOperation::BEVPanOperation(const Mcsf::Point2f &pre, 
const Mcsf::Point2f &cur, const std::string& planUID, DISPLAY_SIZE displaySize):
    mPreCoord(pre),mCurCoord(cur),mPlanUID(planUID), mDisplaySize(displaySize){

}

BEVPanOperation::~BEVPanOperation() {

}

bool BEVPanOperation::ModifyGraphicObject() const {
    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera){
        TPS_LOG_DEV_ERROR<<"The BEV camera does not exist.";
        return false;
    }

    const Point3D* corners = camera->GetWorldFrustumCorners();
    double offsetX = mPreCoord.GetX() - mCurCoord.GetX();
    double offsetY = mPreCoord.GetY() - mCurCoord.GetY();
    offsetX *= (corners[1] - corners[0]).Magnitude();
    offsetY *= (corners[3] - corners[0]).Magnitude();

    double currentOffsetX, currentOffsetY;
    camera->GetFrustumOffset(currentOffsetX, currentOffsetY);
    offsetX += currentOffsetX;
    offsetY += currentOffsetY;
    camera->SetFrustumOffset(offsetX, offsetY);
    camera->SetDirty(true);
    return true;
}

BEVZoomOperation::BEVZoomOperation(float zoomFactor, 
    const std::string& imageUID, const std::string& planUID):
    mZoomFactor(zoomFactor), mImageUID(imageUID), mPlanUID(planUID){

}

BEVZoomOperation::~BEVZoomOperation() {

}

bool BEVZoomOperation::ModifyGraphicObject() const {

    //////////////////////////////////////////////////////////////////////////
    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera){
        TPS_LOG_DEV_ERROR<<"The BEV camera does not exist.";
        return false;
    }
    
    camera->Zoom(mZoomFactor);
    camera->SetDirty(true);
    return true;
}

SwitchBEVCEVOperation::SwitchBEVCEVOperation(const std::string& planUID) : mPlanUID(planUID){

}

SwitchBEVCEVOperation::~SwitchBEVCEVOperation() {

}

bool SwitchBEVCEVOperation::ModifyGraphicObject() const {
    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera){
        TPS_LOG_DEV_ERROR<<"The BEV camera does not exist.";
        return false;
    }

    go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if(nullptr == go || nullptr == goNGCollection) {
        TPS_LOG_DEV_ERROR<<"Failed to get normal group collection";
        return false;
    }
    std::string activeNGUID = goNGCollection->GetActiveNormalGroupUID();
    auto activeNG = goNGCollection->GetNormalGroup(activeNGUID);
    if(nullptr == activeNG) {
        TPS_LOG_DEV_ERROR<<"There is no active normal group!";
        return false;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGo = activeNG->GetActiveBeamGo();
    if(nullptr == beamGo) {
        TPS_LOG_DEV_ERROR<<"beamGo is null !";
        return false;
    }
    std::shared_ptr<TpsSegmentGraphicObject> segment = nullptr;
    segment = beamGo->GetActiveSegmentGo();
    if(nullptr == segment) {
        TPS_LOG_DEV_ERROR<<"segment do not exist!";
        return false;
    }

    BEV_VIEW_TYPE viewtype = mDataRepository->GetDisplayStateManager()->GetBevViewType();
    double collimateAngle = segment->GetCollimatorAngle() * PI / 180 ;
    Mcsf::MedViewer3D::Vector3D vec(0,0,1);
    if (viewtype == BEV_VIEW) vec *= -1;
    Mcsf::MedViewer3D::Quaternion myQuaternion(collimateAngle,vec);
    camera->Rotate(myQuaternion);
    camera->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE
