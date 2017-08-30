////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_voi_dose_volume_value.cpp
/// 
///  \brief class TpsVoiDoseVolumeValueCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_dose_volume_value.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"                 // for TpsSaveObjects
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"                      // for TpsDVHManager
#include "RtTpsDataAccess/tps_da_patientmanager.h"                  // for TpsPatientManager

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"                     // for TpsDoseManager
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"                        // for TpsDoseProxy

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

TPS_BEGIN_NAMESPACE

TpsVoiDoseVolumeValueCmd::TpsVoiDoseVolumeValueCmd(std::string& strPlanUid, std::vector<std::string> vecVoiUid, std::vector<int> vecVoiVolume)
    : m_strPlanUid(strPlanUid), m_vecVoiUid(vecVoiUid), m_vecVoiVolume(vecVoiVolume) {
}

TpsVoiDoseVolumeValueCmd::~TpsVoiDoseVolumeValueCmd() {

}

int TpsVoiDoseVolumeValueCmd::PreExecute() {
    return TPS_ER_SUCCESS;
}

int TpsVoiDoseVolumeValueCmd::Execute() {

    TpsVOIManager *pVoiManager = mDataRepository->GetVoiManager();
    if (pVoiManager == nullptr) {
        TPS_LOG_DEV_ERROR<<"TpsMansetVoiDensityCmd: Failed to find VOI manager";
        return TPS_ER_FAILURE;
    }

    TpsDVHManager* pDvhManager = mDataRepository->GetDVHManager();
    if (pDvhManager == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"TpsMansetVoiDensityCmd: Failed to find DVH manager";
        return TPS_ER_FAILURE;
    }
    // If add new dose volume
    //if (nOpType == 1)   // Add NEW DOSE VOLUME
    //{
    //    pDvhManager->AddVoiDoseVolume(m_strPlanUid, m_vecVoiVolume);
    //}

    // Get the dose volume for all VOI
    pDvhManager->GetVoiDoseVolume(m_strPlanUid, m_vecVoiUid, m_vecVoiVolume, m_vecVoiDoseVolumeValue);

// send voi dose volume to FE
    return TPS_ER_SUCCESS;
}

int TpsVoiDoseVolumeValueCmd::PostExecute() {
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE