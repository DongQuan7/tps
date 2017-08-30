//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  jiandong.zhou     jiandong.zhou@united-imaging.com
///
///  \file tps_plan_manager.cpp
/// 
///  \brief  entry of plan operate
///
///  \version 1.0
///  \date    Aug. 6, 2013
///
///  \modified by maoliang.gong(maoliang.gong@united-imaging.com)
///  \brief  refactor plan manager
///  \date    Jan. 13, 2014
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_drrsetting.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"

TPS_BEGIN_NAMESPACE
TpsNormGroupManager::TpsNormGroupManager(void)
{
    mNormGroupList.clear();
    mDrrsettingList.clear();
    mNormalgroupDoseCalModeMap.clear();
}


TpsNormGroupManager::~TpsNormGroupManager(void)
{
    Dispose();
}

void TpsNormGroupManager::Dispose()
{
    auto it = this->mNormGroupList.begin();
    for (; it != this->mNormGroupList.end(); ++it)
    {
        delete it->second;
        it->second = nullptr;
    }
    mNormGroupList.clear();

    auto itDrr = this->mDrrsettingList.begin();
    for (; itDrr != this->mDrrsettingList.end(); ++itDrr)
    {
        delete itDrr->second;
        itDrr->second = nullptr;
    }
    mDrrsettingList.clear();

    mNormalgroupDoseCalModeMap.clear();
}

void TpsNormGroupManager::ClearPatientData()
{
    Dispose();
}

bool TpsNormGroupManager::GetNormGroup(const std::string& sNormGroupUID,
    RtNormgroup** pNormGroup) const
{
    auto it = mNormGroupList.find(sNormGroupUID);
    if (it == mNormGroupList.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normGroup which uid: " << sNormGroupUID;
        return false;
    }

    *pNormGroup = it->second;
    return true;
}

std::string TpsNormGroupManager::GetPlanUIDByNormGroupUID(const std::string& sNormGroupUID) const
{
    std::string sPlanUID("");
    auto itr = mNormGroupList.find(sNormGroupUID);
    if (itr != mNormGroupList.end() )
    {
        if (nullptr != itr->second)
        {
            sPlanUID = itr->second->get_planuid();
        }
    }

    return sPlanUID;
}

std::vector<std::string> TpsNormGroupManager::GetNormGroupUIDListByPlan(const std::string& sPlanUID) const
{
   std::vector<std::string> vNormGroupUIDList;
   for (auto itr = mNormGroupList.begin(); itr != mNormGroupList.end(); ++itr)
   {
       if (nullptr != itr->second && itr->second->get_planuid() == sPlanUID)
       {
           vNormGroupUIDList.push_back(itr->first);
       }
   }
   return vNormGroupUIDList;
}

std::vector<RtNormgroup*> TpsNormGroupManager::GetNormalGroupList(const std::string& sPlanUID) const
{
    std::vector<RtNormgroup*> vNormGroupList;
    for (auto itr=mNormGroupList.begin(); itr!=mNormGroupList.end(); ++itr){
        if (nullptr != itr->second && itr->second->get_planuid() == sPlanUID) {
            vNormGroupList.push_back(itr->second);
        }
    }
    return vNormGroupList;
}

bool TpsNormGroupManager::AddNormGroup(RtNormgroup* normGroup)
{
    if (nullptr == normGroup)
    {
        TPS_LOG_DEV_ERROR << "normGroup is null.";
        return false;
    }
    const std::string sNormGroupUID = normGroup->get_uid();
    mTpsNormGroupMapIt it = mNormGroupList.find(sNormGroupUID);
    if (it == mNormGroupList.end())
    {
        mNormGroupList[sNormGroupUID] = normGroup;
        return true;
    }

    return false;

}

void TpsNormGroupManager::RemoveNormGroup(const std::string& uid)
{
    auto it = mNormGroupList.find(uid);
    if (it == mNormGroupList.end()){
        TPS_LOG_DEV_WARNING<<"The normalgroup to be removed with uid: "<<uid<<" doesn't exist.";
        return;
    }
    RtNormgroup* pNormgroup = it->second;
    delete pNormgroup;
    mNormGroupList.erase(it);
}


