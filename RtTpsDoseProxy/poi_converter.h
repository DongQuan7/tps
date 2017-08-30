//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file poi_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_POI_CONVERTER_H_
#define TPS_DP_POI_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC PoiConverter
{
public:
    static bool ConvertToALG(const RtPoi& tpsPoi, std::shared_ptr<RTFWK::RTFWKDLDOPOI>* algPoi);

    static bool CreateALGPoiList(
        std::vector<RtPoi*>& tpsPoiList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOPOI>>* algPoiList);

    static bool ConvertToTPS(RTFWK::RTFWKDLDOPOI& algPoi,
        std::map<std::string, std::map<std::string, float> > mBeamDoseContributionPerMuToPOI, RtPoi* tpsPoi);

    static bool ConvertToTPSPoiList(std::vector<RTFWK::RTFWKDLDOPOI*>& algPoiList,
        std::map<std::string,std::map<std::string, float> > mBeamDoseContributionPerMuToPOI,
        std::vector<RtPoi*>* tpsPoiList);
};
TPS_END_NAMESPACE
#endif