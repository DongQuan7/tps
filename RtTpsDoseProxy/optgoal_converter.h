//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file optgoal_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/10/28
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_OPTGOAL_CONVERTER_H_
#define TPS_DP_OPTGOAL_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptgoalbase.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseoptgoalbase.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC OptGoalConverter
{
public:
    static bool ConvertToALG(
        const RtDoseoptgoalbase& tpsOptGoal, std::shared_ptr<RTFWK::RTFWKDLDODoseOptGoalBase>* algOptGoal);

    static bool ConvertToTPS(RTFWK::RTFWKDLDODoseOptGoalBase& algOptGoal, RtDoseoptgoalbase* tpsOptGoal);

    static bool CreateALGOptGoalList(
        std::vector<RtDoseoptgoalbase*>& tpsOptGoalList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDODoseOptGoalBase>>* algOptGoalList);

    static bool ConvertToTPSOptGoalList(
        std::vector<RTFWK::RTFWKDLDODoseOptGoalBase*>& algOptGoalList,
        std::vector<RtDoseoptgoalbase*>* tpsOptGoalList);
};
TPS_END_NAMESPACE
#endif