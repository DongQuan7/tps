//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file optgoal_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "optgoal_converter.h"

#include "segment_converter.h"
#include "dosegrid_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"

TPS_BEGIN_NAMESPACE

bool OptGoalConverter::ConvertToALG(
    const RtDoseoptgoalbase& tpsOptGoal, std::shared_ptr<RTFWK::RTFWKDLDODoseOptGoalBase>* algOptGoal)
{
    algOptGoal->reset(new RTFWK::RTFWKDLDODoseOptGoalBase(false));
    (*algOptGoal)->SetUID(tpsOptGoal.get_uid());
    (*algOptGoal)->SetA(tpsOptGoal.get_a());
    (*algOptGoal)->SetDose(tpsOptGoal.get_dose());
    (*algOptGoal)->SetDoseOptGoalType((RTFWK::DOSEOPTGOAL_TYPE)tpsOptGoal.get_doseoptgoaltype());
    (*algOptGoal)->SetDvhDose(tpsOptGoal.get_dvhdose());
    (*algOptGoal)->SetEUD(tpsOptGoal.get_eud());
    (*algOptGoal)->SetIsHardLimit(tpsOptGoal.get_ishardlimit());
    (*algOptGoal)->SetIsJoined(tpsOptGoal.get_isjoined());
    (*algOptGoal)->SetIsLowerBound(tpsOptGoal.get_islowerbound());
    (*algOptGoal)->SetMaxDose(tpsOptGoal.get_maxdose());
    (*algOptGoal)->SetMinDose(tpsOptGoal.get_mindose());
    (*algOptGoal)->SetNormgroupUID(tpsOptGoal.get_normgroupuid());
    (*algOptGoal)->SetVOIUID(tpsOptGoal.get_voiuid());
    (*algOptGoal)->SetVolume(tpsOptGoal.get_volume());
    (*algOptGoal)->SetVoiWeight(tpsOptGoal.get_voiweight());

    return true;
}

bool OptGoalConverter::ConvertToTPS(
    RTFWK::RTFWKDLDODoseOptGoalBase& algOptGoal, RtDoseoptgoalbase* tpsOptGoal)
{
    tpsOptGoal->set_uid(algOptGoal.GetUID());
    tpsOptGoal->set_a(algOptGoal.GetA());
    tpsOptGoal->set_dose(algOptGoal.GetDose());
    tpsOptGoal->set_doseoptgoaltype((int)algOptGoal.GetDoseOptGoalType());
    tpsOptGoal->set_dvhdose(algOptGoal.GetDvhDose());
    tpsOptGoal->set_eud(algOptGoal.GetEUD());
    tpsOptGoal->set_ishardlimit(algOptGoal.GetIsHardLimit());
    tpsOptGoal->set_isjoined(algOptGoal.GetIsJoined());
    tpsOptGoal->set_islowerbound(algOptGoal.GetIsLowerBound());
    tpsOptGoal->set_maxdose(algOptGoal.GetMaxDose());
    tpsOptGoal->set_mindose(algOptGoal.GetMinDose());
    tpsOptGoal->set_normgroupuid(algOptGoal.GetNormgroupUID());
    tpsOptGoal->set_voiuid(algOptGoal.GetVOIUID());
    tpsOptGoal->set_voiweight(algOptGoal.GetVoiWeight());
    tpsOptGoal->set_volume(algOptGoal.GetVolume());

    return true;
}

bool OptGoalConverter::CreateALGOptGoalList(
    std::vector<RtDoseoptgoalbase*>& tpsOptGoalList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDODoseOptGoalBase>>* algOptGoalList)
{
    std::vector<RtDoseoptgoalbase*>::iterator itTpsOptGoal = tpsOptGoalList.begin();
    for (; itTpsOptGoal != tpsOptGoalList.end(); ++itTpsOptGoal)
    {
        RtDoseoptgoalbase* tpsOptGoal = *itTpsOptGoal;
        std::shared_ptr<RTFWK::RTFWKDLDODoseOptGoalBase> algOptGoal;
        OptGoalConverter::ConvertToALG(*tpsOptGoal, &algOptGoal);
        algOptGoalList->push_back(algOptGoal);
    }
    return true;
}

bool OptGoalConverter::ConvertToTPSOptGoalList(
    std::vector<RTFWK::RTFWKDLDODoseOptGoalBase*>& algOptGoalList,
    std::vector<RtDoseoptgoalbase*>* tpsOptGoalList)
{
    std::vector<RtDoseoptgoalbase*>::iterator itOptGoal = tpsOptGoalList->begin();
    for (; itOptGoal != tpsOptGoalList->end(); ++itOptGoal)
    {
        RtDoseoptgoalbase* tpsOptGoal = *itOptGoal;
        std::vector<RTFWK::RTFWKDLDODoseOptGoalBase*>::iterator itAlgOptGoal = algOptGoalList.begin();
        for (; itAlgOptGoal != algOptGoalList.end(); ++itAlgOptGoal)
        {
            RTFWK::RTFWKDLDODoseOptGoalBase* algOptGoal = *itAlgOptGoal;
            if (tpsOptGoal->get_uid() == algOptGoal->GetUID())
            {
                OptGoalConverter::ConvertToTPS(*algOptGoal, tpsOptGoal);
                break;
            }
        }
    }
    return true;
}
TPS_END_NAMESPACE