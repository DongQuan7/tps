//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file goldersst_converter.h
/// 
///  \version 1.0
/// 
///  \date    2016/06/29
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_GOLDENSTT_CONVERTER_H_
#define TPS_DP_GOLDENSTT_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_goldenstt.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_goldenstt.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"


TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC GoldenSTTConverter 
{
public:
    static void ConvertToALG(
        const RtGoldenstt& tpsGoldenStt,
        std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT>* algGoldenSTT);

    static void CreateALGList(
        std::vector<RtGoldenstt*>& tpsGoldenSttList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT>>* algGoldenSTTList);
};
TPS_END_NAMESPACE
#endif