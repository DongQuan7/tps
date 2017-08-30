////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_rl_volume4d_graphicobjectconverter.cpp
/// 
///  \brief class TpsVolume4dGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"

#include "McsfMedViewer3DDataRepresentation/image_data.h"
#include "McsfMedViewer3DDataRepresentation/data_config.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_volume_api.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"


//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVolume4dGOConverter::TpsVolume4dGOConverter(GO_TYPE goType) 
    : TpsGOConverterBase(goType), m_strImageUID(""){

}

TpsVolume4dGOConverter::~TpsVolume4dGOConverter(){

}

GraphicObjectPtr TpsVolume4dGOConverter::CreateGraphicObject_i()
{
    return std::shared_ptr<Volume4dGraphicObject>(new Volume4dGraphicObject());
}

bool TpsVolume4dGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    m_strImageUID = it->second;
    return true;
}

bool TpsVolume4dGOConverter::Update_i(GraphicObjectPtr go)
{

    return true;
}


TPS_END_NAMESPACE  // end namespace tps
