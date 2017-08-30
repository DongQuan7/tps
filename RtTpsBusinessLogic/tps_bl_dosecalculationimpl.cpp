//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_bl_dosecalculationimpl.cpp
///
/// \brief   DoseCalculationImpl
/// 
/// \version 1.0
/// 
/// \date    2014/01/25
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"



//TPS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "RtTpsCertifiedLib/rt_tps_certified_poidose.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"

#include "RtTpsRenderLibrary/tps_rl_clear_hot_cold_spots_operation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"
#include "RtTpsRenderLibrary/tps_rl_addpoioperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatepoioperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamoperation.h"
#include "RtTpsRenderLibrary/tps_rl_dose_update_mesh_operation.h"
#include "RtTpsRenderLibrary/tps_rl_deleteoperation.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

// local
#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "tps_logger.h"
#include "tps_performance.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"


TPS_BEGIN_NAMESPACE

DoseCalculationImpl::DoseCalculationImpl(void):mSaveObjects(nullptr)
{
}

DoseCalculationImpl::~DoseCalculationImpl(void)
{
}

bool DoseCalculationImpl::Initialize()
{ 
    mDoseProxy->SetDataRepository(mDataRepository);
    if (nullptr == mSaveObjects)
    {
        return false;
    }
    return mDoseProxy->Initialize();
}

bool DoseCalculationImpl::Finalize()
{
    return true;
}

bool DoseCalculationImpl::CalculateNormGroupDvh(
    const std::string& normGroupUid,
    const std::vector<std::string>& voiUidList,
    std::vector<RtDvh*>& tpsDVHList)
{
    try
    {
        if (normGroupUid.empty())
        {
            TPS_LOG_DEV_ERROR << "The normGroup Uid is empty.";
            return false;
        }
        RtNormgroup* normGroup = nullptr;
        if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normGroupUid, &normGroup) && normGroup == nullptr)
        {
            TPS_LOG_DEV_ERROR << "The normalgroup does not exist, uid: " << normGroupUid;
            return false;
        }

        ADD_WORKFLOW_KEYPOINT<<"[NG DVH Calc]: start to calculate normgroup's DVH";
        if (!mDoseProxy->CalculateNormGroupDVH(normGroupUid, voiUidList, tpsDVHList))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate DVH for target[" << normGroupUid << "].";
            return false;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";
        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

