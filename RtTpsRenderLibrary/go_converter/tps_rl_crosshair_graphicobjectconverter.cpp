////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_crosshair_graphicobjectconverter.cpp
/// 
///  \brief class TpsCrosshairGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobjectconverter.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"



using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsCrosshairGOConverter::TpsCrosshairGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType) {

}

TpsCrosshairGOConverter::~TpsCrosshairGOConverter(){

}

GraphicObjectPtr TpsCrosshairGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<CrosshairGraphicObject>(
        new CrosshairGraphicObject());
}

bool TpsCrosshairGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;
    m_strImageUID = it->second;
    return true;
}

bool TpsCrosshairGOConverter::Update_i(GraphicObjectPtr go){
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairGO == nullptr) return false;
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
