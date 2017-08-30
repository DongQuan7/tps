////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_update_voi_display.cpp
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Oct.19,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_update_attribute.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

TPS_BEGIN_NAMESPACE;

VoiUpdateAttributeCmd::VoiUpdateAttributeCmd(ROIEntity *roiEntity) : m_pSrcVoi(roiEntity)
{

}

VoiUpdateAttributeCmd::~VoiUpdateAttributeCmd(void) 
{

}

int VoiUpdateAttributeCmd::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][UpdateVoiCmd][Execute]: "<<"UpdateVoiCmd execute!";
    if (nullptr == m_pSrcVoi){
        return TPS_ER_FAILURE;
    }

    TpsVOIManager* pVoiManager = mDataRepository->GetVoiManager();
    if (pVoiManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "VoiUpdateAttributeCmd : Failed to get the voi manager";
        return TPS_ER_FAILURE;
    }

    auto src_roi = m_pSrcVoi->GetInnerRoi();
    std::string strVoiUid = src_roi->get_uid();
    ROIEntity* roi = pVoiManager->GetROI(strVoiUid);
    if (roi == false)
    {
        TPS_LOG_DEV_ERROR << "VoiUpdateAttributeCmd : Failed to get the roi for uid:" << strVoiUid;
        return TPS_ER_FAILURE;
    }

    // (1) Update VoiManager 
    //     VOI attribute:  src (mVoiEntity) -> target (pVoi)
    auto innerRoi = roi->GetInnerRoi();
    innerRoi->set_name(src_roi->get_name());
    innerRoi->set_color(src_roi->get_color());
    innerRoi->set_alpha3d(src_roi->get_alpha3d());
    innerRoi->set_isvisible(src_roi->get_isvisible());

    int preLockType = innerRoi->get_locktype();
    int curLockType = src_roi->get_locktype();
    //一旦锁定，则清除该VOI的Undo/Redo List
    if (preLockType == RtDbDef::UNLOCK && curLockType == RtDbDef::NORMALLOCK)
    {
        //delete all related commands in redo/undo list of CommandManager
        auto mng = mDataRepository->GetCommandManager();
        TpsSafeLock<TpsCommandManager> lock(mng);
        mng->RemoveCommands(
            [&](TpsRedoUndoCommand* cmd)->bool{
                auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
                return voiCmd != nullptr && voiCmd->GetVoiUid() == strVoiUid;
        });
    }
    innerRoi->set_locktype(curLockType);
    innerRoi->set_planningrole(src_roi->get_planningrole());
    innerRoi->set_subrole(src_roi->get_subrole());
    innerRoi->set_description(src_roi->get_description());
    innerRoi->set_isshaded(src_roi->get_isshaded());
    // ignore density, statistic info, dose because FE will never modify these information

    // (2) Save to database
    //if (!pVoi->IsEmpty()) 
    {
        int iRet = mSaveObjects->UpdateVoiAttributes(*roi);
        if (TPS_ER_SUCCESS != iRet) return iRet;
    }

    // (3) Refresh display
    const std::string strSeriesUid = src_roi->get_seriesuid();
    mRenderProxy->UpdateVOIAttribute(strSeriesUid, strVoiUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE
