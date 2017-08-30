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
#include "RtTpsRenderLibrary/tps_rl_gallery_vrrotate_operation.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/track_ball.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE
TpsGalleryVRRotateOperation::TpsGalleryVRRotateOperation(
    const std::string seriesUID,
    const Mcsf::Point2f& preCoord, const Mcsf::Point2f &currentCoord) :
    mPreCoord(preCoord),mCurrentCoord(currentCoord), mSeriesUID(seriesUID){

}

TpsGalleryVRRotateOperation::~TpsGalleryVRRotateOperation() {

}

bool TpsGalleryVRRotateOperation::ModifyGraphicObject() const {
    auto camera = mModelWarehouse->GetModelObject(
        mSeriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_SURFACE_CAMERA));
    auto surfaceCamera = std::dynamic_pointer_cast<CameraGraphicObject>(camera);
    if(nullptr == camera || nullptr == surfaceCamera) {
        TPS_LOG_DEV_ERROR<<"failed to get surface camera go!";
        return false;
    }
    Mcsf::MedViewer3D::Quaternion rotate = Mcsf::MedViewer3D::TrackBall::MouseMove2TackBallRotation(
        TpsArithmeticConverter::ConvertToPoint2D(mPreCoord),
        TpsArithmeticConverter::ConvertToPoint2D(mCurrentCoord),
        Mcsf::MedViewer3D::Point2D(0.0,0.0),1,1);
    surfaceCamera->Rotate(rotate);
    surfaceCamera->SetDirty(true);

    return true;
}


TPS_END_NAMESPACE