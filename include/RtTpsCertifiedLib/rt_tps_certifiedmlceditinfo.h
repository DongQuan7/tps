////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  SHI chunhui chunhui.shi@united-imaging.com
/// 
///  \file rt_tps_certifiedmlceditinfo.h
/// 
///  \brief struct CertifiedMlcEditInfo
/// 
///  \version 1.0
/// 
///  \date    2014/10/10
////////////////////////////////////////////////////////////////
#ifndef RT_TPS_CERTIFIEDMLCEDITINFO_H_
#define RT_TPS_CERTIFIEDMLCEDITINFO_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE
namespace proto{
    class RT_TPS_MLC_EDIT_INFO;
}

struct CertifiedMlcEditInfo{
    double              mX;
    double              mY;
    int                 mActionType;
    int                 mLastActionType;
    int                 mRealActionType;

    double              mRadius;
    std::string         mBeamUid;
    std::string         mSegmentUid;
    std::string         mPlanUid;
    int                 mSegmentId;

    bool                mIsEditingStarted;
    int                 mMlcEditMode;

    double              mWidth;
    double              mHeight;

    void Convert(const proto::RT_TPS_MLC_EDIT_INFO& protoEditInfo);

    void ConvertBack(proto::RT_TPS_MLC_EDIT_INFO* protoEditInfo) const;
};

TPS_END_NAMESPACE

#endif
