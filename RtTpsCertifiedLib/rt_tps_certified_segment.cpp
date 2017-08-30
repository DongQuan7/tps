////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang  qiaangqiang.zhou@united-imaging.com
/// 
///  \file rt_tps_certified_segment.cpp
/// 
///  \brief struct TpsCertifiedSegment implementation
/// 
///  \version 1.0
/// 
///  \date    Mar 12, 2015
////////////////////////////////////////////////////////////////

#include "RtTpsCertifiedLib/rt_tps_certified_segment.h"
#include "RtTpsProtoLib/rt_tps_proto_segment.pb.h"//new

TPS_BEGIN_NAMESPACE  // begin namespace tps

    TpsCertifiedJaw::TpsCertifiedJaw()
    : x1(0.0f), x2(0.0f), y1(0.0f), y2(0.0f) {
}

TpsCertifiedMlcLeaf::TpsCertifiedMlcLeaf() 
    : x1(0.0f), x2(0.0f) {
}

//initialization is necessary in MLC Auto-shape
TpsCertifiedSegment::TpsCertifiedSegment()
{
    segmentUID = "";
    segmentWeight = 0.f;
    segmentMu = 0.f;
    beamUID = "";
    SequenceNumber = 0;
    startSSD = 0.0f;
    endSSD = 0.0f;
}

void TpsCertifiedSegment::Convert(const proto::RtTpsProtoSegment& protoSegment) 
{
    segmentUID = protoSegment.segmentuid();
    segmentWeight = protoSegment.segmentweight();
    segmentMu = protoSegment.segmentmu();
    beamUID = protoSegment.beamuid();
    SequenceNumber = protoSegment.sequencenumber();

    startJaw.x1 = protoSegment.startjaw().x1();
    startJaw.x2 = protoSegment.startjaw().x2();
    startJaw.y1 = protoSegment.startjaw().y1();
    startJaw.y2 = protoSegment.startjaw().y2();

    endJaw.x1 = protoSegment.endjaw().x1();
    endJaw.x2 = protoSegment.endjaw().x2();
    endJaw.y1 = protoSegment.endjaw().y1();
    endJaw.y2 = protoSegment.endjaw().y2();

    startSSD = protoSegment.startssd();
    endSSD = protoSegment.endssd();

    TpsCertifiedMlcLeaf mlcLeaf;
    for (int index = 0; index < protoSegment.startmlcleaves_size(); index++) {
        mlcLeaf.x1 = protoSegment.startmlcleaves(index).x1();
        mlcLeaf.x2 = protoSegment.startmlcleaves(index).x2();
        startMlcLeaves.push_back(mlcLeaf);
    }

    for (int index = 0; index < protoSegment.endmlcleaves_size(); index++) {
        mlcLeaf.x1 = protoSegment.endmlcleaves(index).x1();
        mlcLeaf.x2 = protoSegment.endmlcleaves(index).x2();
        endMlcLeaves.push_back(mlcLeaf);
    }
}

void TpsCertifiedSegment::ConvertBack(proto::RtTpsProtoSegment* protoSegment) const
{
    protoSegment;
}
TPS_END_NAMESPACE  // end namespace tps
