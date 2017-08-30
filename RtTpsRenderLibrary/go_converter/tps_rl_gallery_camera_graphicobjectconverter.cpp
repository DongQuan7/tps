//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Apr. 17, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_camera_graphicobjectconverter.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryCameraGoConverter::TpsGalleryCameraGoConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mVolume(nullptr){

}

TpsGalleryCameraGoConverter::~TpsGalleryCameraGoConverter(){

}

GraphicObjectPtr TpsGalleryCameraGoConverter::CreateGraphicObject_i(){

    return std::shared_ptr<CameraGraphicObject>(new CameraGraphicObject());
}

bool TpsGalleryCameraGoConverter::PrepareSourceData_i(const GOConverterParameter& /*para*/){
    return true;     
}

bool TpsGalleryCameraGoConverter::Update_i(GraphicObjectPtr go){
    auto bevCamera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (bevCamera == nullptr) return false;

    bevCamera->Initialize();

    return true;    
}
TPS_END_NAMESPACE  // end namespace tps
