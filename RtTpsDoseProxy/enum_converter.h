//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2017
///  All rights reserved.
/// 
///  \author  Wang  zhendong.wang@united-imaging.com
/// 
///  \file enum_converter.h
/// 
///  \version 1.0
/// 
///  \date    2017/01/03
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_ENUM_CONVERTER_H_
#define TPS_DP_ENUM_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/alg_common_algreturn.h"


TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC EnumConverter
{
public:
    static TPS_ERROR_CODES AlgConvertToTPS(ALG_RETURN_TYPE algType);
};
TPS_END_NAMESPACE
#endif