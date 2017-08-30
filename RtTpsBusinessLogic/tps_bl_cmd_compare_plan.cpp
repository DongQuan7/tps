//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_compare_plan.cpp
///  \brief   
///
///  \version 1.0
///  \date  2015/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_compare_plan.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
TPS_BEGIN_NAMESPACE

#define ADD (0)
#define SUBSTRACT (1)

ComparePlanCmd::ComparePlanCmd( CertifiedPlan &primaryPlan, CertifiedPlan &secondaryPlan )
: mPrimaryPlan(primaryPlan), mSencondaryPlan(secondaryPlan), mRegistrationMatrix(nullptr){
	mRegistrationMatrix = new Mcsf::Matrix4f;
}

ComparePlanCmd::~ComparePlanCmd()
{

}

int ComparePlanCmd::Execute()
{
	//0. validate if image are the same or registered
	if (!ValidateImageData_i()) {
		TPS_LOG_DEV_ERROR<<"Plan or image is empty, cannot be compared!";
		return TPS_ER_FAILURE;
	}
	//1. create new plan 
	std::string resultPlanUID = CreatePlan_i();

	if (resultPlanUID.empty())
	{
		TPS_LOG_DEV_ERROR<<"Result plan UID is empty!";
		return TPS_ER_FAILURE;
	}

	//2. add beam  & dose list to new plan
	if (!CreateNormalGroup_i(mPrimaryPlan.mPlanUID, resultPlanUID) ||
		!CreateNormalGroup_i(mSencondaryPlan.mPlanUID, resultPlanUID)) {
			TPS_LOG_DEV_ERROR<<"Failed to create plan or copy beam to new plan!";
			return TPS_ER_FAILURE;
	}

	//3. recalculate dose of new plan 
	TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
	RtPlan *primaryPlan	  = nullptr;
	RtPlan *secondaryPlan = nullptr;
	RtPlan *resultPlan	  = nullptr;

	if (!patientManager->GetPlan(mPrimaryPlan.mPlanUID, &primaryPlan)
		|| primaryPlan == nullptr) {
			TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<mPrimaryPlan.mPlanName;
			return TPS_ER_FAILURE;
	}

	if (!patientManager->GetPlan(mSencondaryPlan.mPlanUID, &secondaryPlan)
		|| secondaryPlan == nullptr) {
			TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<mSencondaryPlan.mPlanName;
			return TPS_ER_FAILURE;
	}

	if (!patientManager->GetPlan(resultPlanUID, &resultPlan)
		|| resultPlan == nullptr) {
			TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<resultPlanUID;
			return TPS_ER_FAILURE;
	}

	if (!mDoseProxy->MergeDose(primaryPlan, secondaryPlan,
		SUBSTRACT, mRegistrationMatrix->Inverse(), resultPlan)) {
		TPS_LOG_DEV_ERROR<<"Failed to merge dose! ";
		return TPS_ER_FAILURE;
	}

	std::string primaryImageUID(primaryPlan->get_imageseriesuid());

    mResultPlanUID = resultPlanUID;
	mRenderProxy->SetEvaluationResult(resultPlanUID,primaryImageUID);
	mRenderProxy->Render();
	//4. render new plan
	//mRenderProxy->RefreshDose(resultPlanUID);

	//5. return  new try
	//mResultPlanUID = resultPlanUID;

	return TPS_ER_SUCCESS;
}

