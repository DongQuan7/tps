////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedbeam.cpp
/// 
///  \brief struct CertifiedBeam implementation
/// 
///  \version 1.0
/// 
///  \date    2014/01/02
////////////////////////////////////////////////////////////////

#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"



TPS_BEGIN_NAMESPACE;  // begin namespace tps

//initialization is necessary in MLC Auto-shape
TpsCertifiedBeam::TpsCertifiedBeam()
{
    mBeamUID = "";
    mBeamNumber = 1;
    mBeamName = "";
    mBeamDescription = "";
    mNormGroupUID = "";
    mCommissionedUnitUID = "";
    mIsocenterUid = "";
    mmlcedittype = 0;//0:Center 1:Top 2:Bottom

    mSsd = 0.f;
    mBeamMU = 0.f;
    mIsVisible = true;
    mIsClockWise = true;
    mIsJoined = true;
    mIsAutoCreated = true;
    mEditType = 0;
    mHasBeamWeight = true;
    mBeamWeight = 1.0f;
    mDoseRate = 0.0f;
    mBeamDose = 0.0f;
    mIsDoseValid = false;

    // Geometry
    mGantryStartAngle = 0.f;
    mGantryArcLength = 0.f;
    mCollimatorAngle = 0.f;
    mCouchAngle = 0.f;

    //Aperture
    mApertureType = 0;	//0:MLC 1:JawOnly 2:Cone 3:Aperture Block
    mIsAutoTrackingShape = false;
    mIsJawFollow = true;

    //shielding block
    mUseShieldingBlock = false;
    //virtual wedge
    mUseVirtualWedge = false;
    //physical wedge
    mUsePhysicalWedge = false;
    //tray
    mTrayUid = "";
    //bolus
    //mUseBolus = false;
    mBolusVoiUid = "";
    //applicator
    mApplicatorUid = "";
    //e-beam block
    mUseEBeamBlock = false;
}

