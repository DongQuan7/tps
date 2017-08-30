////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang  qiangqiang.zhou@united-imaging.com
/// 
///  \file rt_tps_certified_segment.h
/// 
///  \brief struct TpsCertifiedSegment declaration 
/// 
///  \version 1.0
/// 
///  \date    Mar 12, 2015
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIED_SEGMENT_H_
#define RT_TPS_CERTIFIED_SEGMENT_H_

#include "tps_defs.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

    namespace proto{
        class RtTpsProtoSegment;
}

//////////////////////////////////////////////////////////////////////////

struct TpsCertifiedJaw
{
    float x1;
    float x2;
    float y1;
    float y2;

    TpsCertifiedJaw();
};

struct TpsCertifiedMlcLeaf
{
    float x1;
    float x2;

    TpsCertifiedMlcLeaf();
};

// new certified segment
struct TpsCertifiedSegment
{
    std::string segmentUID;
    float segmentWeight;
    float segmentMu;
    std::string beamUID;
    int SequenceNumber;

    TpsCertifiedJaw startJaw;
    TpsCertifiedJaw endJaw;
    std::vector<TpsCertifiedMlcLeaf> startMlcLeaves;
    std::vector<TpsCertifiedMlcLeaf> endMlcLeaves;

    float startSSD;
    float endSSD;

    TpsCertifiedSegment();

    void Convert(const proto::RtTpsProtoSegment& protoSegment);

    void ConvertBack(proto::RtTpsProtoSegment* protoSegment) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIED_SEGMENT_H_
