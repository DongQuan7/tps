////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang mailto:qiangqiang.zhou@united-imaing.com
/// 
///  \file tps_rl_setdrrvisibility_operation.h
/// 
///  \brief class SetDrrVisibilityOperation 
/// 
///  \version 1.0
/// 
///  \date    Apr.16, 2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_setdrrvisibility_operation.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE

SetDrrVisibilityOperation::SetDrrVisibilityOperation(bool bVisibility) : mVisibility(bVisibility) 
{
}

SetDrrVisibilityOperation::~SetDrrVisibilityOperation() {

}

bool SetDrrVisibilityOperation::ModifyGraphicObject() const
{

    std::string key = COMMON_BASE + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_DISPLAY_CONTROL);
    auto stateGO = mModelWarehouse->GetModelObject(key);
    auto go = dynamic_pointer_cast<StateControlGraphicObject>(stateGO);
    if(nullptr == stateGO || go == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get state go!";
        return false;
    }
    go->SetDrrVisibility(mVisibility);
    go->SetDirty(true);

    return true;
}



TPS_END_NAMESPACE

