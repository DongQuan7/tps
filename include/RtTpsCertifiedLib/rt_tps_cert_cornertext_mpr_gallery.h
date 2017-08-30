//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  yuxuan.duan (mailto:yuxuan.duan@united-imaging.com)
///
///  \file   rt_tps_cert_cornertext_mpr_gallery.h 
///  \brief   
///
///  \version 1.0
///  \date  2016/01/07  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef RT_TPS_CORNERTEXT_MPR_GALLERY_H_
#define RT_TPS_CORNERTEXT_MPR_GALLERY_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

    namespace proto {
        class RT_TPS_Gallery_CornerInfo;
}

struct CornerTextGallery
{
    std::string patientname;
    std::string seriesname;
    std::string seriesdatetime;
    std::string patientposition;
    double value;//CT value(HU) for CT, SUV value(bw\lbm\bsa) for PT
    double density;//for CT
    double pixelpositionx;
    double pixelpositiony;
    double pixelpositionz;
    double windowwidth;
    double windowcenter;
    int slicenumber;
    int totalslicenumber;
    double slicethickness;
    bool isValueValid;
    std::string modality;
    double zoomfactor;
    double rotate;

    CornerTextGallery();

    void Convert(const proto::RT_TPS_Gallery_CornerInfo& protoBuffer);

    void ConvertBack(proto::RT_TPS_Gallery_CornerInfo* protoBuffer) const;
};
TPS_END_NAMESPACE  // end namespace tps
#endif