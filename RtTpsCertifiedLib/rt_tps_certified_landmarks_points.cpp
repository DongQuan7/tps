////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  WANG jie  jie.wang@united-imaging.com
/// 
///  \file rt_tps_certified_landmarks_points.cpp
/// 
///  \brief struct CertifiedLandmarkPoints implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/07/26
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certified_landmarks_points.h"
#include "RtTpsProtoLib/rt_tps_landmarkpoint.pb.h"

TPS_BEGIN_NAMESPACE

void CertifiedLandmarkPoints::Convert(const proto::RT_TPS_LANDMARK_POINT_LIST& protoLandmarkPointsList) 
{
    int size = protoLandmarkPointsList.points_size();
    landmarkPointsList.clear();

    for (int i = 0; i <size; ++i)
    {
        proto::RT_TPS_LANDMARK_POINT protoLandmarkPoint = protoLandmarkPointsList.points(i);
        CertifiedLandmarkPoint certifiedPoint;
        certifiedPoint.Convert(protoLandmarkPoint);
        landmarkPointsList.push_back(certifiedPoint);
    }
}

TPS_END_NAMESPACE
