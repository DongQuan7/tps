//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_create_merge_plan.cpp 
///  \brief  
///
///  \version 1.0
///  \date  2015/10/31  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_merge_plan.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "boost/algorithm/string.hpp"

#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamoperation.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_drrsetting.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"


TPS_BEGIN_NAMESPACE

#define ADD (0)
#define SUBSTRACT (1)


CreateMergePlanCmd::CreateMergePlanCmd(MultiPlanOperation operateType) :mOperateType(operateType)
{
    mRegistrationMatrix = new Mcsf::Matrix4f;
}

CreateMergePlanCmd::~CreateMergePlanCmd()
{

}

int CreateMergePlanCmd::Execute()
{
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
    std::string primaryPlanUID = patientManager->GetPrimaryPlanUID();
    std::string secondaryPlanUID = patientManager->GetSecondaryPlanUID();

    //0. validate if image are the same or registered
    if (!ValidateImageData_i(primaryPlanUID, secondaryPlanUID)) {
        TPS_LOG_DEV_ERROR << "Plan or image is empty, cannot be compared!";
        return TPS_ER_FAILURE;
    }
    //1. create new plan 
    std::string resultPlanUID = CreatePlan_i(primaryPlanUID, secondaryPlanUID);

    if (resultPlanUID.empty())
    {
        TPS_LOG_DEV_ERROR << "Result plan UID is empty!";
        return TPS_ER_FAILURE;
    }

    patientManager->SetCurrentResultPlanUID(resultPlanUID);
    //2. add beam  & dose list to new plan
    if (!CreateNormalGroup_i(resultPlanUID)) {
        TPS_LOG_DEV_ERROR << "Failed to create plan or copy beam to new plan!";
        return TPS_ER_FAILURE;
    }
    //3. recalculate dose of new plan 
    RtPlan *primaryPlan = nullptr;
    RtPlan *secondaryPlan = nullptr;
    RtPlan *resultPlan = nullptr;

    if (!patientManager->GetPlan(primaryPlanUID, &primaryPlan)
        || primaryPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << primaryPlanUID;
        return TPS_ER_FAILURE;
    }

    if (!patientManager->GetPlan(secondaryPlanUID, &secondaryPlan)
        || secondaryPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << secondaryPlanUID;
        return TPS_ER_FAILURE;
    }

    if (!patientManager->GetPlan(resultPlanUID, &resultPlan)
        || resultPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << resultPlanUID;
        return TPS_ER_FAILURE;
    }

    if (mOperateType == Compare || mOperateType == Merge)
    {
        int operate = mOperateType == Compare ? SUBSTRACT : ADD;
        if (!mDoseProxy->MergeDose(primaryPlan, secondaryPlan,
            operate, mRegistrationMatrix->Inverse(), resultPlan)) {
            TPS_LOG_DEV_ERROR << "Failed to merge dose! ";
            return TPS_ER_FAILURE;
        }

        TpsBeamManager *beamManager = nullptr;
        beamManager = mDataRepository->GetBeamManager();
        if (beamManager == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to get beamManager ";
            return false;
        }
        std::vector<RtBeam*> beams = beamManager->GetBeamsByPlan(resultPlanUID);
        for (auto pBeam = beams.begin(); pBeam != beams.end(); pBeam++)
        {
            (*pBeam)->set_isvisible(false);
        }


        std::string primaryImageUID(primaryPlan->get_imageseriesuid());
        mRenderProxy->SetEvaluationResult(resultPlanUID, primaryImageUID);
        mRenderProxy->RefreshDoseLine(resultPlanUID);
        mRenderProxy->Render();
    }
    else
    {
        if (!mDoseProxy->MergeDose(primaryPlan, secondaryPlan,
            ADD, mRegistrationMatrix->Inverse(), resultPlan)) {
            TPS_LOG_DEV_ERROR << "Failed to merge dose! ";
            return TPS_ER_FAILURE;
        }

        //1. SAVE plan into db
        //mDatabaseWrapper->BeginTrans();
        std::string currentMergedPlanUID = resultPlanUID;

        if (currentMergedPlanUID.empty()) {
            TPS_LOG_DEV_ERROR << "There is no merged plan in TpsPatientManager.";
            //mDatabaseWrapper->RollBack();
            return TPS_ER_FAILURE;
        }

        RtPlan *mergedPlan = nullptr;
        if (!patientManager->GetPlan(currentMergedPlanUID, &mergedPlan))
        {
            TPS_LOG_DEV_ERROR << "There is no merged plan in TpsPatientManager.";
            //mDatabaseWrapper->RollBack();
            return TPS_ER_FAILURE;
        }
        mergedPlan->setdirty_all_fields(true);
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPlanObject(mergedPlan)){
            TPS_LOG_DEV_ERROR << "InsertVoi Object failed! uid:" << mergedPlan->get_uid();
            //mDatabaseWrapper->RollBack();
            return TPS_ER_DB_ERROR;
        }

        //2. SAVE dose line list into db
        TpsDoseManager *doseManager = mDataRepository->GetDoseManager();
        std::vector<RtDoseline*> doselineList;
        if (!doseManager->GetDoseLineList(currentMergedPlanUID, doselineList))
        {
            TPS_LOG_DEV_ERROR << "Failed to get dose line list!Plan uid:" << mergedPlan->get_uid();
            //mDatabaseWrapper->RollBack();
            return TPS_ER_FAILURE;
        }

        for (auto itDoseLine = doselineList.begin(); itDoseLine != doselineList.end(); ++itDoseLine)
        {
            (*itDoseLine)->setdirty_all_fields(true);
            if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDoselineObject(*itDoseLine)){
                TPS_LOG_DEV_ERROR << "InsertDoselineObject failed! uid:" << mergedPlan->get_uid();
                //mDatabaseWrapper->RollBack();
                return TPS_ER_DB_ERROR;
            }
        }

        // 3. SAVE poi
        //std::vector<std::string> poiUIDList;
        //TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
        //poiUIDList = poiManager->GetPoiUidListByPlanUid(currentMergedPlanUID);
        //for (auto itPoiUID = poiUIDList.begin(); itPoiUID != poiUIDList.end(); ++itPoiUID)
        //{
        //    RtPoi *poi = poiManager->GetPOI(*itPoiUID);
        //    poi->setdirty_all_fields(true);
        //    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPoiObject(*poi)){
        //        TPS_LOG_DEV_ERROR << "InsertPoiObject failed! uid:" << mergedPlan->get_uid();
        //        //mDatabaseWrapper->RollBack();
        //        return TPS_ER_DB_ERROR;
        //    }
        //}

        //4. SAVE normgroup
        std::vector<std::string> normgroupUIDList;
        TpsNormGroupManager *normgroupManager = mDataRepository->GetNormgroupManager();
        normgroupUIDList = normgroupManager->GetNormGroupUIDListByPlan(currentMergedPlanUID);
        for (auto itNormgroupUID = normgroupUIDList.begin(); itNormgroupUID != normgroupUIDList.end(); ++itNormgroupUID)
        {
            RtNormgroup *normgroup = nullptr;
            if (!normgroupManager->GetNormGroup(*itNormgroupUID, &normgroup))
            {
                TPS_LOG_DEV_ERROR << "Failed to get normgroup. Plan uid:" << mergedPlan->get_uid();
                //mDatabaseWrapper->RollBack();
                return TPS_ER_FAILURE;
            }
            normgroup->setdirty_all_fields(true);
            if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertNormgroupObject(normgroup)){
                TPS_LOG_DEV_ERROR << "InsertNormgroupObject failed! uid:" << mergedPlan->get_uid();
                //mDatabaseWrapper->RollBack();
                return TPS_ER_DB_ERROR;
            }

			//5. save Drrsetting
			RtDrrsetting *drrSetting = nullptr;
			if (!normgroupManager->GetDrrSetting(*itNormgroupUID, &drrSetting))
			{
				TPS_LOG_DEV_ERROR << "Failed to get drrsetting.normgroup uid:" << *itNormgroupUID;
				//mDatabaseWrapper->RollBack();
				return TPS_ER_FAILURE;
			}
			drrSetting->setdirty_all_fields(true);
			if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDrrsetting(*drrSetting)){
				TPS_LOG_DEV_ERROR << "InsertDrrsetting failed! uid:" << drrSetting->get_uid();
				//mDatabaseWrapper->RollBack();
				return TPS_ER_DB_ERROR;
			}

            // 6. SAVE beam
            std::vector<std::string> beamUIDList;
            TpsBeamManager *beamManager = mDataRepository->GetBeamManager();
            beamUIDList = beamManager->GetBeamUIDListByNormgroupUID(*itNormgroupUID);
            for (auto itBeamUID = beamUIDList.begin(); itBeamUID != beamUIDList.end(); ++itBeamUID)
            {
                RtBeam *beam = nullptr;
                beam = beamManager->GetBeam(*itBeamUID);

                beam->setdirty_all_fields(true);
                if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertBeamObject(*beam)){
                    TPS_LOG_DEV_ERROR << "InsertNormgroupObject failed! uid:" << mergedPlan->get_uid();
                    //mDatabaseWrapper->RollBack();
                    return TPS_ER_DB_ERROR;
                }
            }

            //7. Save DoseGrid
            if (DB_CODES_SUCCESS != mSaveObjects->SaveDosegridsForNormgroup(*itNormgroupUID))
            {
                TPS_LOG_DEV_ERROR << "InsertNormgroupObject failed! uid:" << mergedPlan->get_uid();
                //mDatabaseWrapper->RollBack();
                return TPS_ER_DB_ERROR;
            }
        }
    }
    //if (!mDatabaseWrapper->Commit())
    //{
    //	mDatabaseWrapper->RollBack();
    //	return TPS_ER_FAILURE;
    //}
    return TPS_ER_SUCCESS;
}
void CreateMergePlanCmd::SetNormgroupUids(std::vector<std::string> &normgroupUids)
{
    mNormgroupUids = normgroupUids;
}

