////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_manset_voi_density.cpp
/// 
///  \brief class TpsMansetVoiDensityCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_manset_density.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"                 // for TpsSaveObjects
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"                  // for TpsPatientManager

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"                     // for TpsDoseManager
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"                        // for TpsDoseProxy

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

TPS_BEGIN_NAMESPACE

TpsVoiMansetDensityCmd::TpsVoiMansetDensityCmd(std::string& strVoiUid,
    float fOverrideMassDensity,
    float fOverrideReferenceEDensity,
    int nOverrideSequence) 
    : m_strVoiUid(strVoiUid),
    m_fOverrideMassDensity(fOverrideMassDensity),
    m_fOverrideReferenceEDensity(fOverrideReferenceEDensity),
    m_nOverrideSequence(nOverrideSequence)
{
}

TpsVoiMansetDensityCmd::~TpsVoiMansetDensityCmd()
{

}

int TpsVoiMansetDensityCmd::PreExecute()
{
    return TPS_ER_SUCCESS;
}

int TpsVoiMansetDensityCmd::Execute()
{
    TpsVOIManager *pVoiManager = mDataRepository->GetVoiManager();
    if (pVoiManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsMansetVoiDensityCmd: Failed to find VOI manager";
        return TPS_ER_FAILURE;
    }

    ROIEntity* pVoiEntity = pVoiManager->GetROI(m_strVoiUid);
    if (pVoiEntity == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsMansetVoiDensityCmd: Failed to find VOI entity for id: " << m_strVoiUid;
        return TPS_ER_FAILURE;
    }
    auto innerRoi = pVoiEntity->GetInnerRoi();

    innerRoi->set_maxdensity(m_fOverrideMassDensity);
    innerRoi->set_mindensity(m_fOverrideMassDensity);
    innerRoi->set_averagedensity(m_fOverrideMassDensity);
    innerRoi->set_overridedenstiy(m_fOverrideMassDensity);
    innerRoi->set_density(m_fOverrideReferenceEDensity);
    innerRoi->set_isoverride(true);

    if (mSaveObjects->UpdateVoiAttributes(*pVoiEntity) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "TpsMansetVoiDensityCmd: Failed to save to TMS for VOI UID:" << m_strVoiUid;
        return TPS_ER_FAILURE;
    }

    m_pVoiEntity = pVoiEntity;
    return TPS_ER_SUCCESS;
}

int TpsVoiMansetDensityCmd::PostExecute()
{
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
