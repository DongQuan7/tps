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

#ifndef TPS_DP_ACCESSORY_CONVERTER_H_
#define TPS_DP_ACCESSORY_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC AccessoryConverter
{
public:
    static bool ConvertToALG(
        const RtAccessory& tpsAccessory,
        std::shared_ptr<RTFWK::RTFWKDLDOAccessory>* algAccessory);

    static bool CreateALGList(
        std::vector<RtAccessory*>& tpsAccessoryList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessory>>* algAccessoryList);
};
TPS_END_NAMESPACE
#endif