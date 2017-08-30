//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xiaoqing.Shaangguan Xiaoqing.Shaangguan@united-imaging.com
//
/// \file    tps_clear_data_manager_cmd.cpp
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/07/08
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_clear_data_manager.h"

#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

ClearDataManagerCmd::ClearDataManagerCmd()
{
}

ClearDataManagerCmd::~ClearDataManagerCmd()
{
}

int ClearDataManagerCmd::Execute(){
	// clear patient manager
	mDataRepository->GetPatientManager()->ClearPatientData();

	// clear image data
	mDataRepository->GetImageDataManager()->ClearPatientData();

	// clear voi manager
	mDataRepository->GetVoiManager()->ClearPatientData();

	// clear poi manager
	mDataRepository->GetPoiManager()->ClearPatientData();

	// clear normgroup manager
	mDataRepository->GetNormgroupManager()->ClearPatientData();

	// clear beam manager
	mDataRepository->GetBeamManager()->ClearPatientData();

	// clear hot/cold spot
	mDataRepository->GetHotColdSpotsManager()->ClearPatientData();

	// clear dose manager
	mDataRepository->GetDoseManager()->ClearPatientData();

	// clear dvh manager
	mDataRepository->GetDVHManager()->ClearPatientData();
	return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE
