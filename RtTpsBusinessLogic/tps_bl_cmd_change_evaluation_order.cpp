//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_change_evaluation_order.cpp
///  \brief   
///
///  \version 1.0
///  \date  2015/10/26  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_change_evaluation_order.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE
ChangeEvaluationOrderCmd::ChangeEvaluationOrderCmd( int order, CertifiedPlan &plan )
: mCertPlan(plan),mEvaluationOrder(order){

}

ChangeEvaluationOrderCmd::ChangeEvaluationOrderCmd(int order)
    : mEvaluationOrder(order){

}

ChangeEvaluationOrderCmd::~ChangeEvaluationOrderCmd()
{

}

int ChangeEvaluationOrderCmd::Execute()
{
    TpsPatientManager* patMgr = mDataRepository->GetPatientManager();
    if (EV1 == mEvaluationOrder || EV2 == mEvaluationOrder)
    {
        std::string planUID = mCertPlan.mPlanUID;
        std::string imageUID("");

        if (!planUID.empty()) {
            //Get the image related to current plan
            RtPlan* plan = nullptr;
            if (!patMgr->GetPlan(planUID, &plan)
                || plan == nullptr) {
                TPS_LOG_DEV_ERROR << "Failed to get plan: " << planUID;
                return -1;
            }

            imageUID = plan->get_imageseriesuid();
        }

        if (planUID.empty() || imageUID.empty()) {
            //do nothing
            TPS_LOG_DEV_ERROR << "plan or image is empty!";
            return -1;
        }

        // set this plan as EV1
        if (EV1 == mEvaluationOrder) {
            mRenderProxy->SetDataSource(SOURCE_GROUP_EV_1, IMAGE, imageUID);
            mRenderProxy->SetDataSource(SOURCE_GROUP_EV_1, PLAN, planUID);
            //mPrimaryPlanUid = planUID;
            patMgr->SetPrimaryPlanUID(planUID);
        }
        // set this plan as EV2
        else if (EV2 == mEvaluationOrder) {
            mRenderProxy->SetDataSource(SOURCE_GROUP_EV_2, IMAGE, imageUID);
            mRenderProxy->SetDataSource(SOURCE_GROUP_EV_2, PLAN, planUID);
            //mSecondaryPlanUid = planUID;
            patMgr->SetSecondaryPlanUID(planUID);
        }
    }
    // remove this plan from MPR window
    else if (NOT_EV1_PLAN == mEvaluationOrder || NOT_EV2_PLAN == mEvaluationOrder) {

        if (NOT_EV2_PLAN == mEvaluationOrder) {
            mRenderProxy->RemoveDataSource(SOURCE_GROUP_EV_2, IMAGE);
            mRenderProxy->RemoveDataSource(SOURCE_GROUP_EV_2, PLAN);
            // remove secondary plan uid in patient manager
            patMgr->SetSecondaryPlanUID("");
        }
        else if (NOT_EV1_PLAN == mEvaluationOrder) {
            mRenderProxy->RemoveDataSource(SOURCE_GROUP_EV_1, IMAGE);
            mRenderProxy->RemoveDataSource(SOURCE_GROUP_EV_1, PLAN);
            // remove primary plan uid in patient manager
            patMgr->SetPrimaryPlanUID("");
        }
    }
    else if (NOT_TempResult == mEvaluationOrder)
    {
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_EV_RESULT, IMAGE);
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_EV_RESULT, PLAN);
        // remove primary plan uid in patient manager
        patMgr->SetCurrentResultPlanUID("");
    }

    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
