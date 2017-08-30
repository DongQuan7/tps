////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file rt_tps_CertifiedPlan.h
/// 
///  \brief struct CertifiedPlan declaration 
/// 
///  \version 1.0
/// 
///  \date    Jan 10, 2014
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CertifiedPlan_H_
#define RT_TPS_CertifiedPlan_H_

#include "tps_defs.h"
#include "RtTpsCertifiedLib/rt_tps_certifieddoseline.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RtTpsProtoPlan;
}

struct CertifiedPlan{
    std::string mPlanUID;
    std::string mPlanName;
    std::string mCourseUID;
    std::string mImageUID;
    int mDataSourceId;
    int mPlanSourceType;
    std::string mSourceImageUid;
    float mdosegridspacingx;
    float mdosegridspacingy;
    float mdosegridspacingz;
    float mdosegridstartx;
    float mdosegridstarty;
    float mdosegridstartz;
    float mdosegridsizex;
    float mdosegridsizey;
    float mdosegridsizez;
    std::string mPlanner;
    std::string mpatientPosition;
    std::string mPlanDescription;
    std::string mSetupPoiUid;

    int mDisplayMode;
    int mHundredPercentDefinition;
    float mSpecifiedDose;
    int mRenderMode;
    float mTransParency;
    bool mEnableSkinDoseView;
    float mMaxSkinDose;
    float mBodyThickness;
    std::vector<CertifiedDoseLine> mDoseLineList;

    void Convert(const proto::RtTpsProtoPlan& protoPlan);

    void ConvertBack(proto::RtTpsProtoPlan* protoPlan) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CertifiedPlan_H_
