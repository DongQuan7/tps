////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_da_machinemanager.cpp
/// 
///  \brief class TpsMachineManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/26
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "tps_logger.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"



TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsMachineManager::TpsMachineManager()
{

}

TpsMachineManager::~TpsMachineManager()
{
    Dispose();
}

void TpsMachineManager::Dispose()
{
    for_each(mMachineMap.begin(), mMachineMap.end(), [&](
        std::pair<std::string, RtMachine*> pair)
    {
        DEL_PTR(pair.second);
    });
    mMachineMap.clear();


    for_each(mCommissonedUnitMap.begin(), mCommissonedUnitMap.end(), [&](
        std::pair<std::string, RtCommissionedunit*> pair)
    {
        DEL_PTR(pair.second);
    });
    mCommissonedUnitMap.clear();


    for_each(mAccessoryMap.begin(), mAccessoryMap.end(), [&](
        std::pair<std::string, RtAccessory*> pair)
    {
        DEL_PTR(pair.second);
    });
    mAccessoryMap.clear();


    for_each(mApproveRecordMap.begin(), mApproveRecordMap.end(), [&](
        std::pair<std::string, RtApproverecord*> pair)
    {
        DEL_PTR(pair.second);
    });
    mApproveRecordMap.clear();


    for_each(mApplicatorMap.begin(), mApplicatorMap.end(), [&](
        std::pair<std::string, RtApplicator*> pair)
    {
        DEL_PTR(pair.second);
    });
    mApplicatorMap.clear();

    for (auto itrChunk = mAccessoryChunkMap.begin(); itrChunk != mAccessoryChunkMap.end(); ++itrChunk)
    {
        DEL_PTR((*itrChunk).second);
    }
    mAccessoryChunkMap.clear();

    for (auto itrKM = mKerneldataMap.begin(); itrKM != mKerneldataMap.end(); ++itrKM)
    {
        DEL_PTR((*itrKM).second);
    }
    mKerneldataMap.clear();

    for (auto itrKLM = mKerneldatalistMap.begin(); itrKLM != mKerneldatalistMap.end(); ++itrKLM)
    {
        DEL_PTR((*itrKLM).second);
    }
    mKerneldatalistMap.clear();

    for (auto itrKWM = mKernelweightMap.begin(); itrKWM != mKernelweightMap.end(); ++itrKWM)
    {
        DEL_PTR((*itrKWM).second);
    }
    mKernelweightMap.clear();

    for (auto itrTM = mTrayMap.begin(); itrTM != mTrayMap.end(); ++itrTM)
    {
        DEL_PTR((*itrTM).second);
    }
    mTrayMap.clear();

    for (auto itrACM = mAccessoryChunkProfileMap.begin(); itrACM != mAccessoryChunkProfileMap.end(); ++itrACM)
    {
        DEL_PTR((*itrACM).second);
    }
    mAccessoryChunkProfileMap.clear();

    for (auto itrMCM = mMcphocommissionedunitMap.begin(); itrMCM != mMcphocommissionedunitMap.end(); ++itrMCM)
    {
        DEL_PTR((*itrMCM).second);
    }
    mMcphocommissionedunitMap.clear();

    for (auto itrGM = mGoldenSttMap.begin(); itrGM != mGoldenSttMap.end(); ++itrGM)
    {
        DEL_PTR((*itrGM).second);
    }
    mGoldenSttMap.clear();

    for (auto itrMCWM = mMcphomonoenergyweightMap.begin();
        itrMCWM != mMcphomonoenergyweightMap.end(); ++itrMCWM)
    {
        std::vector<RtMcphomonoenergyweight*> vMcphomonoenergyweightList = (*itrMCWM).second;
        for (auto itrVMcWeight = vMcphomonoenergyweightList.begin();
            itrVMcWeight != vMcphomonoenergyweightList.end(); ++itrVMcWeight)
        {
            DEL_PTR(*itrVMcWeight);
        }

        mMcphomonoenergyweightMap.clear();
    }

}

bool TpsMachineManager::GetMachine(const std::string& uid, RtMachine*& machine)
{
    auto it = mMachineMap.find(uid);
    if (it == mMachineMap.end())
    {
        TPS_LOG_DEV_ERROR << "The machine does not exist which UID: " << uid;
        return false;
    }
    machine = it->second;
    return true;
}

