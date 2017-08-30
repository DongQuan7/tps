//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_dose_clear_mesh_operation.h
///  \brief   DoseClearMeshOperation implementation
///
///  \version 1.0
///  \date    Jun. 10, 2015
///  
////////////////////////////////////////////////////////////////////////// 

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_dose_clear_mesh_operation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"          // for DoseMeshGraphicObject

#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"                         // for TpsDoseManager

#include "RtTpsAlgorithmProxy/tps_ap_dose_mesh_helper.h"

#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "tps_logger.h"
// ZHENGHE

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

DoseClearMeshOperation::DoseClearMeshOperation(const std::string& strPlanUid)
    :m_strPlanUid(strPlanUid) {
}

DoseClearMeshOperation::~DoseClearMeshOperation(){

}

bool DoseClearMeshOperation::ModifyGraphicObject() const {

    std::string strKey = m_strPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_DOSE_MESH_COLLECTION);
    auto go = mModelWarehouse->GetModelObject(strKey);
    std::shared_ptr<DoseMeshGraphicObjectCollection> spDoseMeshGo = std::dynamic_pointer_cast<DoseMeshGraphicObjectCollection>(go);
    if (spDoseMeshGo == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][DoseUpdateMeshOperation][ModifyGraphicObject]: Failed to find DoseMeshGO.";
        return false;
    }

    // Update GO
    //spDoseMeshGo->SetValidFlag(false);
    //STC_STRIPS* pMesh = nullptr;
    //spDoseMeshGo->SetDoseMesh(pMesh);       // will delete previous pMesh pointer
    //spDoseMeshGo->SetDirty(true);

    return true;
}

TPS_END_NAMESPACE  // end namespace tps
