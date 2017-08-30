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

#ifndef TPS_DP_ACCESSORY_CHUNK_CONVERTER_H_
#define TPS_DP_ACCESSORY_CHUNK_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_accessorychunk.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessorychunk.h"

TPS_BEGIN_NAMESPACE

class RtContour;

class TPS_DP_DECLSPEC AccessoryChunkConverter
{
public:
    static bool ConvertToALG(
        const RtAccessorychunk& tpsAccessorychunk,
        const RtContour* profile,
        std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk>* algAccessorychunk);

    static bool CreateALGList(
        std::vector<RtAccessorychunk*>& tpsAccessorychunkList,
        std::vector<RtContour*>& vProfile,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk>>* algAccessorychunkList);

private:

    static void FillMcPhoOffAxis_i(
        std::vector<RTFWK::RTFWKDLDOMcPhoOffAxis*> &vMcPhoOffAxis,
        std::string sMcPhoOffAxis);
};
TPS_END_NAMESPACE
#endif