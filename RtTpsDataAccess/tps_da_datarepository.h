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
#ifndef TPS_DATA_REPOSITORY_H
#define TPS_DATA_REPOSITORY_H

#include "RtTpsDataAccess/tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

TPS_BEGIN_NAMESPACE

class TpsHeteroManager;

class TPS_DA_DECLSPEC TpsDataRepository : public ITpsDataRepository
{
public:
    TpsDataRepository();
    virtual ~TpsDataRepository();

    TpsPatientManager*          GetPatientManager() const;
    TpsImageDataManager*        GetImageDataManager() const;
    TpsNormGroupManager*        GetNormgroupManager() const;
    TpsDoseManager*             GetDoseManager() const;
    TpsBeamManager*             GetBeamManager() const;
    TpsVOIManager*              GetVoiManager() const;
    TpsPOIManager*              GetPoiManager() const;
    TpsMachineManager*          GetMachineManager() const;
    TpsDisplayStateManager*     GetDisplayStateManager() const;
    TpsConfigurationManager*    GetConfigurationManager() const;
    TpsHotColdSpotsManager*     GetHotColdSpotsManager() const;
    TpsDVHManager*              GetDVHManager() const;
    TpsOptGoalManager*          GetOptGoalManager() const;
    TpsFusionSecondaryDataManager* GetFusionSecondaryDataManager() const;
    TpsGalleryImageManager*     GetGalleryImageManager() const;
    TpsGalleryPatientManager*     GetGalleryPatientManager() const;
    TpsHeteroManager*           GetHeteroManager() const;
    TpsMachineSettingManager*       GetMachineSettingManager()const;
    TpsCommandManager*          GetCommandManager() const;
    TpsTableManager*            GetTableManager() const;
    TpsTcpNTcpManager*          GetTcpNTcpManager() const;

    bool Initialize();
    void Dispose();
    
private:
    TpsPatientManager*          mPatientMgr;
    TpsImageDataManager*        mImageDataMgr;
    TpsNormGroupManager*        mNormgroupMgr;
    TpsDoseManager*             mDoseMgr;
    TpsBeamManager*             mBeamMgr;
    TpsVOIManager*              mVoiMgr;
    TpsPOIManager*              mPoiMgr;
    TpsMachineManager*          mMachineMgr;
    TpsDisplayStateManager*     mDisplayStateMgr;
    TpsConfigurationManager*    mConfigurationManager;
    TpsHotColdSpotsManager*     mHotColdSpotsManager;;
    TpsDVHManager*              mDVHManager;
    TpsOptGoalManager*          mOptGoalMgr;
    TpsFusionSecondaryDataManager* mFusionSecondaryDataMgr;
    TpsGalleryImageManager*     mGalleryImageMgr;
    TpsGalleryPatientManager*   mGalleryPatientMgr;
    TpsHeteroManager*           mHeteroMgr;
    TpsMachineSettingManager*       mMachineSettingManager;
    TpsCommandManager*          mCommandMgr;
    TpsTableManager*            mTableMgr;
    TpsTcpNTcpManager*          mTcpNtcpMgr;
    bool                        mInitialized;
};

TPS_END_NAMESPACE

#endif
