//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_fw_beamstructure.cpp
///  \brief  beam and beamCollection structure for beam template
///  \
///  class  
///  \version 1.0
///  \date  2014/11/19
///  \
//////////////////////////////////////////////////////////////////////////

#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_beamstructure.h"

//TMS

//#include "RtTpsProtoLib/rt_tps_simplebeam.pb.h"
//#include "RtTpsProtoLib/rt_tps_beam_template.pb.h"

TPS_BEGIN_NAMESPACE

SimpleBeam::SimpleBeam()
{

}

SimpleBeam::SimpleBeam(
    float gantrySA, 
    float arcLength, 
    bool isClockwise, 
    float CouchA, 
    float colliA) : 
    mGantryStartAngle(gantrySA), 
    mArcLength(arcLength), 
    mIsClockwise(isClockwise), 
    mCouchAngle(CouchA), 
    mCollimatorAngle(colliA)
{

}

SimpleBeam::SimpleBeam(const SimpleBeam& beam){
    mGantryStartAngle   = beam.mGantryStartAngle;
    mArcLength       = beam.mArcLength;
    mCouchAngle          = beam.mCouchAngle;
    mCollimatorAngle    = beam.mCollimatorAngle;
    mIsClockwise = beam.mIsClockwise;
}

BeamTemplate::BeamTemplate()
{

}

BeamTemplate::BeamTemplate(std::string name, std::string description) 
    : mTemplateName(name), mTemplateDescription(description)
{

}
//void SimpleBeam::GetSimpleBeamFromTemplate(const proto::RT_TPS_SimpleBeam& smpl){
//    mGantryStartAngle   = smpl.gantry_start_angle();
//    mGantryEndAngle     = smpl.gantry_end_angle();
//    mCouchAngle         = smpl.couch_angle();
//    mCollimatorAngle    = smpl.collimator_angle();
//}



//void BeamTemplate::GetBeamTemplateFromProto(const proto::RT_TPS_BeamTemplate &protoBT){
//    mTemplateName = protoBT.templatename();
//    mTemplateDescription = protoBT.templatedescription();
//    for(int i = 0; i < protoBT.beam_size(); ++i){
//        SimpleBeam simpleBeam;
//        simpleBeam.GetSimpleBeamFromTemplate(protoBT.beam(i));
//        mBeamCollection.push_back(simpleBeam);
//    }
//}
//
//void BeamTemplate::SetBeamTemplateToProto(proto::RT_TPS_BeamTemplate* protoBT){
//    protoBT->set_templatename(mTemplateName);
//    protoBT->set_templatedescription(mTemplateDescription);
//    for(auto iter = mBeamCollection.begin(); iter != mBeamCollection.end(); ++iter){
//        proto::RT_TPS_SimpleBeam *smplB = protoBT->add_beam();
//        smplB->set_gantry_start_angle(iter->mGantryStartAngle);
//        smplB->set_gantry_end_angle(iter->mGantryEndAngle);
//        smplB->set_couch_angle(iter->mCouchAngle);
//        smplB->set_collimator_angle(iter->mCollimatorAngle);
//    }
//}


TPS_END_NAMESPACE