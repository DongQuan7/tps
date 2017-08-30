////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedselectcontourslices.h
/// 
///  \brief struct CertifiedSelectContourSlices declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/12
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDSELECTCONTOURSLICES_H_
#define RT_TPS_CERTIFIEDSELECTCONTOURSLICES_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_SELECT_CONTOUR_SLICES;
}

struct CertifiedSelectContourSlices{
    double mX;
    double mY;
    int mWindowType;
    std::string mWindowUid;
    int mActionType;
    int mLastActionType;
    std::string mVOIUid;
    bool mIsEditingStarted;
    int  mFirstSlice;
    int  mSecondSlice;
    int mRealActionType;

    void Convert(const proto::RT_TPS_SELECT_CONTOUR_SLICES& protoEditInfo);

    void ConvertBack(proto::RT_TPS_SELECT_CONTOUR_SLICES* protoEditInfo) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDSMARTCONTOUR_H_