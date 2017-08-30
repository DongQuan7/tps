////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  wenlong zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_deleteoperation.cpp
/// 
///  \brief delete 
/// 
///  \version 1.0
/// 
///  \date    2015/07/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_easyplan_deleteisocenter_operation.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE

    TpsEasyPlanDeleteIsocenterOperation::TpsEasyPlanDeleteIsocenterOperation(const std::string& strImageUid, const std::string& strPlanUid, const std::string &strPoiUid):
m_strImageUid(strImageUid), m_strPlanUid(strPlanUid), m_strPoiUid(strPoiUid) {

}

TpsEasyPlanDeleteIsocenterOperation::~TpsEasyPlanDeleteIsocenterOperation() {

}

bool TpsEasyPlanDeleteIsocenterOperation::ModifyGraphicObject() const {

    std::string key = m_strPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_POI_COLLECTION);
    auto goCollection = mModelWarehouse->GetModelObject(key);
    auto poiCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goCollection);
    if(goCollection == nullptr || poiCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get poi collection of image!"<<m_strImageUid;
        return false;
    }
    poiCollection->DeletePoi(m_strPoiUid);
    poiCollection->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE