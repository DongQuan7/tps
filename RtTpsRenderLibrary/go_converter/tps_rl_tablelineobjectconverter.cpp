////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file tps_rl_tablelineobjectconverter.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2014/12/04
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_tablelineobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_manualremovetablegraphicobject.h"


#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"




TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsTableLineGOConverter::TpsTableLineGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType){
        mTableLien = 0.0f;
}

TpsTableLineGOConverter::~TpsTableLineGOConverter(){

}

GraphicObjectPtr TpsTableLineGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<ManualRemoveTableGraphicObject>(new ManualRemoveTableGraphicObject());
}

bool TpsTableLineGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    float fHeight = 0.0f;
    RtSeries* series = nullptr;
    mDataRepository->GetImageDataManager()->GetImageSeries(it->second, &series);
    if(series == nullptr) return false;
    fHeight = series->get_tabletopposition();
    mTableLien = fHeight;

    return true;
}

bool TpsTableLineGOConverter::Update_i(GraphicObjectPtr go){
    auto tableLineGo = std::dynamic_pointer_cast<ManualRemoveTableGraphicObject>(go);
    if (tableLineGo == nullptr) {
        return false;
    }

    tableLineGo->SetHeight(mTableLien);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps

