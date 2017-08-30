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
#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"
#include "RtTpsProtoLib/rt_tps_proto_plan.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_doselinesetting.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedPlan::Convert(const proto::RtTpsProtoPlan& protoPlan)
{
    mPlanUID = protoPlan.planuid();
    mPlanName = protoPlan.planname();
    mCourseUID = protoPlan.courseuid();
    mImageUID = protoPlan.seriesuid();
    mPlanSourceType = protoPlan.plansourcetype();
    mSourceImageUid = protoPlan.phantomuid();
    mPlanner = protoPlan.planner();

    mdosegridspacingx = protoPlan.dosegridspacingx();
    mdosegridspacingy = protoPlan.dosegridspacingy();
    mdosegridspacingz = protoPlan.dosegridspacingz();
    mdosegridstartx = protoPlan.dosegridstartx();
    mdosegridstarty = protoPlan.dosegridstarty();
    mdosegridstartz = protoPlan.dosegridstartz();
    mdosegridsizex = protoPlan.dosegridsizex();
    mdosegridsizey = protoPlan.dosegridsizey();
    mdosegridsizez = protoPlan.dosegridsizez();
    mpatientPosition = protoPlan.patientposition();
    mPlanDescription = protoPlan.plandescription();
    mSetupPoiUid = protoPlan.setuppoiuid();

    mDisplayMode = protoPlan.displaymode();
    mHundredPercentDefinition = protoPlan.hundredpercentdefinition();
    mSpecifiedDose = protoPlan.specifieddose();
    mRenderMode = protoPlan.rendermode();
    mTransParency = protoPlan.transparency();
    mEnableSkinDoseView = protoPlan.enableskindoseview();
    mMaxSkinDose = protoPlan.maxskindose();
    mBodyThickness = protoPlan.bodythickness();

    mDoseLineList.clear();
    for (int i = 0; i < protoPlan.doselinelist_size(); ++i){
        mDoseLineList.push_back(CertifiedDoseLine());
        mDoseLineList[i].Convert(protoPlan.doselinelist(i));
    }
}

void CertifiedPlan::ConvertBack(proto::RtTpsProtoPlan* protoPlan) const {
    protoPlan->set_planuid(mPlanUID);
    protoPlan->set_planname(mPlanName);
    protoPlan->set_courseuid(mCourseUID);
    protoPlan->set_seriesuid(mImageUID);
    protoPlan->set_plansourcetype(mPlanSourceType);
    protoPlan->set_phantomuid(mSourceImageUid);
    protoPlan->set_planner(mPlanner);

    protoPlan->set_dosegridspacingx(mdosegridspacingx);
    protoPlan->set_dosegridspacingy(mdosegridspacingy);
    protoPlan->set_dosegridspacingz(mdosegridspacingz);
    protoPlan->set_dosegridstartx(mdosegridstartx);
    protoPlan->set_dosegridstarty(mdosegridstarty);
    protoPlan->set_dosegridstartz(mdosegridstartz);
    protoPlan->set_dosegridsizex(mdosegridsizex);
    protoPlan->set_dosegridsizey(mdosegridsizey);
    protoPlan->set_dosegridsizez(mdosegridsizez);

    protoPlan->set_displaymode(mDisplayMode);
    protoPlan->set_hundredpercentdefinition(mHundredPercentDefinition);
    protoPlan->set_specifieddose(mSpecifiedDose);
    protoPlan->set_rendermode(mRenderMode);
    protoPlan->set_transparency(mTransParency);
    protoPlan->set_enableskindoseview(mEnableSkinDoseView);
    protoPlan->set_maxskindose(mMaxSkinDose);
    protoPlan->set_patientposition(mpatientPosition);
    protoPlan->set_plandescription(mPlanDescription);
    protoPlan->set_bodythickness(mBodyThickness);
    protoPlan->set_setuppoiuid(mSetupPoiUid);

    proto::RtTpsProtoDoseLine* doseLine = nullptr;
    for (int i = 0; i < mDoseLineList.size(); ++i){
        doseLine = protoPlan->add_doselinelist();
        mDoseLineList[i].ConvertBack(doseLine);
    }
}

TPS_END_NAMESPACE  // end namespace tps
