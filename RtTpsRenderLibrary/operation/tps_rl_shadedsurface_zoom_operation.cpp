//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file tps_rl_shadedsurface_zoom_operation.cpp
///  \brief class ShadedSurfaceZoomOperation  
///
///  \version 1.0
///  \date    Dec. 23, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_shadedsurface_zoom_operation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"

#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"


TPS_BEGIN_NAMESPACE;
ShadedSurfaceZoomOperation::ShadedSurfaceZoomOperation(
 const std::string &imageuid, float fZoomFactor) : mImageUid(imageuid), mZoomFactor(fZoomFactor){

}

ShadedSurfaceZoomOperation::~ShadedSurfaceZoomOperation() {

}

bool ShadedSurfaceZoomOperation::ModifyGraphicObject() const {

    auto go = mModelWarehouse->GetModelObject(
        mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_SURFACE_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera){
        TPS_LOG_DEV_ERROR<<"The shader surface window's camera does not exist.";
        return false;
    }

    //delta factor
    camera->Zoom(mZoomFactor);
    camera->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE
