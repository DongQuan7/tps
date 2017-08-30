////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedlandmarkpoint.cpp
/// 
///  \brief class CertifiedLandmarkPoint imeplementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/24
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedlandmarkpoint.h"
#include "RtTpsProtoLib/rt_tps_landmarkpoint.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedLandmarkPoint::Convert(const proto::RT_TPS_LANDMARK_POINT& proto)
{
    mX = proto.x();
    mY = proto.y();
    mZ = proto.z();
    mPointIndex = proto.index();
}

TPS_END_NAMESPACE  // end namespace tps