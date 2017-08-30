//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_helper.cpp
/// 
///  \version 1.1
/// 
///  \date    2017/05/05
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"

#include <string.h>
#include <stdio.h>
#include "boost/algorithm/string.hpp"

TPS_BEGIN_NAMESPACE

PatientPosition TpsCmdHelper::GetPatientPosition(std::string sPP)
{
    boost::to_upper(sPP);
    PatientPosition pp = UNKNOWN_POSITION;
    if (sPP == "HFS")       { pp = HFS;  }
    else if (sPP == "HFP" ) { pp = HFP;  }
    else if (sPP == "HFP" ) { pp = HFP;  }
    else if (sPP == "HFDR") { pp = HFDR; }
    else if (sPP == "HFDL") { pp = HFDL; }
    else if (sPP == "FFS" ) { pp = FFS;  }
    else if (sPP == "FFP" ) { pp = FFP;  }
    else if (sPP == "FFDR") { pp = FFDR; }
    else if (sPP == "FFDL") { pp = FFDL; }
    return pp;
}

bool TpsCmdHelper::IsArcBeam(RtDbDef::PLAN_TYPE planType)
{
    bool bIsArc = false;
    switch (planType)
    {
        case RtDbDef::PLAN_Simple_Arc:
        case RtDbDef::PLAN_Conformal_Arc:
        case RtDbDef::PLAN_VMAT_Arc:
        case RtDbDef::PLAN_Burst_Arc:
            bIsArc = true;
            break;
        default:
            break;
    }

    return bIsArc;
}

bool TpsCmdHelper::IsImrtBeam(RtDbDef::PLAN_TYPE planType)
{
    bool bIsImrt = false;
    switch (planType)
    {
        case RtDbDef::PLAN_Step_Shoot:
        case RtDbDef::PLAN_Sliding_Window:
            bIsImrt = true;
            break;
        default:
            break;
    }

    return bIsImrt;
}

bool TpsCmdHelper::ValidateMachineMlcJawParameter(RtMachine* preMachine, RtMachine* curMachine)
{
    // 校验机器的Jaw/MLC等核心参数是否发生变化
    if (preMachine->get_xjawnum() != curMachine->get_xjawnum()
        || preMachine->get_yjawnum() != curMachine->get_yjawnum())
    {
        return false;
    }

    if (!TPS_FLOAT_COMPARE(preMachine->get_xmin_1(), curMachine->get_xmin_1())
        || !TPS_FLOAT_COMPARE(preMachine->get_xmax_1(), curMachine->get_xmax_1())
        || !TPS_FLOAT_COMPARE(preMachine->get_xmin_2(), curMachine->get_xmin_2())
        || !TPS_FLOAT_COMPARE(preMachine->get_xmax_2(), curMachine->get_xmax_2())
        || !TPS_FLOAT_COMPARE(preMachine->get_ymin_1(), curMachine->get_ymin_1())
        || !TPS_FLOAT_COMPARE(preMachine->get_ymax_1(), curMachine->get_ymax_1())
        || !TPS_FLOAT_COMPARE(preMachine->get_ymin_2(), curMachine->get_ymin_2())
        || !TPS_FLOAT_COMPARE(preMachine->get_ymax_2(), curMachine->get_ymax_2()))
    {
        return false;
    }

    if (!TPS_FLOAT_COMPARE(preMachine->get_xmin_w(), curMachine->get_xmin_w())
        || !TPS_FLOAT_COMPARE(preMachine->get_xmax_w(), curMachine->get_xmax_w())
        || !TPS_FLOAT_COMPARE(preMachine->get_ymin_w(), curMachine->get_ymin_w())
        || !TPS_FLOAT_COMPARE(preMachine->get_ymax_w(), curMachine->get_ymax_w())
        || !TPS_FLOAT_COMPARE(preMachine->get_jawtrackinggap(), curMachine->get_jawtrackinggap()))
    {
        return false;
    }

    if (preMachine->get_leafnum() != curMachine->get_leafnum()
        || !TPS_FLOAT_COMPARE(preMachine->get_minleafendgap(), curMachine->get_minleafendgap())
        || !TPS_FLOAT_COMPARE(preMachine->get_minimumjawgap(), curMachine->get_minimumjawgap())
        || !TPS_FLOAT_COMPARE(preMachine->get_negativeleafrangemax(), curMachine->get_negativeleafrangemax())
        || !TPS_FLOAT_COMPARE(preMachine->get_negativeleafrangemin(), curMachine->get_negativeleafrangemin())
        || !TPS_FLOAT_COMPARE(preMachine->get_positiveleafrangemax(), curMachine->get_positiveleafrangemax())
        || !TPS_FLOAT_COMPARE(preMachine->get_positiveleafrangemin(), curMachine->get_positiveleafrangemin())
        || !TPS_FLOAT_COMPARE(preMachine->get_leafspan(), curMachine->get_leafspan())
        || !TPS_FLOAT_COMPARE(preMachine->get_mlcdirectiontype(), curMachine->get_mlcdirectiontype())
        || !TPS_FLOAT_COMPARE(preMachine->get_isinterdigitation(), curMachine->get_isinterdigitation()))
    {
        return false;
    }

    return true;
}

