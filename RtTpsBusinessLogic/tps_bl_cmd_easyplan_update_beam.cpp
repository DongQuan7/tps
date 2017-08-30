//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_update_beam.cpp 
///  \brief  update beam in easy plan
///
///  \version 1.0
///  \date  2015/11/03  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_update_beam.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE
UpdateEasyPlanBeamCmd::UpdateEasyPlanBeamCmd( std::string &easyPlanUID, TpsCertifiedBeam easyBeam )
: mEasyPlanUID(easyPlanUID), mCertBeam(easyBeam){

}


UpdateEasyPlanBeamCmd::~UpdateEasyPlanBeamCmd()
{

}

int UpdateEasyPlanBeamCmd::Execute()
{
	RtNormgroup *normGroup = nullptr;
	TpsNormGroupManager *normgroupManager = mDataRepository->GetNormgroupManager();
    if (!normgroupManager->GetNormGroup(mCertBeam.mNormGroupUID,&normGroup) 
        || nullptr ==normGroup) {
            TPS_LOG_DEV_ERROR<<"Failed to get normgroup:"<<mCertBeam.mNormGroupUID;
            return TPS_ER_FAILURE;
    }
    RtBeam *beam = nullptr;
	TpsBeamManager *beamManager = mDataRepository->GetBeamManager();
    beam = beamManager->GetBeam(mCertBeam.mBeamUID);
    if (beam == nullptr || beam->get_uid() != mCertBeam.mBeamUID) {
        return TPS_ER_FAILURE;
    }
    beam->set_name(mCertBeam.mBeamName);
    beam->set_beamnumber(mCertBeam.mBeamNumber);
    beam->set_beamtype(RtDbDef::STATICIMRT);
    //beam->set_radiationtype(normGroup->get_radiationtype());
    if (mCertBeam.mHasBeamWeight)
    {
        beam->set_beamweight(mCertBeam.mBeamWeight);
    }
    beam->set_commissionedunituid(mCertBeam.mCommissionedUnitUID);


    // items need to set
    //beam->set_alpha();
    beam->set_aperturetype(mCertBeam.mApertureType);
    //beam->set_beamdeliverytype(mCertBeam);
    /*std::string beamID;
    char c[8];
    itoa(mCertBeam.mBeamNumber,c,10);
    beam->set_beamid(c);*/
    //beam->set_beammode();
    //beam->set_beammu();
    //beam->set_beamnumber();
    //beam->set_beamsegments();
    //beam->set_beamtype(mCertBeam);
    //beam->set_beamweight();
    //beam->set_blue();
    //beam->set_bolusthickness();
    //beam->set_bolusvoiuid();
    //beam->set_commissionedunituid();
    beam->set_description(mCertBeam.mBeamDescription);
    beam->set_doserate(mCertBeam.mDoseRate);
    //该字段TMS需要用于计算CP MU，是必填字段，经TMS同事确认，默认值应该设置为1，详情可查看Task 56352. by lx 2016/11/26
    beam->set_finalcumulativemetersetweight(1);
    //beam->set_green();
    beam->set_isautocreated(mCertBeam.mIsAutoCreated);
    //beam->set_isjoincalc(mCertBeam.);
    //beam->set_isjoinopt(mCertBeam.is);
    beam->set_isocenterpoiuid(mCertBeam.mIsocenterUid);
    beam->set_isvisible(mCertBeam.mIsVisible);
    beam->set_mlcedittype(mCertBeam.mmlcedittype);
    //beam->set_name(mCertBeam.mBeamName);
    beam->set_normgroupuid(mCertBeam.mNormGroupUID);
    beam->set_beammu(mCertBeam.mBeamMU);
    //beam->set_normpointbeamdose(mCertBeam.);
    if(mCertBeam.mUsePhysicalWedge){
        beam->set_physicalwedgeuid(mCertBeam.mPhysicalWedgeUid);
    }
    //beam->set_protocal(mCertBeam.);
    //beam->set_red();
    //beam->set_relatedrtimageuid(mCertBeam.);
    //beam->set_ssd();
    //beam->set_tmsverno(mCertBeam.);
    //beam->set_toleranceuid(mCertBeam);
    //beam->set_tpsverno();
    beam->set_trayuid(mCertBeam.mTrayUid);
    beam->set_uid(mCertBeam.mBeamUID);
    beam->set_ssd(mCertBeam.mSsd);
    //beam->set_updatetime(mCertBeam.);
    if(mCertBeam.mUseVirtualWedge){
        beam->set_virtualwedgeuid(mCertBeam.mVirtualWedgeUid);
    }

    // Update segments in beam
    // Noticing that no matter what aperture type the beam is, it's segments will always 
    //      contain the Jaw and MLC hardware, so we need to tell the 
    //if(mCertBeam.mApertureType == 0){
        std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
        if (segments.size() == 0) {
            std::vector<RtBeamsegment*> newSegments;
            RtBeamsegment* newSegment = new RtBeamsegment(true);
            newSegment->set_beamuid(mCertBeam.mBeamUID);
            if(mCertBeam.mSegmentList.size() == 1
                && !mCertBeam.mSegmentList[0].segmentUID.empty()){
                newSegment->set_uid(mCertBeam.mSegmentList[0].segmentUID);
            }
            if(!UpdateSegment(mCertBeam, newSegment)) {
                return TPS_ER_FAILURE;
            }
            newSegments.push_back(newSegment);
            beam->set_beamsegments(newSegments);
			for (auto itr=newSegments.begin(); itr!=newSegments.end(); ++itr) DEL_PTR(*itr);
        }
        else {
            for(int i=0; i<segments.size(); i++) {
                if(!UpdateSegment(mCertBeam, segments[i])) {
                    return TPS_ER_FAILURE;
                }
            }
        }
    //}

    //Update aperture block
    if(mCertBeam.mApertureType == 3){
        tps::RtBlock* apertureBlock = beam->get_aperture_block();
        if(apertureBlock == nullptr){
            beam->create_aperture_block();
            apertureBlock = beam->get_aperture_block();
        }
        UpdateBlock(mCertBeam, apertureBlock, 0);
    }

    // Update shield block
    if(mCertBeam.mUseShieldingBlock){
        tps::RtBlock* shieldBlock = beam->get_shield_block();
        if(shieldBlock == nullptr){
            beam->create_shield_block();
            shieldBlock = beam->get_shield_block();
        }
        UpdateBlock(mCertBeam, shieldBlock, 1);
    }

    // Update beam mode
    RtCommissionedunit *unit = nullptr;
	TpsMachineManager *machineManager = mDataRepository->GetMachineManager();
    if (!machineManager->GetCommissionedUnit(mCertBeam.mCommissionedUnitUID, unit)) {
        TPS_LOG_DEV_ERROR<<"Failed to get unit. ";
        return TPS_ER_FAILURE;
    }
    if (unit != nullptr) {
        std::string sBeamMode = std::to_string((long double)(unit->get_energy()));
        beam->set_beammode(sBeamMode);
    }

    // this API crashes every time.
    //TODO waiting for TMS 
    //tmsBeam->SetRelatedAccessoryUID(beam.mWedgeUIDList);
	//std::vector<RTFWK::RTFWKDLDOAccessory*> accessoryList;
	//for (auto itrWedgeUid=beam.mWedgeUIDList.cbegin(); itrWedgeUid!=beam.mWedgeUIDList.cend(); ++itrWedgeUid){
	//	RTFWK::RTFWKDLDOAccessory* pAccessory = nullptr;
	//	TpsMachineManager::GetInstance()->GetAccessory(*itrWedgeUid, pAccessory);
	//	if (nullptr != pAccessory){
	//		accessoryList.push_back(pAccessory);
	//	}
	//}
	//TpsBeamManager::GetInstance()->SetRelatedAccessoryList(beam.mBeamUID, accessoryList);
	mRenderProxy->EasyPlanModifyBeamInfo(beam->get_uid());
	return TPS_ER_SUCCESS;
}

