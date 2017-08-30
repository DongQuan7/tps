////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  yuxuan.duan  mailto:yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_textgraphicobjectconverter.cpp
/// 
///  \brief class GalleryTextGOConverter definition
/// 
///  \version 1.01
/// 
///  \date    2015-06-12
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"


TPS_BEGIN_NAMESPACE

TpsGalleryTextGOConverter::TpsGalleryTextGOConverter(tps::GO_TYPE goType)
    : tps::TpsGOConverterBase(goType) {

}
TpsGalleryTextGOConverter::~TpsGalleryTextGOConverter() {

}

tps::GraphicObjectPtr TpsGalleryTextGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<GalleryTextGraphicObject>(
        new GalleryTextGraphicObject());
}  

bool TpsGalleryTextGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    mSeries = para.obj_uid;
    return true;
}

bool TpsGalleryTextGOConverter::Update_i(tps::GraphicObjectPtr go){
    if (go == nullptr) return false;
    auto textGO = std::dynamic_pointer_cast<GalleryTextGraphicObject>(go);
    if(textGO == nullptr)
    {
        return false;
    }
    if (!textGO->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize Gallery Text graphic object";
        return false;
    }

    return true;
}
TPS_END_NAMESPACE