bool ComparePlanCmd::ValidateImageData_i()
{
	std::string  primaryPlanUid = mPrimaryPlan.mPlanUID;
	std::string secondaryPlanUid = mSencondaryPlan.mPlanUID;

	if (primaryPlanUid.empty() || secondaryPlanUid.empty()) {
		TPS_LOG_DEV_ERROR<<"Primary plan or secondary plan uid is empty. ";
		return false;
	}

	RtPlan* primaryPlan = nullptr;
	TpsPatientManager* patientManager = mDataRepository->GetPatientManager();
	if (!patientManager->GetPlan(primaryPlanUid, &primaryPlan)
		|| primaryPlan == nullptr) {
			TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<primaryPlanUid;
			return false;
	}

	RtPlan* secondaryPlan = nullptr;
	if (!patientManager->GetPlan(secondaryPlanUid, &secondaryPlan)
		|| secondaryPlan == nullptr) {
			TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<secondaryPlanUid;
			return false;
	}

	std::string primaryImageUID(primaryPlan->get_imageseriesuid());
	std::string secondaryImageUID(secondaryPlan->get_imageseriesuid());

	if (secondaryImageUID == primaryImageUID) {
		//set registration matrix as identity
		*mRegistrationMatrix = mRegistrationMatrix->Identity();
		return true;
	}
	else{
		TpsImage3DEntity *primaryImage = nullptr;
		TpsImageDataManager* imageManager = mDataRepository->GetImageDataManager();
		if (!imageManager->GetImage3DData(primaryImageUID,&primaryImage)) {
			TPS_LOG_DEV_ERROR<<"Failed to get primary image ";
			return false;
		}

		return primaryImage->GetRegistrationMatrix(secondaryImageUID,*mRegistrationMatrix);
	}
}

std::string ComparePlanCmd::CreatePlan_i()
{
	std::string mergePlanName(""); //"plan1_plan2"
	mergePlanName = mPrimaryPlan.mPlanName + "_" + mSencondaryPlan.mPlanName;

	RtPlan *aPlan = new RtPlan(true);
	aPlan->set_name(mergePlanName);
	aPlan->set_courseuid(mPrimaryPlan.mCourseUID);
	aPlan->set_plansourcetype(RtDbDef::MergedPlan);
	std::string planUID = aPlan->get_uid();

	std::string primaryImageUID(mPrimaryPlan.mImageUID);
	if (primaryImageUID.empty()) {
		TPS_LOG_DEV_ERROR<<"The image uid of plan: "<<mPrimaryPlan.mPlanName<<" does not exist.";
		return "";
	}

	aPlan->set_imageseriesuid(primaryImageUID);

	//add default dose grid for try & set new dose grid to result
	if (!GenerateDoseGrid_i(*aPlan)) {
		TPS_LOG_DEV_ERROR<<"Failed to create dose grid for new plan!";
		return "";
	}


	// comments from SCH:
	// access to database to get poilist is wrong and should not be allowed, 
	// instead we should get poilist from manager
	//if(!primaryPlan->GetPOIUIDList(primaryPOIList)){
	//    TPS_LOG_DEV_WARNING<<"Failed to get poi list from primary plan";
	//}
	TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
	std::vector<std::string> primaryPOIList =
		poiManager->GetPoiUidListBySeriesUid(mPrimaryPlan.mImageUID);

	// deep copy poi
	for (auto itPrimaryPOI = primaryPOIList.begin(); itPrimaryPOI != primaryPOIList.end();++itPrimaryPOI) {
		RtPoi *primaryPOI = poiManager->GetPOI(*itPrimaryPOI);

		// copy poi data
		RtPoi *newPOI = new RtPoi(*primaryPOI);
		//newPOI->set_planuid(planUID);

		newPOI->set_uid(mDatabaseWrapper->GeneraterUid());
		//if(RtDbDef::IsoCenter == newPOI.get_type()) {
		//	mIsoCenterReplacement[*itPrimaryPOI] = tempPOI.get_uid();
		//}

		if(!poiManager->AddPOI(newPOI)) {
			TPS_LOG_DEV_ERROR<<"Failed to add poi to TpsPOIManager.";
			return "";
		}
	}

	std::vector<std::string> secondaryPOIList =
		poiManager->GetPoiUidListBySeriesUid(mSencondaryPlan.mImageUID);

	for (auto itSecondaryPOI = secondaryPOIList.begin(); itSecondaryPOI != secondaryPOIList.end(); ++itSecondaryPOI) {
		RtPoi *secondaryPOI = poiManager->GetPOI(*itSecondaryPOI);

		// copy poi data
		RtPoi *newPOI = new RtPoi(*secondaryPOI);
		//newPOI->set_planuid(planUID);
		newPOI->set_uid(mDatabaseWrapper->GeneraterUid());

		//if(RTFWK::IsoCenter == newPOI.GetType()) {
		//	mIsoCenterReplacement[*itSecondaryPOI] = tempPOI.GetUID();
		//}

		Mcsf::Point3f coordBeforeTrans(newPOI->get_coordinate3dx(), newPOI->get_coordinate3dy(), newPOI->get_coordinate3dz());
		Mcsf::Matrix4f matrixSecondary2Primary = mRegistrationMatrix->Inverse();
		Mcsf::Point3f coordAfterTrans = matrixSecondary2Primary.Transform(coordBeforeTrans);
		newPOI->set_coordinate3dx(coordAfterTrans.GetX());
		newPOI->set_coordinate3dy(coordAfterTrans.GetY());
		newPOI->set_coordinate3dz(coordAfterTrans.GetZ());

		if(!poiManager->AddPOI(newPOI)) {
			TPS_LOG_DEV_ERROR<<"Failed to add poi to TpsPOIManager.";
			return "";
		}
	}

	if (!CreateDoseLineSetting_i(aPlan)){
		TPS_LOG_DEV_ERROR<<"Failed to create dose line setting.";
		return "";
	}

	TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
	if(!patientManager->AddPlan(aPlan)) {
		TPS_LOG_DEV_ERROR<<"Failed to add plan in TpsPatientManager.";
		return "";
	}
	//check image data
	//TpsImage3DEntity* image = nullptr;
	//if (!TpsImageDataManager::GetInstance()->GetImage3DData(primaryImageUID, &image)){
	//    TPS_LOG_DEV_ERROR<<"The image entity with UID: "<<primaryImageUID<<" does not exist.";
	//    return "";
	//}

	//IDataSaver *dataSaver = TpsCommonControllerApp::GetInstance()->GetDataSaver();
	//if (!dataSaver->SavePlanToTMS(planUID)) {
	//    TPS_LOG_DEV_ERROR<<"Failed to save plan.";
	//    return "";
	//}
	return planUID;
}

