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
#include "RtTpsCertifiedLib/rt_tps_cert_cornertext_bev.h"
#include "RtTpsProtoLib/rt_tps_cornertext_bev.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
CornerTextBEV::CornerTextBEV()
{
    beamname="";
    beamweight=0.0;
    beammu=0.0;
    displaycontent=0; // 0, bev; 1, cev;
    windowwidth=0.0;
    windowcenter=0.0;
    zoomfactor=1.0;
    segmentindex=0;
    segmentweight=0.0;
    segmentmu=0.0;
    leafindex=0;
}

void CornerTextBEV::Convert(const proto::RT_TPS_CornerText_BEV& protoBuffer)
{
    beamname = protoBuffer.beamname();
    beammu = protoBuffer.beammu();
    beamweight = protoBuffer.beamweight();
    displaycontent = protoBuffer.displaycontent();
    windowwidth = protoBuffer.windowwidth();
    windowcenter = protoBuffer.windowcenter();
    zoomfactor = protoBuffer.zoomfactor();
    segmentindex = protoBuffer.segmentindex();
    segmentweight = protoBuffer.segmentweight();
    segmentmu = protoBuffer.segmentmu();
    leafindex = protoBuffer.leafindex();
}

void CornerTextBEV::ConvertBack(proto::RT_TPS_CornerText_BEV* protoBuffer) const
{
    if (protoBuffer!=nullptr)
    {
        protoBuffer->set_beamname(beamname);
        protoBuffer->set_beamweight(beamweight);
        protoBuffer->set_beammu(beammu);
        protoBuffer->set_displaycontent(displaycontent);
        protoBuffer->set_windowwidth(windowwidth);
        protoBuffer->set_windowcenter(windowcenter); 
        protoBuffer->set_zoomfactor(zoomfactor);
        protoBuffer->set_segmentindex(segmentindex);
        protoBuffer->set_segmentweight(segmentweight);
        protoBuffer->set_segmentmu(segmentmu);
        protoBuffer->set_leafindex(leafindex);
    }
}

TPS_END_NAMESPACE  // end namespace tps