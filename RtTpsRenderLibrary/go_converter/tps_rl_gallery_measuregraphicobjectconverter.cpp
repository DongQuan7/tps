////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  yuxuan.duan  mailto:yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measuregraphicobjectconverter.cpp
/// 
///  \brief class GalleryMeasureGOConverter definition
/// 
///  \version 1.0
/// 
///  \date    2015-03-09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"


TPS_BEGIN_NAMESPACE

TpsGalleryMeasureGOConverter::TpsGalleryMeasureGOConverter(tps::GO_TYPE goType)
    : tps::TpsGOConverterBase(goType) {

}
TpsGalleryMeasureGOConverter::~TpsGalleryMeasureGOConverter() {

}

tps::GraphicObjectPtr TpsGalleryMeasureGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<GalleryMeasureGraphicObject>(
        new GalleryMeasureGraphicObject());
}       

bool TpsGalleryMeasureGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mSeries = it->second;
    return true;
}

bool TpsGalleryMeasureGOConverter::Update_i(tps::GraphicObjectPtr go){
    if (go == nullptr) return false;
    auto measureGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
    if (!measureGO->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize GalleryMeasure graphic object";
        return false;
    }
    return true;
}

TPS_END_NAMESPACE