bool UpdateEasyPlanBeamCmd::UpdateSegment( TpsCertifiedBeam &beam, RtBeamsegment* segment ){
	segment->set_startgantryangle(beam.mGantryStartAngle);
	segment->set_startcollimatorangle(beam.mCollimatorAngle);
	segment->set_starttablerotationangle(beam.mCouchAngle);

    RtMlcshape *startMlcShape = nullptr;
    RtMlcshape *endMlcShape = nullptr;
	startMlcShape = segment->get_startmlcshape();
    endMlcShape = segment->get_endmlcshape();
	if (startMlcShape == nullptr || endMlcShape == nullptr) {
        return false;
		//add one mlc shape
		//mlcShape = new RtMlcshape;
		//mlcShape->set_segmentuid(segment->get_uid());
	}
    if (mCertBeam.mSegmentList.empty()){
        return false;
    }

    if(beam.mIsJawFollow){
       // startMlcShape->set_jawtype(RtDbDef::JAWTYPE_Follow);
    }
    else 
    {
        //startMlcShape->set_jawtype(RtDbDef::JAWTYPE_Manual);
    }
	startMlcShape->set_xlowerjawpos(mCertBeam.mSegmentList[0].startJaw.x1);
	startMlcShape->set_xupperjawpos(mCertBeam.mSegmentList[0].startJaw.x2);
	startMlcShape->set_ylowerjawpos(mCertBeam.mSegmentList[0].startJaw.y1);
	startMlcShape->set_yupperjawpos(mCertBeam.mSegmentList[0].startJaw.y2);
    std::vector<db_Point2d> startLeafPosPtList;
    tps::db_Point2d startLeafPosPt;
    for(int i=0; i<mCertBeam.mSegmentList[0].startMlcLeaves.size(); i++){
        startLeafPosPt = tps::db_Point2d(mCertBeam.mSegmentList[0].startMlcLeaves[i].x1, mCertBeam.mSegmentList[0].startMlcLeaves[i].x2);
        startLeafPosPtList.push_back(startLeafPosPt);
    }
    startMlcShape->set_leafpositions(startLeafPosPtList);

    if(beam.mIsJawFollow){
       // endMlcShape->set_jawtype(RtDbDef::JAWTYPE_Follow);
    }
    else 
    {
     //   endMlcShape->set_jawtype(RtDbDef::JAWTYPE_Manual);
    }
    endMlcShape->set_xlowerjawpos(mCertBeam.mSegmentList[0].endJaw.x1);
    endMlcShape->set_xupperjawpos(mCertBeam.mSegmentList[0].endJaw.x2);
    endMlcShape->set_ylowerjawpos(mCertBeam.mSegmentList[0].endJaw.y1);
    endMlcShape->set_yupperjawpos(mCertBeam.mSegmentList[0].endJaw.y2);
    std::vector<db_Point2d> endLeafPosPtList;
    tps::db_Point2d endLeafPosPt;
    for(int i=0; i<mCertBeam.mSegmentList[0].endMlcLeaves.size(); i++){
        endLeafPosPt = tps::db_Point2d(mCertBeam.mSegmentList[0].endMlcLeaves[i].x1, mCertBeam.mSegmentList[0].endMlcLeaves[i].x2);
        endLeafPosPtList.push_back(endLeafPosPt);
    }
    endMlcShape->set_leafpositions(endLeafPosPtList);


	//// TEST BEGIN, set fake leaf position for now, need to get leaf position from datamanager later after yilong.cheng modified it. //
	////create default MLC shape
	//RtNormgroup *normGroup = nullptr;
	//TpsNormGroupManager *normgroupManager = mDataRepository->GetNormgroupManager();
	//if (!normgroupManager->GetNormGroup(mCertBeam.mNormGroupUID,&normGroup) 
	//	|| nullptr ==normGroup) {
	//		TPS_LOG_DEV_ERROR<<"Failed to get normgroup:"<<mCertBeam.mNormGroupUID;
	//		return false;
	//}
	//RtMachine* machine = nullptr;
	//TpsMachineManager* machineManager = mDataRepository->GetMachineManager();
	//if (!machineManager->GetMachine(
	//	normGroup->get_machineuid(), machine) || machine == nullptr){
	//		TPS_LOG_DEV_ERROR<<"Failed to get machine.";
	//		return false;
	//}
	//
	//RTFWK::RTFWKDLDOMLCSetting *mlcSetting = machine->GetMLCSetting();

	//int leafNum = machine->get_leafnum();;
	//std::vector<RTFWK::RTFWKDLDOLeafPosition> vecLeafPos;
	//RTFWK::RTFWKDLDOLeafPosition leafPos;
	//for (int i = 0; i < leafNum; i++) {
	//	leafPos.m_dLeafLowerPosition = -10; //-200;
	//	leafPos.m_dLeafUpperPosition = 10; //200;
	//	vecLeafPos.push_back(leafPos);
	//}
	//// TEST END //
	//mlcShape->SetLeafPositions(vecLeafPos);
	//SetLeafPositions(std::vector<RTFWKDLDOLeafPosition> vLeafPositions); //RTFWKDLDOLeafPosition represents a pair of leaves, upper one is in right side

	// HERE SHOULD MAKE SURE MLCSHAPE HAVE DIFERENT UID.
	//segment->SetEndMLCShape(mlcShape);
	return true;
}

