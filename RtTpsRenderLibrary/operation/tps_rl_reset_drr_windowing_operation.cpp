////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_reset_drr_windowing.cpp
/// 
///  \brief class ResetDrrWindowing implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/03/13
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_reset_drr_windowing_operation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

ResetDrrWindowingOperation::ResetDrrWindowingOperation(
    const std::string& imageUID) : mImageUID(imageUID){

}

ResetDrrWindowingOperation::~ResetDrrWindowingOperation(){

}

bool ResetDrrWindowingOperation::ModifyGraphicObject() const{
    std::string volumeKey = mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
    if (volume == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][PrepareSourceData_i]: "
            <<"The volume data with UID: "<<mImageUID<<" does not exist.";
        return false;
    }

    go = mModelWarehouse->GetModelObject(
        mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_DRR));
    auto drr = std::dynamic_pointer_cast<DrrGraphicObejct>(go);
    if(drr == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get drr go:"<<mImageUID;
        return false;
    }

    double dWindowWidth(256.0), dWindowCenter(127.0);
    //volume->GetDefaultWindowing(&dWindowWidth, &dWindowCenter);
    drr->SetWindowing(dWindowWidth, dWindowCenter);
    drr->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps