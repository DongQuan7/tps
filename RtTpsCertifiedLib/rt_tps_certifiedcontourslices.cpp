////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedcontourslices.cpp
/// 
///  \brief struct CertifiedContoursSlices implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedcontourslices.h"
#include "RtTpsProtoLib/rt_tps_contourslices.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedContoursSlices::Convert(const proto::RT_TPS_CONTOUR_SLICES& contourSlicesInfo) {
    if (contourSlicesInfo.has_first_slice()) {
        mFirstSlice = contourSlicesInfo.first_slice();
    }
    if(contourSlicesInfo.has_second_slice()) {
        mSecondSlice = contourSlicesInfo.second_slice();
    }
    mVOIUid = contourSlicesInfo.voi_uid();
    if (contourSlicesInfo.has_edit_mode()) {
        mEditMode = contourSlicesInfo.edit_mode();
    }
    if (contourSlicesInfo.has_sub_edit_mode()) {
        mSubEditMode = contourSlicesInfo.sub_edit_mode();
    }
    if (contourSlicesInfo.has_windowtype()) {
        mWindowType = contourSlicesInfo.windowtype();
    }
    if (contourSlicesInfo.has_windowuid()) {
        mWindowUid = contourSlicesInfo.windowuid();
    }
}

void CertifiedContoursSlices::ConvertBack(proto::RT_TPS_CONTOUR_SLICES* contourSlicesInfo) const{
    contourSlicesInfo->set_voi_uid(mVOIUid);
    contourSlicesInfo->set_second_slice(mSecondSlice);
    contourSlicesInfo->set_first_slice(mFirstSlice);
    contourSlicesInfo->set_edit_mode(mEditMode);
    contourSlicesInfo->set_sub_edit_mode(mSubEditMode);
    contourSlicesInfo->set_windowtype(mWindowType);
    contourSlicesInfo->set_windowuid(mWindowUid);
}

TPS_END_NAMESPACE  // end namespace tps