////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZUO Kai kai.zuo@united-imaging.com
/// 
///  \file tps_rl_showdosegridboundaryoperation.cpp
/// 
///  \brief class TpsShowDoseGridBoundaryOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/11
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_showdosegridboundaryoperation.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsShowDoseGridBoundaryOperation::TpsShowDoseGridBoundaryOperation(
    const std::string planUID, bool isShowBoundary) : mPlanUID(planUID),
    mIsShowBoundary(isShowBoundary){

}

TpsShowDoseGridBoundaryOperation::~TpsShowDoseGridBoundaryOperation(){

}

bool TpsShowDoseGridBoundaryOperation::ModifyGraphicObject() const{
    std::string doseGridBoundaryKey = mPlanUID + "|" + 
        GOTypeHelper::ToString(GO_TYPE_DOSE_GRID_BOUNDARY);
    auto go = mModelWarehouse->GetModelObject(doseGridBoundaryKey);
    auto doseGridBoundaryGo = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(go);
    if (doseGridBoundaryGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid boundary go with key: "
            <<doseGridBoundaryKey;
        return false;
    }
    doseGridBoundaryGo->SetType("DOSEGRID");
    doseGridBoundaryGo->SetVisible(mIsShowBoundary);
    doseGridBoundaryGo->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps