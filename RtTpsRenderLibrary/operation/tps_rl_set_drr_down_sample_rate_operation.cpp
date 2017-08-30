////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_set_drr_down_sample_rate_operation.cpp
/// 
///  \brief class SetDrrDownSampleRate implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_set_drr_down_sample_rate_operation.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

SetDrrDownSampleRateOperation::SetDrrDownSampleRateOperation(float downSampleRate)
: mDownSampleRate(downSampleRate){

}

SetDrrDownSampleRateOperation::~SetDrrDownSampleRateOperation(){

}

bool SetDrrDownSampleRateOperation::ModifyGraphicObject() const{
    std::string key = COMMON_BASE + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_DISPLAY_CONTROL);
    auto go = mModelWarehouse->GetModelObject(key);
    auto stateGO = std::dynamic_pointer_cast<StateControlGraphicObject>(go);
    if(nullptr == stateGO) {
        TPS_LOG_DEV_ERROR<<"Failed to get state go!";
        return false;
    }

    stateGO->SetDownSampleRate(mDownSampleRate);
    stateGO->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps