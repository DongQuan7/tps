
#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_updatevolumeoperation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_volumegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"



TPS_BEGIN_NAMESPACE

UpdateVolumeOperation::UpdateVolumeOperation(const std::string &imageUid) : mImageUid(imageUid) {

}

UpdateVolumeOperation::~UpdateVolumeOperation() {

}

bool UpdateVolumeOperation::ModifyGraphicObject() const {
    std::string goType = GOTypeHelper::ToString(GO_TYPE_VOLUME);
    std::string volumeKey = mImageUid + "|" + goType;
    auto volumeGO = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(volumeGO);
    if(nullptr == volumeGO || nullptr == volumeGO) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume GO!";
        return false;
    }

    auto convert = mConverterFactory->GetConverter(GO_TYPE_VOLUME);
    if(nullptr == convert) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume converter!";
        return false;
    }

    GOConverterParameter para;
    para.data_source_map[IMAGE] = mImageUid;
    if(!convert->Update(para,volume)) {
        TPS_LOG_DEV_ERROR<<"update volume failed!";
        return false;
    }
    volume->SetEnable(true);
    volume->SetDirty(true);
    return true;
}





TPS_END_NAMESPACE
