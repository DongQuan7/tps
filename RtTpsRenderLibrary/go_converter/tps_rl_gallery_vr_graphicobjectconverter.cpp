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
///  \date    Jul. 10, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_vr_graphicobjectconverter.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h" 
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_vr_api.h"                              // VRAPI

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsGalleryVRGOConverter::TpsGalleryVRGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), m_spVolumeRendering(nullptr){

}

TpsGalleryVRGOConverter::~TpsGalleryVRGOConverter(){

}

GraphicObjectPtr TpsGalleryVRGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<GraphicObjectVolumeRendering>(new GraphicObjectVolumeRendering());
}

bool TpsGalleryVRGOConverter::PrepareSourceData_i(const GOConverterParameter& /*para*/){
    return true;     
}

bool TpsGalleryVRGOConverter::Update_i(GraphicObjectPtr go){
    auto volumeRendering = std::dynamic_pointer_cast<GraphicObjectVolumeRendering>(go);
    if (volumeRendering == nullptr) return false;

    volumeRendering->Initialize();
    volumeRendering->SetScale(1);
    volumeRendering->SetRenderAlgorithmType(0);
    volumeRendering->SetAlgoMode(Mcsf::MedViewer3D::VR_BRICK_MIPMAP);
    volumeRendering->SetMaskMode(Mcsf::MedViewer3D::VR_NON_MASK);
    volumeRendering->SetDepthMode(VR_DEPTH_DISABLED);
    volumeRendering->SetSSDGrayValue(1);
    // not use. PlaceVRCamera in VRRenderStrategy's Render Method
    //Mcsf::MedViewer3D::VRAPI::PlaceVRCamera(volumeRendering);

    return true;    
}
TPS_END_NAMESPACE  // end namespace tps
