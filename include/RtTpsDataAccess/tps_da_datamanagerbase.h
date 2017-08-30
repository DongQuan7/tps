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
#ifndef TPS_DATA_MANAGER_BASE_H
#define TPS_DATA_MANAGER_BASE_H

#include "tps_da_defs.h"

TPS_BEGIN_NAMESPACE

class ITpsDataRepository;
class TpsPatientManager;
class TpsImageDataManager;
class TpsNormGroupManager;
class TpsDoseManager;
class TpsBeamManager;
class TpsVOIManager;
class TpsPOIManager;
class TpsMachineManager;
class TpsMachineSettingManager;
class TpsGalleryPatientManager;
class TpsGalleryImageManager;

class TPS_DA_DECLSPEC TpsDataManagerBase {
public:
    TpsDataManagerBase();
    ~TpsDataManagerBase();
    
    void SetRepository(ITpsDataRepository* repos) {
        mDataRepository = repos;
    }

    TpsPatientManager* GetPatientManager() const;
    TpsImageDataManager* GetImageDataManager() const;
    TpsDoseManager* GetDoseManager() const;
    TpsMachineManager* GetMachineManager() const;
    TpsVOIManager* GetVoiManager() const;
    TpsPOIManager* GetPoiManager() const;
    TpsBeamManager* GetBeamManager() const;
    TpsNormGroupManager* GetNormgroupManager() const;

    TpsMachineSettingManager* GetMachineSettingManager()const;

    TpsGalleryPatientManager* GetGalleryPatientManager() const;
    TpsGalleryImageManager* GetGalleryImageManager() const;


protected:
    ITpsDataRepository*  mDataRepository;

};

TPS_END_NAMESPACE

#endif
