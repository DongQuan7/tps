////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedsmartcontour.cpp
/// 
///  \brief struct CertifiedSmartContour imaplementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/22
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedsmartcontour.h"
#include "RtTpsProtoLib/rt_tps_voieditinfo.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedVOIEditInfo::Convert(const proto::RT_TPS_EDIT_VOI_INFO& protoEditInfo) 
{
    mX = protoEditInfo.x();
    mY = protoEditInfo.y();
    mActionType = protoEditInfo.mouseactiontype();
    mLastActionType = protoEditInfo.lastmouseactiontype();
    mVOIUid = protoEditInfo.voiuid();
    mWindowType = protoEditInfo.windowtype();
    mWindowUid = protoEditInfo.windowuid();
    mRadius = protoEditInfo.radius();
    mVoiEditMode = protoEditInfo.voieditingmode();
    mIsInterpolate = protoEditInfo.isinterpolate();
    for (int i = 0; i < protoEditInfo.avoidvoiuid_size(); ++i) {
        mAvoidVoisUid.push_back(protoEditInfo.avoidvoiuid(i));
    }
    mIsEditStarted = protoEditInfo.iseditstarted();
    // copy point list into certfied
    for (int n=0; n<protoEditInfo.ptxlist_size(); n++) {
        mPtList.push_back(Mcsf::MedViewer3D::Point2D(protoEditInfo.ptxlist(n), protoEditInfo.ptylist(n)));
    }
}

void CertifiedVOIEditInfo::ConvertBack(proto::RT_TPS_EDIT_VOI_INFO* protoEditInfo) const
{
    protoEditInfo->set_x(mX);
    protoEditInfo->set_y(mY);
    protoEditInfo->set_mouseactiontype(mActionType);
    protoEditInfo->set_lastmouseactiontype(mLastActionType);
    protoEditInfo->set_voiuid(mVOIUid);
    protoEditInfo->set_windowtype(mWindowType);
    protoEditInfo->set_windowuid(mWindowUid);
    protoEditInfo->set_radius(mRadius);
    protoEditInfo->set_voieditingmode(mVoiEditMode);
    protoEditInfo->set_isinterpolate(mIsInterpolate);
    protoEditInfo->set_iseditstarted(mIsEditStarted);
    for (int n=0; n<mPtList.size(); n++) {
        protoEditInfo->add_ptxlist(mPtList[n].x);
        protoEditInfo->add_ptylist(mPtList[n].y);
    }
    // missing avoid voi uid
}

TPS_END_NAMESPACE  // end namespace tps