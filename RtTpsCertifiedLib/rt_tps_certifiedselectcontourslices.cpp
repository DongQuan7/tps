////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedselectcontourslices.cpp
/// 
///  \brief struct CertifiedSelectContourSlices implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/12
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedselectcontourslices.h"
#include "RtTpsProtoLib/rt_tps_selectcontourslices.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedSelectContourSlices::Convert(const proto::RT_TPS_SELECT_CONTOUR_SLICES& protoEditInfo) {

    mX = protoEditInfo.x();
    mY = protoEditInfo.y();
    mActionType = protoEditInfo.mouseactiontype();
    mLastActionType = protoEditInfo.lastmouseactiontype();
    mVOIUid = protoEditInfo.voiuid();
    mWindowType = protoEditInfo.windowtype();
    mWindowUid = protoEditInfo.windowuid();
    mIsEditingStarted = protoEditInfo.iseditingstarted();
    if(protoEditInfo.has_firstslice()) {
        mFirstSlice = protoEditInfo.firstslice();
    }
    if(protoEditInfo.has_secondslice()) {
        mSecondSlice = protoEditInfo.secondslice();
    }
}

void CertifiedSelectContourSlices::ConvertBack(proto::RT_TPS_SELECT_CONTOUR_SLICES* protoEditInfo) const{

    protoEditInfo->set_x(mX);
    protoEditInfo->set_y(mY);
    protoEditInfo->set_mouseactiontype(mActionType);
    protoEditInfo->set_lastmouseactiontype(mLastActionType);
    protoEditInfo->set_voiuid(mVOIUid);
    protoEditInfo->set_windowtype(mWindowType);
    protoEditInfo->set_windowuid(mWindowUid);
    protoEditInfo->set_iseditingstarted(mIsEditingStarted);
    protoEditInfo->set_firstslice(mFirstSlice);
    protoEditInfo->set_secondslice(mSecondSlice);
}

TPS_END_NAMESPACE  // end namespace tps
