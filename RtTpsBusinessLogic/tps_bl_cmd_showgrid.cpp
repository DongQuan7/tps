//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  xiaowei.li mailto:xiaowei.li@united-imaging.com
///
///  \file    tps_bl_cmd_showgrid.cpp
///  \brief   show dose grid command
///
///  \version 1.0
///  \date    Dec. 14, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_showgrid.h"

//Rt
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsShowGridCMD::TpsShowGridCMD(CertifiedGridSize *pocoGridSize) 
    : mPocoGridSize(pocoGridSize) {
}


TpsShowGridCMD::~TpsShowGridCMD(void) {
}

int TpsShowGridCMD::Execute() {
    std::string planUid = mPocoGridSize->mPlanUid;
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan)
        || plan == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<planUid;
            return TPS_ER_FAILURE;
    }

    RtDosegrid* dosegrid = plan->get_dosegrid();
    if (dosegrid == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<planUid;
        return TPS_ER_FAILURE;
    }

    mRenderProxy->ShowDoseGridBoundary(mPocoGridSize->mPlanUid, mPocoGridSize->mIsShowBoundary);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE