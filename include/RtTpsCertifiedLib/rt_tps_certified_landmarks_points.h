////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  WANG jie  jie.wang@united-imaging.com
/// 
///  \file rt_tps_certified_landmarks_points.h
/// 
///  \brief struct CertifiedLandmarkPoints implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/07/26
////////////////////////////////////////////////////////////////


#ifndef RT_TPS_CERTIFIEDLANDMARKPOINTS_H_
#define RT_TPS_CERTIFIEDLANDMARKPOINTS_H_

#include "tps_defs.h"
#include <vector>
#include "RtTpsCertifiedLib/rt_tps_certifiedlandmarkpoint.h"
#include "RtTpsFramework/tps_fw_common_enums.h"


TPS_BEGIN_NAMESPACE

namespace proto
{
    /*class CertifiedLandmarkPoint;*/
    class RT_TPS_LANDMARK_POINT_LIST;
}

struct CertifiedLandmarkPoints 
{
public:
    void Convert(const proto::RT_TPS_LANDMARK_POINT_LIST& protoLandmarkPointsList);

public:
    //CertifiedLandmarkPoint mLandmarkPoints[LANDMARK_POINT_PAIR_COUNT * 2];
    std::vector<CertifiedLandmarkPoint>   landmarkPointsList;

};

TPS_END_NAMESPACE

#endif