bool DoseCalculationImpl::CalculateBioParams(
    const std::string& normalGroupUid,
    const std::vector<std::string>& voiUidList,
    std::vector<ROIEntity*>& vVois)
{
    try
    {
        if (normalGroupUid.empty())
        {
            TPS_LOG_DEV_ERROR << "The normGroup Uid is empty.";
            return false;
        }

        if (voiUidList.size() < 1)
        {
            TPS_LOG_DEV_ERROR << "The voiUidList is empty.";
            return false;
        }

        if (!mDoseProxy->ComputeTcpNTcpByNormalGroup(normalGroupUid, voiUidList, vVois))
        {
            TPS_LOG_DEV_ERROR<<"Failed to Compute TcpNTcp for normalgroup[" << normalGroupUid << "].";
            return false;
        }

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

int DoseCalculationImpl::CalculateDoseForPlan(const std::string& planUID)
{
    try
    {
        if (planUID.empty())
        {
            TPS_LOG_DEV_ERROR << "The plan UID is empty.";
            return TPS_ER_FAILURE;
        }
        RtPlan* plan = nullptr;
        if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan) && plan == nullptr)
        {
            TPS_LOG_DEV_ERROR << "The plan does not exist, uid: " << planUID;
            return TPS_ER_FAILURE;
        }

        // step 1. calculation
        ADD_WORKFLOW_KEYPOINT<<"[PLAN DOSE CALC]:start dose calculation for plan["<<planUID<<"].";
        std::vector<std::string> normGroupUIDList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
        ADD_WORKFLOW_KEYPOINT<<"["<<normGroupUIDList.size()<<"] normgroups selected for calculation.";
        for (int i = 0; i < normGroupUIDList.size(); ++i)
        {
            ADD_WORKFLOW_KEYPOINT<<"start calculate dose for ["<<i<<"] normgroup, uid:["<<normGroupUIDList[i]<<".";
            int calcResult = mDoseProxy->CalculateDoseForNormgroup(normGroupUIDList[i]);
            if (TPS_ER_SUCCESS != calcResult)
            {
                TPS_LOG_DEV_ERROR << "CalculateDose_i failed with NormGroupUID:"<< normGroupUIDList[i];
                ClearDoseForNormgroup(normGroupUIDList[i], true);
                return calcResult;
            }
            ADD_WORKFLOW_KEYPOINT<<"DONE!";
        }

        ADD_WORKFLOW_KEYPOINT<<"start compose dose of plan";
        if (!mDoseProxy->ComposeDoseForPlan(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to compose dose grid for plan[" << planUID << "].";
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";
        
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start calculate dvh and update voi dose.";
        if (!CalculateDvhAndUpdateVoiDose(planUID))
        {
            TPS_LOG_DEV_ERROR << "UpdateDvhAfterDoseCalculation: Failed to calculate dvh of plan: " << planUID;
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start update poi dose.";
        if (!UpdatePoiDoseAfterDoseCalculation_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update poi dose.";
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start update max point dose.";
        if (!UpdateMaxPoint_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update max point dose.";
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";


        UpdateBeamDoseValidationByPlan_i(planUID);
        UpdateNormalGroupDoseValidationByPlan_i(planUID);
        plan->get_dosegrid()->set_isdosevalid(true);

        // step 2. save
        ADD_WORKFLOW_KEYPOINT<<"lockMu is true, start to save["<<normGroupUIDList.size()<<"] normgroups to database";
        for (int i = 0; i < normGroupUIDList.size(); ++i)
        {
            RtDbDef::DoseCalculateMode doseCalculateMode;
            mDataRepository->GetNormgroupManager()->GetDoseCalculateModeViaNgUid(normGroupUIDList[i], doseCalculateMode);
            bool isLockMU = doseCalculateMode == RtDbDef::BASE_ON_MU;
            if (isLockMU)
            {
                if (!SaveNormgroup_i(normGroupUIDList[i]))
                {
                    TPS_LOG_DEV_ERROR << "failed to save ngUid:" << normGroupUIDList[i];
                    return TPS_ER_FAILURE;
                }
                ADD_WORKFLOW_KEYPOINT<<"succeed to save ["<<normGroupUIDList[i]<<"] normgroup to database";
            }
            
        }
        ADD_WORKFLOW_KEYPOINT<<"succeed to save all normgroups to database";


        ADD_WORKFLOW_KEYPOINT<<"start to save all beams.";
        if (!SavePlanBeam_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to save plan beam poi, plan uid:" << planUID;
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start to save POIs.";
        if (!SaveBeamPoiDose_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to save poi dose with plan uid" << planUID;
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        // step 3. render
        UpdateDoseInModelWarehouse_i(planUID);
        std::vector<std::string> ngList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
        for (auto itrNg = ngList.cbegin(); itrNg != ngList.cend(); ++itrNg)
        {
            UpdateBeamsInModelWarehouse_i(*itrNg);
        }
        InvalidateView_i();


        ADD_WORKFLOW_KEYPOINT<<"[PLAN DOSE CALC]:finish dose calculation for plan["<<planUID<<"].";
        return TPS_ER_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return TPS_ER_FAILURE;
    }
}

int DoseCalculationImpl::CalculateDoseForNormgroup(const std::string& ngUID)
{
    try
    {
        if (ngUID.empty())
        {
            TPS_LOG_DEV_ERROR << "The plan UID is empty.";
            return TPS_ER_FAILURE;
        }

        RtNormgroup* pNormalGroup = nullptr;
        if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUID, &pNormalGroup))
        {
            TPS_LOG_DEV_ERROR << "The normal group[" << ngUID << "] does not exist.";
            return TPS_ER_FAILURE;
        }

        std::string planUID = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUID);
        if (planUID.empty())
        {
            TPS_LOG_DEV_ERROR << "Failed to find its plan for normal group[" << ngUID << "] in normal group manager";
            return TPS_ER_FAILURE;
        }

        RtPlan* plan = nullptr;
        if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan))
        {
            TPS_LOG_DEV_ERROR << "The plan[" << planUID << "] does not exist.";
            return TPS_ER_FAILURE;
        }

        // step 1. calculation
        ADD_WORKFLOW_KEYPOINT<<"[NG DOSE CACL]: start to calculate dose for normgroup["<<ngUID<<"].";
        int calcResult = mDoseProxy->CalculateDoseForNormgroup(ngUID);
        if (TPS_ER_SUCCESS != calcResult)
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate dose of normgroup: " << ngUID;
            ClearDoseForNormgroup(ngUID, true);
            return calcResult;
        }

        ADD_WORKFLOW_KEYPOINT<<"start to compose dose grid for plan.";
        if (!mDoseProxy->ComposeDoseForPlan(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to compose dose grid for plan[" << planUID << "].";
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start calculate dvh and update voi dose.";
        if (!CalculateDvhAndUpdateVoiDose(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate dvh of plan: " << planUID;
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start update poi dose.";
        if (!UpdatePoiDoseAfterDoseCalculation_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update poi dose.";
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start update max point dose.";
        if (!UpdateMaxPoint_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update max point dose.";
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        // step 2. update dosevalidation 
        UpdateBeamDoseValidationByNormalGroup_i(ngUID);
        pNormalGroup->get_dosegrid()->set_isdosevalid(true);
        plan->get_dosegrid()->set_isdosevalid(true);

        // step 3. save
        //if isLockMU is true, then the calculation will change the NG prescription Goal
        RtDbDef::DoseCalculateMode doseCalculateMode;
        mDataRepository->GetNormgroupManager()->GetDoseCalculateModeViaNgUid(ngUID, doseCalculateMode);
        bool isLockMU = doseCalculateMode == RtDbDef::BASE_ON_MU;
        if (isLockMU)
        {
            ADD_WORKFLOW_KEYPOINT<<"lockMu is true, start to save normgroup["<<ngUID<<"] to database";
            if (!SaveNormgroup_i(ngUID))
            {
                TPS_LOG_DEV_ERROR << "failed to save ngUid:" << ngUID;
                return TPS_ER_FAILURE;
            }
            ADD_WORKFLOW_KEYPOINT<<"DONE!";
        }

        ADD_WORKFLOW_KEYPOINT<<"start to save all beams.";
        if (!SaveNormgroupBeam_i(ngUID))
        {
            TPS_LOG_DEV_ERROR << "failed to save ngUid beam poi, ngUid:" << ngUID << ", plan uid:" << planUID;
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start to save POIs.";
        if (!SaveBeamPoiDose_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to save poi dose with plan uid" << planUID;
            return TPS_ER_FAILURE;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";
        
        // step 4. render
        UpdateDoseInModelWarehouse_i(planUID);
        UpdateBeamsInModelWarehouse_i(ngUID);
        InvalidateView_i();

        return TPS_ER_SUCCESS;
    }
    catch(std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return TPS_ER_FAILURE;
    }
}

bool DoseCalculationImpl::ComposeDoseForPlanWithoutRendering(const std::string& planUID)
{
    try
    {
        if (planUID.empty())
        {
            TPS_LOG_DEV_ERROR<<"The plan UID is empty.";
            return false;
        }
        std::vector<std::string> vNgUids = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
        for (std::vector<std::string>::iterator itNgUid = vNgUids.begin(); itNgUid != vNgUids.end(); ++itNgUid)
        {
            if (!mDoseProxy->ComposeDoseForNormgroup(*itNgUid))
            {
                TPS_LOG_DEV_ERROR << "Failed to compose dose of normal group: " << *itNgUid;
                return false;
            }
        }
        if (!mDoseProxy->ComposeDoseForPlan(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to compose dose of plan: " << planUID;
            return false;
        }
        if (!UpdatePoiDoseAfterDoseCalculation_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update poi dose.";
            return false;
        }
        if (!CalculateDvhAndUpdateVoiDose(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate dvh of plan: " << planUID;
            return false;
        }
        if (!UpdatePoiDoseForImportedPlan_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate dvh of plan: " << planUID;
            return false;
        }
        if (!UpdateMaxPoint_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update max point dose.";
            return TPS_ER_FAILURE;
        }

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

bool DoseCalculationImpl::ReComposeDoseForNormgroup(const std::string& ngUID)
{
    try
    {
        //TEST_PERFORMANCE_INIT;
        //TEST_PERFORMANCE_BEGIN;
        if (ngUID.empty())
        {
            TPS_LOG_DEV_ERROR << "The normal group UID is empty.";
            return false;
        }
        std::string planUID = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUID);
        if (planUID.empty())
        {
            TPS_LOG_DEV_ERROR << "Failed to get plan uid by normal group";
            return false;
        }

        if (!mDoseProxy->ComposeDoseForNormgroup(ngUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to compose dose of normal group: " << ngUID;
            return false;
        }
        if (!mDoseProxy->ComposeDoseForPlan(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to compose dose of plan: " << planUID;
            return false;
        }
        if (!UpdatePoiDoseAfterDoseCalculation_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update poi dose.";
            return false;
        }
        if (!CalculateDvhAndUpdateVoiDose(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate dvh of plan: " << planUID;
            return false;
        }
        if (!UpdateMaxPoint_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update max point dose.";
            return TPS_ER_FAILURE;
        }

        // Render
        UpdateDoseInModelWarehouse_i(planUID);
        std::vector<std::string> ngList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
        for (auto itrNg = ngList.cbegin(); itrNg != ngList.cend(); ++itrNg)
        {
            UpdateBeamsInModelWarehouse_i(*itrNg);
        }
        InvalidateView_i();

        //TEST_PERFORMANCE_END("DoseCalculationImpl::ReComposeDoseForNormgroup");
        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

int DoseCalculationImpl::RecomposePlanDose(const std::string& planUID)
{
    try
    {
        if (planUID.empty())
        {
            TPS_LOG_DEV_ERROR << "Invalid input for RecomposePlanDose";
            return TPS_ER_FAILURE;
        }

        if (!mDoseProxy->ComposeDoseForPlan(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to compose dose of plan: " << planUID;
            return TPS_ER_FAILURE;
        }

        if (!UpdatePoiDoseAfterDoseCalculation_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update poi dose.";
            return TPS_ER_FAILURE;
        }

        if (!CalculateDvhAndUpdateVoiDose(planUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate dvh of plan: " << planUID;
            return TPS_ER_FAILURE;
        }

        // Render
        UpdateDoseInModelWarehouse_i(planUID);

        std::vector<std::string> ngList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
        for (auto itrNg = ngList.cbegin(); itrNg != ngList.cend(); ++itrNg)
        {
            UpdateBeamsInModelWarehouse_i(*itrNg);
        }

        InvalidateView_i();

        return TPS_ER_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

int DoseCalculationImpl::ClearDoseForPlan(const std::string& planUID, bool isSave = false)
{
    try
    {
        RtPlan* plan = nullptr;
        if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan))
        {
            TPS_LOG_DEV_ERROR << "The Plan[" << planUID << "] does not exist.";
            return TPS_ER_FAILURE;
        }

        mDataRepository->GetDoseManager()->ClearDosegridForPlan(planUID);
        ClearBeamParams_i(mDataRepository->GetBeamManager()->GetBeamsByPlan(planUID));
        std::vector<std::string> vNguid = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
        for (std::vector<std::string>::iterator itNgUid = vNguid.begin(); itNgUid != vNguid.end(); ++itNgUid)
        {
            ClearPoiDoseForNormgroup_i(*itNgUid);
        }

        if (!UpdatePoiDoseAfterDoseCalculation_i(planUID))
        {
            TPS_LOG_DEV_ERROR << "failed to update poi dose.";
            return TPS_ER_FAILURE;
        }
        DeleteDvhAndClearVoiDoseForPlan_i(planUID);
        RemoveMaxPoint_i(planUID);

        ClearDoseInModelWarehouse_i(planUID);
        InvalidateView_i();

        if (isSave)
        {
            if (!SavePlanBeam_i(planUID))
            {
                TPS_LOG_DEV_ERROR << "failed to save plan's beam poi, plan uid:" << planUID;
                return TPS_ER_FAILURE;
            }

            if (!SaveBeamPoiDose_i(planUID))
            {
                TPS_LOG_DEV_ERROR << "failed to save poi dose with plan uid" << planUID;
                return TPS_ER_FAILURE;
            }
        }

        return TPS_ER_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return TPS_ER_FAILURE;
    }
}

int DoseCalculationImpl::ClearDoseForNormgroup(const std::string& ngUID, bool isSave = false)
{
    try
    {
        std::string planUID = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUID);
        RtPlan* plan = nullptr;
        if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan))
        {
            TPS_LOG_DEV_ERROR << "The Plan[" << planUID << "] does not exist.";
            return TPS_ER_FAILURE;
        }

        mDataRepository->GetDoseManager()->ClearDosegridForNormgroup(ngUID);
        RemoveMaxPoint_i(planUID);
        ClearBeamParams_i(mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUID));
        ClearPoiDoseForNormgroup_i(ngUID);
        DeleteDvhAndClearVoiDoseForPlan_i(planUID);

        //清空剂量热区冷区
        std::shared_ptr<TpsClearHotColdSpotsOperation> spClearHotColdSpotOp(new TpsClearHotColdSpotsOperation(planUID));
        mRenderProxy->ProcessOperation(spClearHotColdSpotOp);

        InvalidateView_i();
        if (isSave)
        {
            std::vector<RtBeam*> vBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUID);
            for (auto itr = vBeams.begin(); itr != vBeams.end(); ++itr)
            {
                (*itr)->get_dosegrid()->clear_dosegrid_buffer();
            }

            if (!SaveNormgroupBeam_i(ngUID))
            {
                TPS_LOG_DEV_ERROR << "failed to save ngUid beam poi, ngUid:" << ngUID << ", plan uid:" << planUID;
                return TPS_ER_FAILURE;
            }

            if (!SaveBeamPoiDose_i(planUID))
            {
                TPS_LOG_DEV_ERROR << "failed to save poi dose with plan uid" << planUID;
                return TPS_ER_FAILURE;
            }
        }

        return TPS_ER_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return TPS_ER_FAILURE;
    }
}

int DoseCalculationImpl::RescaleDose(const std::string& ngUid, const RescaleDosePara& doseRescalePara)
{
    if (ngUid.empty())
    {
        return TPS_ER_FAILURE;
    }

    // 什么都没修改直接返回
    if (doseRescalePara.doseRescaleReasonType == 0)
    {
        return TPS_ER_SUCCESS;
    }

    RtNormgroup* pNg = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &pNg);
    if (nullptr == pNg)
    {
        TPS_LOG_DEV_ERROR << "normgroup is null.";
        return TPS_ER_FAILURE;
    }

    std::string planUid = pNg->get_planuid();
    RtPlan* pPlan = nullptr ;
    mDataRepository->GetPatientManager()->GetPlan(planUid, &pPlan);
    if (nullptr == pPlan)
    {
        TPS_LOG_DEV_ERROR << "GetPlan error with planUID: " << planUid;
        return TPS_ER_FAILURE;
    }

    TpsImage3DEntity* img3D = nullptr;
    mDataRepository->GetImageDataManager()->GetImage3DData(pPlan->get_imageseriesuid(), &img3D);
    if (img3D == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d entity.";
        return TPS_ER_FAILURE;
    }

    std::vector<RtNormgroup*> ngList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(planUid);
    std::vector<RtBeam*> beamList = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUid);
    std::vector<RtDosegrid*> ngDosegridList;
    std::vector<float> vNormalgroupRescaleFactor;
    std::vector<RtDosegrid*> beamDosegridList;
    std::vector<float> vBeamRescaleFactor;

    int iRet = -1;

    // 处方目标修改
    if (doseRescalePara.doseRescaleReasonType & NORMALGROUP_GOAL_TARGET_CHANGED
        || doseRescalePara.doseRescaleReasonType & BEAM_WEIGHT_CHANGED)
    {
        // 计算新的BeamMU
        if (pNg->get_isnorm2voi())
        {
            ROIEntity* perscriptionVoi = mDataRepository->GetVoiManager()->GetROI(pNg->get_voiuid());
            
            iRet = mDoseProxy->CalculateBeamMU(*img3D, pNg, perscriptionVoi, &beamList);
            if (iRet != TPS_ER_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "CalculateBeamMU for roi error.";
                return iRet;
            }
        }
        else
        {
            std::map<std::string, std::map<std::string, float>> beamdose2PoiMap;
            std::vector<RtBeam*>::iterator itTpsBeam = beamList.begin();
            for (; itTpsBeam != beamList.end(); ++itTpsBeam)
            {
                RtBeam* beam = *itTpsBeam;
                std::map<std::string, float> poiDoseMap;
                RtPoi* poi = mDataRepository->GetPoiManager()->GetPOI(pNg->get_normalpointpoiuid());
                float dose = 0.f;
                if (!poi->get_poidose(beam->get_uid(), dose))
                {
                    TPS_LOG_DEV_ERROR << "RescaleDose : failed to get beam poi dose with beam uid" << beam->get_uid() << " poi uid" <<poi->get_uid();
                    return TPS_ER_FAILURE;
                }
                poiDoseMap[poi->get_uid()] = dose;
                beamdose2PoiMap[beam->get_uid()] = poiDoseMap;
            }

            iRet = mDoseProxy->CalculateBeamMU(pNg, beamdose2PoiMap, &beamList);
            if (iRet != TPS_ER_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "CalculateBeamMU for poi error.";
                return iRet;
            }
        }
    }
    else if (doseRescalePara.doseRescaleReasonType & BEAM_MU_CHANGED)
    {
        // 计算Beam新的weight 和 ng的处方剂量
        if (pNg->get_isnorm2voi())
        {
            ROIEntity* perscriptionVoi = mDataRepository->GetVoiManager()->GetROI(pNg->get_voiuid());
            iRet = mDoseProxy->ComputeBeamWeight(*img3D, pNg, perscriptionVoi, &beamList);
            if (iRet != TPS_ER_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "ComputeBeamWeight for roi error.";
                return iRet;
            }
        }
        else
        {
            std::map<std::string, std::map<std::string, float>> beamdose2PoiMap;
            std::vector<RtBeam*>::iterator itTpsBeam = beamList.begin();
            for (; itTpsBeam != beamList.end(); ++itTpsBeam)
            {
                RtBeam* beam = *itTpsBeam;
                std::map<std::string, float> poiDoseMap;
                RtPoi* poi = mDataRepository->GetPoiManager()->GetPOI(pNg->get_normalpointpoiuid());
                float dose = 0.f;
                if (!poi->get_poidose(beam->get_uid(), dose))
                {
                    TPS_LOG_DEV_ERROR << "RescaleDose : failed to get beam poi dose with beam uid" << beam->get_uid() << " poi uid" <<poi->get_uid();
                    return TPS_ER_FAILURE;
                }
                poiDoseMap[poi->get_uid()] = dose;
                beamdose2PoiMap[beam->get_uid()] = poiDoseMap;
            }

            iRet = mDoseProxy->ComputeBeamWeight(pNg, beamdose2PoiMap, &beamList);
            if (iRet != TPS_ER_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "ComputeBeamWeight for poi error.";
                return iRet;
            }
        }
    }
    else if (doseRescalePara.doseRescaleReasonType & DOSE_PERFRACTION_PERCENTAGE_PRODUCT_CHANGED)// 处方计量修改
    {
        // 如果是是百分比或者是处方单次剂量修改则更新Beam MU和Segment MU
        float mu = 0;
        std::vector<RtBeam*> vecBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUid);
        for (size_t i = 0; i < vecBeams.size(); ++i)
        {
            mu = vecBeams[i]->get_beammu();
            if (fabs(mu) > 1e-6)
            {
                vecBeams[i]->set_beammu(mu * doseRescalePara.muRescaleFactor);
                std::vector<RtBeamsegment*> vecSegments;
                vecSegments = vecBeams[i]->get_beamsegments();
                for (size_t j = 0; j < vecSegments.size(); ++j)
                {
                    mu = vecSegments[j]->get_mu();
                    vecSegments[j]->set_mu(mu * doseRescalePara.muRescaleFactor);
                }
            }
        }
    }

    // 重新组装Ng Dosegrid
    std::vector<RtBeam*>::iterator itTpsBeam = beamList.begin();
    for (; itTpsBeam != beamList.end(); ++itTpsBeam)
    {
        vBeamRescaleFactor.push_back((*itTpsBeam)->get_beammu() * pNg->get_fraction());
        beamDosegridList.push_back((*itTpsBeam)->get_dosegrid());
    }

    iRet = mDoseProxy->RescaleDosegrid(beamDosegridList, vBeamRescaleFactor, pNg->get_dosegrid());

    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Compose beamdosegrid fail.";
        return iRet;
    }

    // 重新组装Plan Dosegrid
    std::vector<RtNormgroup*>::iterator itTpsNormalgroup = ngList.begin();
    for (; itTpsNormalgroup != ngList.end(); ++itTpsNormalgroup)
    {
        vNormalgroupRescaleFactor.push_back(1.0f);
        ngDosegridList.push_back((*itTpsNormalgroup)->get_dosegrid());
    }

    iRet = mDoseProxy->RescaleDosegrid(ngDosegridList, vNormalgroupRescaleFactor, pPlan->get_dosegrid());

    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Compose normalgroup dosegrid fail.";
        return iRet;
    }

    if (!CalculateDvhAndUpdateVoiDose(planUid))
    {
        TPS_LOG_DEV_ERROR << "Failed to calculate dvh of plan: " << planUid;
        return TPS_ER_FAILURE;
    }

    if (!UpdatePoiDoseAfterDoseCalculation_i(planUid))
    {
        TPS_LOG_DEV_ERROR << "failed to update poi dose.";
        return TPS_ER_FAILURE;
    }

    if (doseRescalePara.doseRescaleReasonType & NORMALGROUP_GOAL_TARGET_CHANGED
        || doseRescalePara.doseRescaleReasonType & BEAM_WEIGHT_CHANGED
        || doseRescalePara.doseRescaleReasonType & DOSE_PERFRACTION_PERCENTAGE_PRODUCT_CHANGED
        || doseRescalePara.doseRescaleReasonType & BEAM_MU_CHANGED)
    {
        // 暂时不考虑PlanQA的场景，此种场景不需要存储dosegrid
        if (!SaveNormgroupBeam_i(ngUid, false))
        {
            TPS_LOG_DEV_ERROR << "failed to save ngUid beam poi, ngUid:" << ngUid << ", plan uid:" << planUid;
            return TPS_ER_FAILURE;
        }
    }

    if (doseRescalePara.doseRescaleReasonType & BEAM_MU_CHANGED)
    {
        if (!SaveNormgroup_i(ngUid))
        {
            TPS_LOG_DEV_ERROR << "failed to save SaveNormgroup_i, ngUid:" << ngUid;
            return TPS_ER_FAILURE;
        }
    }

    if (!SaveBeamPoiDose_i(planUid))
    {
        TPS_LOG_DEV_ERROR << "failed to save poi dose with plan uid" << planUid;
        return TPS_ER_FAILURE;
    }

    UpdateDoseInModelWarehouse_i(planUid);

    UpdateBeamsInModelWarehouse_i(ngUid);

    InvalidateView_i();

    return TPS_ER_SUCCESS;
}



#pragma region Update tmsnorm_voi/tmsplan_voi
// After ng approve, update tmsnorm_voi data for tms
int DoseCalculationImpl::UpadteNormalVoi(const std::string& ngUid)
{
    if (ngUid.empty())
    {
        TPS_LOG_DEV_ERROR << "The normalgroup UID is empty.";
        return TPS_ER_FAILURE;
    }

    RtNormgroup* pNormalGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &pNormalGroup))
    {
        TPS_LOG_DEV_ERROR << "The normal group[" << ngUid << "] does not exist.";
        return TPS_ER_FAILURE;
    }

    std::string planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUid);
    if (planUid.empty())
    {
        TPS_LOG_DEV_ERROR << "Failed to find its plan for normal group[" << ngUid << "] in normal group manager";
        return TPS_ER_FAILURE;
    }

    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan))
    {
        TPS_LOG_DEV_ERROR << "The plan[" << planUid << "] does not exist.";
        return TPS_ER_FAILURE;
    }

    std::string seriesUid = plan->get_imageseriesuid();
    auto vecVoiUids = mDataRepository->GetVoiManager()->GetROIUidListBySeries(seriesUid);
    if (vecVoiUids.size() < 1)
    {
        TPS_LOG_DEV_ERROR << "voi list is empty of plan[" << planUid << "].";
        return TPS_ER_FAILURE;
    }

    std::vector<RtDvh*> vDvhs;
    for (int i = 0; i < vecVoiUids.size(); ++i)
    {
        RtDvh* dvh = new RtDvh();
        dvh->set_uid(vecVoiUids[i]);
        vDvhs.push_back(dvh);
    }

    if (!mDoseProxy->CalculateNormGroupDVH(ngUid, vecVoiUids, vDvhs))
    {
        TPS_LOG_DEV_ERROR << "Failed to CalculateNormGroupDVH of normalgroup[" << ngUid << "].";
        for (std::vector<RtDvh*>::iterator dvhIt = vDvhs.begin(); dvhIt != vDvhs.end(); ++dvhIt)
        {
            DEL_PTR(*dvhIt);
        }
        return TPS_ER_FAILURE;
    }

    if (vDvhs.size() > 0)
    {
        RtNorm_voi *normalVoi = new RtNorm_voi();
        for (std::vector<RtDvh*>::iterator it = vDvhs.begin(); it != vDvhs.end(); ++it)
        {
            normalVoi->set_normgroupuid(ngUid);
            normalVoi->set_voiuid((*it)->get_uid());
            normalVoi->set_mindosecontribution((*it)->get_mindose());
            normalVoi->set_maxdosecontribution((*it)->get_maxdose());
            normalVoi->set_meandosecontribution((*it)->get_meandose());

            if (TPS_ER_SUCCESS != mSaveObjects->SaveNormVoi(*normalVoi))
            {
                TPS_LOG_DEV_ERROR << "Failed to save NormVoi normalgroup" << ngUid;
                DEL_PTR(normalVoi)
                for (std::vector<RtDvh*>::iterator dvhIt = vDvhs.begin(); dvhIt != vDvhs.end(); ++dvhIt)
                {
                    DEL_PTR(*dvhIt);
                }
                return TPS_ER_DB_ERROR;
            }
        }
        DEL_PTR(normalVoi);
    }

    for (std::vector<RtDvh*>::iterator dvhIt = vDvhs.begin(); dvhIt != vDvhs.end(); ++dvhIt)
    {
        DEL_PTR(*dvhIt);
    }

    return TPS_ER_SUCCESS;
}

int DoseCalculationImpl::DeleteNormalVoi(const std::string& ngUid)
{
    return mSaveObjects->DeleteNormVoiViaNormalUid(ngUid);
}

// After plan approve, update tmsplan_voi data for tms
int DoseCalculationImpl::UpdatePlanVoi(const std::string& planUid)
{
    if (planUid.empty())
    {
        TPS_LOG_DEV_ERROR << "The plan Uid is empty" << planUid ;
        return TPS_ER_FAILURE;
    }

    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan))
    {
        TPS_LOG_DEV_ERROR << "The plan[" << planUid << "] does not exist.";
        return TPS_ER_FAILURE;
    }

    auto vecVoiUids = mDataRepository->GetVoiManager()->GetROIUidListBySeries(plan->get_imageseriesuid());

    if (!mDoseProxy->CalculateDVH(planUid))
    {
        TPS_LOG_DEV_ERROR << "Failed to calculate DVH for target[" << planUid << "].";
        return TPS_ER_FAILURE;
    }

    std::vector<RtDvh*> dvhList = mDataRepository->GetDVHManager()->GetPlanDvhs(planUid);
    if (dvhList.size() > 0)
    {
        RtPlan_voi *planVoi = new RtPlan_voi();
        for (std::vector<RtDvh*>::iterator it = dvhList.begin(); it != dvhList.end(); ++it)
        {
            planVoi->set_planuid(planUid);
            planVoi->set_voiuid((*it)->get_uid());
            planVoi->set_mindosecontribution((*it)->get_mindose());
            planVoi->set_maxdosecontribution((*it)->get_maxdose());
            planVoi->set_meandosecontribution((*it)->get_meandose());
            planVoi->set_ingrid((*it)->get_ingrid());

            if (TPS_ER_SUCCESS != mSaveObjects->SavePlanVoi(*planVoi))
            {
                TPS_LOG_DEV_ERROR << "Failed to save PlanVoi plan" << planUid;
                DEL_PTR(planVoi)
                return TPS_ER_DB_ERROR;
            }
        }
        DEL_PTR(planVoi);
    }

    return TPS_ER_SUCCESS;
}

int DoseCalculationImpl::DeletePlanVoi(const std::string& planUid)
{
    return mSaveObjects->DeletePlanVoiViaPlanUid(planUid);
}
#pragma endregion

#pragma region Poi
void DoseCalculationImpl::RemoveMaxPoint_i(const std::string& planUid)
{
    RtPoi* maxPointInMgr = mDataRepository->GetPoiManager()->GetMaxPoint(planUid);
    if (maxPointInMgr != nullptr)
    {
        mDataRepository->GetPoiManager()->DeleteMaxPoint(planUid);
        mRenderProxy->DeleteMaxPoint(planUid, planUid);
    }
}

void DoseCalculationImpl::ClearPoiDoseForNormgroup_i(const std::string& ngUid)
{
    std::string planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUid);
    std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);

    std::vector<RtPoi*> vecPoi = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);

    if (vecPoi.size() > 0)
    {
        std::vector<RtBeam*> vBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUid);
        for (std::vector<RtBeam*>::iterator itBeam = vBeams.begin(); itBeam != vBeams.end(); ++itBeam)
        {
            RtBeam* beam = *itBeam;
            if (beam->get_isjoinopt() && beam->get_isjoincalc())
            {
                for (std::vector<RtPoi*>::iterator itPoi = vecPoi.begin(); itPoi != vecPoi.end(); ++itPoi)
                {
                    RtPoi* poi = *itPoi;
                    poi->set_plandoseinfo(planUid, 0.0f, RtDbDef::NotCalculated);
                    poi->set_poidose(beam->get_uid(), 0.0f);
                }
            }
        }
    }
}

