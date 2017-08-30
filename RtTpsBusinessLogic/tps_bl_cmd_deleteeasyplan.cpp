//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \file   tps_cmd_deleteeasyplan.cpp 
///  \brief   delete easy plan
///
///  \version 1.0
///  \date  2015/10/28
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_deleteeasyplan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

//#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"


TPS_BEGIN_NAMESPACE

TpsDeleteEasyPlanCmd::TpsDeleteEasyPlanCmd(const std::string& planUid) : mPlanUid(planUid){

}

TpsDeleteEasyPlanCmd::~TpsDeleteEasyPlanCmd()
{

}

int TpsDeleteEasyPlanCmd::Execute()
{
	RtPlan* plan = nullptr;
	TpsPatientManager* patientManager = this->mDataRepository->GetPatientManager();
	TpsNormGroupManager* normGroupManager = this->mDataRepository->GetNormgroupManager();
	TpsBeamManager* beamManager = this->mDataRepository->GetBeamManager();
	TpsImageDataManager* imageDataManager = this->mDataRepository->GetImageDataManager();
	std::string planUID = mPlanUid;

//	RTFWK::RTFWKDLDataAccess *dataAccess = RTFWK::RTFWKDLDataAccess::GetInstance();

	if (!patientManager->GetPlan(planUID, &plan) || plan == nullptr){
		TPS_LOG_DEV_ERROR << "Failed to get plan: " << planUID;
		return TPS_ER_FAILURE;
	}

	std::string rtImageUID;//=plan->GetRTImageUID();

	//delete rtimage
	if (rtImageUID.empty()) {
		imageDataManager->GetEasyPlanImageUID(planUID, &rtImageUID);
	}
	if (!rtImageUID.empty()) {
		int bRet = mSaveObjects->EraseObject(rtImageUID, RtDbDef::TYPE_RTIMAGE);
		if (bRet!=TPS_ER_SUCCESS) {
			TPS_LOG_DEV_ERROR << "Failed to delete rtImage from TMS";
			return bRet;
		}
	}

	////delete plan    
	int bRet = mSaveObjects->EraseObject(planUID, RtDbDef::TYPE_PLAN);
	if (bRet != TPS_ER_SUCCESS) {
		TPS_LOG_DEV_ERROR << "Failed to delete easy plan from TMS";
		return bRet;
	}
	if (!patientManager->RemovePlan(planUID)) {
		TPS_LOG_DEV_ERROR << "Failed to delete easy plan from TpsPatientManager";
		return TPS_ER_FAILURE;
	}
	//if (plan->GetApprovalStatus() == RTFWK::APPROVED) {
	//    isObjectInDB = true;
	//}
	std::vector<std::string> easyNormGroupUID = normGroupManager->GetNormGroupUIDListByPlan(planUID);
	if (easyNormGroupUID.empty()) {
		// TEST BEGIN //
		TPS_LOG_DEV_INFO << "Failed to get normal group uid list of easy plan: " << planUID;
		return TPS_ER_SUCCESS;
		// TEST END //
		//TPS_LOG_DEV_ERROR<<"Failed to get normal group uid list of easy plan: "<<planUID;
		//return false;
	}
	//delete normgroup
	RtNormgroup *normGroup = nullptr;
	if (!normGroupManager->GetNormGroup(easyNormGroupUID[0], &normGroup)) {
		TPS_LOG_DEV_ERROR << "Failed to get normgroup of normal group uid: " << easyNormGroupUID[0];
		return TPS_ER_FAILURE;
	}

	std::vector<std::string> beamUIDList = beamManager->GetBeamUIDListByNormgroupUID(easyNormGroupUID[0]);

	bRet = mSaveObjects->EraseObject(normGroup->get_uid(),RtDbDef::TYPE_NORMGROUP);
	if (bRet != TPS_ER_SUCCESS) {
		TPS_LOG_DEV_ERROR << "Failed to delete normGroup from TMS";
		return bRet;
	}

	normGroupManager->RemoveNormGroup(easyNormGroupUID[0]);
    normGroupManager->RemoveNormGroupDoseCalculateMode(easyNormGroupUID[0]);

	//delete beam 
	for (auto it = beamUIDList.begin(); it != beamUIDList.end(); ++it) {
		if (TPS_ER_SUCCESS != beamManager->DeleteBeam(*it)) {
			TPS_LOG_DEV_ERROR << "delete beam from beam manager failed!" << *it;
			return TPS_ER_FAILURE;
		}

		mRenderProxy->DeleteBeam(planUID, *it);
	}

    // delete EasyPlan image and opposed image from ImageDataManager
    std::string easyPlanImage = "";
    std::string easyPlanOpposedImage = "";
    imageDataManager->GetEasyPlanImageUID(planUID, &easyPlanImage);
    imageDataManager->GetEasyPlanOpposedImageUID(planUID, &easyPlanOpposedImage);
    imageDataManager->DeleteImage2D(easyPlanImage);
    imageDataManager->DeleteImage2D(easyPlanOpposedImage);

    // delete EasyPlan image and beam relation in ImageDataManager
    imageDataManager->DeleteEasyPlanImageUID(planUID);
    imageDataManager->DeleteEasyPlanOpposedImageUID(planUID);
    /*for(auto it = beamUIDList.begin(); it != beamUIDList.end(); ++it) {
        imageDataManager->DeleteEasyPlanBeamImageUID(*it);
    }*/

    mRenderProxy->RemoveDataSource(SOURCE_GROUP_EZPLAN, IMAGE);
    mRenderProxy->RemoveDataSource(SOURCE_GROUP_EZPLAN, PLAN);
	mRenderProxy->Render();	
	return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE