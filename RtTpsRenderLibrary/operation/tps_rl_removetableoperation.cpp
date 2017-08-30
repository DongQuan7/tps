////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_removetableoperation.cpp
/// 
///  \brief class RemoveTableOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/04
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_removetableoperation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_manualremovetablegraphicobject.h"


TPS_BEGIN_NAMESPACE
RemoveTableOperation::RemoveTableOperation(bool isVisible, float height, std::string seriesUid) :
mIsVisible(isVisible),mHeight(height), mSeriesUid(seriesUid) {

}

RemoveTableOperation::~RemoveTableOperation() {

}

bool RemoveTableOperation::ModifyGraphicObject() const {

    auto go = mModelWarehouse->GetModelObject(
        mSeriesUid +"|"+ GOTypeHelper::ToString(GO_TYPE_MANUL_REMOVE_TABLE));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get remove table go!";
        return false;
    }
    auto removeTableGO = std::dynamic_pointer_cast<ManualRemoveTableGraphicObject>(go);
    if(nullptr == removeTableGO) {
        TPS_LOG_DEV_ERROR<<"failed to get remove table go!";
        return false;
    }
    removeTableGO->SetVisibilty(mIsVisible);
    removeTableGO->SetHeight(mHeight);
    removeTableGO->SetDirty(true);
    return true;
}



TPS_END_NAMESPACE