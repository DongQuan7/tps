////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedbeam.h
/// 
///  \brief struct CertifiedBeam declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/02
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDBEAM_H_
#define RT_TPS_CERTIFIEDBEAM_H_

#include "tps_defs.h"
#include <vector>
#include "RtTpsCertifiedLib/rt_tps_certified_segment.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedblock.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto
{
    class RtTpsProtoBeam;
}

//class RtBeam;

struct TpsCertifiedBeam
{
    std::string mBeamUID;
    std::string mNormGroupUID;
    int         mBeamNumber;            //300A00C0
    std::string mBeamName;              //300A00C2
    std::string mBeamDescription;       //300A00C3
    std::string mCommissionedUnitUID;
    std::string mIsocenterUid;
    int         mmlcedittype;//0:Center 1:Top 2:Bottom

    float       mSsd;
    float       mBeamMU;
    bool        mIsVisible;
    bool        mIsClockWise;
    bool        mIsJoined;
    bool        mIsAutoCreated;
    int mEditType;
    bool mHasBeamWeight;
    float mBeamWeight;
    float mDoseRate;
    float mBeamDose;
    bool mIsDoseValid;
    std::vector<TpsCertifiedSegment> mSegmentList;
    std::string mCurrentSegmentUid;

    // Geometry
    float mGantryStartAngle;
    float mGantryArcLength;
    float mCollimatorAngle;
    float mCouchAngle;

    //Aperture
    int mApertureType;	//0:MLC 1:JawOnly 2:Cone 3:Aperture Block
    bool mIsAutoTrackingShape;
    std::string mTargetVoiUid;
    double mTargetMarginX;
    double mTargetMarginY;
    std::string mAvoidVoiUid;
    double mAvoidMarginX;
    double mAvoidMarginY;
    bool mIsJawFollow;
    double mJawFollowMarginX;
    double mJawFollowMarginY;

    bool mIsXSymmetric;
    bool mIsYSymmetric;
    TpsCertifiedBlock mApertureBlock;

    //shielding block
    TpsCertifiedBlock mShieldingBlock;
    bool mUseShieldingBlock;
    bool mIsShieldingBlockAutoTrackingShape;
    std::string mShieldingBlockTargetVoiUid;
    double mShieldingBlockTargetMarginX;
    double mShieldingBlockTargetMarginY;
    std::string mShieldingBlockAvoidVoiUid;
    double mShieldingBlockAvoidMarginX;
    double mShieldingBlockAvoidMarginY;

    //virtual wedge
    bool mUseVirtualWedge;
    std::string mVirtualWedgeUid;

    //physical wedge
    bool mUsePhysicalWedge;
    std::string mPhysicalWedgeUid;

    //tray
    std::string mTrayUid;

    //bolus
    std::string mBolusVoiUid;

    //applicator
    std::string mApplicatorUid;

    //e-beam block
    bool mUseEBeamBlock;
    TpsCertifiedBlock mEBeamBlock;

    //dose algorithmtype
    int mDoseAlgorithmType;

    //just for easyplan
    bool mIsOppositeBeam;

    //initialization is necessary in MLC Auto-shape
    TpsCertifiedBeam();

    void Convert(const proto::RtTpsProtoBeam& protoBeam);

    void ConvertBack(proto::RtTpsProtoBeam* pProtoBeam) const;

};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CERTIFIEDBEAM_H_
