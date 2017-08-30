//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \file    tps_bl_cmd_easy_plan_beam_delete.cpp 
///  \brief   delete easy plan beam
///
///  \version 1.0
///  \date  2015/10/30
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_easy_plan_beam_delete.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE;

EasyPlanBeamDeleteCmd::EasyPlanBeamDeleteCmd(const std::string &beamUid) : mUid(beamUid){
}

EasyPlanBeamDeleteCmd::~EasyPlanBeamDeleteCmd() {

}

int EasyPlanBeamDeleteCmd::Execute() 
{
	RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mUid);
	if (beam==nullptr) {
		TPS_LOG_DEV_ERROR << "Failed to get Beam: " << mUid;
		return TPS_ER_FAILURE;
	}

	RtNormgroup* normalGroup = nullptr;
	mDataRepository->GetNormgroupManager()->GetNormGroup(beam->get_normgroupuid(), &normalGroup);
	if (normalGroup == nullptr)
	{
		TPS_LOG_DEV_ERROR << "Failed to get NormalGroup for Beam: " << mUid;
		return TPS_ER_FAILURE;
	}

	std::string planUID = normalGroup->get_planuid();
	if (planUID.empty())
	{
		TPS_LOG_DEV_ERROR << "Failed to get Planuid for Beam: " << mUid;
		return TPS_ER_FAILURE;
	}


	RtPlan *plan = nullptr;
	if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan)|| plan == nullptr) {
		TPS_LOG_DEV_ERROR << "Failed to get plan: " << planUID;
		return TPS_ER_FAILURE;
	}

	//if (plan->GetApprovalStatus() == RTFWK::APPROVED) {
	//    isObjectInDB = true;
	//}
	int bRet = mSaveObjects->EraseObject(mUid, RtDbDef::TYPE_BEAM);
	if (bRet!=TPS_ER_SUCCESS){
		TPS_LOG_DEV_ERROR << "delete beam from DataBase failed! BeamUid:" << mUid;
		return TPS_ER_FAILURE;
	}

	if (TPS_ER_SUCCESS != mDataRepository->GetBeamManager()->DeleteBeam(mUid)) {
		TPS_LOG_DEV_ERROR << "delete beam from beam manager failed! BeamUid:" << mUid;
		return TPS_ER_FAILURE;
	}

    std::string imageUid;
    // delete the beam image relation from ImageDataManager.
    /*TpsImageDataManager *imageDataManager = mDataRepository->GetImageDataManager();
    imageDataManager->DeleteEasyPlanBeamImageUID(mUid);*/
    
    mRenderProxy->DeleteBeam(planUID, mUid);
    /*if (!imageUid.empty()) {
        mRenderProxy->RemoveSpecifiedDataSource(IMAGE, imageUid);
    }*/
    mRenderProxy->SetEasyPlanPixelSpace();
    mRenderProxy->Render();

    // clear dose of points
    /*std::vector<RtPoi*> poiList;
    poiList = mDataRepository->GetPoiManager()->GetAllPoiInPlan(planUID);

    for (auto iterPoi = poiList.begin(); iterPoi != poiList.end(); ++iterPoi)
    {
    if ((*iterPoi) == nullptr)
    {
    continue;
    }
    (*iterPoi)->set_dose(0.0f);
    }*/

	return TPS_ER_SUCCESS;
   
}

TPS_END_NAMESPACE
