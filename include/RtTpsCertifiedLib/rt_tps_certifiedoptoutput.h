////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file rt_tps_certifiedoptoutput.h
/// 
///  \brief struct CertifiedOptOutput declaration 
/// 
///  \version 1.0
/// 
///  \date    Apr 21, 2014
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDOPTRESULT_H_
#define RT_TPS_CERTIFIEDOPTRESULT_H_

#include "tps_defs.h"
#include <map>

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto
{
    class RT_TPS_OptOutput;
}

struct CertifiedOptOutput
{
    std::string                  mNormalGroupUid;
    float                        mIterationNumber;
    float                        mIterationResult;
    float                        mProgressValue;
    float                        mPlanMU;
    int                          mPlanSegmentCount;
    int                          mTimeLeft;
    int                          mTimeDelived;
    std::map<std::string, float> mEudResult;
    std::map<std::string, float> mObjectValue;
    int                          mOptReturnValue;
    int                          mAlgStatus;

    void ConvertBack(proto::RT_TPS_OptOutput* protoOptOutput);
};

TPS_END_NAMESPACE  // end namespace tps

#endif 
