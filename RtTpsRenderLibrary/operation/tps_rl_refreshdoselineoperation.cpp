////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_rl_rereshdoselineoperation.h
/// 
///  \brief class TpsRefreshDoseLineOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/05
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoselineoperation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

//graphic object type defines
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"



TPS_BEGIN_NAMESPACE

TpsRefreshDoseLineOperation::TpsRefreshDoseLineOperation(const std::string& planUID)
    : mPlanUID(planUID){

}

TpsRefreshDoseLineOperation::~TpsRefreshDoseLineOperation(){

}

bool TpsRefreshDoseLineOperation::ModifyGraphicObject() const{

    auto doseLineGo = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_DOSE_LINE_SETTING));
    auto doseLine = dynamic_pointer_cast<TpsDoseLineGraphicObject>(doseLineGo);
    if(nullptr == doseLine) {
        TPS_LOG_DEV_ERROR<<"Failed to get dose line !";
        return false;
    }
    auto converter = mConverterFactory->GetConverter(GO_TYPE_DOSE_LINE_SETTING);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get dose line graphic object converter.";
        return false;
    }

    GOConverterParameter para;
    para.data_source_map[PLAN] = mPlanUID;
    if (!converter->Update(para, doseLine)){
        TPS_LOG_DEV_ERROR<<"update dose line go failed !";
        return false;
    }
    doseLine->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps