//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  xiaowei.li@united-imaging.com
///
///  \file    tps_phantom_voi_update_cmd.cpp
///  \brief   PhantomUpdateCmd
///
///  \version 1.0
///  \date    2017/07/18
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_phantom_update.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;

PhantomUpdateCmd::PhantomUpdateCmd(const std::string& strPatientUid, const std::string& strPhantomUid, const std::string& strPhantomName, const std::string& srtPhantomDescription) :
m_strPatientUid(strPatientUid) ,m_strPhantomUid(strPhantomUid), m_strPhantomName(strPhantomName), m_strPhantomDescription(srtPhantomDescription){

}

PhantomUpdateCmd::~PhantomUpdateCmd() {

}

int PhantomUpdateCmd::Execute() 
{
    _ASSERTE( _CrtCheckMemory() );

    //RtPatient* pPatient = new RtPatient(false);
    //pPatient->setdirty_all_fields(false);
    //pPatient->set_uid(m_strPatientUid);
    //pPatient->set_lastname(m_strPhantomName);

    //if (TPS_ER_SUCCESS != mSaveObjects->UpdatePatient(pPatient))
    //{
    //    TPS_LOG_DEV_ERROR<<"PhantomUpdateCmd: Failed to update phantom patient:" << m_strPatientUid;
    //    return TPS_ER_FAILURE;
    //}

    RtSeries* pSeries = new RtSeries(false);
    pSeries->setdirty_all_fields(false);
    pSeries->set_uid(m_strPhantomUid);
    pSeries->set_isphantom(true);
    pSeries->set_patientname(m_strPhantomName);
    pSeries->set_seriesdescription(m_strPhantomDescription);

    if (TPS_ER_SUCCESS != mSaveObjects->UpdateSeries(*pSeries))
    {
        TPS_LOG_DEV_ERROR<<"PhantomUpdateCmd: Failed to update phantom series:" << m_strPhantomUid;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE

