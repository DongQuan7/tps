//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file beam_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_COMMISSIONED_UNIT_CONVERTER_H_
#define TPS_DP_COMMISSIONED_UNIT_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"
#include "alg/alg_commission.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmachinesetting.h"

namespace RTFWK{
    class RTFWKDLDOCommissionedunit;
    class RTFWKDLDOMachineUnit;
    class RTFWKDLDOSartable;
    class RTFWKDLDOTimecalParameters;
    class RTFWKDLDOConvolutionData;
    class RTFWKDLDOConvDataStruct;
    class RTFWKDLDOKernelWeight;
    class RTFWKDLDOMeasuredData;
	class RTFWKDLDOWedgeFactor;
}

TPS_BEGIN_NAMESPACE

class CommissionUintSuit;
class RtKernelweight;
class RtMeasureddata;
class RtWedgefactor;

class TPS_DP_DECLSPEC CommissionedUnitConverter
{
public:
    static bool ConvertToALG(
        const RtCommissionedunit& tpsCommissionedUnit,
        std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>* algCommissionedUnit);

    static bool CreateALGList(
        std::vector<RtCommissionedunit*>& tpsCommissionedUnitList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>>* algCommissionedUnitList);

    static bool ConvertToALG(CommissionUintSuit* sourceSuit, std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>& target);
    static bool ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOMachineUnit>& target);
    static bool ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOSartable>& target);
    static bool ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOTimecalParameters>& target);
    static bool ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOConvolutionData>& target);
    static bool ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOConvDataStruct>& target);
    static bool ConvertToALG(RtKernelweight* source, std::shared_ptr<RTFWK::RTFWKDLDOKernelWeight>& target);
    static bool ConvertToALG(RtMeasureddata* source, std::shared_ptr<RTFWK::RTFWKDLDOMeasuredData>& target);
    static bool ConvertToALG(RtWedgefactor* source, std::shared_ptr<RTFWK::RTFWKDLDOWedgeFactor>& target);
    static bool ALGConvertToDB(const RTFWK::RTFWKDLDOTimecalParameters& source, RtCommissionedunit& target);
    static bool ConvertToALG(std::vector<RTALG::EnergyAndWeight>energyWeight, std::vector<EnergyAndWeight>&SpectrumCollection);
private:
    static void SetMachineUnit_i(
        const RtCommissionedunit& tpsCommissionedUnit,
        RTFWK::RTFWKDLDOMachineUnit* machineUnit);

    static void SetSarTable_i(
        const RtCommissionedunit& tpsCommissionedUnit,
        RTFWK::RTFWKDLDOSartable* sar);

    static void SetTimecal_i(
        const RtCommissionedunit& tpsCommissionedUnit,
        RTFWK::RTFWKDLDOTimecalParameters* pTimecal);

    static void SetConvolutionData_i(
        const RtCommissionedunit& tpsCommissionedUnit,
        RTFWK::RTFWKDLDOConvolutionData* pConvolutionData);
   
};
TPS_END_NAMESPACE
#endif