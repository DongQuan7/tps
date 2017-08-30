////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_switch_bev_dispay_mode_operation.cpp
/// 
///  \brief class SwitchBevDisplayMode implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_switch_bev_dispay_mode_operation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

SwitchBevDisplayModeOperation::SwitchBevDisplayModeOperation(int bevDisplayMode)
: mBevDisplayMode(bevDisplayMode){

}

SwitchBevDisplayModeOperation::~SwitchBevDisplayModeOperation(){

}

bool SwitchBevDisplayModeOperation::ModifyGraphicObject() const{
    std::string key = COMMON_BASE + "|" + GOTypeHelper::ToString(
        GO_TYPE_BEV_DISPLAY_CONTROL);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto stateControlGo = std::dynamic_pointer_cast<StateControlGraphicObject>(go);
    if (stateControlGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get state control graphic object.";
        return false;
    }
    stateControlGo->SetType((BEV_DISPLAY_TYPE)mBevDisplayMode);
    stateControlGo->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps