//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file contour_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/10/29
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_CONTOUR_CONVERTER_H_
#define TPS_DP_CONTOUR_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC ContourConverter
{
public:

    static bool ConvertToALG(const RtContour& tpsContour, std::shared_ptr<RTFWK::RTFWKDLDOContour>* algContour);

    static bool CreateALGContourList(
        std::vector<RtContour*>& tpsContourList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOContour>>* algContourList);
};
TPS_END_NAMESPACE
#endif