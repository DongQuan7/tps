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
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsDataManagerBase::TpsDataManagerBase()
{
}

TpsDataManagerBase::~TpsDataManagerBase()
{
}

TpsPatientManager* TpsDataManagerBase::GetPatientManager() const
{
    return mDataRepository->GetPatientManager();
}

TpsImageDataManager* TpsDataManagerBase::GetImageDataManager() const
{
    return mDataRepository->GetImageDataManager();
}

TpsDoseManager* TpsDataManagerBase::GetDoseManager() const
{
    return mDataRepository->GetDoseManager();
}

TpsMachineManager* TpsDataManagerBase::GetMachineManager() const
{
    return mDataRepository->GetMachineManager();
}

TpsVOIManager* TpsDataManagerBase::GetVoiManager() const
{
    return mDataRepository->GetVoiManager();
}

TpsPOIManager* TpsDataManagerBase::GetPoiManager() const
{
    return mDataRepository->GetPoiManager();
}

TpsBeamManager* TpsDataManagerBase::GetBeamManager() const
{
    return mDataRepository->GetBeamManager();
}

TpsNormGroupManager* TpsDataManagerBase::GetNormgroupManager() const
{
    return mDataRepository->GetNormgroupManager();
}

TpsMachineSettingManager* TpsDataManagerBase::GetMachineSettingManager() const
{
    return mDataRepository->GetMachineSettingManager();
}

TpsGalleryPatientManager* TpsDataManagerBase::GetGalleryPatientManager() const
{
    return mDataRepository->GetGalleryPatientManager();
}

TpsGalleryImageManager* TpsDataManagerBase::GetGalleryImageManager() const
{
    return mDataRepository->GetGalleryImageManager();
}

TPS_END_NAMESPACE
