//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  xiaoqing.shangguan xiaoqing.shangguan@united-imaging.com
///
///  \file    tps_da_optgoalmanager.h
///  \brief
///
///  \version 1.0
///  \date    April.29,2013
///  \{
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_optgoalmanager.h"

// TPS
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseoptgoalbase.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE


TpsOptGoalManager::TpsOptGoalManager(void) {
}


TpsOptGoalManager::~TpsOptGoalManager(void) {
	auto itOptGoal = mOptGoalMap.begin();
	while (itOptGoal != mOptGoalMap.end()) {
		delete itOptGoal->second;
		itOptGoal->second = nullptr;
		itOptGoal++;
	}

	mOptGoalMap.clear();
}

bool TpsOptGoalManager::AddOptGoal(const RtDoseoptgoalbase& optGoal){
    std::map<std::string, RtDoseoptgoalbase*>::iterator optIt = mOptGoalMap.find(optGoal.get_uid());
    if (optIt != mOptGoalMap.end()){
        TPS_LOG_DEV_ERROR<<"TpsOptGoalManager already has: " << optGoal.get_uid();
        return false;
    }

    RtDoseoptgoalbase* pOptGoal = new RtDoseoptgoalbase(optGoal);
    mOptGoalMap[optGoal.get_uid()] = pOptGoal;
    return true;
}

bool TpsOptGoalManager::RemoveOptGoal(const std::string& optGoalUid){
    std::map<std::string, RtDoseoptgoalbase*>::iterator optIt = mOptGoalMap.find(optGoalUid);
    if (optIt == mOptGoalMap.end()){
        TPS_LOG_DEV_ERROR<<"TpsOptGoalManager dose not has: " << optGoalUid;
        return false;
    }
    if (optIt->second){
        delete optIt->second;
        optIt->second = nullptr;
    }
    mOptGoalMap.erase(optIt);
    return true;
}

bool TpsOptGoalManager::UpdateOptGoal(const RtDoseoptgoalbase& optGoal){
    std::map<std::string, RtDoseoptgoalbase*>::iterator optIt = mOptGoalMap.find(optGoal.get_uid());
    if (optIt == mOptGoalMap.end()){
        TPS_LOG_DEV_ERROR<<"TpsOptGoalManager dose not has: " << optGoal.get_uid();
        return false;
    }
    if (optIt->second){
        delete optIt->second;
        RtDoseoptgoalbase* pOptGoal = new RtDoseoptgoalbase(optGoal);
        optIt->second = pOptGoal;
    }

    return true;
}

bool TpsOptGoalManager::GetOptGoal(std::string optGoalUid, RtDoseoptgoalbase* optGoal){
    std::map<std::string, RtDoseoptgoalbase*>::iterator optIt = mOptGoalMap.find(optGoalUid);
    if (optIt == mOptGoalMap.end()){
        TPS_LOG_DEV_ERROR<<"TpsOptGoalManager dose not has: " << optGoalUid;
        return false;
    }
    optGoal = optIt->second;
    return true;
}

bool TpsOptGoalManager::GetOptGoalListByNormalizationGroupUID(std::string normgroupUid, 
    std::vector<RtDoseoptgoalbase*>& optGoals){
    try{
        std::map<std::string, RtDoseoptgoalbase*>::iterator optIt = mOptGoalMap.begin();
        for(; optIt != mOptGoalMap.end(); ++optIt){
            RtDoseoptgoalbase* optGoal = optIt->second;
            if (normgroupUid == optGoal->get_normgroupuid()){
                optGoals.push_back(optGoal);
            }
        }
    }
    catch (std::exception ex){
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
    return true;
}

void TpsOptGoalManager::ClearPatientData()
{
    for (auto it = mOptGoalMap.begin(); it != mOptGoalMap.end(); ++it)
    {
        delete it->second;
    }
    mOptGoalMap.clear();
}

TPS_END_NAMESPACE