bool DoseCalculationImpl::UpdatePoiDoseAfterDoseCalculation_i(const std::string& planUid)
{
    try
    {
        std::vector<CertifiedPoiDose> vPoiDose;
        // step 1. find all beams which have contribution to the plan
        std::vector<std::string> vNguids = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUid);
        for (std::vector<std::string>::iterator itNgUid = vNguids.begin(); itNgUid != vNguids.end(); ++itNgUid)
        {
            RtNormgroup* normgroup = nullptr;
            mDataRepository->GetNormgroupManager()->GetNormGroup(*itNgUid, &normgroup);
            if (normgroup != nullptr && normgroup->get_isjoined())
            {
                std::vector<RtBeam*> vBeams =mDataRepository->GetBeamManager()->GetBeamsByNormGroup(normgroup->get_uid());
                for (std::vector<RtBeam*>::iterator itBeam = vBeams.begin(); itBeam != vBeams.end(); ++itBeam)
                {
                    RtBeam* beam = *itBeam;
                    if (beam->get_isjoincalc())
                    {
                        CertifiedPoiDose poiDose;
                        poiDose.m_beamUid = beam->get_uid();
                        poiDose.m_beamMU = beam->get_beammu();
                        poiDose.m_normgroupFraction = normgroup->get_fraction();
                        vPoiDose.push_back(poiDose);
                    }
                }
            }
        }

        // step 2. update dose for all coordinate fixed pois in given plan
        std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
        std::vector<RtPoi*> vPois = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
        for (std::vector<RtPoi*>::iterator itPoi = vPois.begin(); itPoi != vPois.end(); ++itPoi)
        {
            RtPoi* poi = *itPoi;
            float poiDose = 0.0f;
            bool bExistBeamPoi = false;
            for (std::vector<CertifiedPoiDose>::iterator itPoiDose = vPoiDose.begin(); itPoiDose != vPoiDose.end(); ++itPoiDose)
            {
                CertifiedPoiDose certPoiDose = *itPoiDose;
                float beamDosePerMu = 0.f;
                if (!poi->get_poidose(certPoiDose.m_beamUid, beamDosePerMu)) continue;
                bExistBeamPoi = true;
                float beamMu = certPoiDose.m_beamMU;
                int fraction = certPoiDose.m_normgroupFraction;
                float beamPoiDose = beamDosePerMu * beamMu * fraction;
                poiDose += beamPoiDose;
            }
            if (bExistBeamPoi)
            {
                poi->set_plandoseinfo(planUid, poiDose);
            }
        }
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
    return true;
}

