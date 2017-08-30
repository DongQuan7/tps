////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  yuxuan.duan  mailto:yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measurevaluegraphicobjectconverter.cpp
/// 
///  \brief class GalleryMeasureValGOConverter definition
/// 
///  \version 1.0
/// 
///  \date    2015-03-23
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobject.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"


TPS_BEGIN_NAMESPACE

TpsGalleryMeasureValGOConverter::TpsGalleryMeasureValGOConverter(tps::GO_TYPE goType)
    : tps::TpsGOConverterBase(goType) {

}
TpsGalleryMeasureValGOConverter::~TpsGalleryMeasureValGOConverter() {

}

tps::GraphicObjectPtr TpsGalleryMeasureValGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<GalleryMeasureValGraphicObject>(
        new GalleryMeasureValGraphicObject());
}  

bool TpsGalleryMeasureValGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    mSeries = para.obj_uid;
    return true;
}

bool TpsGalleryMeasureValGOConverter::Update_i(tps::GraphicObjectPtr go){
    if (go == nullptr) return false;
    auto measureValGO = std::dynamic_pointer_cast<GalleryMeasureValGraphicObject>(go);
    if(measureValGO == nullptr)
    {
        return false;
    }
    if (!measureValGO->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize GalleryMeasureVal graphic object";
        return false;
    }

    return true;
}
TPS_END_NAMESPACE