bool TpsCmdHelper::ValidateMachineGeometryParameter(RtBeam* beam, RtMachine* machine, RtDbDef::PLAN_TYPE planType)
{
    if (beam == nullptr || machine == nullptr || planType == RtDbDef::PLAN_UNKNOWN_TYPE)
    {
        TPS_LOG_DEV_ERROR << "Input parameter is illegal";
        return false;
    }

    std::vector<RtBeamsegment*> segmentList = beam->get_beamsegments();
    RtBeamsegment* segment = segmentList[0];

    if (segment->get_startgantryangle() < machine->get_gantrymin()
        || segment->get_startgantryangle() > machine->get_gantrymax())
    {
        return false;
    }

    if (machine->get_tableanglemin() < machine->get_tableanglemax())
    {
        if (segment->get_starttablerotationangle() < machine->get_tableanglemin()
            || segment->get_starttablerotationangle() > machine->get_tableanglemax())
        {
            return false;
        }
    }
    else
    {
        double tablemin = machine->get_tableanglemin() > 180 ? machine->get_tableanglemin() - 360 : machine->get_tableanglemin();
        double tablemax = machine->get_tableanglemax() > 180 ? machine->get_tableanglemax() - 360 : machine->get_tableanglemax();
        double tableangle = segment->get_starttablerotationangle();
        tableangle = tableangle > 180  ? tableangle - 360 : tableangle;
        if (tableangle < tablemin || tableangle > tablemax)
        {
            return false;
        }
    }

    if (segment->get_startcollimatorangle() < machine->get_collimatoranglemin()
        || segment->get_startcollimatorangle() > machine->get_collimatoranglemax())
    {
        return false;
    }

    if (TpsCmdHelper::IsArcBeam(planType))
    {
        if (segment->get_gantryrotationdirection() == RtDbDef::CW)
        {
            if (segment->get_arclength() > machine ->get_gantryclockwiserange())
            {
                return false;
            }
        }
        else
        {
            if (segment->get_arclength() > machine->get_gantrycounterclockwiserange())
            {
                return false;
            }
        }
    }

    return true;
}
bool TpsCmdHelper::CommissionedUnitComparision(RtCommissionedunit* srcCommission, RtCommissionedunit* compareCommssion)
{
    std::vector<float> doserateList = srcCommission->get_discretedoserate();
    if (doserateList.size() < 1)
    {
        return false;
    }

    if (!srcCommission->get_ispencilebeamsupport()
        && !srcCommission->get_isconvolutionsupport()
        && !srcCommission->get_ismontecarlosupport())
    {
        return false;
    }

    if (srcCommission->get_radiationtype() == compareCommssion->get_radiationtype()
        && srcCommission->get_energy() == compareCommssion->get_energy()
        && srcCommission->get_isflattened() == compareCommssion->get_isflattened())
    {
        return true;
    }
    else
    {
        return false;
    }
}

RtDbDef::BEAM_TYPE TpsCmdHelper::GetBeamTypeViaPlanType(RtDbDef::PLAN_TYPE planType)
{
    RtDbDef::BEAM_TYPE beamType = RtDbDef::BEAM_UNKNOWN_TYPE;
    switch (planType)
    {
        case RtDbDef::PLAN_Sliding_Window:
            beamType = RtDbDef::DYNAMICIMRT;
            break;
        case RtDbDef::PLAN_3D_CRT:
        case RtDbDef::PLAN_Step_Shoot:
        case RtDbDef::PLAN_2D:
            beamType = RtDbDef::STATICIMRT;
            break;
        case RtDbDef::PLAN_Burst_Arc:
        case RtDbDef::PLAN_Simple_Arc:
            beamType = RtDbDef::BURST_ARC;
            break;
        case RtDbDef::PLAN_Conformal_Arc:
        case RtDbDef::PLAN_VMAT_Arc:
            beamType = RtDbDef::VMAT;
            break;
        default:
            TPS_LOG_DEV_ERROR << "Unrecognized plan type: " << (int)planType;
    }

    return beamType;
}

TPS_END_NAMESPACE