void CreateMergePlanCmd::SetDoseLineTemplate(DoseLineTemplate* doseLineTemplate)
{
    mDoseLineTemplate = doseLineTemplate;
}

bool CreateMergePlanCmd::ValidateImageData_i(const std::string &primaryPlanUid, const std::string &secondaryPlanUid)
{
    if (primaryPlanUid.empty() || secondaryPlanUid.empty()) {
        TPS_LOG_DEV_ERROR << "Primary plan or secondary plan uid is empty. ";
        return false;
    }

    RtPlan* primaryPlan = nullptr;
    TpsPatientManager* patientManager = mDataRepository->GetPatientManager();
    if (!patientManager->GetPlan(primaryPlanUid, &primaryPlan)
        || primaryPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << primaryPlanUid;
        return false;
    }

    RtPlan* secondaryPlan = nullptr;
    if (!patientManager->GetPlan(secondaryPlanUid, &secondaryPlan)
        || secondaryPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << secondaryPlanUid;
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
        if (!imageManager->GetImage3DData(primaryImageUID, &primaryImage)) {
            TPS_LOG_DEV_ERROR << "Failed to get primary image ";
            return false;
        }

        return primaryImage->GetRegistrationMatrix(secondaryImageUID, *mRegistrationMatrix);
    }
}

