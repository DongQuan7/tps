//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file block_converter.h
/// 
///  \version 1.0
/// 
///  \date    2016/05/05
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_BLOCK_CONVERTER_H_
#define TPS_DP_BLOCK_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC BlockConverter
{
public:
    static bool ConvertToALG(const RtBlock& tpsBlock, std::shared_ptr<RTFWK::RTFWKDLDOBlock>* algBlock);

    static bool CreateALGBlockList(
        std::vector<RtBlock*>& tpsBlockList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBlock>>* algBlockList);

};
TPS_END_NAMESPACE
#endif