// From FE to BE
void TpsCertifiedBeam::Convert(const proto::RtTpsProtoBeam& protoBeam)
{
    mBeamUID = protoBeam.beam_uid();
    mNormGroupUID = protoBeam.normal_group_uid();
    mBeamNumber = protoBeam.beam_number();
    mBeamName = protoBeam.beam_name();
    mBeamDescription = protoBeam.beam_description();
    mCommissionedUnitUID = protoBeam.commissioned_unit_uid();
    mIsocenterUid = protoBeam.isocenter_uid();
    mmlcedittype = protoBeam.mlcedittype();

    mSsd = protoBeam.ssd();
    mBeamMU = protoBeam.beam_mu();
    mIsVisible = protoBeam.is_visible();
    mIsClockWise = protoBeam.is_clockwise();
    mIsJoined = protoBeam.is_joined();
    mIsAutoCreated = protoBeam.is_auto_created();
    mEditType = protoBeam.edit_type();
    mHasBeamWeight = protoBeam.has_beam_weight();
    if (mHasBeamWeight)
    {
        mBeamWeight = protoBeam.beam_weight();
    }

    if(protoBeam.has_dose_rate()){
        mDoseRate = protoBeam.dose_rate();
    }
    mBeamDose = protoBeam.beam_dose();
    mIsDoseValid = protoBeam.is_dose_valid();
    mSegmentList.clear();
    TpsCertifiedSegment segment;
    for (int i = 0; i < protoBeam.segment_list_size(); ++i){
        segment.Convert(protoBeam.segment_list(i));
        mSegmentList.push_back(segment);
    }

    mGantryStartAngle = protoBeam.gantry_start_angle();
    mGantryArcLength = protoBeam.gantry_arc_length();
    mCollimatorAngle = protoBeam.collimator_angle();
    mCouchAngle = protoBeam.couch_angle();

    mApertureType = protoBeam.aperture_type();
    mIsAutoTrackingShape = protoBeam.is_auto_tracking_shaped();
    mTargetVoiUid = protoBeam.target_voi_uid();
    mTargetMarginX = protoBeam.target_margin_x();
    mTargetMarginY = protoBeam.target_margin_y();
    mAvoidVoiUid = protoBeam.avoid_voi_uid();
    mAvoidMarginX = protoBeam.avoid_margin_x();
    mAvoidMarginY = protoBeam.avoid_margin_y();
    mIsJawFollow = protoBeam.is_jaw_follow();
    mJawFollowMarginX = protoBeam.jaw_follow_margin_x();
    mJawFollowMarginY = protoBeam.jaw_follow_margin_y();
    mIsXSymmetric = protoBeam.is_x_symmetric();
    mIsYSymmetric = protoBeam.is_y_symmetric();
    if (mApertureType == 3)
    {
        mApertureBlock.Convert(protoBeam.aperture_block());
    }

    mUseShieldingBlock = protoBeam.use_shielding_block();
    if (mUseShieldingBlock)
    {
        mShieldingBlock.Convert(protoBeam.shielding_block());
        mIsShieldingBlockAutoTrackingShape = protoBeam.is_shielding_block_auto_tracking_shape();
        mShieldingBlockTargetVoiUid = protoBeam.shielding_block_target_voi_uid();
        mShieldingBlockTargetMarginX = protoBeam.shielding_block_target_margin_x();
        mShieldingBlockTargetMarginY = protoBeam.shielding_block_target_margin_y();
        mShieldingBlockAvoidVoiUid = protoBeam.shielding_block_avoid_voi_uid();
        mShieldingBlockAvoidMarginX = protoBeam.shielding_block_avoid_margin_x();
        mShieldingBlockAvoidMarginY = protoBeam.shielding_block_avoid_margin_y();
    }

    mVirtualWedgeUid = protoBeam.virtual_wedge_uid();
    mPhysicalWedgeUid = protoBeam.physical_wedge_uid();
    mTrayUid = protoBeam.tray_uid();
    mBolusVoiUid = protoBeam.bolus_voi_uid();
    mApplicatorUid = protoBeam.applicator_uid();
    mUseEBeamBlock = protoBeam.use_shielding_block();
    if (mUseEBeamBlock)
    {
        mEBeamBlock.Convert(protoBeam.e_beam_block());
    }
    mDoseAlgorithmType = protoBeam.algorithmtype();
    mCurrentSegmentUid = protoBeam.current_segment_uid();
    mIsOppositeBeam = protoBeam.is_opposite_beam();
}