std::string CreateMergePlanCmd::CreatePlan_i(const std::string &primaryPlanUid, const std::string &secondaryPlanUid)
{
    RtPlan* primaryPlan = nullptr;
    TpsPatientManager* patientManager = mDataRepository->GetPatientManager();
    if (!patientManager->GetPlan(primaryPlanUid, &primaryPlan)
        || primaryPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << primaryPlanUid;
        return false;
    }

    RtPlan* secondaryPlan = nullptr;
    if (!patientManager->GetPlan(secondaryPlanUid, &secondaryPlan)
        || secondaryPlan == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get plan: " << secondaryPlanUid;
        return false;
    }

    RtPlan *aPlan = new RtPlan(true);
    aPlan->set_name(mMergedPlanName);
    aPlan->set_courseuid(primaryPlan->get_courseuid());
    aPlan->set_plansourcetype(RtDbDef::MergedPlan);
    aPlan->setdirty_all_fields(true);

    if (mOperateType == Compare)
    {
        float specfiedDose = 0.0f;
        std::vector<RtNormgroup*> mNormGroupList;
        mNormGroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(primaryPlanUid);
        for (int i = 0; i < mNormGroupList.size(); ++i)
        {
            if (mNormGroupList[i] == nullptr || !mNormGroupList[i]->get_isjoined()) continue;
            specfiedDose += mNormGroupList[i]->get_fraction() * mNormGroupList[i]->get_doseperfraction();
        }
        aPlan->set_specifieddose(specfiedDose);
        //if specified dose is invalid, follow the total goal value
    }

    else if (mOperateType == Merge || mOperateType == CreateMerge)
    {
        float specfiedDose = 0.0f;
        std::vector<RtNormgroup*> mNormGroupList;
        mNormGroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(primaryPlanUid);
        for (int i = 0; i < mNormGroupList.size(); ++i)
        {
            if (mNormGroupList[i] == nullptr || !mNormGroupList[i]->get_isjoined()) continue;
            specfiedDose += mNormGroupList[i]->get_fraction() *
                mNormGroupList[i]->get_doseperfraction();
        }

        mNormGroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(secondaryPlanUid);
        for (int i = 0; i < mNormGroupList.size(); ++i)
        {
            if (mNormGroupList[i] == nullptr || !mNormGroupList[i]->get_isjoined()) continue;
            specfiedDose += mNormGroupList[i]->get_fraction() *
                mNormGroupList[i]->get_doseperfraction();
        }
        aPlan->set_specifieddose(specfiedDose);
    }

    std::string planUID = aPlan->get_uid();

    std::string primaryImageUID = primaryPlan->get_imageseriesuid();
    if (primaryImageUID.empty()) {
        TPS_LOG_DEV_ERROR << "The image uid of plan: " << primaryPlan->get_name() << " does not exist.";
        return "";
    }

    aPlan->set_imageseriesuid(primaryImageUID);

    //add default dose grid for try & set new dose grid to result
    if (!GenerateDoseGrid_i(*primaryPlan, *secondaryPlan, *aPlan)) {
        TPS_LOG_DEV_ERROR << "Failed to create dose grid for new plan!";
        return "";
    }

    //TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
    //std::vector<std::string> primaryPOIList =
    //    poiManager->GetPoiUidListByPlanUid(primaryPlanUid);

    //// deep copy poi
    //for (auto itPrimaryPOI = primaryPOIList.begin(); itPrimaryPOI != primaryPOIList.end(); ++itPrimaryPOI) {
    //    RtPoi *primaryPOI = poiManager->GetPOI(*itPrimaryPOI);

    //    // copy poi data
    //    RtPoi *newPOI = new RtPoi(*primaryPOI);
    //    newPOI->set_planuid(planUID);
    //    newPOI->setdirty_all_fields(true);

    //    if (primaryPOI->get_type() == RtDbDef::IsoCenter)
    //    {
    //        newPOI->set_name(primaryPOI->get_name() +"_"+ primaryPlan->get_name());
    //    }


    //    newPOI->set_uid(mDatabaseWrapper->GeneraterUid());
    //    poiUidMaps[primaryPOI->get_uid()] = newPOI->get_uid();

    //    if (!poiManager->AddPOI(newPOI)) {
    //        TPS_LOG_DEV_ERROR << "Failed to add poi to TpsPOIManager.";
    //        return "";
    //    }
    //}

    //std::vector<std::string> secondaryPOIList =
    //    poiManager->GetPoiUidListByPlanUid(secondaryPlanUid);

    //for (auto itSecondaryPOI = secondaryPOIList.begin(); itSecondaryPOI != secondaryPOIList.end(); ++itSecondaryPOI) {
    //    RtPoi *secondaryPOI = poiManager->GetPOI(*itSecondaryPOI);

    //    // copy poi data
    //    RtPoi *newPOI = new RtPoi(*secondaryPOI);
    //    newPOI->setdirty_all_fields(true);
    //    newPOI->set_planuid(planUID);
    //    newPOI->set_uid(mDatabaseWrapper->GeneraterUid());
    //    poiUidMaps[secondaryPOI->get_uid()] = newPOI->get_uid();

    //    if (secondaryPOI->get_type() == RtDbDef::IsoCenter)
    //    {
    //        newPOI->set_name(secondaryPOI->get_name() + "_" + secondaryPlan->get_name());
    //    }

    //    Mcsf::Point3f coordBeforeTrans(newPOI->get_coordinate3dx(), newPOI->get_coordinate3dy(), newPOI->get_coordinate3dz());
    //    Mcsf::Matrix4f matrixSecondary2Primary = mRegistrationMatrix->Inverse();
    //    Mcsf::Point3f coordAfterTrans = matrixSecondary2Primary.Transform(coordBeforeTrans);
    //    newPOI->set_coordinate3dx(coordAfterTrans.GetX());
    //    newPOI->set_coordinate3dy(coordAfterTrans.GetY());
    //    newPOI->set_coordinate3dz(coordAfterTrans.GetZ());

    //    if (!poiManager->AddPOI(newPOI)) {
    //        TPS_LOG_DEV_ERROR << "Failed to add poi to TpsPOIManager.";
    //        return "";
    //    }
    //}

    if (!CreateDoseLineSetting_i(aPlan)){
        TPS_LOG_DEV_ERROR << "Failed to create dose line setting.";
        return "";
    }

    if (!patientManager->AddPlan(aPlan)) {
        TPS_LOG_DEV_ERROR << "Failed to add plan in TpsPatientManager.";
        return "";
    }
    return planUID;
}

