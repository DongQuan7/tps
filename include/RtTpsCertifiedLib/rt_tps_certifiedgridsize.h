////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedgridsize.h
/// 
///  \brief struct Certifiedgridsize declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/09
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDGRIDSIZE_H_
#define RT_TPS_CERTIFIEDGRIDSIZE_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_GridSize;
}

struct CertifiedGridSize {
    float mx;
    float my;
    float mz;
    float mstartx;
    float mstarty;
    float mstartz;
    float msizex;
    float msizey;
    float msizez;
    std::string mPlanUid;
    bool mIsShowBoundary;
    std::string mNormGroup;

    void Convert(const proto::RT_TPS_GridSize& protoGridSize);

    void ConvertBack(proto::RT_TPS_GridSize* protoGridSize);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDGRIDSIZE_H_