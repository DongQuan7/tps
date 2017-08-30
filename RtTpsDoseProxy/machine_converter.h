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

#ifndef TPS_DP_MACHINE_CONVERTER_H_
#define TPS_DP_MACHINE_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"

namespace RTFWK
{
    class RTFWKDLDOMLCSetting;
    class RTFWKDLDOMachine;
    class RTFWKDLDOTray;
    class RTFWKDLDOAccessory;
}

TPS_BEGIN_NAMESPACE

class RtTray;
class RtMachine;
class MachineSuit;
class RtAccessory;


class TPS_DP_DECLSPEC MachineConverter
{
public:
    static bool ConvertToALG(
        const RtMachine& tpsMachine,
        std::vector<RtTray*>& vTray,
        std::shared_ptr<RTFWK::RTFWKDLDOMachine>* algMachine);

    static bool CreateALGList(
        std::vector<RtMachine*>& tpsMachineList,
        std::vector<RtTray*>& vTray,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMachine>>* algMachineList);

    static bool ConvertToALG(MachineSuit* source, std::shared_ptr<RTFWK::RTFWKDLDOMachine>& target);

    static bool ConvertToALG(RtTray* source, std::shared_ptr<RTFWK::RTFWKDLDOTray>& target);

    static bool ConvertToALG(RtMachine* source, std::shared_ptr<RTFWK::RTFWKDLDOMLCSetting>& target);

    static bool ConvertToALG(RtAccessory* source, std::shared_ptr<RTFWK::RTFWKDLDOAccessory>& target);
private:
    static void SetMLCSetting_i(const RtMachine& tpsMachine, std::shared_ptr<RTFWK::RTFWKDLDOMLCSetting>* algMlcSetting);
private:
};
TPS_END_NAMESPACE
#endif