bool DoseCalculationImpl::UpdateMaxPoint_i(const std::string& planUid)
{
    try
    {
        // find max dose point, max point do not save in database
        RtPlan* plan = nullptr;
        mDataRepository->GetPatientManager()->GetPlan(planUid, &plan);
        if (plan != nullptr)
        {
            float maxDoseValueInDoseGrid = 0.0f;
            float xMaxPointInPat = 0.0f, yMaxPointInPat = 0.0f, zMaxPointInPat = 0.0f;
            mDataRepository->GetDoseManager()->GetMaxDosePointOfPlan(
                planUid, xMaxPointInPat, yMaxPointInPat, zMaxPointInPat, maxDoseValueInDoseGrid);

            float maxDoseValueInPoiList = 0.0f;
            RtPoi* maxDosePointInPoiList = nullptr;
            std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
            std::vector<RtPoi*> vPois = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
            for (std::vector<RtPoi*>::iterator itPoi = vPois.begin(); itPoi != vPois.end(); ++itPoi)
            {
                RtPoi* poi = *itPoi;
                if (maxDoseValueInPoiList < poi->get_plandose(planUid))
                {
                    maxDoseValueInPoiList = poi->get_plandose(planUid);
                    maxDosePointInPoiList = poi;
                }
            }

            if (maxDoseValueInDoseGrid < 0.001f && maxDoseValueInPoiList < 0.001f)
            {
                RemoveMaxPoint_i(planUid);
                TPS_LOG_DEV_INFO << "there is no dose for plan uid: " << planUid;
                return true;
            }
            RtPoi* maxPoint = new RtPoi(false);
            if (maxDoseValueInDoseGrid > maxDoseValueInPoiList)
            {
                maxPoint->set_coordinate3dx(xMaxPointInPat);
                maxPoint->set_coordinate3dy(yMaxPointInPat);
                maxPoint->set_coordinate3dz(zMaxPointInPat);
                maxPoint->set_plandoseinfo(planUid, maxDoseValueInDoseGrid);
            }
            else
            {
                maxPoint->set_coordinate3dx(maxDosePointInPoiList->get_coordinate3dx());
                maxPoint->set_coordinate3dy(maxDosePointInPoiList->get_coordinate3dy());
                maxPoint->set_coordinate3dz(maxDosePointInPoiList->get_coordinate3dz());
                maxPoint->set_plandoseinfo(planUid, maxDoseValueInPoiList);
            }

            //SSFS 43189 SSFS_TPS_APP_Planning_POI_MaxPoint
            maxPoint->set_uid(planUid);
            maxPoint->set_type(RtDbDef::Max);
            maxPoint->set_name("Max");
            maxPoint->set_seriesuid(plan->get_imageseriesuid());
            maxPoint->set_red(1.0f);
            maxPoint->set_green(0.647058845f);//orange color
            maxPoint->set_blue(0.0f);
            maxPoint->set_alpha(1.0f);
            maxPoint->set_isvisible(true);
            maxPoint->set_locktype(2);

            mDataRepository->GetPoiManager()->AddMaxPoint(maxPoint);

            // render max dose point
            UpdatedMaxPointInModelWarehouse_i(planUid, maxPoint->get_uid());
        }
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
    return true;
}

bool DoseCalculationImpl::UpdatePoiDoseForImportedPlan_i(const std::string& planUid)
{
    std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
    std::vector<RtPoi*> vPois = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
    for (std::vector<RtPoi*>::iterator itPoi = vPois.begin(); itPoi != vPois.end(); ++itPoi)
    {
        RtPoi* poi = *itPoi;
        float doseValue = 0.0f;
        if (poi->get_plancalcmethod(planUid) == RtDbDef::NotCalculated)
        {
            mDataRepository->GetDoseManager()->GetPointDoseValueofPlan(planUid, (*itPoi)->get_coordinate3dx(), 
                (*itPoi)->get_coordinate3dy(), (*itPoi)->get_coordinate3dz(), doseValue);
            (*itPoi)->set_plandoseinfo(planUid, doseValue, RtDbDef::Interpolated);
            poi->set_plandoseinfo(planUid, doseValue, RtDbDef::Interpolated);
        }
    }
    return true;
}

bool DoseCalculationImpl::SaveBeamPoiDose_i(const std::string& planUid)
{
    std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
    std::vector<RtPoi*> vPois = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
    for (std::vector<RtPoi*>::iterator itPoi = vPois.begin(); itPoi != vPois.end(); ++itPoi)
    {
        //和强强茂亮讨论，计算完之后应该不需要更新Poi，只需要更新PoiDose lx 2016/12/19
        if (TPS_ER_SUCCESS != mSaveObjects->UpdateBeamPoiDose(*itPoi))
        {
            TPS_LOG_DEV_ERROR << "Failed to update poi dose to database with poi uid" << (*itPoi)->get_uid() <<"].";
            return false;
        }
    }

    return true;
}

#pragma endregion

#pragma region DVH/VOI
bool DoseCalculationImpl::CalculateDvhAndUpdateVoiDose(const std::string& planUid)
{
    try
    {
        ADD_WORKFLOW_KEYPOINT<<"[PLAN DVH Calc]:start calculate DVH and update VOI dose for plan["<<planUid<<"].";
        if (!mDoseProxy->CalculateDVH(planUid))
        {
            TPS_LOG_DEV_ERROR << "Failed to calculate DVH for target["<<planUid<<"].";
            return false;
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";

        ADD_WORKFLOW_KEYPOINT<<"start to update dose of voi.";
        std::vector<RtDvh*> dvhList = mDataRepository->GetDVHManager()->GetPlanDvhs(planUid);
        if (dvhList.size() > 0)
        {
            ROIEntity *voi = nullptr;
            for (std::vector<RtDvh*>::iterator it = dvhList.begin(); it != dvhList.end(); ++it)
            {
                std::string voiUid = (*it)->get_uid();
                voi = mDataRepository->GetVoiManager()->GetROI(voiUid);
                if (voi == nullptr) continue;
                voi->SetMinDose((*it)->get_mindose());
                voi->SetMaxDose((*it)->get_maxdose());
                voi->SetMeanDose((*it)->get_meandose());
                voi->GetInnerRoi()->set_standarddeviation((*it)->get_standarddeviation());
            }
        }
        else
        {
            auto imgUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
            auto voiUidList = mDataRepository->GetVoiManager()->GetROIUidListBySeries(imgUid);

            ROIEntity *voi = nullptr;
            for (auto it = voiUidList.begin(); it != voiUidList.end(); ++it)
            {
                voi = mDataRepository->GetVoiManager()->GetROI(*it);
                if (voi == nullptr)
                {
                    TPS_LOG_DEV_ERROR << "Failed to get VOI[" << *it << "] from voi manager";
                    return false;
                }
                voi->SetMinDose(0.f);
                voi->SetMaxDose(0.f);
                voi->SetMeanDose(0.f);
                voi->GetInnerRoi()->set_standarddeviation(0.f);
            }
        }
        ADD_WORKFLOW_KEYPOINT<<"DONE!";
        ADD_WORKFLOW_KEYPOINT<<"[PLAN DVH Calc]: DONE!";

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

void DoseCalculationImpl::DeleteDvhAndClearVoiDoseForPlan_i(const std::string& planUID)
{
    mDataRepository->GetDVHManager()->DeletePlanDvh(planUID);
    
    auto imgUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUID);
    auto voiUids = mDataRepository->GetVoiManager()->GetROIUidListBySeries(imgUid);
    ROIEntity *voi = nullptr;
    for (auto it = voiUids.begin(); it != voiUids.end(); ++it)
    {
        voi = mDataRepository->GetVoiManager()->GetROI(*it);
        if (voi != nullptr)
        {
            voi->SetMinDose(0.0f);
            voi->SetMaxDose(0.0f);
            voi->SetMeanDose(0.0f);
            voi->GetInnerRoi()->set_tcpntcp(0.0f);
            voi->GetInnerRoi()->set_standarddeviation(0.0f);
        }
    }
}
#pragma endregion

#pragma region Render
void DoseCalculationImpl::UpdatedMaxPointInModelWarehouse_i(const std::string &planUID, const std::string& poiUID)
{
    std::shared_ptr<TpsUpdateMaxPointOperation> updatePoiOp(new TpsUpdateMaxPointOperation(planUID, poiUID));
    mRenderProxy->ProcessOperation(updatePoiOp);
}

void DoseCalculationImpl::ClearDoseInModelWarehouse_i(const std::string& planUID) 
{
    mRenderProxy->RefreshDose(planUID);
    mRenderProxy->RefreshDoseLine(planUID);

    // 清空剂量热区冷区
    std::shared_ptr<TpsClearHotColdSpotsOperation> spClearHotColdSpotOp(new TpsClearHotColdSpotsOperation(planUID));
    mRenderProxy->ProcessOperation(spClearHotColdSpotOp);
}

void DoseCalculationImpl::UpdateDoseInModelWarehouse_i(const std::string& planUID) 
{
    mRenderProxy->RefreshDose(planUID);
    mRenderProxy->RefreshDoseLine(planUID);
}

void DoseCalculationImpl::UpdateBeamsInModelWarehouse_i(const std::string& sNormgroupUid)
{
    std::vector<std::string> beamList = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(sNormgroupUid);
    for (auto itr = beamList.cbegin(); itr != beamList.cend(); ++itr)
    {
        //For update segment weight & MU's image text in BEV
        std::shared_ptr<TpsUpdateBeamOperation> updateBeamOp(
            new TpsUpdateBeamOperation(sNormgroupUid, *itr));
        mRenderProxy->ProcessOperation(updateBeamOp);
    }
}

void DoseCalculationImpl::InvalidateView_i()
{
    std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
    mRenderProxy->ProcessOperation(fire);
}

#pragma endregion

#pragma region DoseValid

bool DoseCalculationImpl::CheckDoseValidForNormalgroup(const std::string& ngUID)
{
    RtNormgroup *pNormalgroup = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(ngUID, &pNormalgroup);
    if (pNormalgroup == nullptr)
    {
        return false;
    }
    if (pNormalgroup->get_dosegrid() == nullptr)
    {
        return false;
    }
    if (!pNormalgroup->get_dosegrid()->get_isdosevalid())
    {
        return false;
    }

    return true;
}

bool DoseCalculationImpl::CheckDoseValidForPlan(const std::string& planUID)
{
    std::vector<std::string> vNguid = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
    if (vNguid.empty())
    {
        return false;
    }
    for (std::vector<std::string>::iterator itNgUid = vNguid.begin(); itNgUid != vNguid.end(); ++itNgUid)
    {
        if (CheckDoseValidForNormalgroup(*itNgUid))
        {
            return true;
        }
    }

    return false;
}

void DoseCalculationImpl::UpdateBeamDoseValidationByNormalGroup_i(const std::string& ngUid)
{
    std::vector<RtBeam*> vBeamList = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUid);
    std::vector<RtBeam*>::iterator itBeam = vBeamList.begin();
    for (; itBeam != vBeamList.end(); ++itBeam)
    {
        RtBeam* pBeam = *itBeam;
        RtDosegrid* pDosegridBeam = pBeam ->get_dosegrid();
        if (pDosegridBeam != nullptr)
        {
            pDosegridBeam->set_isdosevalid(true);
        }
    }
}

void DoseCalculationImpl::UpdateBeamDoseValidationByPlan_i(const std::string& planUid)
{
    std::vector<RtBeam*> vBeamList = mDataRepository->GetBeamManager()->GetBeamsByPlan(planUid);
    std::vector<RtBeam*>::iterator itBeam = vBeamList.begin();
    for (; itBeam != vBeamList.end(); ++itBeam)
    {
        RtBeam* pBeam = *itBeam;
        RtDosegrid* pDosegridBeam = pBeam ->get_dosegrid();
        if (pDosegridBeam != nullptr)
        {
            pDosegridBeam->set_isdosevalid(true);
        }
    }
}

void DoseCalculationImpl::UpdateNormalGroupDoseValidationByPlan_i(const std::string& planUid)
{
    std::vector<RtNormgroup*> vNormalGroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(planUid);
    std::vector<RtNormgroup*>::iterator itNg = vNormalGroupList.begin();
    for (; itNg != vNormalGroupList.end(); ++itNg)
    {
        RtNormgroup* pNormalGroup = *itNg;
        RtDosegrid* pDosegridBeam = pNormalGroup ->get_dosegrid();
        if (pDosegridBeam != nullptr)
        {
            pDosegridBeam->set_isdosevalid(true);
        }
    }
}
#pragma endregion

bool DoseCalculationImpl::SavePlanBeam_i(const std::string& planUID, bool isSaveDosegrid /*= true*/)
{
    std::vector<std::string> ngUidList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);

    for (auto itngUid = ngUidList.begin(); itngUid != ngUidList.end(); ++itngUid)
    {
        if (!this->SaveNormgroupBeam_i(*itngUid, isSaveDosegrid))
        {
            TPS_LOG_DEV_ERROR << "Failed to SaveNormgroupBeam_i to database nguid" << *itngUid << "].";
            return false;
        }
    }

    return true;
}

bool DoseCalculationImpl::SaveNormgroupBeam_i(const std::string& ngUid, bool isSaveDosegrid /*= true*/)
{
    std::vector<RtBeam*> ngBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(ngUid);
    for (auto itBeam = ngBeams.begin(); itBeam != ngBeams.end(); ++itBeam)
    {
        // update beammu
        if (TPS_ER_SUCCESS != mSaveObjects->UpdateBeamAttributes(*(*itBeam)))
        {
            TPS_LOG_DEV_ERROR << "Failed to save beam object to database. uid" << (*itBeam)->get_uid() << "].";
            return false;
        }

        // QA Beam Segment can not be modified
        RtDbDef::DoseCalculateMode doseCalculateMode;
        mDataRepository->GetNormgroupManager()->GetDoseCalculateModeViaNgUid(ngUid, doseCalculateMode);
        bool isLockMU = doseCalculateMode == RtDbDef::BASE_ON_MU;
        if (!isLockMU)
        {
            // update segment mu
            if (TPS_ER_SUCCESS != mSaveObjects->UpdateBeamSegments((*itBeam)->get_beamsegments()))
            {
                TPS_LOG_DEV_ERROR << "Failed to save beam segment to database.";
                return false;
            }
        }
    }
    // save ng dosegrid
    if (isSaveDosegrid)
    {
        if (TPS_ER_SUCCESS != mSaveObjects->SaveDosegridsForNormgroup(ngUid))
        {
            TPS_LOG_DEV_ERROR << "Failed to save dose grid for norm group. uid" << ngUid <<"].";
            return false;
        }
    }

    return true;
}

// planqa dose calculate save ng prescription
bool DoseCalculationImpl::SaveNormgroup_i(const std::string& ngUid)
{
    RtNormgroup* normgroup = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &normgroup);
    if (TPS_ER_SUCCESS != mSaveObjects->SaveNormgroup(*normgroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to save normgroup object to database. uid" << ngUid << "].";
        return false;
    }
    return true;
}

