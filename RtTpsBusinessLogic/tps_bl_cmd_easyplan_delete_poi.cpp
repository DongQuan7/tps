//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \brief   delete poi
///
///  \version 1.0
///  \date  2015/10/30  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "algorithm"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_delete_image.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_delete_poi.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"



TPS_BEGIN_NAMESPACE

TpsDeletePoiEasyPlanCmd::TpsDeletePoiEasyPlanCmd(const std::string& sEzPlanUID, const std::string& sEzPoiUID, const RtDbDef::POI_TYPE & sEZPoiType) :mPlanUid(sEzPlanUID), mPoiUid(sEzPoiUID), mPoiType(sEZPoiType)
{

}

TpsDeletePoiEasyPlanCmd::~TpsDeletePoiEasyPlanCmd()
{

}



int TpsDeletePoiEasyPlanCmd::Execute()
{
    // 1. find the plan the poi belongs to.
    TpsPatientManager* patientManager = mDataRepository->GetPatientManager();
    RtPlan * pPlan;
    if (!patientManager->GetPlan(mPlanUid, &pPlan)
        || pPlan == nullptr){
        TPS_LOG_DEV_ERROR << "The plan with UID: " << mPlanUid << " does not exist.";
        return false;
    }

    //2. delete poi of the plan in poi manager
    TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
    RtPoi *oldPOI = nullptr;
    oldPOI = poiManager->GetPOI(mPoiUid);
    if (oldPOI == nullptr) {
        TPS_LOG_DEV_WARNING << "Failed to delete poi because this poi with uid: " << mPoiUid
            << " dose not exist in poi manager ";
        return true;
    }
    else {
        // modify old poi existing in poi manager 
        if (!poiManager->DeletePOI(mPoiUid)) {
            TPS_LOG_DEV_ERROR << "Failed to add poi from poi manager of plan: " << mPlanUid;
            return false;
        }
    }

    //3. update GO and render
    if ((RtDbDef::POI_TYPE)mPoiType == RtDbDef::NormalPoint
        || (RtDbDef::POI_TYPE)mPoiType == RtDbDef::Calculation) {
        mRenderProxy->DeletePoi(mPlanUid,mPoiUid);
        mRenderProxy->Render();
    }

    return TPS_ER_SUCCESS;
    
}

TPS_END_NAMESPACE