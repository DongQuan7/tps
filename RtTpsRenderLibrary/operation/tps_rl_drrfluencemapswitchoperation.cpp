////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_drrfluencemapswitchoperation.h
/// 
///  \brief class DRRFluenceMapSwitch 
/// 
///  \version 1.0
/// 
///  \date    2014/07/21
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_drrfluencemapswitchoperation.h"

#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

TPS_BEGIN_NAMESPACE

DRRFluenceMapSwitch::DRRFluenceMapSwitch(
const std::string& imageUID, bool flag) : mImageUID(imageUID), mFlag(flag) {

}

DRRFluenceMapSwitch::~DRRFluenceMapSwitch() {

}

bool DRRFluenceMapSwitch::ModifyGraphicObject() const {

    std::string key = COMMON_BASE + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_DISPLAY_CONTROL);
    auto stateGO = mModelWarehouse->GetModelObject(key);
    auto go = dynamic_pointer_cast<StateControlGraphicObject>(stateGO);
    if(nullptr == stateGO || go == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get state go!";
        return false;
    }
    if(mFlag) {
        go->SetType(FLUENCE_MAP);
    }
    else {
        go->SetType(DRR);
    }
    go->SetDirty(true);
    return true;
}



TPS_END_NAMESPACE

