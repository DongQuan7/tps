//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file segment_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/18
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_SEGMENT_CONVERTER_H_
#define TPS_DP_SEGMENT_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_segment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC SegmentConverter
{
public:
    static bool ConvertToALG(const RtBeamsegment* source, RTFWK::RTFWKDLDOBeamsegment* target);

    static bool ConvertToTPS(RtBeamsegment* tpsSegment, RTFWK::RTFWKDLDOBeamsegment& algSegment);

    static bool CreateALGSegmentList(std::vector<RtBeamsegment*>& source, std::vector<RTFWK::RTFWKDLDOBeamsegment*>& target);

    static bool ConvertToTPSSegmentList(
        std::vector<RtBeamsegment*>* tpsSegmentList,
        std::vector<RTFWK::RTFWKDLDOBeamsegment*>& algSegmentList);

    static bool Convert(RTFWK::RTFWKDLDOBeamsegment* source, RtBeamsegment* target);
    static bool ConvertRange(std::vector<RTFWK::RTFWKDLDOBeamsegment*>& source, std::vector<RtBeamsegment*>& target);
};
TPS_END_NAMESPACE
#endif