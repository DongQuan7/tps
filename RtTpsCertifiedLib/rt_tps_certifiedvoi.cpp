////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedvoi.cpp
/// 
///  \brief struct CertifiedVOI implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/22
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsProtoLib/rt_tps_proto_voi.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

//initialization is necessary
CertifiedVOI::CertifiedVOI()
{
    mVoiUid = "";
    mSeriesUid= "";
    mName= "";
    mPlanningRole = 0;
    memset(mColor, 0, 4 * sizeof(float));
    mColor[3] = 1.0f;       // default alpha = 1.0f
    mOpacity3d = 1.0f;
    mIsVisible = true;
    mIsLocked = false;
    mAverageDensity = 0.0f;
    mMaxDensity= 0.0f;
    mMinDensity= 0.0f;
    mVolume= 0.0f;
    mSurfaceArea= 0.0f;
    mCentroidX= 0.0f;
    mCentroidY= 0.0f;
    mCentroidZ= 0.0f;
    mSlices.clear();
    mMinDose= 0.0f;
    mMaxDose= 0.0f;
    mMeanDose= 0.0f;
    mStandardDeviation= 0.0f;
    mIsShaded = false;
    mSubrole = 0;
    mIsApproved = 0;
    mLockType = 1;
    mTcpNTcp = 0;
    mDescription = "";
    mOrganType = -1;
}

void CertifiedVOI::Convert(const proto::RtTpsProtoVoi& protoVOI) 
{
    mVoiUid = protoVOI.voiuid();
    mSeriesUid = protoVOI.seriesuid();
    mName = protoVOI.voiname();
    mPlanningRole = protoVOI.planningrole();

    mColor[0] = protoVOI.voicolor(0);
    mColor[1] = protoVOI.voicolor(1);
    mColor[2] = protoVOI.voicolor(2);
    mColor[3] = protoVOI.voicolor(3);
    mOpacity3d = protoVOI.voicolor(4);

    mIsVisible = protoVOI.isvisible();
    mIsLocked = protoVOI.islocked();
    mAverageDensity = protoVOI.averagedensity();
    mMaxDensity = protoVOI.maxdensity();
    mMinDensity = protoVOI.mindensity();
    mVolume = protoVOI.volume();
    mSurfaceArea = protoVOI.surfacearea();
    mCentroidX = protoVOI.centroidx();
    mCentroidY = protoVOI.centroidy();
    mCentroidZ = protoVOI.centroidz();
    // TODO: mSlices = protoVOI.slices();
    mMinDose = protoVOI.mindose();
    mMaxDose = protoVOI.maxdose();
    mMeanDose = protoVOI.meandose();
    mStandardDeviation = protoVOI.standard_deviation();
    mIsShaded = protoVOI.isshaded();
    mSubrole = protoVOI.subrole();
    mIsApproved = protoVOI.isapproved();
    mLockType = protoVOI.locktype();
    mInGrid = protoVOI.ingrid();
    mTcpNTcp = protoVOI.tcpntcp();
    mDescription = protoVOI.description();
    mIsOverride = protoVOI.isoverride();
    mOverrideMassDensity = protoVOI.overridemassdensity();
    mOverrideReferenceEDensity = protoVOI.overridereferenceelectrondensity();
    mOrganType = protoVOI.organtype();
}

void CertifiedVOI::ConvertBack(proto::RtTpsProtoVoi* protoVOI) const 
{
    protoVOI->Clear();
    protoVOI->add_voicolor(mColor[0]);
    protoVOI->add_voicolor(mColor[1]);
    protoVOI->add_voicolor(mColor[2]);
    protoVOI->add_voicolor(mColor[3]);
    protoVOI->add_voicolor(mOpacity3d);
    //todo slice
    for (int i = 0; i < mSlices.size(); ++i) {
        protoVOI->add_slices(mSlices[i]);
    }

    protoVOI->set_voiuid(mVoiUid);
    protoVOI->set_seriesuid(mSeriesUid);
    protoVOI->set_voiname(mName);
    protoVOI->set_planningrole(mPlanningRole);
    protoVOI->set_isvisible(mIsVisible);
    protoVOI->set_islocked(mIsLocked);
    protoVOI->set_averagedensity(mAverageDensity);
    protoVOI->set_maxdensity(mMaxDensity);
    protoVOI->set_mindensity(mMinDensity);
    protoVOI->set_volume(mVolume);
    protoVOI->set_surfacearea(mSurfaceArea);
    protoVOI->set_centroidx(mCentroidX);
    protoVOI->set_centroidy(mCentroidY);
    protoVOI->set_centroidz(mCentroidZ);
    protoVOI->set_mindose(mMinDose);
    protoVOI->set_maxdose(mMaxDose);
    protoVOI->set_meandose(mMeanDose);
    protoVOI->set_standard_deviation(mStandardDeviation);
    protoVOI->set_isshaded(mIsShaded);
    protoVOI->set_subrole(mSubrole);
    protoVOI->set_isapproved(mIsApproved);
    protoVOI->set_locktype(mLockType);
    protoVOI->set_ingrid(mInGrid);
    protoVOI->set_tcpntcp(mTcpNTcp);
    protoVOI->set_description(mDescription);
    protoVOI->set_isoverride(mIsOverride);
    protoVOI->set_overridemassdensity(mOverrideMassDensity);
    protoVOI->set_overridereferenceelectrondensity(mOverrideReferenceEDensity);
}

TPS_END_NAMESPACE  // end namespace tps