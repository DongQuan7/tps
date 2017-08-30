////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedcontourslices.h
/// 
///  \brief struct CertifiedContoursSlices declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDCONTOURSLICES_H_
#define RT_TPS_CERTIFIEDCONTOURSLICES_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_CONTOUR_SLICES;
}

struct CertifiedContoursSlices{

    int mFirstSlice;
    int mSecondSlice;
    std::string mVOIUid;
    int mEditMode;
    int mSubEditMode;
    int mWindowType;
    std::string mWindowUid;

    void Convert(const proto::RT_TPS_CONTOUR_SLICES& contourSlicesInfo);

    void ConvertBack(proto::RT_TPS_CONTOUR_SLICES* contourSlicesInfo) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDCONTOURSLICES_H_