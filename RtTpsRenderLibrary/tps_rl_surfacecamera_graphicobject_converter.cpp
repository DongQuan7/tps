//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_rl_surfacecamera_graphicobject_converter.cpp
///
/// \brief   SurfaceCameraGraphicObjectConverter
/// 
/// \version 1.0
/// 
/// \date    2015/06/09
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_surfacecamera_graphicobject_converter.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"


using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE   // begin namespace tps

SurfaceCameraGraphicObjectConverter::SurfaceCameraGraphicObjectConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType) {

}

SurfaceCameraGraphicObjectConverter::~SurfaceCameraGraphicObjectConverter() {

}

GraphicObjectPtr SurfaceCameraGraphicObjectConverter::CreateGraphicObject_i() {

    return std::shared_ptr<CameraGraphicObject>(new CameraGraphicObject());
}

bool SurfaceCameraGraphicObjectConverter::Update_i(GraphicObjectPtr go) {
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (camera == nullptr){
        return false;
    }

    camera->SetEye(Point3D(-10.0, -10.0, 4.0));
    camera->SetLookAt(Point3D(0.0, 0.0, 0.0));
    camera->SetUpDirection(Vector3D(0.2, 0.2, 1.0));

    camera->SetProjectionType(Orthographic);
    camera->SetNearClipDistance(1.0);
    camera->SetFarClipDistance(22.0);
    camera->SetOrthoWindow(2.5, 2.5);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
