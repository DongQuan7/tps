//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenlong.zhao (mailto:wenlong.zhao@united-imaging.com)
///
///  \file   rt_tps_certified_imagereferenceinfo.h 
///  \brief   
///
///  \version 1.0
///  \date  2015/04/29  
///  \
//////////////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_cert_cornertext_mpr.h"
#include "RtTpsProtoLib/rt_tps_cornertext_mpr.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
CornerTextMPR::CornerTextMPR()
{
    planname="";
    seriesname="";
    seriesdatetime="";
    patientposition="";
    value=0.0;
    density=0.0;
    dosevalue =0.0;
    pixelpositionx=0.0;
    pixelpositiony=0.0;
    pixelpositionz=0.0;
    windowwidth=0.0;
    windowcenter=0.0;
    slicenumber=0;
    slicethickness=0.0;
    isValueValid = false;
    modality = "";
}

void CornerTextMPR::Convert(const proto::RT_TPS_CornerText_MPR& protoBuffer)
{
    planname=protoBuffer.planname();
    seriesname=protoBuffer.seriesname();
    seriesdatetime=protoBuffer.seriesdatetime();
    patientposition=protoBuffer.patientposition();
    value=protoBuffer.value();
    density=protoBuffer.density();
    dosevalue=protoBuffer.dosevalue();
    pixelpositionx=protoBuffer.pixelpositionx();
    pixelpositiony=protoBuffer.pixelpositiony();
    pixelpositionz=protoBuffer.pixelpositionz();
    windowwidth=protoBuffer.windowwidth();
    windowcenter=protoBuffer.windowcenter();
    slicenumber=protoBuffer.slicenumber();
	startSliceNumber = protoBuffer.startslicenumber();
    slicethickness=protoBuffer.slicethickness(); 
    isValueValid = protoBuffer.isvaluevalid();
    modality = protoBuffer.modality();
}

void CornerTextMPR::ConvertBack(proto::RT_TPS_CornerText_MPR* protoBuffer) const
{
    if (protoBuffer!=nullptr)
    {
        protoBuffer->set_planname(planname);
        protoBuffer->set_seriesname(seriesname);
        protoBuffer->set_seriesdatetime(seriesdatetime);
        protoBuffer->set_patientposition(patientposition);
        protoBuffer->set_value(value);
        protoBuffer->set_density(density);
        protoBuffer->set_dosevalue(dosevalue);
        protoBuffer->set_pixelpositionx(pixelpositionx);
        protoBuffer->set_pixelpositiony(pixelpositiony);
        protoBuffer->set_pixelpositionz(pixelpositionz);
        protoBuffer->set_windowwidth(windowwidth);
        protoBuffer->set_windowcenter(windowcenter);
        protoBuffer->set_slicenumber(slicenumber);
		protoBuffer->set_startslicenumber(startSliceNumber);
        protoBuffer->set_slicethickness(slicethickness);
        protoBuffer->set_isvaluevalid(isValueValid);
        protoBuffer->set_modality(modality);
    }
}
TPS_END_NAMESPACE  // end namespace tps