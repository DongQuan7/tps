//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_helper.h
/// 
///  \version 1.1
/// 
///  \date    2017/05/05
//////////////////////////////////////////////////////////////////////////

#include "tps_bl_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsFramework/tps_fw_common_enums.h"



#ifndef TPS_BL_CMD_HELPER_H_
#define TPS_BL_CMD_HELPER_H_


TPS_BEGIN_NAMESPACE  // begin namespace TPS

class RtMachine;
class RtBeam;
class RtCommissionedunit;


class TPS_BL_DECLSPEC TpsCmdHelper
{
public:
    static PatientPosition GetPatientPosition(std::string sPP);

    static bool IsArcBeam(RtDbDef::PLAN_TYPE planType);

    static bool IsImrtBeam(RtDbDef::PLAN_TYPE planType);

    static bool ValidateMachineMlcJawParameter(RtMachine* preMachine, RtMachine* curMachine);

    static bool ValidateMachineGeometryParameter(RtBeam* beam, RtMachine* machine, RtDbDef::PLAN_TYPE planType);

    static bool CommissionedUnitComparision(RtCommissionedunit* srcCommission, RtCommissionedunit* compareCommssion);

    static RtDbDef::BEAM_TYPE GetBeamTypeViaPlanType(RtDbDef::PLAN_TYPE planType);
};

TPS_END_NAMESPACE  // end namespace TPS

#endif  //TPS_BL_CMD_HELPER_H_
