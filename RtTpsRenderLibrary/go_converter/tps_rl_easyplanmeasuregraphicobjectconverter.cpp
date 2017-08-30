////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  wenlong.zhao  mailto:wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplanmeasuregraphicobjectconverter.h
/// 
///  \brief class TDAIGRT EasyPlanMeasureGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014-11-14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_easyplanmeasuregraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuregraphicobject.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"


TPS_BEGIN_NAMESPACE

TpsEasyPlanDRRMeasureGOConverter::TpsEasyPlanDRRMeasureGOConverter(tps::GO_TYPE goType)
    : tps::TpsGOConverterBase(goType) {

}
TpsEasyPlanDRRMeasureGOConverter::~TpsEasyPlanDRRMeasureGOConverter() {

}

tps::GraphicObjectPtr TpsEasyPlanDRRMeasureGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<EasyPlanMeasureGraphicObject>(
        new EasyPlanMeasureGraphicObject());
}       

bool TpsEasyPlanDRRMeasureGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    TpsImage2DEntity* image;
    if (!mDataRepository->GetImageDataManager()->GetImage2DData(it->second, &image)){
        TPS_LOG_DEV_ERROR<<"Image 2d entity with UID: "<<it->second << " does not exist.";
        return false;
    }

    return true;
}

bool TpsEasyPlanDRRMeasureGOConverter::Update_i(tps::GraphicObjectPtr go){
    if (go == nullptr) return false;
    auto measureGO = std::dynamic_pointer_cast<EasyPlanMeasureGraphicObject>(go);
    if (!measureGO->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize EasyPlanDRRMeasure graphic object";
        return false;
    }
    return true;
}

TPS_END_NAMESPACE