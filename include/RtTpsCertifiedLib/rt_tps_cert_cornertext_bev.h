//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenlong.zhao (mailto:wenlong.zhao@united-imaging.com)
///
///  \file   
///  \brief   
///
///  \version 1.0
///  \date  2015/04/29  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef RT_TPS_CORNERTEXT_BEV_H_
#define RT_TPS_CORNERTEXT_BEV_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto {
        class RT_TPS_CornerText_BEV;
}

struct CornerTextBEV
{
    std::string beamname;
    double beamweight;
    double beammu;
    int displaycontent; // 0, bev; 1, cev;
    double windowwidth;
    double windowcenter;
    double zoomfactor;
    double segmentindex;
    double segmentweight;
    double segmentmu;
    double leafindex;

    CornerTextBEV();
    
    void Convert(const proto::RT_TPS_CornerText_BEV& protoBuffer);

    void ConvertBack(proto::RT_TPS_CornerText_BEV* protoBuffer) const;
};
TPS_END_NAMESPACE  // end namespace tps
#endif