bool ComparePlanCmd::GenerateDoseGrid_i( RtPlan& aPlan )
{
	TpsDoseManager *doseManager = mDataRepository->GetDoseManager();

	std::string planUID = aPlan.get_uid();
	//RTFWK::RTFWKDLDODoseGrid *primaryDoseGrid = doseManager->GetDoseGrid(mPrimaryPlanUid, DOSE_GRID_PLAN);
	//RTFWK::RTFWKDLDODoseGrid *secondaryDoseGrid = doseManager->GetDoseGrid(mSecondaryPlanUid, DOSE_GRID_PLAN);
	RtDosegrid *primaryDoseGrid = GetPlanDosegrid_i(mPrimaryPlan.mPlanUID);
	RtDosegrid *secondaryDoseGrid = GetPlanDosegrid_i(mSencondaryPlan.mPlanUID);
	if (primaryDoseGrid==nullptr || secondaryDoseGrid==nullptr) {
		TPS_LOG_DEV_ERROR<<"At least one of two merging plan dosegrid is nullptr.";
		return false;
	}

	//Mcsf::Point3f primaryStart(primaryDoseGrid->m_pCoordinate3DStart);
	float priSpacingX = primaryDoseGrid->get_xcoordinate3dinc();
	float priSpacingY = primaryDoseGrid->get_ycoordinate3dinc();
	float priSpacingZ = primaryDoseGrid->get_zcoordinate3dinc();
	int priCountX = primaryDoseGrid->get_xcount();
	int priCountY = primaryDoseGrid->get_ycount();
	int priCountZ = primaryDoseGrid->get_zcount();

	//Mcsf::Point3f secondaryStart(secondaryDoseGrid->m_pCoordinate3DStart);
	float secSpacingX = secondaryDoseGrid->get_xcoordinate3dinc();
	float secSpacingY = secondaryDoseGrid->get_ycoordinate3dinc();
	float secSpacingZ = secondaryDoseGrid->get_zcoordinate3dinc();
	int secCountX = secondaryDoseGrid->get_xcount();
	int secCountY = secondaryDoseGrid->get_ycount();
	int secCountZ = secondaryDoseGrid->get_zcount();

	//secondary dose grid cs to primary dose grid cs
	Mcsf::Matrix4f matSG2PG = TpsArithmeticConverter::ConvertToMatrix4f(secondaryDoseGrid->get_grid_to_pat_t()); 

	TpsImage3DEntity *primaryImage = nullptr;

	std::string primaryImageUID(mPrimaryPlan.mImageUID);
	std::string secondaryImageUID(mSencondaryPlan.mImageUID);

	TpsImageDataManager *imageManager = mDataRepository->GetImageDataManager();
	if (!imageManager->GetImage3DData(primaryImageUID,&primaryImage)) {
		TPS_LOG_DEV_ERROR<<"Failed to get primary image ";
		return false;
	}
	Mcsf::Matrix4f primaryMatG2P = TpsArithmeticConverter::ConvertToMatrix4f(primaryDoseGrid->get_grid_to_pat_t());

	matSG2PG.Prepend(mRegistrationMatrix->Inverse());

	if (!primaryMatG2P.HasInverse()) {
		TPS_LOG_DEV_ERROR<<"Failed to get inverse of grid to patient matrix ";
		return false;
	}
	matSG2PG.Prepend(primaryMatG2P.Inverse());

	Mcsf::Point3f allVertics[8]; //secondary dose grid vertics

	allVertics[0] = matSG2PG.Transform(Mcsf::Point3f(0,0,0));
	allVertics[1] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, 0, 0));
	allVertics[2] = matSG2PG.Transform(Mcsf::Point3f(0, secCountY - 1, 0));
	allVertics[3] = matSG2PG.Transform(Mcsf::Point3f(0,0, secCountZ - 1));
	allVertics[4] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, 0, secCountZ - 1));
	allVertics[5] = matSG2PG.Transform(Mcsf::Point3f(0, secCountY - 1, secCountZ - 1));
	allVertics[6] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, secCountY - 1, 0));
	allVertics[7] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, secCountY - 1, secCountZ - 1));

	float minX = 0, minY = 0, minZ = 0;
	float maxX = priCountX - 1, maxY = priCountY -1 , maxZ = priCountZ - 1;
	for (int i = 0; i < 8; ++i) {
		minX = TPS_MIN(minX, allVertics[i].GetX());
		minY = TPS_MIN(minY, allVertics[i].GetY());
		minZ = TPS_MIN(minZ, allVertics[i].GetZ());

		maxX = TPS_MAX(maxX, allVertics[i].GetX());
		maxY = TPS_MAX(maxY, allVertics[i].GetY());
		maxZ = TPS_MAX(maxZ, allVertics[i].GetZ());
	}

	float spacingX = TPS_MAX(priSpacingX, secSpacingX);
	float spacingY = TPS_MAX(priSpacingY, secSpacingY);
	float spacingZ = TPS_MAX(priSpacingZ, secSpacingZ);
	if (spacingX < 1e-6 || spacingY < 1e-6 || spacingZ < 1e-6
		|| abs(spacingX) <= 1e-6 || abs(spacingY) <= 1e-6 || abs(spacingZ) <= 1e-6) {
			TPS_LOG_DEV_ERROR<<"Spacing is equal or less than 0 ";
			return false;
	}

	Mcsf::Point3f startPointInPat = primaryMatG2P.Transform(Mcsf::Point3f(minX, minY, minZ));
	Mcsf::Point3f rightTopPointInPat = primaryMatG2P.Transform(Mcsf::Point3f(maxX, maxY, maxZ));

	//LZY: 长宽高必须为正值
	float spanX = abs(rightTopPointInPat.GetX() - startPointInPat.GetX());
	float spanY = abs(rightTopPointInPat.GetY() - startPointInPat.GetY());
	float spanZ = abs(startPointInPat.GetZ() -rightTopPointInPat.GetZ()) ;

	int dimX = (int)(spanX/spacingX) + 1;
	int dimY = (int)(spanY/spacingY) + 1;
	int dimZ = (int)(spanZ/spacingZ) + 1;
	if (0 != (int)(spanX * 1e6 / spacingX) % 1000000) {
		dimX += 1;
	}
	if (0 != (int)(spanY * 1e6 / spacingY) % 1000000) {
		dimY += 1;
	}
	if (0 != (int)(spanZ * 1e6 / spacingZ) % 1000000) {
		dimZ += 1;
	}

	//////////////////////////////////////////////////////////////////////////
	////Set the axis of dose grid 
	//////////////////////////////////////////////////////////////////////////
	float spacing[3] = {spacingX, spacingY, spacingZ};
	int dim[3] = {dimX, dimY, dimZ};
	Mcsf::Vector3f axis[3] = {
		primaryMatG2P.Transform(Mcsf::Vector3f(1.f, 0.f, 0.f)),
		primaryMatG2P.Transform(Mcsf::Vector3f(0.f, 1.f, 0.f)),
		primaryMatG2P.Transform(Mcsf::Vector3f(0.f, 0.f, 1.f))};

	axis[0].Normalize();
	axis[1].Normalize();
	axis[2].Normalize();

	RtDosegrid* pDoseGrid = nullptr;
	if (TPS_ER_SUCCESS != doseManager->CreateDoseGrid(startPointInPat,
		axis,spacing,dim, pDoseGrid)){
			TPS_LOG_DEV_ERROR<<"Failed to create dose grid for plan["<<
				planUID<<"].";
			return false;
	}

	RtDosegrid *originalDoseGrid = aPlan.get_dosegrid();
	std::string dosegridUid = originalDoseGrid->get_uid();
	*originalDoseGrid = *pDoseGrid;
	originalDoseGrid->set_uid(dosegridUid);
	originalDoseGrid->set_planuid(aPlan.get_uid());

	delete pDoseGrid; pDoseGrid = nullptr;

	return true;
}

