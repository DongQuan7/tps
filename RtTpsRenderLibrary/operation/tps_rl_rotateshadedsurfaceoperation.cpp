////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_rotateshadedsurfaceoperation.h
/// 
///  \brief RotateShadedSurfaceOperation 
/// 
///  \version 1.0
/// 
///  \date    2014/4/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_rotateshadedsurfaceoperation.h"

//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/track_ball.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE
RotateShadedSurfaceOperation::RotateShadedSurfaceOperation(
 const std::string &imageuid,const Mcsf::Point2f& preCoord, const Mcsf::Point2f &currentCoord) :
    mPreCoord(preCoord),mCurrentCoord(currentCoord),mImageUid(imageuid){

}

RotateShadedSurfaceOperation::~RotateShadedSurfaceOperation() {

}

bool RotateShadedSurfaceOperation::ModifyGraphicObject() const {
    auto camera = mModelWarehouse->GetModelObject(
        mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_SURFACE_CAMERA));
    auto surfaceCamera = dynamic_pointer_cast<CameraGraphicObject>(camera);
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
