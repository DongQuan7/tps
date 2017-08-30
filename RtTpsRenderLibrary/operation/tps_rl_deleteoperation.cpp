////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_deleteoperation.cpp
/// 
///  \brief delete 
/// 
///  \version 1.0
/// 
///  \date    2014/3/07
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_deleteoperation.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"

#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"

#include "boost/lexical_cast.hpp"


TPS_BEGIN_NAMESPACE
TpsDeleteVOIOperation::TpsDeleteVOIOperation(const std::string &imageUid,  const std::string &uid) :
    mImageUid(imageUid), mVoiUid(uid) {

}

TpsDeleteVOIOperation::~TpsDeleteVOIOperation() {

}

bool TpsDeleteVOIOperation::ModifyGraphicObject() const
{
    std::string key = mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOI_COLLECTION);
    auto goCollection = mModelWarehouse->GetModelObject(key);
    auto voiCollection = dynamic_pointer_cast<TpsVOIGOCollection>(goCollection);
    if(goCollection == nullptr || voiCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi collection of image!"<<mImageUid;
        return false;
    }

    voiCollection->RemoveVOI(mVoiUid);
    voiCollection->SetDirty(true);
    return true;
}


TpsDeletePOIOperation::TpsDeletePOIOperation(const std::string& strSeriesUid, const std::string &strPoiUid):
 m_strSeriesUid(strSeriesUid), m_strPoiUid(strPoiUid) {
     
}

TpsDeletePOIOperation::~TpsDeletePOIOperation() {

}

bool TpsDeletePOIOperation::ModifyGraphicObject() const 
{
    if (m_strSeriesUid.empty()){
        TPS_LOG_DEV_ERROR<<"Failed to get series uid when running delete poi operation!";
        return false;
    }
    std::string key = m_strSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_POI_COLLECTION);
    auto goCollection = mModelWarehouse->GetModelObject(key);
    auto poiCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goCollection);
    if(goCollection == nullptr || poiCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get poi collection of series:"<<m_strSeriesUid;
        return false;
    }
    poiCollection->DeletePoi(m_strPoiUid);
    poiCollection->SetDirty(true);
    return true;
}

TpsDeleteMaxPointOperation::TpsDeleteMaxPointOperation(const std::string& strPlanUid, const std::string& strPoiUid):
m_strPlanUid(strPlanUid), m_strPoiUid(strPoiUid) {

}

TpsDeleteMaxPointOperation::~TpsDeleteMaxPointOperation() {

}

bool TpsDeleteMaxPointOperation::ModifyGraphicObject() const 
{
    if (m_strPlanUid.empty()){
        TPS_LOG_DEV_ERROR<<"Failed to get plan uid when running delete poi operation!";
        return false;
    }
    std::string key = m_strPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_MAX_POINT_COLLECTION);
    auto goCollection = mModelWarehouse->GetModelObject(key);
    auto poiCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goCollection);
    if(goCollection == nullptr || poiCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get poi collection of plan:"<<m_strPlanUid;
        return false;
    }
    auto poiGo = poiCollection->SelectPoi(m_strPoiUid);
    if(nullptr == poiGo) {
        TPS_LOG_DEV_INFO << "TpsDeleteMaxPointOperation failed to get max poi of Plan:" <<m_strPlanUid << " MaxType:" <<m_strPoiUid;
        return true;
    }
    poiCollection->DeletePoi(m_strPoiUid);
    poiCollection->SetDirty(true);
    return true;
}

TpsDeleteBeamOperation::TpsDeleteBeamOperation(const std::string& normGroupUid , 
    const std::string &beamUid): mNormGroupUid(normGroupUid),mBeamUid(beamUid) {

}

TpsDeleteBeamOperation::~TpsDeleteBeamOperation() {

}

bool TpsDeleteBeamOperation::ModifyGraphicObject() const {

    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(mNormGroupUid, &normGroup) && normGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"The plan does not exist which uid: "<<mNormGroupUid;
        return false;
    }
    std::string planUID = normGroup->get_planuid();

    auto go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<mNormGroupUid;
        return false;
    }

    auto beamCollection = ngCollection->GetNormalGroup(mNormGroupUid);
    if (beamCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get beam collection of plan: "<<mNormGroupUid;
        return false;
    }

    auto activeBeam = beamCollection->GetActiveBeamGo();
    auto beamToBeDeleted = beamCollection->GetBeamMap()[mBeamUid];
    bool needResetCamera = 
        ngCollection->GetActiveNormalGroupUID() == mNormGroupUid && 
        activeBeam == beamToBeDeleted;

    beamCollection->DeleteBeam(mBeamUid);
    beamCollection->SetDirty(true);

    if (needResetCamera){
        auto go = mModelWarehouse->GetModelObject(
            planUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
        auto cameraForBEV = std::dynamic_pointer_cast<CameraGraphicObject>(go);
        if (cameraForBEV == nullptr){
            TPS_LOG_DEV_WARNING<<"Failed to get the bev camera, \
            may because there is no BEV window, so camera GO has never been created, \
            or unknown error happens.";
            return true;
        }

        GOConverterParameter para;
        para.data_source_map[PLAN] = planUID;
        auto converter = mConverterFactory->GetConverter(GO_TYPE_BEV_CAMERA);
        if (!converter->Update(para, cameraForBEV)){
            TPS_LOG_DEV_ERROR<<"Failed to update bev camera.";
            return false;
        }
        cameraForBEV->SetDirty(true);
    }
    return true;
}

TpsDeleteNormGroupOperation::TpsDeleteNormGroupOperation(const std::string& planUid , 
    const std::string &ngUid): mPlanUid(planUid), mNormGroupUid(ngUid) {

}

TpsDeleteNormGroupOperation::~TpsDeleteNormGroupOperation() {

}

bool TpsDeleteNormGroupOperation::ModifyGraphicObject() const {
    if (mPlanUid.empty() || mNormGroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"The input parameter is invalid.";
        return false;
    }

    auto go = mModelWarehouse->GetModelObject(
        mPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<mPlanUid;
        return false;
    }
    ngCollection->RemoveNormalGroup(mNormGroupUid);
    return true;
}
TPS_END_NAMESPACE