//From BE to FE
void TpsCertifiedBeam::ConvertBack(proto::RtTpsProtoBeam* pProtoBeam) const
{
    if (pProtoBeam == nullptr) return;

    pProtoBeam->set_beam_uid(mBeamUID);
    pProtoBeam->set_normal_group_uid(mNormGroupUID);
    pProtoBeam->set_beam_number(mBeamNumber);
    pProtoBeam->set_beam_name(mBeamName);
    pProtoBeam->set_beam_description(mBeamDescription);
    pProtoBeam->set_is_auto_created(mIsAutoCreated);
    pProtoBeam->set_commissioned_unit_uid(mCommissionedUnitUID);
    pProtoBeam->set_isocenter_uid(mIsocenterUid);
    pProtoBeam->set_mlcedittype(mmlcedittype);

    pProtoBeam->set_ssd(mSsd);
    pProtoBeam->set_beam_mu(mBeamMU);
    pProtoBeam->set_is_visible(mIsVisible);
    pProtoBeam->set_is_clockwise(mIsClockWise);
    pProtoBeam->set_is_joined(mIsJoined);

    pProtoBeam->set_edit_type(mEditType);
    pProtoBeam->set_beam_weight(mBeamWeight);

    pProtoBeam->set_dose_rate(mDoseRate);
    pProtoBeam->set_beam_dose(mBeamDose);
    pProtoBeam->set_is_dose_valid(mIsDoseValid);
    pProtoBeam->clear_segment_list();
    proto::RtTpsProtoSegment* protoSegment = nullptr;
    for (int i = 0; i < mSegmentList.size(); ++i){
        protoSegment = pProtoBeam->add_segment_list();
        mSegmentList[i].ConvertBack(protoSegment);
    }

    pProtoBeam->set_gantry_start_angle(mGantryStartAngle);
    pProtoBeam->set_gantry_arc_length(mGantryArcLength);
    pProtoBeam->set_collimator_angle(mCollimatorAngle);
    pProtoBeam->set_couch_angle(mCouchAngle);

    pProtoBeam->set_aperture_type(mApertureType);
    pProtoBeam->set_is_auto_tracking_shaped(mIsAutoTrackingShape);
    pProtoBeam->set_target_voi_uid(mTargetVoiUid);
    pProtoBeam->set_target_margin_x(mTargetMarginX);
    pProtoBeam->set_target_margin_y(mTargetMarginY);
    pProtoBeam->set_avoid_voi_uid(mAvoidVoiUid);
    pProtoBeam->set_avoid_margin_x(mAvoidMarginX);
    pProtoBeam->set_avoid_margin_y(mAvoidMarginY);
    pProtoBeam->set_is_jaw_follow(mIsJawFollow);
    pProtoBeam->set_jaw_follow_margin_x(mJawFollowMarginX);
    pProtoBeam->set_jaw_follow_margin_y(mJawFollowMarginY);
    pProtoBeam->set_is_x_symmetric(mIsXSymmetric);
    pProtoBeam->set_is_y_symmetric(mIsYSymmetric);
    if (mApertureType == 3)
    {
        auto block = pProtoBeam->aperture_block();
        mApertureBlock.ConvertBack(&block);
    }

    pProtoBeam->set_use_shielding_block(mUseShieldingBlock);
    if (mUseShieldingBlock)
    {
        auto block = pProtoBeam->shielding_block();
        mShieldingBlock.ConvertBack(&block);
        pProtoBeam->set_is_shielding_block_auto_tracking_shape(mIsShieldingBlockAutoTrackingShape);
        pProtoBeam->set_shielding_block_target_voi_uid(mShieldingBlockTargetVoiUid);
        pProtoBeam->set_shielding_block_target_margin_x(mShieldingBlockTargetMarginX);
        pProtoBeam->set_shielding_block_target_margin_y(mShieldingBlockTargetMarginY);
        pProtoBeam->set_shielding_block_avoid_voi_uid(mShieldingBlockAvoidVoiUid);
        pProtoBeam->set_shielding_block_avoid_margin_x(mShieldingBlockAvoidMarginX);
        pProtoBeam->set_shielding_block_avoid_margin_y(mShieldingBlockAvoidMarginY);
    }

    pProtoBeam->set_use_virtual_wedge(mUseVirtualWedge);
    if (mUseVirtualWedge)
    {
        pProtoBeam->set_virtual_wedge_uid(mVirtualWedgeUid);
    }

    pProtoBeam->set_use_physical_wedge(mUsePhysicalWedge);
    if (mUsePhysicalWedge)
    {
        pProtoBeam->set_physical_wedge_uid(mPhysicalWedgeUid);
    }

    pProtoBeam->set_tray_uid(mTrayUid);

    /*pProtoBeam->set_use_bolus(mUseBolus);
    if (mUseBolus)
    {
        pProtoBeam->set_bolus_density(mBolusDensity);
        pProtoBeam->set_bolus_thickness(mBolusThickness);
    }*/

    pProtoBeam->set_bolus_voi_uid(mBolusVoiUid);

    pProtoBeam->set_applicator_uid(mApplicatorUid);

    pProtoBeam->set_use_shielding_block(mUseEBeamBlock);
    if (mUseEBeamBlock)
    {
        auto block = pProtoBeam->e_beam_block();
        mEBeamBlock.ConvertBack(&block);
    }
}
TPS_END_NAMESPACE  // end namespace tps
