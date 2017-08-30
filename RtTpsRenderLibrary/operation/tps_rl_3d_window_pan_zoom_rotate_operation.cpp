#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_3d_window_pan_zoom_rotate_operation.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE

_3dWindowPanOperation::_3dWindowPanOperation(
const std::string& imageUid, DISPLAY_SIZE displaySize, 
float startx, float starty, float stopx, float stopy) : 
    mImageUid(imageUid), mStartX(startx), 
    mStartY(starty), mStopX(stopx), mStopY(stopy), mDisplaySize(displaySize)
{

}

_3dWindowPanOperation::~_3dWindowPanOperation()
{

}

bool _3dWindowPanOperation::ModifyGraphicObject() const
{
    auto camera = mModelWarehouse->GetModelObject(
        mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_SURFACE_CAMERA));
    auto surfaceCamera = dynamic_pointer_cast<CameraGraphicObject>(camera);

    if(nullptr == surfaceCamera)
    {
        TPS_LOG_DEV_ERROR<<"failed to get surface camera go!";
        return false;
    }

    surfaceCamera->AdaptFovViaAspect((double)mDisplaySize.width / mDisplaySize.height);
    auto corners = surfaceCamera->GetWorldFrustumCorners();
    Mcsf::MedViewer3D::Vector3D translation = 
        (corners[1] - corners[0]) * (mStopX - mStartX) + 
        (corners[3] - corners[0]) * (mStartY - mStopY);
    surfaceCamera->Translate(translation);
    surfaceCamera->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE
