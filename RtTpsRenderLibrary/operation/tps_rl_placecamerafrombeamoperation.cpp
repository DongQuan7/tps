
#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_placecamerafrombeamoperation.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"


TPS_BEGIN_NAMESPACE
PlaceCameraFromBeamOperation::PlaceCameraFromBeamOperation(
const std::string &planUID) : mPlanUID(planUID) {

}

PlaceCameraFromBeamOperation::~PlaceCameraFromBeamOperation() {


}

bool PlaceCameraFromBeamOperation::ModifyGraphicObject() const {
    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto cameraForBEV = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (cameraForBEV == nullptr){
        TPS_LOG_DEV_WARNING<<"There is no bev camera, maybe no BEV window or error occurs.";
        return true;
    }

    GOConverterParameter para;
    para.data_source_map[PLAN] = mPlanUID;
    auto converter = mConverterFactory->GetConverter(GO_TYPE_BEV_CAMERA);
    if (!converter->Update(para, cameraForBEV)){
        TPS_LOG_DEV_ERROR<<"Failed to update bev camera.";
        return false;
    }
    cameraForBEV->SetDirty(true);
    return true;
}


TPS_END_NAMESPACE
