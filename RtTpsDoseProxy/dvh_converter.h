//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file dvh_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/20
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_DVH_CONVERTER_H_
#define TPS_DP_DVH_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dvh.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dvh.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC DVHConverter
{
public:
    //static bool ConvertToALG(const RtDvh& tpsDVH, std::shared_ptr<RTFWK::RTFWKDLDODVH>* algDVH);

    static bool ConvertToTPS(const RTFWK::RTFWKDLDODVH& algDVH, ITpsDataRepository *dataRepository, RtDvh* tpsDVH);

   /* static bool CreateALGDVHList(
        std::vector<RtDvh*>& tpsDVHList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDODVH>>* algDVHList);*/

    static bool ConvertToTPSDvhList(
        std::vector<RTFWK::RTFWKDLDODVH*>& algDVHList,
        ITpsDataRepository *dataRepository,
        std::vector<RtDvh*>* tpsDVHList);
};
TPS_END_NAMESPACE
#endif