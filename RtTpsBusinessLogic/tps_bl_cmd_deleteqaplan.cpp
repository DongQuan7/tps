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


#include "RtTpsBusinessLogic/tps_bl_cmd_deleteqaplan.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_template.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsFramework/tps_fw_doselinedatastructure.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "tps_logger.h"


TPS_BEGIN_NAMESPACE
TpsPlanQADeleteCmd::TpsPlanQADeleteCmd(std::string planUid, std::string seriesUid, std::string courseUid) :
mPlanUid(planUid), mSeriesUid(seriesUid), mCourseUid(courseUid) {
}

TpsPlanQADeleteCmd::~TpsPlanQADeleteCmd() {

}

int TpsPlanQADeleteCmd::Execute() {
    if(!DeletePlan_i(mPlanUid)) {
        TPS_LOG_DEV_ERROR<<"delete try failed!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

bool TpsPlanQADeleteCmd::DeletePlan_i(const std::string &planUid) {

    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
    TpsNormGroupManager *normGroupManager = mDataRepository->GetNormgroupManager();
    TpsBeamManager *beamManager = mDataRepository->GetBeamManager();

    std::vector<std::string> normGroupUidList = normGroupManager->GetNormGroupUIDListByPlan(planUid);

    RtPlan* planTemp = nullptr;
    if (!patientManager->GetPlan(planUid,&planTemp)) {
        TPS_LOG_DEV_ERROR<<"Failed to get try from plan manager!"
            <<planUid;
        return false;
    }

    int bRet = mSaveObjects->EraseObject(planUid, RtDbDef::TYPE_PLAN);
    if (bRet != TPS_ER_SUCCESS) {
        TPS_LOG_DEV_ERROR<<"Failed to delete try from TMS";
        return false;
    }

    if (!patientManager->RemovePlan(planUid)) {
        TPS_LOG_DEV_ERROR<<"remove try from patient manager failed."
            <<planUid;
        return false;
    }
    for (auto itr = normGroupUidList.begin(); itr != normGroupUidList.end(); ++itr) {
        normGroupManager->RemoveNormGroup(*itr);
        normGroupManager->RemoveNormGroupDoseCalculateMode(*itr);
        std::vector<std::string> beamUidList = beamManager->GetBeamUIDListByNormgroupUID(*itr);
        for (auto itrb = beamUidList.begin(); itrb != beamUidList.end(); ++itrb) {
            beamManager->DeleteBeam(*itrb);
        }
    }

    std::string currPlanDS("");
    mRenderProxy->GetDataSource(SOURCE_GROUP_PLANQA, PLAN, currPlanDS);
    if (currPlanDS == planUid){
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_PLANQA, PLAN);
        //mRenderProxy->RemoveDataSource(SOURCE_GROUP_PLANQA, IMAGE);
    }

    //todo: clear datasource and go

    mRenderProxy->Render();

    return true;
}

TPS_END_NAMESPACE
