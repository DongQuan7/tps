////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_pca_planningdeletecmd.h
/// 
///  \brief class TpsPlanningDeleteCmd definition 
/// 
///  \version 1.0
/// 
///  \date    2014/3/08
//////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_delete_plans.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_optgoalmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"


#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_clear_hot_cold_spots_operation.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;

TpsPlanningDeleteCmd::TpsPlanningDeleteCmd(std::string sUid, int iType) : mUid(sUid), mType(iType){
}

TpsPlanningDeleteCmd::~TpsPlanningDeleteCmd() {

}

int TpsPlanningDeleteCmd::Execute() 
{
    bool needRender = false;
    int iRet = TPS_ER_FAILURE;
    if (0 == mType) {
        iRet = DeletePlan_i(mUid);
        if(TPS_ER_SUCCESS != iRet) {
            TPS_LOG_DEV_ERROR<<"delete try failed!";
            return iRet;
        }
        needRender = true;
    }
    else if (1 == mType) {
        iRet = DeleteNormGroup_i(mUid);
        if(TPS_ER_SUCCESS != iRet) {
            TPS_LOG_DEV_ERROR<<"delete plan failed!";
            return iRet;
        }
        needRender = true;
    }
    else{//3
        iRet = DeleteBeam_i(mUid);
        if(TPS_ER_SUCCESS != iRet) {
            TPS_LOG_DEV_ERROR<<"delete beam failed!";
            return iRet;
        }
        needRender = true;
    }

    if (needRender){
        mRenderProxy->Render();
    }
    iRet = TPS_ER_SUCCESS;
    return iRet;
}

int TpsPlanningDeleteCmd::DeleteBeam_i(const std::string &beamUid)
{
    RtBeam* pBeam = mDataRepository->GetBeamManager()->GetBeam(beamUid);
    if (nullptr == pBeam){
        TPS_LOG_DEV_ERROR<<"failed to delete beam:"<<beamUid;
        return TPS_ER_FAILURE;
    }

    const std::string sNgUid = pBeam->get_normgroupuid();

    if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(beamUid, RtDbDef::TYPE_BEAM)){
        TPS_LOG_DEV_ERROR<<"failed to delete beam:"<<beamUid;
        return TPS_ER_DB_ERROR;
    }

    //删除beam的时候会删除DRR
    int retCode = DeleteDRRImage(beamUid);
    if(TPS_ER_SUCCESS != retCode){
        TPS_LOG_DEV_ERROR<<"failed to delete DRRImage from beam:"<<beamUid;
        return retCode;
    }

    retCode = (TPS_ERROR_CODES)mDataRepository->GetBeamManager()->DeleteBeam(beamUid);
    if(retCode != TPS_ER_SUCCESS){
        TPS_LOG_DEV_ERROR<<"failed to delete beam:"<<beamUid<<" from BeamManager";
        return retCode;
    }

    bool bRet = mRenderProxy->DeleteBeam(sNgUid, beamUid);
    if(!bRet){
        TPS_LOG_DEV_ERROR<<"failed to delete beam from render proxy!";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}
//现在load plan会默认load DRRImage，所以删除beam的时候要删除相应的DRRImage，此函数只负责删除manager里的drr数据
int TpsPlanningDeleteCmd::DeleteDRRImage(const std::string &beamUid)
{
    auto imageDataManager = mDataRepository->GetImageDataManager();
    if (imageDataManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get imageDataManager ";
        return false;
    }
    TpsImage2DEntity* image2DEntity = nullptr;
    bool iRet = imageDataManager->Get2DImageDataByBeamUID(beamUid, image2DEntity);
    //beam没有关联的DRR图像是正常情况
    if (!iRet) return TPS_ER_SUCCESS;

    std::string imageUID = "";
    if (!image2DEntity->GetUID(imageUID))
    {
        TPS_LOG_DEV_ERROR<<"Failed get image2D uid.";
        return TPS_ER_FAILURE;
    }

    if (!imageDataManager->DeleteImage2D(imageUID))
    {
        TPS_LOG_DEV_ERROR<<"failed to delete Image2D:"<<imageUID<<" from ImageDataManager";
        return TPS_ER_FAILURE;
    }

    bool bRet = mRenderProxy->DeleteDrrImage(beamUid, imageUID);
    if(!bRet){
        TPS_LOG_DEV_ERROR<<"failed to delete drrimage from render proxy!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int TpsPlanningDeleteCmd::DeleteNormGroup_i(const std::string& normGroupUID)
{
    TPS_LOG_DEV_INFO << "TpsPlanningDeleteCmd Delete normgroup begin with uid:"<<normGroupUID;
    std::vector<RtBeam*> vBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(normGroupUID);
    for (auto itr = vBeams.begin(); itr != vBeams.end(); ++itr) 
    {
        if(TPS_ER_SUCCESS != DeleteBeam_i((*itr)->get_uid()))
        {
            TPS_LOG_DEV_ERROR<<"delete beam failed"<<*itr;
            return TPS_ER_FAILURE;
        }
    }

    RtNormgroup *normGroup = nullptr;
    TpsNormGroupManager *normGroupManager = mDataRepository->GetNormgroupManager();
    if (!normGroupManager->GetNormGroup(normGroupUID, &normGroup)) {
        TPS_LOG_DEV_ERROR<<"Failed to get plan from plan manager!"
            <<normGroupUID;
        return TPS_ER_FAILURE;
    }

    std::string planUid = normGroup->get_planuid();
    RtPlan* plan;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan)){
        TPS_LOG_DEV_ERROR<<"The plan with UID: "<<normGroup->get_planuid()<<" does not exist.";
        return TPS_ER_FAILURE;
    }

    if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(normGroupUID, RtDbDef::TYPE_NORMGROUP)){
        TPS_LOG_DEV_ERROR<<"failed to delete normgroup:"<<normGroupUID;
        return TPS_ER_DB_ERROR;
    }

    RtDrrsetting *pDrrsetting = nullptr;
    normGroupManager->GetDrrSetting(normGroupUID, &pDrrsetting);
    if(pDrrsetting != nullptr)
    {
        if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(pDrrsetting->get_uid(), RtDbDef::TYPE_DRRSETTING)){
            TPS_LOG_DEV_ERROR<<"failed to delete normgroup:"<<normGroupUID;
            return TPS_ER_DB_ERROR;
        }

        normGroupManager->RemoveDrrSetting(normGroupUID);
    }

    // delete ng optgoal
    std::vector<RtDoseoptgoalbase*> vDoseOptGoals;
    if (mDataRepository->GetOptGoalManager()->GetOptGoalListByNormalizationGroupUID(normGroupUID, vDoseOptGoals))
    {
        for (auto itOptGoal = vDoseOptGoals.begin(); itOptGoal != vDoseOptGoals.end(); ++itOptGoal)
        {
            if (TPS_ER_SUCCESS != mSaveObjects->EraseObject((*itOptGoal)->get_uid(), RtDbDef::TYPE_DOSEOPTGOALBASE))
            {
                TPS_LOG_DEV_ERROR << "failed to delete optgoal:" << (*itOptGoal)->get_uid();
                return TPS_ER_DB_ERROR;
            }
            mDataRepository->GetOptGoalManager()->RemoveOptGoal((*itOptGoal)->get_uid());
        }
    }

    normGroupManager->RemoveNormGroup(normGroupUID);
    normGroupManager->RemoveNormGroupDoseCalculateMode(normGroupUID);
    mRenderProxy->DeleteNormalGroup(planUid, normGroupUID);
    mRenderProxy->RefreshDoseLine(planUid);
    TPS_LOG_DEV_INFO << "TpsPlanningDeleteCmd Delete normgroup end with uid:"<<normGroupUID;
    return TPS_ER_SUCCESS;
}

