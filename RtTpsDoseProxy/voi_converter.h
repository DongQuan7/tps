//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file voi_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_VOI_CONVERTER_H_
#define TPS_DP_VOI_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC VoiConverter
{
public:
    static bool ConvertToALG(const RtVoi& tpsVoi, std::shared_ptr<RTFWK::RTFWKDLDOVOI>* algVoi);

    static bool ConvertToTPS(RTFWK::RTFWKDLDOVOI& algVoi, RtVoi* tpsVoi);

    static bool CreateALGVoiList(
        std::vector<RtVoi*>& tpsVoiList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>>* algVoiList);

    static bool ConvertToTPSVoiList(
        std::vector<RTFWK::RTFWKDLDOVOI*>& algVoiList,
        std::vector<RtVoi*>* tpsVoiList);
};
TPS_END_NAMESPACE
#endif