RtDosegrid* ComparePlanCmd::GetNormgroupDosegrid_i( const std::string& strNgUID )
{
	RtNormgroup* pNg = nullptr;
	TpsNormGroupManager *ngManager = mDataRepository->GetNormgroupManager();
	if (!ngManager->GetNormGroup(strNgUID, &pNg)) {
		TPS_LOG_DEV_ERROR<<"Failed to get normgroup["<<strNgUID<<"] from normgroup manager";
		return nullptr;
	}

	return pNg->get_dosegrid();
}

bool ComparePlanCmd::CreateDoseLineSetting_i( RtPlan* plan )
{
	std::string planUID = plan->get_uid();
	plan->set_specifieddose(0.0);
	plan->set_hundredpercentdefinition(RtDbDef::MaxDose);
	plan->set_displaymode(RtDbDef::Relative);
	plan->set_rendermode(RtDbDef::Line);

	std::vector<RtDoseline*> doseLineList;
	int lineNum = 5;
	doseLineList.reserve(lineNum);

	float red(0.f), green(0.f), blue(1.f), stepR(1.f / (lineNum - 1)),
		stepG(2.f / (lineNum - 1)), stepB(-1.f / (lineNum - 1));
	float ratio = -0.04f, stepRatio = 0.02f;
	for (int i = 0; i < lineNum; ++i, 
		ratio += stepRatio, red += stepR, blue += stepB, 
		green += (i > lineNum / 2 ? - stepG : + stepG)){
			if (ratio == 0.0f)
			{
				continue;
			}
			RtDoseline* doseLine = new RtDoseline(true);
			doseLine->set_doseratio(ratio);
			doseLine->set_planuid(planUID);
			doseLine->set_isvisible(true);
			doseLine->set_red(red);
			doseLine->set_green(green);
			doseLine->set_blue(blue);
			doseLine->set_alpha(1.f);
			doseLineList.push_back(doseLine);
	}

	TpsDoseManager *doseManager = mDataRepository->GetDoseManager();
	// TODO: release memory. deep copy
	if (!doseManager->AddDoseLineList(planUID, doseLineList)){
		TPS_LOG_DEV_ERROR<<"Failed to add dose line list into dose manager.";
		return false;
	}
	return true;
}

