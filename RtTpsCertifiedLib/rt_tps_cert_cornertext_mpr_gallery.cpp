//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  yuxuan.duan (mailto:yuxuan.duan@united-imaging.com)
///
///  \file   rt_tps_cert_cornertext_mpr_gallery.cpp
///  \brief   
///
///  \version 1.0
///  \date  2016/01/07  
///  \
//////////////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_cert_cornertext_mpr_gallery.h"
#include "RtTpsProtoLib/rt_tps_gallery.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
    CornerTextGallery::CornerTextGallery()
{
    patientname = "";
    seriesname = "";
    seriesdatetime = "";
    patientposition = "";
    value = 0.0;
    density = 0.0;
    pixelpositionx = 0.0;
    pixelpositiony = 0.0;
    pixelpositionz = 0.0;
    windowwidth = 0.0;
    windowcenter = 0.0;
    slicenumber = 0;
    totalslicenumber = 0;
    slicethickness = 0.0;
    isValueValid = false;
    modality = "";
    zoomfactor = 0.0;
    rotate = 0.0;
}

void CornerTextGallery::Convert(const proto::RT_TPS_Gallery_CornerInfo& protoBuffer)
{
    patientname = protoBuffer.patientname();
    seriesname = protoBuffer.seriesname();
    seriesdatetime = protoBuffer.seriesdatetime();
    patientposition = protoBuffer.patientposition();
    value = protoBuffer.value();
    density = protoBuffer.density();
    pixelpositionx = protoBuffer.pixelpositionx();
    pixelpositiony = protoBuffer.pixelpositiony();
    pixelpositionz = protoBuffer.pixelpositionz();
    windowwidth = protoBuffer.windowwidth();
    windowcenter = protoBuffer.windowcenter();
    slicenumber = protoBuffer.slicenumber();
    totalslicenumber = protoBuffer.totalslicenumber();
    slicethickness = protoBuffer.slicethickness(); 
    isValueValid = protoBuffer.isvaluevalid();
    modality = protoBuffer.modality();
    zoomfactor = protoBuffer.zoomfactor();
    rotate = protoBuffer.rotate();
}

void CornerTextGallery::ConvertBack(proto::RT_TPS_Gallery_CornerInfo* protoBuffer) const
{
    if (protoBuffer!=nullptr)
    {
        protoBuffer->set_patientname(patientname);
        protoBuffer->set_seriesname(seriesname);
        protoBuffer->set_seriesdatetime(seriesdatetime);
        protoBuffer->set_patientposition(patientposition);
        protoBuffer->set_value(value);
        protoBuffer->set_density(density);
        protoBuffer->set_pixelpositionx(pixelpositionx);
        protoBuffer->set_pixelpositiony(pixelpositiony);
        protoBuffer->set_pixelpositionz(pixelpositionz);
        protoBuffer->set_windowwidth(windowwidth);
        protoBuffer->set_windowcenter(windowcenter);
        protoBuffer->set_slicenumber(slicenumber);
        protoBuffer->set_totalslicenumber(totalslicenumber);
        protoBuffer->set_slicethickness(slicethickness);
        protoBuffer->set_isvaluevalid(isValueValid);
        protoBuffer->set_modality(modality);
        protoBuffer->set_zoomfactor(zoomfactor);
        protoBuffer->set_rotate(rotate);
    }
}
TPS_END_NAMESPACE  // end namespace tps