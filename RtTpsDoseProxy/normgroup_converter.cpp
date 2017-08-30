//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file normgroup_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "normgroup_converter.h"
#include "dosegrid_converter.h"

TPS_BEGIN_NAMESPACE

bool NormGroupConverter::ConvertToALG(const RtNormgroup& tpsNormGroup, std::shared_ptr<RTFWK::RTFWKDLDONormgroup>* algNormGroup)
{
    algNormGroup->reset(new RTFWK::RTFWKDLDONormgroup(false));
    (*algNormGroup)->SetUID(tpsNormGroup.get_uid());
    (*algNormGroup)->SetName(tpsNormGroup.get_name());
    (*algNormGroup)->SetDosePerFraction(tpsNormGroup.get_doseperfraction());
    (*algNormGroup)->SetFraction(tpsNormGroup.get_fraction());
    (*algNormGroup)->SetIsNorm2VOI(tpsNormGroup.get_isnorm2voi());
    (*algNormGroup)->SetIsNorm2VOIMeanDose(tpsNormGroup.get_isnorm2voimeandose());
    (*algNormGroup)->SetMachineUID(tpsNormGroup.get_machineuid());
    (*algNormGroup)->SetNormalPointPoiUID(tpsNormGroup.get_normalpointpoiuid());
    (*algNormGroup)->SetPercentage(tpsNormGroup.get_percentage());
    (*algNormGroup)->SetPlanType((RTFWK::PLAN_TYPE)tpsNormGroup.get_plantype());
    (*algNormGroup)->SetPlanUID(tpsNormGroup.get_planuid());
    (*algNormGroup)->SetRadiationType((RTFWK::RADIATION_TYPE)tpsNormGroup.get_radiationtype());
    RtDosegrid* tpsDosegrid = tpsNormGroup.get_dosegrid();

    RTFWK::RTFWKDLDODoseGrid* algDosegrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*tpsDosegrid, &doseGridCarrier);
    algDosegrid = doseGridCarrier.get();
    (*algNormGroup)->SetDoseGrid(*algDosegrid);
    (*algNormGroup)->SetNormVOIPercentageVolume(tpsNormGroup.get_normvoipercentagevolume());
    (*algNormGroup)->SetFraction(tpsNormGroup.get_fraction());
    (*algNormGroup)->SetVOIUID(tpsNormGroup.get_voiuid());
    return true;
}

bool NormGroupConverter::CreateALGNormgroupList(
    std::vector<RtNormgroup*>& tpsNormgroupList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDONormgroup>>* algNormgroupList)
{
    std::vector<RtNormgroup*>::iterator itTpsNormgroup = tpsNormgroupList.begin();
    for (; itTpsNormgroup != tpsNormgroupList.end(); ++itTpsNormgroup)
    {
        RtNormgroup* tpsNormgroup = *itTpsNormgroup;
        std::shared_ptr<RTFWK::RTFWKDLDONormgroup> algNormgroup;
        NormGroupConverter::ConvertToALG(*tpsNormgroup, &algNormgroup);
        algNormgroupList->push_back(algNormgroup);
    }
    return true;
}

bool NormGroupConverter::ConvertToTPS(RTFWK::RTFWKDLDONormgroup& algNormGroup, RtNormgroup* tpsNormGroup)
{
    tpsNormGroup->set_uid(algNormGroup.GetUID());
    tpsNormGroup->set_doseperfraction(algNormGroup.GetDosePerFraction());
    tpsNormGroup->set_fraction(algNormGroup.GetFraction());
    tpsNormGroup->set_percentage(algNormGroup.GetPercentage());
    RtDosegrid* tpsDosegrid = tpsNormGroup->get_dosegrid();
    RTFWK::RTFWKDLDODoseGrid algDosegrid = algNormGroup.GetDoseGrid();
    DosegridConverter::ConvertToTPS(algDosegrid, tpsDosegrid);
    
    return true;
}

bool NormGroupConverter::Convert(RTFWK::RTFWKDLDONormgroup* source, RtNormgroup* target)
{
    if (source == nullptr || target == nullptr)
    {
        return false;
    }

    target->set_doseperfraction(source->GetDosePerFraction());
    target->set_fraction(source->GetFraction());
    target->set_percentage(source->GetPercentage());

    return true;
}

TPS_END_NAMESPACE