void TpsMachineManager::AddMachine(const std::string& machineUID, RtMachine* &machine)
{
    auto it = mMachineMap.find(machineUID);
    if (it != mMachineMap.end())
    {
        TPS_LOG_TRACE_INFO << "The machine unit already exists, replace it.";
        DEL_PTR(it->second);
    }
    mMachineMap[machineUID] = machine;
}

void TpsMachineManager::GetAllMachine(std::vector<RtMachine*>& vMachines)
{
    vMachines.clear();
    vMachines.resize(mMachineMap.size());
    std::transform(mMachineMap.begin(), mMachineMap.end(), vMachines.begin(),
        [](const std::map<std::string, RtMachine*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddCommissonedUnit(const std::string unitUID, RtCommissionedunit* commissionedUnit)
{
    auto it = mCommissonedUnitMap.find(unitUID);
    if (it != mCommissonedUnitMap.end())
    {
        TPS_LOG_TRACE_INFO << "The commissioned unit already exists, replace it.";
        DEL_PTR(it->second);
    }
    mCommissonedUnitMap[unitUID] = commissionedUnit;
}

bool TpsMachineManager::GetCommissionedUnit(const std::string unitUID, RtCommissionedunit*& commissionedUnit)
{
    auto it = mCommissonedUnitMap.find(unitUID);
    if (it == mCommissonedUnitMap.end())
    {
        TPS_LOG_DEV_ERROR << "The commissioned unit does not exist which UID: "<< unitUID;
        return false;
    }
    commissionedUnit = it->second;
    return true;
}

void TpsMachineManager::GetAllCommissionedUnit(std::vector<RtCommissionedunit*>& vCommissionedUnits)
{
    vCommissionedUnits.clear();
    vCommissionedUnits.resize(mCommissonedUnitMap.size());
    std::transform(mCommissonedUnitMap.begin(), mCommissonedUnitMap.end(), vCommissionedUnits.begin(),
        [](const std::map<std::string, RtCommissionedunit*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::GetSpecifiedCommissonedUnit(const std::string& machineUid,
    int radiationType,
    int techniqueType,
    std::vector<RtCommissionedunit*>& vCommissionedUnits)
{

    vCommissionedUnits.clear();

    for (auto itr = mCommissonedUnitMap.begin(); itr !=mCommissonedUnitMap.end(); ++itr)
    {
        if (nullptr != (*itr).second)
        {
            if ((*itr).second->get_machineuid() == machineUid && (*itr).second->get_radiationtype() == radiationType)
            {
                if (radiationType == (int)RtDbDef::RADIATION_PHOTON)
                {
                    std::string strPhotonPlanTypeList = (*itr).second->get_availablephotonplantype();
                    std::vector<std::string> vecValue;
                    (void)boost::split(vecValue, strPhotonPlanTypeList, boost::is_any_of(","));
                    size_t iLen = vecValue.size();

                    if (techniqueType <= iLen && vecValue[techniqueType - 1] == "1")
                    {
                        vCommissionedUnits.push_back((*itr).second);
                    }
                }
                else if (radiationType == (int)RtDbDef::RADIATION_ELETRON)
                {
                    std::string strelectronPlanTypeList = (*itr).second->get_availableelectronplantype();
                    std::vector<std::string> vecValue;
                    (void)boost::split(vecValue, strelectronPlanTypeList, boost::is_any_of(","));
                    size_t iLen = vecValue.size();

                    for (int i = 0; i < iLen; i++)
                    {
                        if (!vecValue[i].empty())
                        {
                            if (techniqueType == atoi(vecValue[i].c_str()))
                            {
                                vCommissionedUnits.push_back((*itr).second);
                            }
                        }
                    }
                }

            }
        }
    }
}

bool TpsMachineManager::GetAccessory(const std::string& accessoryUID, RtAccessory*& accessory)
{
    auto it = mAccessoryMap.find(accessoryUID);
    if (it == mAccessoryMap.end())
    {
        TPS_LOG_DEV_ERROR << "The Accessory does not exist which UID: " << accessoryUID;
        return false;
    }
    accessory = it->second;
    return true;
}

void TpsMachineManager::GetAllAccessory(std::vector<RtAccessory*>& accessoryList)
{
    accessoryList.clear();
    accessoryList.resize(mAccessoryMap.size());
    std::transform(mAccessoryMap.begin(), mAccessoryMap.end(), accessoryList.begin(),
        [](const std::map<std::string, RtAccessory*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddAccessory(RtAccessory* accessory)
{
    if (accessory == nullptr)
    {
        return;
    }

    auto it = mAccessoryMap.find(accessory->get_uid());
    if (it != mAccessoryMap.end())
    {
        TPS_LOG_TRACE_INFO << "The accessory unit already exists, replace it.";
        DEL_PTR(it->second);
    }
    mAccessoryMap[accessory->get_uid()] = accessory;
}

bool TpsMachineManager::GetApplicator(const std::string& sApplicatorUID, RtApplicator*& pApplicator) const
{
    auto it = mApplicatorMap.find(sApplicatorUID);
    if (it == mApplicatorMap.end())
    {
        TPS_LOG_DEV_ERROR << "The applicator does not exist which UID: " << sApplicatorUID;
        return false;
    }
    pApplicator = it->second;
    return true;
}

void TpsMachineManager::AddApplicator(const std::string& sApplicatorUID, RtApplicator* pApplicator)
{
    auto it = mApplicatorMap.find(sApplicatorUID);
    if (it != mApplicatorMap.end())
    {
        TPS_LOG_TRACE_INFO << "The applicator already exists, replace it.";
        DEL_PTR(it->second);
    }
    mApplicatorMap[sApplicatorUID] = pApplicator;
}

void TpsMachineManager::AddAccessoryChunk(RtAccessorychunk* accessoryChunk)
{
    if (accessoryChunk == nullptr)
    {
        return;
    }
    auto it = mAccessoryChunkMap.find(accessoryChunk->get_uid());
    if (it != mAccessoryChunkMap.end())
    {
        TPS_LOG_TRACE_INFO << "The accessoryChunk already exists, replace it.";
        DEL_PTR(it->second);
    }
    mAccessoryChunkMap[accessoryChunk->get_uid()] = accessoryChunk;
}

void TpsMachineManager::GetAllAccessoryChunk(std::vector<RtAccessorychunk*>& accessoryChunkList)
{
    accessoryChunkList.clear();
    accessoryChunkList.resize(mAccessoryChunkMap.size());
    std::transform(mAccessoryChunkMap.begin(), mAccessoryChunkMap.end(), accessoryChunkList.begin(),
        [](const std::map<std::string, RtAccessorychunk*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddKerneldata(RtKerneldata* kerneldata)
{
    if (kerneldata == nullptr)
    {
        return;
    }
    auto it = mKerneldataMap.find(kerneldata->get_uid());
    if (it != mKerneldataMap.end())
    {
        TPS_LOG_TRACE_INFO << "The kerneldata already exists, replace it.";
        DEL_PTR(it->second);
    }
    mKerneldataMap[kerneldata->get_uid()] = kerneldata;
}

void TpsMachineManager::GetAllKerneldata(std::vector<RtKerneldata*>& vKerneldata)
{
    vKerneldata.clear();
    vKerneldata.resize(mKerneldataMap.size());
    std::transform(mKerneldataMap.begin(), mKerneldataMap.end(), vKerneldata.begin(),
        [](const std::map<std::string, RtKerneldata*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddMcphocommissionedunit(RtMcphocommissionedunit* mcphocommissionedunit)
{
    if (mcphocommissionedunit == nullptr)
    {
        return;
    }

    auto it = mMcphocommissionedunitMap.find(mcphocommissionedunit->get_uid());
    if (it != mMcphocommissionedunitMap.end())
    {
        TPS_LOG_TRACE_INFO << "The mcphocommissionedunit already exists, replace it.";
        DEL_PTR(it->second);
    }
    mMcphocommissionedunitMap[mcphocommissionedunit->get_uid()] = mcphocommissionedunit;
}

void TpsMachineManager::GetAllMcphocommissionedunit(std::vector<RtMcphocommissionedunit*>& vMcphocommissionedunit)
{
    vMcphocommissionedunit.clear();
    vMcphocommissionedunit.resize(mMcphocommissionedunitMap.size());
    std::transform(mMcphocommissionedunitMap.begin(), mMcphocommissionedunitMap.end(), vMcphocommissionedunit.begin(),
        [](const std::map<std::string, RtMcphocommissionedunit*>::value_type& item)
    {
        return item.second;
    });
}


void TpsMachineManager::AddGoldenstt(RtGoldenstt* goldenstt)
{
    if (goldenstt == nullptr)
    {
        return;
    }

    auto it = mGoldenSttMap.find(goldenstt->get_uid());
    if (it != mGoldenSttMap.end())
    {
        TPS_LOG_TRACE_INFO << "The goldenstt already exists, replace it.";
        DEL_PTR(it->second);
    }
    mGoldenSttMap[goldenstt->get_uid()] = goldenstt;
}

void TpsMachineManager::GetAllGoldenstt(std::vector<RtGoldenstt*>& vGoldenstt) 
{
    vGoldenstt.clear();
    vGoldenstt.resize(mGoldenSttMap.size());
    std::transform(mGoldenSttMap.begin(), mGoldenSttMap.end(), vGoldenstt.begin(),
        [](const std::map<std::string, RtGoldenstt*>::value_type& item) 
    {
        return item.second;
    });
}

void TpsMachineManager::AddMcphomonoenergyweight(
    const std::string& sMcphocommissionedunitUid,
    std::vector<RtMcphomonoenergyweight*>& vMcphomonoenergyweight)
{
    if (sMcphocommissionedunitUid.empty() || vMcphomonoenergyweight.size() == 0)
    {
        return;
    }

    auto it = mMcphomonoenergyweightMap.find(sMcphocommissionedunitUid);
    if (it != mMcphomonoenergyweightMap.end())
    {
        TPS_LOG_TRACE_INFO << "The mcphomonoenergyweight already exists, replace it.";
    }
    mMcphomonoenergyweightMap[sMcphocommissionedunitUid].clear();
    mMcphomonoenergyweightMap[sMcphocommissionedunitUid] = vMcphomonoenergyweight;
}

void TpsMachineManager::GetMcphomonoenergyweightListByMcphocommissionedunituid(
    const std::string& sMcphocommissionedunitUid,
    std::vector<RtMcphomonoenergyweight*>& vMcphomonoenergyweight)
{
    if (sMcphocommissionedunitUid.empty())
    {
        return;
    }

    auto it = mMcphomonoenergyweightMap.find(sMcphocommissionedunitUid);
    if (it != mMcphomonoenergyweightMap.end())
    {
        vMcphomonoenergyweight = mMcphomonoenergyweightMap[sMcphocommissionedunitUid];
    }
    else
    {
        TPS_LOG_TRACE_INFO << "cannot find mcphomonoenergyweight of mcphocommissionedunit, uid"
            << sMcphocommissionedunitUid ;
    }
}


void TpsMachineManager::AddKerneldatalist(RtKerneldatalist* kerneldatalist)
{
    if (kerneldatalist == nullptr)
    {
        return;
    }
    auto it = mKerneldatalistMap.find(kerneldatalist->get_uid());
    if (it != mKerneldatalistMap.end())
    {
        TPS_LOG_TRACE_INFO << "The kerneldatalist already exists, replace it, uid:" << kerneldatalist->get_uid();
        DEL_PTR(it->second);
    }
    mKerneldatalistMap[kerneldatalist->get_uid()] = kerneldatalist;
}

void TpsMachineManager::GetAllKerneldatalist(std::vector<RtKerneldatalist*>& vKerneldatalist)
{
    vKerneldatalist.clear();
    vKerneldatalist.resize(mKerneldatalistMap.size());
    std::transform(mKerneldatalistMap.begin(), mKerneldatalistMap.end(), vKerneldatalist.begin(),
        [](const std::map<std::string, RtKerneldatalist*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddKernelweight(RtKernelweight* kernelweight)
{
    if (kernelweight == nullptr)
    {
        return;
    }

    auto it = mKernelweightMap.find(kernelweight->get_uid());
    if (it != mKernelweightMap.end())
    {
        TPS_LOG_TRACE_INFO << "The kernelweight already exists, replace it, uid:" << kernelweight->get_uid();
        DEL_PTR(it->second);
    }
    mKernelweightMap[kernelweight->get_uid()] = kernelweight;
}

void TpsMachineManager::GetAllKernelweight(std::vector<RtKernelweight*>& vKernelweight)
{
    vKernelweight.clear();
    vKernelweight.resize(mKernelweightMap.size());
    std::transform(mKernelweightMap.begin(), mKernelweightMap.end(), vKernelweight.begin(),
        [](const std::map<std::string, RtKernelweight*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddTray(RtTray* tray)
{
    if (tray == nullptr)
    {
        return;
    }

    auto it = mTrayMap.find(tray->get_uid());
    if (it != mTrayMap.end())
    {
        TPS_LOG_TRACE_INFO << "The tray already exists, replace it, uid:" << tray->get_uid();
        DEL_PTR(it->second);
    }
    mTrayMap[tray->get_uid()] = tray;
}

void TpsMachineManager::GetAllTray(std::vector<RtTray*>& vTray)
{
    vTray.clear();
    vTray.resize(mTrayMap.size());
    std::transform(mTrayMap.begin(), mTrayMap.end(), vTray.begin(),
        [](const std::map<std::string, RtTray*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddAccessoryChunkProfile(RtContour* profile)
{
    if (profile == nullptr)
    {
        return;
    }
    auto it = mAccessoryChunkProfileMap.find(profile->get_uid());
    if (it != mAccessoryChunkProfileMap.end())
    {
        TPS_LOG_TRACE_INFO << "The profile exists, replace it, uid:" << profile->get_uid();
        DEL_PTR(it->second);
    }
    mAccessoryChunkProfileMap[profile->get_uid()] = profile;
}

void TpsMachineManager::GetAllAccessoryChunkProfile(std::vector<RtContour*>& vProfile)
{
    vProfile.clear();
    vProfile.resize(mAccessoryChunkProfileMap.size());
    std::transform(mAccessoryChunkProfileMap.begin(), mAccessoryChunkProfileMap.end(), vProfile.begin(),
        [](const std::map<std::string, RtContour*>::value_type& item)
    {
        return item.second;
    });
}

int TpsMachineManager::GetMachineUnitAccessory(const std::string& machineUID,
    RtMachine*& machine,
    std::vector<RtCommissionedunit*>& vCommissionedunitList,
    std::vector<RtAccessorychunk*>& vRtAccessorychunkList,
    std::vector<RtGoldenstt*>& vRtGoldensttList,
    std::vector<RtAccessory*>& vRtAccessoryList)
{
    if (!this->GetMachine(machineUID, machine))
    {
        return TPS_ER_FAILURE;
    }

    vCommissionedunitList.clear();
    for (auto it = mCommissonedUnitMap.begin(); it != mCommissonedUnitMap.end(); ++it)
    {
        if (it->second->get_machineuid() == machineUID)
        {
            vCommissionedunitList.push_back(it->second);
        }
    }

    vRtAccessoryList.clear();
    for (auto it = mAccessoryMap.begin(); it != mAccessoryMap.end(); ++it)
    {
        if (it->second->get_machineuid() == machineUID)
        {
            vRtAccessoryList.push_back(it->second);
        }
    }

    vRtAccessorychunkList.clear();
    vRtGoldensttList.clear();
    std::string unitUid;
    for (int i = 0; i < vCommissionedunitList.size(); ++i){
        unitUid = vCommissionedunitList[i]->get_uid();
        for (auto it = mAccessoryChunkMap.begin(); it != mAccessoryChunkMap.end(); ++it)
        {
            if (it->second->get_commissionedunituid() == unitUid)
            {
                vRtAccessorychunkList.push_back(it->second);
            }
        }
        for (auto it = mGoldenSttMap.begin(); it != mGoldenSttMap.end(); ++it)
        {
            if (it->second->get_commissionedunituid() == unitUid)
            {
                vRtGoldensttList.push_back(it->second);
            }
        }
    }
    return TPS_ER_SUCCESS;
}

void TpsMachineManager::GetAllApproveRecord(std::vector<RtApproverecord*>& approveRecordList)
{
    approveRecordList.clear();
    approveRecordList.resize(mApproveRecordMap.size());
    std::transform(mApproveRecordMap.begin(), mApproveRecordMap.end(), approveRecordList.begin(),
        [](const std::map<std::string, RtApproverecord*>::value_type& item)
    {
        return item.second;
    });
}

void TpsMachineManager::AddApproveRecord(RtApproverecord* approverecord)
{
    if (approverecord == nullptr) return;

    auto it = mApproveRecordMap.find(approverecord->get_uid());
    if (it != mApproveRecordMap.end())
    {
        TPS_LOG_TRACE_INFO << "The approverecord already exists, replace it.";
        DEL_PTR(it->second);
    }
    mApproveRecordMap[approverecord->get_uid()] = approverecord;
}


void TpsMachineManager::SetLeafGapOffsetInSmallField(const float& leafGapOffsetInSmallField)
{
    mLeafGapOffsetInSmallField = leafGapOffsetInSmallField;
}

float TpsMachineManager::GetLeafGapOffsetInSmallField() const
{
    return mLeafGapOffsetInSmallField;
}

void TpsMachineManager::SetLeafGapOffsetInLargeField(const float& leafGapOffsetInLargeField)
{
    mLeafGapOffsetInLargeField = leafGapOffsetInLargeField;
}

float TpsMachineManager::GetLeafGapOffsetInLargeField() const
{
    return mLeafGapOffsetInLargeField;
}


TPS_END_NAMESPACE  // end namespace tps
