////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  SHI chunhui chunhui.shi@united-imaging.com
/// 
///  \file rt_tys_certifiedmlceditinfo.cpp
/// 
///  \brief struct 
/// 
///  \version 1.0
/// 
///  \date    2014/10/10
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedmlceditinfo.h"
#include "RtTpsProtoLib/rt_tps_mlceditinfo.pb.h"

TPS_BEGIN_NAMESPACE

void CertifiedMlcEditInfo::Convert(const proto::RT_TPS_MLC_EDIT_INFO& protoEditInfo) {
    protoEditInfo;
    mX = protoEditInfo.x();
    mY = protoEditInfo.y();
    mActionType = protoEditInfo.mouseactiontype();
    mLastActionType = protoEditInfo.lastmouseactiontype();

    mRadius = protoEditInfo.radius();

    mBeamUid = protoEditInfo.beamuid();
    mSegmentId = protoEditInfo.segmentid();

    mIsEditingStarted = protoEditInfo.iseditingstarted();
    mMlcEditMode = protoEditInfo.mlceditmode();
    mWidth = protoEditInfo.width();
    mHeight = protoEditInfo.height();
}

void CertifiedMlcEditInfo::ConvertBack(proto::RT_TPS_MLC_EDIT_INFO* protoEditInfo) const{
     protoEditInfo;
    protoEditInfo->set_x(mX);
    protoEditInfo->set_y(mY);
    protoEditInfo->set_mouseactiontype(mActionType);
    protoEditInfo->set_lastmouseactiontype(mLastActionType);

    protoEditInfo->set_radius(mRadius);
    protoEditInfo->set_beamuid(mBeamUid);
    protoEditInfo->set_segmentid(mSegmentId);

    protoEditInfo->set_iseditingstarted(mIsEditingStarted);
    protoEditInfo->set_mlceditmode(mMlcEditMode);
}
TPS_END_NAMESPACE