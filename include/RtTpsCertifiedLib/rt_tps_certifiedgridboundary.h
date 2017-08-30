////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedgridboundary.h
/// 
///  \brief struct Certifiedgridboundary declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/10
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDGRIDBOUNDARY_H_
#define RT_TPS_CERTIFIEDGRIDBOUNDARY_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_GridBoundary;
}

struct CertifiedGridBoundary {
    double mX;
    double mY;
    std::string mWindowUid;
    int mWindowType;
    int mActionType;
    std::string mPlanUid;
    std::string mNormGroup;

    void Convert(const proto::RT_TPS_GridBoundary& protoGridBoundary);

    void ConvertBack(proto::RT_TPS_GridBoundary* protoGridBoundary);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDGRIDBOUNDARY_H_