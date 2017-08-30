////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Song LI  song.li@united-imaging.com
/// 
///  \file tps_cmd_poi_delete.cpp
/// 
///  \brief class PoiDeleteCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/12/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_poi_delete.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

PoiDeleteCmd::PoiDeleteCmd(const CertifiedDelete& certifiedDelete) : 
m_stcCertifiedDelete(certifiedDelete){

}

PoiDeleteCmd::~PoiDeleteCmd() {

}

int PoiDeleteCmd::Execute() {

    std::string strObjectType = m_stcCertifiedDelete.mObjectType;
    std::string strUid = m_stcCertifiedDelete.mUid;
    if(strObjectType != "POI") {
        TPS_LOG_DEV_ERROR<<"PoiDeleteCmd: Object type error";
        return TPS_ER_FAILURE;
    }

    TpsPOIManager *pPoiManager = mDataRepository->GetPoiManager();
    RtPoi *pPoi = pPoiManager->GetPOI(strUid);
    if(nullptr == pPoi) {
        TPS_LOG_DEV_ERROR<<"PoiDeleteCmd: Failed to find poi in poi mananger, UID = " << strUid;
        return TPS_ER_FAILURE;
    }

    if (pPoi->get_seriesuid().empty()){
        TPS_LOG_DEV_ERROR<<"PoiDeleteCmd: Failed to get series uid for the poi:" << strUid;
        return TPS_ER_FAILURE;
    }

    //delete poi from renderProxy (graphic object)
    mRenderProxy->DeletePoi(pPoi->get_seriesuid(), strUid);
    mRenderProxy->Render();

    // Delete poi from manager.
    if(!pPoiManager->DeletePOI(strUid)) {
        TPS_LOG_DEV_ERROR<<"PoiDeleteCmd: Failed to delete poi from poi manager! UID = "<< strUid;
        return TPS_ER_FAILURE;
    }
    // Delete from database
    if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(strUid, RtDbDef::TYPE_POI)){
        TPS_LOG_DEV_ERROR<<"PoiDeleteCmd: Failed to delete poi from DB with UID = "<< strUid;
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