bool UpdateEasyPlanBeamCmd::UpdateBlock( TpsCertifiedBeam &certBeam, RtBlock* block, int blockType){
    if (blockType == 0)
    {
        block->set_uid(certBeam.mApertureBlock.mBlockUid);
        block->set_blockname(certBeam.mApertureBlock.mBlockName);
        block->set_blocktype(RtDbDef::APERTURE);
        std::string sMaterial = "";
        if (certBeam.mApertureBlock.mMaterial == 0) 
        sMaterial = "WOLFRAM";
        else 
        sMaterial = "LEAD";
        block->set_materialid(sMaterial);
        block->set_blockthickness(certBeam.mApertureBlock.mThickness);

        std::vector<db_Point2f> apertureBlockPtList;
        tps::db_Point2f apertureBlockPt;
        for (int i = 0; i < certBeam.mApertureBlock.mBlockData.size(); i++) {
            apertureBlockPt = tps::db_Point2f(certBeam.mApertureBlock.mBlockData[i].GetX(), certBeam.mApertureBlock.mBlockData[i].GetY());
            apertureBlockPtList.push_back(apertureBlockPt);
        }
        block->set_points(apertureBlockPtList);
    }
    else
    {
        block->set_uid(certBeam.mShieldingBlock.mBlockUid);
        block->set_blockname(certBeam.mShieldingBlock.mBlockName);
        block->set_blocktype(RtDbDef::SHIELDING);
        std::string sMaterial = "";
        if (certBeam.mShieldingBlock.mMaterial == 0) 
            sMaterial = "WOLFRAM";
        else 
            sMaterial = "LEAD";
        block->set_materialid(sMaterial);
        block->set_blockthickness(certBeam.mShieldingBlock.mThickness);

        std::vector<db_Point2f> shieldBlockPtList;
        tps::db_Point2f shieldBlockPt;
        for (int i = 0; i < certBeam.mShieldingBlock.mBlockData.size(); i++) {
            shieldBlockPt = tps::db_Point2f(certBeam.mShieldingBlock.mBlockData[i].GetX(), certBeam.mShieldingBlock.mBlockData[i].GetY());
            shieldBlockPtList.push_back(shieldBlockPt );
        }
        block->set_points(shieldBlockPtList);
    }
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE