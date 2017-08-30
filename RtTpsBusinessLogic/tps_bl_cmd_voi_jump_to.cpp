////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Song LI  song.lig@united-imaging.com
/// 
///  \file tps_cmd_voi_jump_to.cpp
/// 
///  \brief class VoiJumpToCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_voi_jump_to.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

//#include "RtTpsCommonControllerApp/tps_comca_icommoncontroller.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

VoiJumpToCmd::VoiJumpToCmd(std::string strVoiUid) : m_strVoiUid(strVoiUid)
{
    m_strCommandName = "VoiJumpToCmd";
}

VoiJumpToCmd::~VoiJumpToCmd() 
{
}

int VoiJumpToCmd::Execute() 
{
    try
    {
        TpsVOIManager* pVoiManager = mDataRepository->GetVoiManager();
        if (pVoiManager == nullptr)
        {
            TPS_LOG_DEV_ERROR << m_strCommandName << ": Failed to get the voi manager";
            return TPS_ER_FAILURE;
        }

        ROIEntity* pVoi = pVoiManager->GetROI(m_strVoiUid);
        if (pVoi == nullptr)
        {
            TPS_LOG_DEV_ERROR << m_strCommandName << ": Failed to get the voi for uid:" << m_strVoiUid;
            return TPS_ER_FAILURE;
        }
        auto innerRoi = pVoi->GetInnerRoi();

        mRenderProxy->JumpTo(pVoi->GetSeriesUid(), 
            innerRoi->get_centroidx(), innerRoi->get_centroidy(), innerRoi->get_centroidz()); 

        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    catch (std::exception ex){
        TPS_LOG_DEV_INFO << ex.what();
        return TPS_ER_FAILURE;
    }
}

TPS_END_NAMESPACE
