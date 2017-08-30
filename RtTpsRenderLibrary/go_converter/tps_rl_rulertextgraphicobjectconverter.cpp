////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xi.Chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_cornerinfographicobjectconverter.h
/// 
///  \brief class TpsRulerTextGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/23
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_rulertextgraphicobjectconverter.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

TpsRulerTextGOConverter::TpsRulerTextGOConverter(GO_TYPE goType) 
    : TpsGOConverterBase(goType), mRulerLength(0.0f), mXOrigin(0.0f), mYOrigin(-0.8f) {

}

TpsRulerTextGOConverter::~TpsRulerTextGOConverter() {

}

GraphicObjectPtr TpsRulerTextGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsTextOverlayGraphicObject>(
        new TpsTextOverlayGraphicObject());
}

bool TpsRulerTextGOConverter::Update_i(GraphicObjectPtr go){
    auto textGo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
    if (textGo == nullptr){
        return false;
    }

    std::vector<std::pair<std::string, std::wstring>> rulerValTextKeys;
    rulerValTextKeys.push_back(make_pair("RulerValText0", L"0"));
    rulerValTextKeys.push_back(make_pair("RulerValText1", L"2"));
    rulerValTextKeys.push_back(make_pair("RulerValText2", L"4"));
    rulerValTextKeys.push_back(make_pair("RulerValText3", L"6"));
    rulerValTextKeys.push_back(make_pair("RulerValText4", L"8"));
    rulerValTextKeys.push_back(make_pair("RulerValText5", L"10"));

    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    Pixel32* pStrBuffer = nullptr;
    int strBufWidth = 0;
    int strBufHeight = 0;
    std::wstringstream wss;

    auto it = rulerValTextKeys.begin();
    for (; it != rulerValTextKeys.end(); ++it) {
        wss.clear();
        wss.str(L"");
        wss << it->second;
        if (!pBufGen->GenerateRasterBuffer(wss.str(), &pStrBuffer, strBufWidth, strBufHeight)) {
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for slice.\n";
            return false;
        }

        TextBlock textBlock(it->first, (unsigned char*)pStrBuffer, strBufWidth, strBufHeight);
        textGo->AddTextBlock(it->first, &textBlock);

        if( pStrBuffer != nullptr){
            delete[] pStrBuffer;
            pStrBuffer = nullptr;
        }
    }
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
