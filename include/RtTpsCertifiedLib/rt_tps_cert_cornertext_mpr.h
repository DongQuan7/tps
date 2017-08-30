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
#ifndef RT_TPS_CORNERTEXT_MPR_H_
#define RT_TPS_CORNERTEXT_MPR_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto {
    class RT_TPS_CornerText_MPR;
}

struct CornerTextMPR
{
    std::string planname;
    std::string seriesname;
    std::string seriesdatetime;
    std::string patientposition;
    double value;//CT value(HU) for CT, SUV value(bw\lbm\bsa) for PT
    double density;//for CT
    double dosevalue;
    double pixelpositionx;
    double pixelpositiony;
    double pixelpositionz;
    double windowwidth;
    double windowcenter;
    int slicenumber;
	int startSliceNumber;
    double slicethickness;
    bool isValueValid;
    std::string modality;

    CornerTextMPR();

    void Convert(const proto::RT_TPS_CornerText_MPR& protoBuffer);

    void ConvertBack(proto::RT_TPS_CornerText_MPR* protoBuffer) const;
};
TPS_END_NAMESPACE  // end namespace tps
#endif