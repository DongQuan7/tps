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
#ifndef TPS_OPTGOAL_MANAGER_H_
#define TPS_OPTGOAL_MANAGER_H_

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE

class RtDoseoptgoalbase;

class TPS_DA_DECLSPEC TpsOptGoalManager : public TpsDataManagerBase
{
public:
    TpsOptGoalManager(void);
    ~TpsOptGoalManager(void);

    bool AddOptGoal(const RtDoseoptgoalbase& optGoal);
    bool RemoveOptGoal(const std::string& optGoalUid);
    bool UpdateOptGoal(const RtDoseoptgoalbase& optGoal);
    bool GetOptGoal(std::string optGoalUid, RtDoseoptgoalbase* optGoal);
    bool GetOptGoalListByNormalizationGroupUID(std::string normgroupUid, std::vector<RtDoseoptgoalbase*>& optGoals);

    void ClearPatientData();


private:
    std::map<std::string, RtDoseoptgoalbase*> mOptGoalMap;
};
TPS_END_NAMESPACE

#endif