int TpsPlanningDeleteCmd::DeletePlan_i(const std::string &planUid) 
{
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

    std::vector<std::string> normGroupUidList = 
        mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUid);
    for (auto itr = normGroupUidList.begin(); itr != normGroupUidList.end(); ++itr) {
        if(TPS_ER_SUCCESS != DeleteNormGroup_i(*itr)) {
            TPS_LOG_DEV_ERROR<<"delete plan failed!"<<*itr;
            return TPS_ER_FAILURE;
        }
    }

    mRenderProxy->RefreshDose(planUid);
    
    mRenderProxy->Synchronize();

    //clear DVH
    //if(TPS_ER_SUCCESS != mCommonController->ClearDoseDVHForPlan(planUid)){
    //    TPS_LOG_DEV_ERROR<<"ClearDoseDVHForPlan failed with PlanUID:"<<planUid;
    //    return false;
    //}

    RtPlan* pPlan = nullptr;
    mDataRepository->GetPatientManager()->GetPlan(planUid, &pPlan);
    if(pPlan == nullptr){
        return TPS_ER_FAILURE;
    }
    std::string imgUid = pPlan->get_imageseriesuid();

    //Delete DoseLine
    std::vector<RtDoseline*> vDoseLine;
    mDataRepository->GetDoseManager()->GetDoseLineList(planUid, vDoseLine);
    for (auto itrDL=vDoseLine.begin(); itrDL!=vDoseLine.end(); ++itrDL){
        if(TPS_ER_SUCCESS != mSaveObjects->EraseObject((*itrDL)->get_uid(), RtDbDef::TYPE_DOSELINE)){
            TPS_LOG_DEV_WARNING<<"failed to delete poi:"<<planUid;
            //mLastError = TPS_ER_DB_ERROR;
            //return false;
        }
    }

    //清空剂量热区冷区
    std::shared_ptr<TpsClearHotColdSpotsOperation> spClearHotColdSpotOp(new TpsClearHotColdSpotsOperation(planUid));
    mRenderProxy->ProcessOperation(spClearHotColdSpotOp);

    if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(planUid, RtDbDef::TYPE_PLAN)){
        TPS_LOG_DEV_ERROR<<"failed to delete poi:"<<planUid;
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetDoseManager()->RemoveDoseLineList(planUid)){
        TPS_LOG_DEV_ERROR<<"Failed to delete doseLine from TpsDoseManager";
        return TPS_ER_FAILURE;
    }

    if (!patientManager->RemovePlan(planUid)){
        TPS_LOG_DEV_ERROR<<"remove try from patient manager failed."<<planUid;
        return TPS_ER_FAILURE;
    }

    //const std::string sPlanName = mDataRepository->GetPatientManager()->GetCurrentPlanName();
    //update plan name for image text in MPR
    //this->mPlanController->UpdateImageText(sPlanName);

    //renderProxy->RefreshDose(planUid);
    //如果要删除的plan为当前显示的plan，清除data source
    mRenderProxy->RemoveSpecifiedDataSource(PLAN, planUid);
    mRenderProxy->ClearGraphicObjectsViaDataSource(PLAN, planUid);
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
