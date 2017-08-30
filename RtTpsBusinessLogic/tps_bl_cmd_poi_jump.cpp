////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenqiao.Jiang  Wenqiao.Jiang@united-imaging.com
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

#include "RtTpsBusinessLogic/tps_bl_cmd_poi_jump.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

//#include "RtTpsCommonControllerApp/tps_comca_icommoncontroller.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
PoiJumpCmd::PoiJumpCmd(const CertifiedPoi &poi) : mCertifiedPoi(poi){
    m_strCommandName = "PoiJumpCmd";
}

PoiJumpCmd::~PoiJumpCmd() {
}

int PoiJumpCmd::Execute() 
{
    try
    {
        std::string strImageUid = mCertifiedPoi.m_strSeriesUid;
        mRenderProxy->JumpTo(strImageUid, mCertifiedPoi.m_fXCoordInPat, mCertifiedPoi.m_fYCoordInPat, mCertifiedPoi.m_fZCoordInPat);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    catch (std::exception ex)
    {
        TPS_LOG_DEV_INFO << ex.what();
        return TPS_ER_FAILURE;
    }
}
TPS_END_NAMESPACE
