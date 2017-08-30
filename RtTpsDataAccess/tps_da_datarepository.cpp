//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  chengcheng.rong chengcheng.rong@united-imaging.com
/// 
///  \file 
/// 
///  \brief  

///  \version 1.0
///  \date    Jul 10, 2015
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tps_da_datarepository.h"

#include "boost/thread.hpp"

#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsDataAccess/tps_da_optgoalmanager.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsDataAccess/tps_da_tcpntcpmanager.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsDataRepository::TpsDataRepository(): mInitialized(false)
{

}

TpsDataRepository::~TpsDataRepository()
{
    Dispose();
}

bool TpsDataRepository::Initialize()
{
    if (mInitialized)
    {
        TPS_LOG_DEV_ERROR<<"TpsDataRepository has been initialized.";
        return false;
    }

    mPatientMgr = new TpsPatientManager();
    mImageDataMgr = new TpsImageDataManager();
    mNormgroupMgr = new TpsNormGroupManager();
    mDoseMgr = new TpsDoseManager();
    mBeamMgr = new TpsBeamManager();
    mVoiMgr = new TpsVOIManager();
    mPoiMgr = new TpsPOIManager();
    mMachineMgr = new TpsMachineManager();
    mDisplayStateMgr = new TpsDisplayStateManager();
    mConfigurationManager = new TpsConfigurationManager();
    mHotColdSpotsManager = new TpsHotColdSpotsManager();
    mDVHManager = new TpsDVHManager();
    mOptGoalMgr = new TpsOptGoalManager();
    mFusionSecondaryDataMgr = new TpsFusionSecondaryDataManager();
    mGalleryImageMgr = new TpsGalleryImageManager();
    mGalleryPatientMgr = new TpsGalleryPatientManager();
    mMachineSettingManager = new TpsMachineSettingManager();
    mCommandMgr = new TpsCommandManager();
    mTableMgr = new TpsTableManager();
    mTcpNtcpMgr = new TpsTcpNTcpManager();

    mPatientMgr->SetRepository(this);
    mImageDataMgr->SetRepository(this);
    mNormgroupMgr->SetRepository(this);
    mDoseMgr->SetRepository(this);
    mBeamMgr->SetRepository(this);
    mVoiMgr->SetRepository(this);
    mPoiMgr->SetRepository(this);
    mMachineMgr->SetRepository(this);
    mDisplayStateMgr->SetRepository(this);
    mConfigurationManager->SetRepository(this);
    mHotColdSpotsManager->SetRepository(this);
    mDVHManager->SetRepository(this);
    mOptGoalMgr->SetRepository(this);
    mFusionSecondaryDataMgr->SetRepository(this);
    mGalleryImageMgr->SetRepository(this);
    mGalleryPatientMgr->SetRepository(this);
    mMachineSettingManager->SetRepository(this);
    mTableMgr->SetRepository(this);
    mTcpNtcpMgr->SetRepository(this);

    return true;
}

void TpsDataRepository::Dispose()
{
    TPS_DEL_PTR(mPatientMgr);
    TPS_DEL_PTR(mNormgroupMgr);
    TPS_DEL_PTR(mDoseMgr);
    TPS_DEL_PTR(mBeamMgr);
    TPS_DEL_PTR(mVoiMgr);
    TPS_DEL_PTR(mPoiMgr);
    TPS_DEL_PTR(mMachineMgr);
    TPS_DEL_PTR(mConfigurationManager);
    TPS_DEL_PTR(mHotColdSpotsManager);
    TPS_DEL_PTR(mDVHManager);

    TPS_DEL_PTR(mDisplayStateMgr);
    TPS_DEL_PTR(mOptGoalMgr);
    TPS_DEL_PTR(mFusionSecondaryDataMgr);
    TPS_DEL_PTR(mGalleryImageMgr);
    TPS_DEL_PTR(mGalleryPatientMgr);
    TPS_DEL_PTR(mMachineSettingManager);
    TPS_DEL_PTR(mCommandMgr);
    TPS_DEL_PTR(mTableMgr);

    TPS_DEL_PTR(mImageDataMgr);
    TPS_DEL_PTR(mTcpNtcpMgr);

}

TpsPatientManager* TpsDataRepository::GetPatientManager() const
{
    return mPatientMgr;
}

TpsImageDataManager* TpsDataRepository::GetImageDataManager() const
{
    return mImageDataMgr;
}

TpsDoseManager* TpsDataRepository::GetDoseManager() const
{
    return mDoseMgr;
}

TpsMachineManager* TpsDataRepository::GetMachineManager() const
{
    return mMachineMgr;
}

TpsVOIManager* TpsDataRepository::GetVoiManager() const
{
    return mVoiMgr;
}

TpsPOIManager* TpsDataRepository::GetPoiManager() const
{
    return mPoiMgr;
}

TpsBeamManager* TpsDataRepository::GetBeamManager() const
{
    return mBeamMgr;
}

TpsNormGroupManager* TpsDataRepository::GetNormgroupManager() const
{
    return mNormgroupMgr;
}

TpsDisplayStateManager* TpsDataRepository::GetDisplayStateManager() const
{
    return mDisplayStateMgr;
}

TpsConfigurationManager* TpsDataRepository::GetConfigurationManager() const
{
    return mConfigurationManager;
}

TpsHotColdSpotsManager* TpsDataRepository::GetHotColdSpotsManager() const
{
    return mHotColdSpotsManager;
}

TpsDVHManager* TpsDataRepository::GetDVHManager() const
{
    return mDVHManager;
}

TpsOptGoalManager* TpsDataRepository::GetOptGoalManager() const
{
    return mOptGoalMgr;
}

TpsFusionSecondaryDataManager* TpsDataRepository::GetFusionSecondaryDataManager() const
{
    return mFusionSecondaryDataMgr;
}

TpsGalleryImageManager* TpsDataRepository::GetGalleryImageManager() const
{
    return mGalleryImageMgr;
}

TpsGalleryPatientManager* TpsDataRepository::GetGalleryPatientManager() const
{
    return mGalleryPatientMgr;
}

TpsMachineSettingManager* TpsDataRepository::GetMachineSettingManager()const
{
    return mMachineSettingManager;
}

TpsCommandManager* TpsDataRepository::GetCommandManager() const
{
    return mCommandMgr;
}

TpsTableManager* TpsDataRepository::GetTableManager() const
{
    return mTableMgr;
}

TpsTcpNTcpManager* TpsDataRepository::GetTcpNTcpManager() const
{
    return mTcpNtcpMgr;
}
TPS_END_NAMESPACE