bool TpsNormGroupManager::GetDrrSetting(const std::string& normGroupUid, 
    RtDrrsetting** pDrrsetting) const
{
    auto it = mDrrsettingList.find(normGroupUid);
    if (it == mDrrsettingList.end() || it->second == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get drr setting which normalgroupuid: "<<normGroupUid;
        return false;
    }

    *pDrrsetting = it->second;
    return true;
}

bool TpsNormGroupManager::AddDrrSetting(RtDrrsetting* drrsetting)
{
    if (nullptr==drrsetting) {
        TPS_LOG_DEV_ERROR<<"drrsetting is null.";
        return false;
    }
    const std::string sNormGroupUID = drrsetting->get_normalgroupuid();
    auto it = mDrrsettingList.find(sNormGroupUID);
    if (it == mDrrsettingList.end()) {
        mDrrsettingList[sNormGroupUID] = drrsetting;
        return true;
    }

    return false;
}

void TpsNormGroupManager::RemoveDrrSetting(const std::string& normalgroupUid)
{
    auto it = mDrrsettingList.find(normalgroupUid);
    if (it == mDrrsettingList.end())
    {
        TPS_LOG_DEV_WARNING << "The drrsetting to be removed with normalgroup uid: "<<normalgroupUid<<" doesn't exist.";
        return;
    }
    RtDrrsetting* pdrrSetting = it->second;
    delete pdrrSetting;
    mDrrsettingList.erase(it);
}

std::vector<std::string> TpsNormGroupManager::FilterLoadedDoseGridUid(const std::vector<std::string>& doseGridUidList)
{
    std::vector<std::string> unloadedUids;
    bool hasLoaded = false;
    RtDosegrid* dosegrid = nullptr;
    for (int i = 0; i < doseGridUidList.size(); ++i)
    {
        hasLoaded = false;
        for (auto it = mNormGroupList.begin(); it != mNormGroupList.end(); ++it)
        {
            dosegrid = it->second->get_dosegrid();
            if (dosegrid != nullptr && dosegrid->get_uid() == doseGridUidList[i])
            {
                hasLoaded = true;
                break;
            }
        }
        if (!hasLoaded) unloadedUids.push_back(doseGridUidList[i]);
    }
    return unloadedUids;
}

bool TpsNormGroupManager::AddNgDoseCalculateMode(const std::string& ngUid, RtDbDef::DoseCalculateMode doseCalculateMode)
{
     if (ngUid.empty())
    {
        TPS_LOG_DEV_ERROR << "ngUid is empty.";
        return false;
    }

    std::map<std::string, RtDbDef::DoseCalculateMode>::iterator it = mNormalgroupDoseCalModeMap.find(ngUid);
    if (it == mNormalgroupDoseCalModeMap.end())
    {
        mNormalgroupDoseCalModeMap[ngUid] = doseCalculateMode;
        return true;
    }

    return false;
}

bool TpsNormGroupManager::GetDoseCalculateModeViaNgUid(const std::string& ngUid, RtDbDef::DoseCalculateMode& doseCalculateMode)
{
    auto it = mNormalgroupDoseCalModeMap.find(ngUid);
    if (it == mNormalgroupDoseCalModeMap.end())
    {
        TPS_LOG_DEV_ERROR << "Failed to GetDoseCalculateModeViaNgUid ng uid: "<< ngUid;
        return false;
    }

    doseCalculateMode = it->second;
    return true;
}

bool TpsNormGroupManager::UpdateDoseCalculateMode(const std::string& ngUid, RtDbDef::DoseCalculateMode doseCalculateMode)
{
    auto it = mNormalgroupDoseCalModeMap.find(ngUid);
    if (it == mNormalgroupDoseCalModeMap.end())
    {
        TPS_LOG_DEV_ERROR << "Failed to UpdateDoseCalculateMode which ng uid: "<< ngUid;
        return false;
    }

    it->second = doseCalculateMode;
    return true;
}

void TpsNormGroupManager::RemoveNormGroupDoseCalculateMode(const std::string& uid)
{
    auto it = mNormalgroupDoseCalModeMap.find(uid);
    if (it == mNormalgroupDoseCalModeMap.end())
    {
        TPS_LOG_DEV_WARNING << "The normalgroup to be removed with uid: "<< uid << " doesn't exist.";
        return;
    }

    mNormalgroupDoseCalModeMap.erase(it);
}

TPS_END_NAMESPACE
