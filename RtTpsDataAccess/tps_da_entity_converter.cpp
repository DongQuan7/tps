////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_da_entity_converter.h
/// 
///  \brief class EntiyConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    Oct.13,2014
////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"

//TPS
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

IMPLEMENT_SINGLETON(EntityConverter)

EntityConverter::EntityConverter(void) {
}


EntityConverter::~EntityConverter(void) {

}

void EntityConverter::ROIEntiyToCertifiedVOI(const ROIEntity& roiEntity, CertifiedVOI* voiCertified)
{
    if (nullptr == voiCertified){
        TPS_LOG_DEV_ERROR<<"voiCertified is null";
        return;
    }

    auto innerRoi = roiEntity.GetInnerRoi();
    voiCertified->mVoiUid = innerRoi->get_uid();
    voiCertified->mSeriesUid= innerRoi->get_seriesuid();
    voiCertified->mName= innerRoi->get_name();

    voiCertified->mPlanningRole = innerRoi->get_planningrole();
    voiCertified->mSubrole = innerRoi->get_subrole();

    memcpy(voiCertified->mColor, innerRoi->get_color(), 4 * sizeof(float));
    voiCertified->mIsVisible = innerRoi->get_isvisible();
    voiCertified->mIsLocked = innerRoi->get_locktype() != 0;
    voiCertified->mLockType = innerRoi->get_locktype();

    voiCertified->mAverageDensity = innerRoi->get_averagedensity();
    voiCertified->mMaxDensity= innerRoi->get_maxdensity();
    voiCertified->mMinDensity= innerRoi->get_mindensity();
    voiCertified->mIsOverride = innerRoi->get_isoverride();
    voiCertified->mOverrideMassDensity = innerRoi->get_overridedenstiy();
    voiCertified->mOverrideReferenceEDensity = innerRoi->get_density();
    voiCertified->mVolume= innerRoi->get_volume();
    voiCertified->mSurfaceArea= innerRoi->get_surfacearea();
    voiCertified->mCentroidX= innerRoi->get_centroidx();
    voiCertified->mCentroidY= innerRoi->get_centroidy();
    voiCertified->mCentroidZ= innerRoi->get_centroidz();
    voiCertified->mTcpNTcp = innerRoi->get_tcpntcp();
    voiCertified->mStandardDeviation= innerRoi->get_standarddeviation();
    voiCertified->mIsShaded = innerRoi->get_isshaded();

    voiCertified->mSlices.clear();
    std::set<int> slices = roiEntity.GetContourSlices();
    for_each(slices.begin(),slices.end(),[&](int slice){
        voiCertified->mSlices.push_back(slice);
    });

    voiCertified->mInGrid = roiEntity.GetInGrid();
    voiCertified->mMinDose= roiEntity.GetMinDose();
    voiCertified->mMaxDose= roiEntity.GetMaxDose();
    voiCertified->mMeanDose= roiEntity.GetMeanDose();
    voiCertified->mIsQA = roiEntity.IsQA();
}

void EntityConverter::CertifiedVOIToROIEntiy(const CertifiedVOI& voiCertified, ROIEntity* pRoiEntity)
{
    if (nullptr == pRoiEntity){
        TPS_LOG_DEV_ERROR<<"pVoiEntity is null";
        return;
    }

    //voiTms->SetVOIUID(voiCertified.mVoiUid);
    auto innerRoi = pRoiEntity->GetInnerRoi();
    innerRoi->set_seriesuid(voiCertified.mSeriesUid);
    innerRoi->set_name(voiCertified.mName);
    innerRoi->set_planningrole(voiCertified.mPlanningRole);
    innerRoi->set_subrole(voiCertified.mSubrole);
    innerRoi->set_color(voiCertified.mColor);
    innerRoi->set_isvisible(voiCertified.mIsVisible);
    innerRoi->set_locktype(RtDbDef::RTDB_VOI_LOCKTYPE(voiCertified.mLockType));

    innerRoi->set_isoverride(voiCertified.mIsOverride);
    innerRoi->set_overridedenstiy(voiCertified.mOverrideMassDensity);
    innerRoi->set_density(voiCertified.mOverrideReferenceEDensity);
    innerRoi->set_averagedensity(voiCertified.mAverageDensity);
    innerRoi->set_maxdensity(voiCertified.mMaxDensity);
    innerRoi->set_mindensity(voiCertified.mMinDensity);
    innerRoi->set_volume(voiCertified.mVolume);
    innerRoi->set_surfacearea(voiCertified.mSurfaceArea);
    innerRoi->set_centroidx(voiCertified.mCentroidX);
    innerRoi->set_centroidy(voiCertified.mCentroidY);
    innerRoi->set_centroidz(voiCertified.mCentroidZ);
    innerRoi->set_standarddeviation(voiCertified.mStandardDeviation);
    innerRoi->set_isshaded(voiCertified.mIsShaded);

    pRoiEntity->SetMinDose(voiCertified.mMinDose);
    pRoiEntity->SetMaxDose(voiCertified.mMaxDose);
    pRoiEntity->SetMeanDose(voiCertified.mMeanDose);
    pRoiEntity->SetIsQA(voiCertified.mIsQA);
}
TPS_END_NAMESPACE