// if isLockMU is true, then the beam MU should not be deleted.
void DoseCalculationImpl::ClearBeamParams_i(const std::vector<RtBeam*> beamList)
{
    for (auto itBeam = beamList.begin(); itBeam != beamList.end(); itBeam++)
    {
        RtDbDef::DoseCalculateMode doseCalculateMode;
        mDataRepository->GetNormgroupManager()->GetDoseCalculateModeViaNgUid((*itBeam)->get_normgroupuid(), doseCalculateMode);
        bool isLockMU = doseCalculateMode == RtDbDef::BASE_ON_MU;

        if (!isLockMU)
        {
            // if it is not locking mu then the beam mu and beam segment mu should all be set 0.
            (*itBeam)->set_beammu(.0f);
            auto segments = (*itBeam)->get_beamsegments();
            for (auto itSegemnt = segments.begin();
                itSegemnt != segments.end(); itSegemnt++)
            {
                (*itSegemnt)->set_mu(.0f);
            }
        }
        else
        {
            (*itBeam)->set_beamweight(0);
            (*itBeam)->disable_field(BEAM_BEAMWEIGHT); //when lockMU , Beam Weight should be invalid
        }
        (*itBeam)->set_normpointbeamdose(.0f);
    }
}

TPS_END_NAMESPACE
