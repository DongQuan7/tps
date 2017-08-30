//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file mlc_shape_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/18
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_MLC_SAHPE_CONVERTER_H_
#define TPS_DP_MLC_SAHPE_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC MlcShapeConverter
{
public:
    static bool ConvertToALG(const RtMlcshape& tpsMlcShape, std::shared_ptr<RTFWK::RTFWKDLDOMLCShape>* algMlcShape);
    static bool ConvertToTPS(RTFWK::RTFWKDLDOMLCShape& algMlcShape, RtMlcshape* tpsMlcShape);
};
TPS_END_NAMESPACE
#endif