////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifieddoseline.h
/// 
///  \brief class CertifiedDoseLine declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/09/29
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDDOSELINE_H_
#define RT_TPS_CERTIFIEDDOSELINE_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RtTpsProtoDoseLine;
}

struct CertifiedDoseLine{
    std::string     mUid;
    std::string     mPlanUid;
    double          mDoseRatio;
    double          mColor[4];//RGBA
    bool            mIsVisible;

    void Convert(const proto::RtTpsProtoDoseLine &doseLine);

    void ConvertBack(proto::RtTpsProtoDoseLine *protoLine) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif