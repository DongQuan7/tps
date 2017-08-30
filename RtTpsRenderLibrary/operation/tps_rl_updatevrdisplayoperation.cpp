////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatevrdisplayoperation.cpp
/// 
///  \brief UpdateVRDisplayOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/30
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_updatevrdisplayoperation.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE

UpdateVRDisplayOperation::UpdateVRDisplayOperation(float transparency, float maxCtValue,
   float minCtValue) : mMaxValue(maxCtValue), mMinValue(minCtValue), mTransparency(transparency) {

}

UpdateVRDisplayOperation::~UpdateVRDisplayOperation() {

}

bool UpdateVRDisplayOperation::ModifyGraphicObject() const {
    auto vrGo = mModelWarehouse->GetModelObject(
        COMMON_BASE + "|"+ GOTypeHelper::ToString(GO_TYPE_VR));
    auto vr = dynamic_pointer_cast<ShadedSurfaceGraphicObject>(vrGo);
    if(nullptr == vrGo || nullptr == vr) {
        TPS_LOG_DEV_ERROR<<"failed to get vr go";
        return false;
    }
    vr->SetTransParency(mTransparency);
    vr->SetDisplayValue(mMinValue,mMaxValue);
    vr->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE

