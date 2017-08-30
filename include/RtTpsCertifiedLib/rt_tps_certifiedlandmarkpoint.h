////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedlandmarkpoint.h
/// 
///  \brief class CertifiedLandmarkPoint declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/24
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDLANDMARKPOINT_H_
#define RT_TPS_CERTIFIEDLANDMARKPOINT_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto
{
    class RT_TPS_LANDMARK_POINT;
}

/// \class CertifiedLandmarkPoint
/// 
/// \brief *****
struct CertifiedLandmarkPoint
{
    double mX;
    double mY;
    double mZ;
    int mPointIndex;

    void Convert(const proto::RT_TPS_LANDMARK_POINT& proto);
};

TPS_END_NAMESPACE  // end namespace tps

#endif