bool ComparePlanCmd::CreateNormalGroup_i( const std::string &originalPlanUID, const std::string &mergePlan/*, std::vector<std::string> *mergePlanList*/ )
{
	TpsPatientManager *patientManager = nullptr;
	patientManager = mDataRepository->GetPatientManager();

	TpsNormGroupManager *normGroupManager = nullptr;
	normGroupManager = mDataRepository->GetNormgroupManager();

	TpsBeamManager *beamManager = nullptr;
	beamManager = mDataRepository->GetBeamManager();

	TpsDoseManager *doseManager = nullptr;
	doseManager = mDataRepository->GetDoseManager();

	std::vector<std::string> normGroupList =
		normGroupManager->GetNormGroupUIDListByPlan(originalPlanUID);

	std::vector<std::string> beamUIDList;
	std::vector<std::string> doseLineList;
	std::string mergeNormGroupName("");
	std::string mergeNormGroupUID;


	RtPlan *plan = nullptr;
	if(!patientManager->GetPlan(originalPlanUID, &plan)) {
		TPS_LOG_DEV_ERROR<<"Failed to get plan of UID: "<<originalPlanUID;
		return false;
	}

	mergeNormGroupName += plan->get_name();

	//IDataSaver *dataSaver = TpsCommonControllerApp::GetInstance()->GetDataSaver();

	// 这里的逻辑是：Merge的Plan中，从原来的Plan过来的Normgroup都必须是新的实例，无论在
	// DB还是内存中。注意：
	auto iterNormGroup = normGroupList.begin();
	for (; iterNormGroup != normGroupList.end(); ++iterNormGroup) {
		RtNormgroup *tmsNormGroup = nullptr;
		if(!normGroupManager->GetNormGroup(*iterNormGroup, &tmsNormGroup)) {
			TPS_LOG_DEV_ERROR<<"Failed to get normal group for plan: "<<originalPlanUID;
			return false;
		}

		RtNormgroup *normGroup = new RtNormgroup(*tmsNormGroup);
		// 给它一个新的UID，替换掉前面深拷贝得来的
		normGroup->set_uid(mDatabaseWrapper->GeneraterUid());
		mergeNormGroupName += tmsNormGroup->get_name();
		normGroup->set_name(mergeNormGroupName);
		normGroup->set_planuid(mergePlan);
		mergeNormGroupUID = normGroup->get_uid();

		// TMS提供的拷贝构造，会连UID一并拷贝，所以这里需要替换上新的UID
		normGroup->get_dosegrid()->set_uid(mDatabaseWrapper->GeneraterUid());

		//doseManager->AddDoseGrid(mergeNormGroupUID,mergeNgDoseGrid, DOSE_GRID_NORMGROUP);
		if(!normGroupManager->AddNormGroup(normGroup)) {
			TPS_LOG_DEV_ERROR<<"Failed to add plan in TpsPlanManager.";
			return false;
		}

        if (!normGroupManager->AddNgDoseCalculateMode(normGroup->get_uid(), RtDbDef::BASE_ON_WEIGHT))
        {
            TPS_LOG_DEV_ERROR << "Failed to add dose calculate mode.";
            return false;
        }

		//if (!dataSaver->SaveNormGroupToTMS(mergeNormGroupUID)) {
		//    TPS_LOG_DEV_ERROR<<"Failed to Save plan.";
		//    return false;
		//}

		//beam list
		beamUIDList = beamManager->GetBeamUIDListByNormgroupUID(*iterNormGroup);
		for (auto iterBeam = beamUIDList.begin(); iterBeam != beamUIDList.end(); ++iterBeam) {
			RtBeam *tmsBeam = beamManager->GetBeam(*iterBeam);
			// 执行拷贝构造出新的‘merge beam’
			RtBeam *beam = new RtBeam(*tmsBeam);

			// 更新Beam的UID
			beam->set_uid(mDatabaseWrapper->GeneraterUid());
			beam->set_normgroupuid(mergeNormGroupUID);
			std::string newBeamUID = beam->get_uid();

			// update isocenter uid of beam
			std::string oldIsoCenterUID = beam->get_isocenterpoiuid();
			//if (mIsoCenterReplacement[oldIsoCenterUID].empty()) {
			//	TPS_LOG_DEV_ERROR<<"Failed to find new isocenter uid recorded in evaluation app.";
			//	continue;
			//}

			//beam->set_isocenterpoiuid(mIsoCenterReplacement[oldIsoCenterUID]);

			// 跟新Beam的Dosegrid的UID
			beam->get_dosegrid()->set_uid(mDatabaseWrapper->GeneraterUid());

			// 处理segments
			std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
			std::vector<RtBeamsegment*> newSegments;
			for (auto iterSegment = segments.begin(); iterSegment != segments.end(); ++iterSegment) {
				RtBeamsegment *newSegment = new RtBeamsegment(**iterSegment);
				newSegment->set_uid(mDatabaseWrapper->GeneraterUid());
				newSegment->set_beamuid(newBeamUID);

				//will be deleted in SetEndMLCShape
				std::string newSegmentUID = newSegment->get_uid();
				RtMlcshape *newStartMLC = new RtMlcshape(*(*iterSegment)->get_startmlcshape());
				newStartMLC->set_uid(mDatabaseWrapper->GeneraterUid());
				newStartMLC->set_segmentuid(newSegmentUID);
				*newSegment->get_startmlcshape() = *newStartMLC;

				//will be deleted in SetEndMLCShape
				RtMlcshape *newEndMLC = new RtMlcshape(*(*iterSegment)->get_endmlcshape());
				newEndMLC->set_uid(mDatabaseWrapper->GeneraterUid());
				newEndMLC->set_segmentuid(newSegmentUID);
				*newSegment->get_endmlcshape() = *newEndMLC;

				newSegments.push_back(newSegment);

			}
			beam->set_beamsegments(newSegments);
			for (auto itr=newSegments.begin(); itr!=newSegments.end(); ++itr) DEL_PTR(*itr);

			beamManager->AddBeam2Map(beam);

            TpsIECCoordinateSystem *iecCS = beamManager->GetBeamCS(*iterBeam);
            beamManager->AddBeamCS(newBeamUID, iecCS);
            if (!mRenderProxy->AddBeam(mergeNormGroupUID, newBeamUID)){
                TPS_LOG_DEV_ERROR<<"Failed to add beam in render proxy.";
                return TPS_ER_FAILURE;
            }
		}

	}

	return true;
}

RtDosegrid* ComparePlanCmd::GetPlanDosegrid_i( const std::string& planUID )
{
	TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

	RtPlan* plan = nullptr;
	if(!patientManager->GetPlan(planUID, &plan) ||
		plan == nullptr)
	{
		TPS_LOG_DEV_ERROR<<"The plan: "<<planUID<<" does not exist.";
		return nullptr;
	}

	return plan->get_dosegrid();
}

std::string ComparePlanCmd::GetComparedPlanUID()
{
    return mResultPlanUID;
}
TPS_END_NAMESPACE