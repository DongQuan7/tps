////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedvoi.h
/// 
///  \brief struct CertifiedVOI declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/22
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDVOI_H_
#define RT_TPS_CERTIFIEDVOI_H_

#include "tps_defs.h"
#include <vector>

// #include <boost/archive/text_iarchive.hpp>
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/serialization/string.hpp>
// #include <boost/serialization/base_object.hpp>
// #include <boost/serialization/utility.hpp>
// #include <boost/serialization/list.hpp>
// #include <boost/serialization/assume_abstract.hpp>

namespace boost{
    namespace serialization{
        class access;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_Voi;
    class RtTpsProtoVoi;
}

enum eVoiDisplayMode
{
    CONTOUR,
    SHADED
};

struct CertifiedVOI{

    std::string mVoiUid;
    std::string mSeriesUid;
    std::string mName;
    int         mPlanningRole;
    float mColor[4];
    float mOpacity3d;
    bool mIsVisible;
    bool mIsLocked;
    float mAverageDensity;
    float mMaxDensity;
    float mMinDensity;
    float mVolume;
    float mSurfaceArea;
    float mCentroidX;
    float mCentroidY;
    float mCentroidZ;
    std::vector<int> mSlices;
    float mMinDose;
    float mMaxDose;
    float mMeanDose;
    float mStandardDeviation;
    bool mIsShaded;
    bool mIsQA;
    int mSubrole;
    int mIsApproved;
    int mLockType;
    float mInGrid;
    float mTcpNTcp;
    std::string mDescription;
    bool mIsOverride;
    float mOverrideMassDensity;
    float mOverrideReferenceEDensity;
    int mOrganType;

    //initialization is necessary
    CertifiedVOI();

    void Convert(const proto::RtTpsProtoVoi& protoVOI);

    void ConvertBack(proto::RtTpsProtoVoi* protoVOI) const;

    //serialize to db
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        version;
        ar & mName;
        ar & mPlanningRole;
        ar & mColor;
    }
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDVOI_H_
