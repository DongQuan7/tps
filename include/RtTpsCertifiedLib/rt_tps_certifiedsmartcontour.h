////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedsmartcontour.h
/// 
///  \brief struct CertifiedSmartContour declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/22
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDSMARTCONTOUR_H_
#define RT_TPS_CERTIFIEDSMARTCONTOUR_H_

#include "tps_defs.h"
#include <list>
#include <vector>
#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_EDIT_VOI_INFO;
}

struct CertifiedVOIEditInfo {
    double mX;
    double mY;
    std::vector<Mcsf::MedViewer3D::Point2D> mPtList;
    int mWindowType;
    std::string mWindowUid;
    int mActionType;
    int mLastActionType;
    std::string mVOIUid;
    double mRadius;
    int mRealActionType;
    int mVoiEditMode;
    bool mIsInterpolate;
    std::list<std::string> mAvoidVoisUid;
    bool mIsEditStarted;

    void Convert(const proto::RT_TPS_EDIT_VOI_INFO& protoEditInfo);

    void ConvertBack(proto::RT_TPS_EDIT_VOI_INFO* protoEditInfo) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDSMARTCONTOUR_H_