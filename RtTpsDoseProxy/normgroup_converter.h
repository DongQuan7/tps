//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file normgroup_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_NORMGROUP_CONVERTER_H_
#define TPS_DP_NORMGROUP_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC NormGroupConverter
{
public:
    static bool ConvertToALG(
        const RtNormgroup& tpsNormGroup, std::shared_ptr<RTFWK::RTFWKDLDONormgroup>* algNormGroup);

    static bool ConvertToTPS(
        RTFWK::RTFWKDLDONormgroup& algNormGroup, RtNormgroup* tpsNormGroup);

    static bool CreateALGNormgroupList(
        std::vector<RtNormgroup*>& tpsNormgroupList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDONormgroup>>* algNormgroupList);

    static bool Convert(RTFWK::RTFWKDLDONormgroup* source, RtNormgroup* target);

};
TPS_END_NAMESPACE
#endif