bool CreateMergePlanCmd::GenerateDoseGrid_i(RtPlan &primaryPlan, RtPlan &secondaryPlan, RtPlan& aPlan)
{
    TpsDoseManager *doseManager = mDataRepository->GetDoseManager();

    std::string planUID = aPlan.get_uid();
    //RTFWK::RTFWKDLDODoseGrid *primaryDoseGrid = doseManager->GetDoseGrid(mPrimaryPlanUid, DOSE_GRID_PLAN);
    //RTFWK::RTFWKDLDODoseGrid *secondaryDoseGrid = doseManager->GetDoseGrid(mSecondaryPlanUid, DOSE_GRID_PLAN);
    RtDosegrid *primaryDoseGrid = GetPlanDosegrid_i(primaryPlan.get_uid());
    RtDosegrid *secondaryDoseGrid = GetPlanDosegrid_i(secondaryPlan.get_uid());
    if (primaryDoseGrid == nullptr || secondaryDoseGrid == nullptr) {
        TPS_LOG_DEV_ERROR << "At least one of two merging plan dosegrid is nullptr.";
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

    std::string primaryImageUID(primaryPlan.get_imageseriesuid());
    std::string secondaryImageUID(secondaryPlan.get_imageseriesuid());

    TpsImageDataManager *imageManager = mDataRepository->GetImageDataManager();
    if (!imageManager->GetImage3DData(primaryImageUID, &primaryImage)) {
        TPS_LOG_DEV_ERROR << "Failed to get primary image ";
        return false;
    }
    Mcsf::Matrix4f primaryMatG2P = TpsArithmeticConverter::ConvertToMatrix4f(primaryDoseGrid->get_grid_to_pat_t());

    matSG2PG.Prepend(mRegistrationMatrix->Inverse());

    if (!primaryMatG2P.HasInverse()) {
        TPS_LOG_DEV_ERROR << "Failed to get inverse of grid to patient matrix ";
        return false;
    }
    matSG2PG.Prepend(primaryMatG2P.Inverse());

    Mcsf::Point3f allVertics[8]; //secondary dose grid vertics

    allVertics[0] = matSG2PG.Transform(Mcsf::Point3f(0, 0, 0));
    allVertics[1] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, 0, 0));
    allVertics[2] = matSG2PG.Transform(Mcsf::Point3f(0, secCountY - 1, 0));
    allVertics[3] = matSG2PG.Transform(Mcsf::Point3f(0, 0, secCountZ - 1));
    allVertics[4] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, 0, secCountZ - 1));
    allVertics[5] = matSG2PG.Transform(Mcsf::Point3f(0, secCountY - 1, secCountZ - 1));
    allVertics[6] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, secCountY - 1, 0));
    allVertics[7] = matSG2PG.Transform(Mcsf::Point3f(secCountX - 1, secCountY - 1, secCountZ - 1));

    float minX = 0, minY = 0, minZ = 0;
    float maxX = priCountX - 1, maxY = priCountY - 1, maxZ = priCountZ - 1;
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
        TPS_LOG_DEV_ERROR << "Spacing is equal or less than 0 ";
        return false;
    }

    Mcsf::Point3f startPointInPat = primaryMatG2P.Transform(Mcsf::Point3f(minX, minY, minZ));
    Mcsf::Point3f rightTopPointInPat = primaryMatG2P.Transform(Mcsf::Point3f(maxX, maxY, maxZ));

    //LZY: 长宽高必须为正值
    float spanX = abs(rightTopPointInPat.GetX() - startPointInPat.GetX());
    float spanY = abs(rightTopPointInPat.GetY() - startPointInPat.GetY());
    float spanZ = abs(startPointInPat.GetZ() - rightTopPointInPat.GetZ());

    int dimX = (int)(spanX / spacingX) + 1;
    int dimY = (int)(spanY / spacingY) + 1;
    int dimZ = (int)(spanZ / spacingZ) + 1;
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
    float spacing[3] = { spacingX, spacingY, spacingZ };
    int dim[3] = { dimX, dimY, dimZ };
    Mcsf::Vector3f axis[3] = {
        primaryMatG2P.Transform(Mcsf::Vector3f(1.f, 0.f, 0.f)),
        primaryMatG2P.Transform(Mcsf::Vector3f(0.f, 1.f, 0.f)),
        primaryMatG2P.Transform(Mcsf::Vector3f(0.f, 0.f, 1.f)) };

    axis[0].Normalize();
    axis[1].Normalize();
    axis[2].Normalize();

    RtDosegrid* pDoseGrid = nullptr;
    if (TPS_ER_SUCCESS != doseManager->CreateDoseGrid(startPointInPat,
        axis, spacing, dim, pDoseGrid)){
        TPS_LOG_DEV_ERROR << "Failed to create dose grid for plan[" <<
            planUID << "].";
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

bool CreateMergePlanCmd::CreateDoseLineSetting_i(RtPlan* plan)
{
    std::string planUID = plan->get_uid();
    plan->set_displaymode(RtDbDef::Relative);
    plan->set_rendermode(RtDbDef::Line);
    plan->set_hundredpercentdefinition(RtDbDef::SpecifiedDose);

    std::vector<RtDoseline*> doseLineList;
    std::list<DoseLineTemplateItem> templateDoseList;
    RtDoseline* doseLine = nullptr;

    if (mOperateType == Compare)
    {
        int lineNum = 5;
        doseLineList.reserve(lineNum);

        float red(0.f), green(0.f), blue(1.f), stepR(1.f / (lineNum - 1)),
            stepG(2.f / (lineNum - 1)), stepB(-1.f / (lineNum - 1));
        float ratio = -0.04f, stepRatio = 0.02f;
        for (int i = 0; i < lineNum; ++i,
            ratio += stepRatio, red += stepR, blue += stepB,
            green += (i > lineNum / 2 ? -stepG : +stepG)){
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
    }
    else
    {
        mDoseLineTemplate->GetDoseLineTemplateItemList(templateDoseList);
        doseLineList.reserve(templateDoseList.size());
        for (auto item = templateDoseList.crbegin(); item != templateDoseList.crend(); ++item){
            doseLine = new RtDoseline(true);
            doseLine->set_doseratio(item->GetDosePercent());
            const float* fColor = item->GetColor();
            doseLine->set_red(fColor[0]);
            doseLine->set_green(fColor[1]);
            doseLine->set_blue(fColor[2]);
            doseLine->set_alpha(fColor[3]);
            doseLine->set_isvisible(true);
            doseLine->set_planuid(plan->get_uid());
            doseLineList.push_back(doseLine);
        }

    }

    TpsDoseManager *doseManager = mDataRepository->GetDoseManager();
    // TODO: release memory. deep copy
    if (!doseManager->AddDoseLineList(planUID, doseLineList)){
        TPS_LOG_DEV_ERROR << "Failed to add dose line list into dose manager.";
        return false;
    }
    return true;
}

RtDosegrid* CreateMergePlanCmd::GetPlanDosegrid_i(const std::string& planUID)
{
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

    RtPlan* plan = nullptr;
    if (!patientManager->GetPlan(planUID, &plan) ||
        plan == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The plan: " << planUID << " does not exist.";
        return nullptr;
    }

    return plan->get_dosegrid();
}

bool CreateMergePlanCmd::CreateNormalGroup_i(const std::string &mergedPlanUid)
{
    TpsPatientManager *patientManager = nullptr;
    patientManager = mDataRepository->GetPatientManager();
    if (patientManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get patientManager ";
        return false;
    }

    TpsNormGroupManager *normGroupManager = nullptr;
    normGroupManager = mDataRepository->GetNormgroupManager();
    if (normGroupManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normGroupManager ";
        return false;
    }

    TpsBeamManager *beamManager = nullptr;
    beamManager = mDataRepository->GetBeamManager();
    if (beamManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beamManager ";
        return false;
    }

    TpsDoseManager *doseManager = nullptr;
    doseManager = mDataRepository->GetDoseManager();
    if (doseManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get doseManager ";
        return false;
    }

    TpsImageDataManager *imageDataManager = nullptr;
    imageDataManager = mDataRepository->GetImageDataManager();
    if (imageDataManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get imageDataManager ";
        return false;
    }

    TpsMachineManager *machineManager = nullptr;
    machineManager = mDataRepository->GetMachineManager();
    if (machineManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get machineManager ";
        return false;
    }

    TpsPOIManager *PoiManager = nullptr;
    PoiManager = mDataRepository->GetPoiManager();
    if (PoiManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get PoiManager ";
        return false;
    }

    RtPlan *targetPlan = nullptr;
    if (!patientManager->GetPlan(mergedPlanUid, &targetPlan)) {
        TPS_LOG_DEV_ERROR << "Failed to get plan of UID: " << mergedPlanUid;
        return false;
    }

    std::string seriesUid = targetPlan->get_imageseriesuid();
    RtSeries *series = nullptr;
    if (!imageDataManager->GetImageSeries(seriesUid,&series))
    {
        TPS_LOG_DEV_ERROR << "Failed to get imageDataManager of seriesUid " << seriesUid;
        return false;
    }
    std::string patientPosition = series->get_patientposition();

    for (auto iter = mNormgroupUids.begin(); iter != mNormgroupUids.end(); ++iter)
    {
        RtNormgroup *normgroup = nullptr;
        if (!normGroupManager->GetNormGroup(*iter, &normgroup)
            || normgroup == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to get normgroup of UID: " << *iter;
            return false;
        }

        std::string planUid = normgroup->get_planuid();
        RtPlan *plan = nullptr;
        if (!patientManager->GetPlan(planUid, &plan)) {
            TPS_LOG_DEV_ERROR << "Failed to get plan of UID: " << planUid;
            return false;
        }

        std::string mergeNormGroupName("");
        std::string mergeNormGroupUID;

        mergeNormGroupName += plan->get_name();

        RtNormgroup *mergedNormGroup = new RtNormgroup(*normgroup);

        mergedNormGroup->setdirty_all_fields(true);
        // 给它一个新的UID，替换掉前面深拷贝得来的
        mergedNormGroup->set_uid(mDatabaseWrapper->GeneraterUid());
        mergeNormGroupName += normgroup->get_name();
        mergedNormGroup->set_name(mergeNormGroupName);
        mergedNormGroup->set_planuid(mergedPlanUid);
        mergeNormGroupUID = mergedNormGroup->get_uid();
        mergedNormGroup->get_dosegrid()->set_uid(mDatabaseWrapper->GeneraterUid());

        if (!normGroupManager->AddNormGroup(mergedNormGroup)) {
            TPS_LOG_DEV_ERROR << "Failed to add plan in TpsPlanManager.";
            return false;
        }

        if (!normGroupManager->AddNgDoseCalculateMode(mergedNormGroup->get_uid(), RtDbDef::BASE_ON_WEIGHT))
        {
            TPS_LOG_DEV_ERROR << "Failed to add dose calculate mode.";
            return false;
        }

		//Drrsetting
	 	RtDrrsetting* pDrrsetting = nullptr;
		normGroupManager->GetDrrSetting(normgroup->get_uid(), &pDrrsetting);
		if (pDrrsetting != nullptr)
		{
			const RtDrrsetting* drrsetting = pDrrsetting;
			RtDrrsetting *newDrrsetting = new RtDrrsetting(*drrsetting);
			newDrrsetting->set_uid(mDatabaseWrapper->GeneraterUid());
			newDrrsetting->set_normalgroupuid(mergedNormGroup->get_uid());
			if (!normGroupManager->AddDrrSetting(newDrrsetting)) {
				TPS_LOG_DEV_ERROR << "Failed to add DrrSetting in TpsPlanManager.";
				return false;
			}
		}

        //beam list

        std::vector<std::string> beamUIDList;
        std::vector<std::string> doseLineList;

        RtMachine *machine = nullptr;
        if (!machineManager->GetMachine(normgroup->get_machineuid(), machine))
        {
            TPS_LOG_DEV_ERROR << "Failed to  get machine in machineManager of uid " << normgroup->get_machineuid();
            return false;
        }


        beamUIDList = beamManager->GetBeamUIDListByNormgroupUID(*iter);
        for (auto iterBeam = beamUIDList.begin(); iterBeam != beamUIDList.end(); ++iterBeam) {
            RtBeam *tmsBeam = beamManager->GetBeam(*iterBeam);
            // 执行拷贝构造出新的‘merge beam’
            RtBeam *beam = new RtBeam(*tmsBeam);
            beam->setdirty_all_fields(true);

            // 更新Beam的UID
            beam->set_uid(mDatabaseWrapper->GeneraterUid());
            beam->set_normgroupuid(mergeNormGroupUID);
            std::string newBeamUID = beam->get_uid();

            // 更新Beam的Dosegrid的UID
            RtDosegrid* doseGrid = beam->get_dosegrid();
            if (doseGrid != nullptr)
            {
                doseGrid->setdirty_all_fields(true);
                doseGrid->set_uid(mDatabaseWrapper->GeneraterUid());
                doseGrid->set_beamuid(beam->get_uid());
            }

            // 更新shieldBlock
            RtBlock* shieldBlock = beam->get_shield_block();
            if (shieldBlock != nullptr)
            {
                shieldBlock->setdirty_all_fields(true);
                shieldBlock->set_uid(mDatabaseWrapper->GeneraterUid());
                shieldBlock->set_beamuid(beam->get_uid());
            }

            // 更新apertureBlock
            RtBlock* apertureBlock = beam->get_aperture_block();
            if (apertureBlock != nullptr)
            {
                apertureBlock->setdirty_all_fields(true);
                apertureBlock->set_uid(mDatabaseWrapper->GeneraterUid());
                apertureBlock->set_beamuid(beam->get_uid());
            }

            // 处理segments
            std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
            std::vector<RtBeamsegment*> newSegments;
            for (auto iterSegment = segments.begin(); iterSegment != segments.end(); ++iterSegment) {
                RtBeamsegment *newSegment = new RtBeamsegment(**iterSegment);
                newSegment->setdirty_all_fields(true);
                newSegment->set_uid(mDatabaseWrapper->GeneraterUid());
                newSegment->set_beamuid(newBeamUID);

                //will be deleted in SetEndMLCShape
                std::string newSegmentUID = newSegment->get_uid();
                RtMlcshape *newStartMLC = new RtMlcshape(*(*iterSegment)->get_startmlcshape());
                newStartMLC->setdirty_all_fields(true);
                newStartMLC->set_uid(mDatabaseWrapper->GeneraterUid());
                newStartMLC->set_segmentuid(newSegmentUID);
                *newSegment->get_startmlcshape() = *newStartMLC;

                //will be deleted in SetEndMLCShape
                RtMlcshape *newEndMLC = new RtMlcshape(*(*iterSegment)->get_endmlcshape());
                newEndMLC->setdirty_all_fields(true);
                newEndMLC->set_uid(mDatabaseWrapper->GeneraterUid());
                newEndMLC->set_segmentuid(newSegmentUID);
                *newSegment->get_endmlcshape() = *newEndMLC;

                newSegments.push_back(newSegment);

            }
            beam->set_beamsegments(newSegments);
			for (auto itr=newSegments.begin(); itr!=newSegments.end(); ++itr) DEL_PTR(*itr);

            beamManager->AddBeam2Map(beam);

            //重新Initialize一个BeamCS，使用原来的BeamCS，删除的时候会出问题
            if (beam->get_beamsegments().size() <= 0)
            {
                TPS_LOG_DEV_ERROR << "beamsegments size <= 0.";
                return false;
            }
            RtBeamsegment *segment = nullptr;
            segment = beam->get_beamsegments()[0];
            if (segment == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to get segment ";
                return false;
            }
            RtPoi *poi = nullptr;
            poi = PoiManager->GetPOI(beam->get_isocenterpoiuid());
            if (poi == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to get poi of uid " << beam->get_isocenterpoiuid();
                return false;
            }
            Mcsf::Point3f original(poi->get_coordinate3dx(), poi->get_coordinate3dy(), poi->get_coordinate3dz());
            TpsIECCoordinateSystem* newBeamCS = new TpsIECCoordinateSystem();
            if (!newBeamCS->Initialize(original, TpsCmdHelper::GetPatientPosition(patientPosition), machine->get_sad(), segment->get_startgantryangle(), \
                                segment->get_startcollimatorangle(), segment->get_starttablerotationangle()))
            {
                TPS_LOG_DEV_ERROR << "Failed to Initialize newBeamCS.";
                return false;
            }
            if (TPS_ER_SUCCESS != beamManager->AddBeamCS(newBeamUID, newBeamCS))
            {
                TPS_LOG_DEV_ERROR << "Failed to AddBeamCS to beamManager.";
                return false;
            }

            //if (!mRenderProxy->AddBeam(mergeNormGroupUID, newBeamUID)){
            //    TPS_LOG_DEV_ERROR<<"Failed to add beam in render proxy.";
            //    return TPS_ER_FAILURE;
            //}

            //creat fluence map
            FluenceMapEntity* fluenceMap = new FluenceMapEntity();
            if (!mDoseProxy->CalculateFluenceMap(beam, fluenceMap))
            {
                TPS_LOG_DEV_ERROR << "Failed to create FluenceMap for beam[" << beam->get_uid() << "].";
                DEL_PTR(fluenceMap);
                DEL_PTR(beam);
                return TPS_ER_FAILURE;
            }
            mDataRepository->GetBeamManager()->AddFluenceMap(beam->get_uid(), fluenceMap);
        }
    }

    return true;
}


TPS_END_NAMESPACE