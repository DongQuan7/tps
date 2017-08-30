//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_volume4d_change_current_operation.cpp
///  \brief   Volume4d change current implementation
///
///  \version 1.0
///  \date    Jul. 20, 2015
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_volume4d_change_current_operation.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"                       // for Volume4dGraphicObject
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"       // for Mcsf::MedViewer3D::GraphicObjVolume

#include "RtTpsFramework/tps_fw_modelwarehouse.h"                                   // for ModelWarehouse
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"                     // for GO_TYPE_XXX definition
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"                          // for GOTypeHelper

// ZHENGHE
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps


Volume4dChangeCurrentOperation::Volume4dChangeCurrentOperation(const std::string& strMainSeriesUid, const std::string& strCurrentSeriesUid)
    :m_strMainSeriesUid(strMainSeriesUid), 
     m_strCurrentSeriesUid(strCurrentSeriesUid)
{
}

Volume4dChangeCurrentOperation::~Volume4dChangeCurrentOperation(){

}

bool Volume4dChangeCurrentOperation::ModifyGraphicObject() const {

    try{
        std::string strKey = m_strMainSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME_D4);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto spVolume4dGO = std::dynamic_pointer_cast<Volume4dGraphicObject>(go);
        if (spVolume4dGO == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][Volume4dChangeCurrentOperation][ModifyGraphicObject]: "
                <<"Failed to get Volume4dGraphicObject.";
            return false;
        }

        strKey = m_strCurrentSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spCurrentVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
        if (spCurrentVolumeGO == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
                <<"Failed to get GraphicObjVolume.";
            return false;
        }

        spVolume4dGO->SetCurrentVolume(spCurrentVolumeGO);
        spVolume4dGO->SetDirty(true);
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Volume4dChangeCurrentOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
