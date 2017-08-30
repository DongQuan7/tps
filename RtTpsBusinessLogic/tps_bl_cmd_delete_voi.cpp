////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_delete_voi.cpp
/// 
///  \brief class  delete voi  
/// 
///  \version 1.0
/// 
///  \date    Oct.21,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_delete_voi.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;

DeleteVoiCmd::DeleteVoiCmd(const std::string& sVoiUid):mVoiUid(sVoiUid)
{
}

DeleteVoiCmd::~DeleteVoiCmd() {

}

int DeleteVoiCmd::Execute() 
{
    //_ASSERTE( _CrtCheckMemory() );

    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();
    ROIEntity *voi = voiManager->GetROI(mVoiUid);
    if(nullptr == voi) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi from voi manager!"<<mVoiUid;
        return TPS_ER_FAILURE;
    }

    auto innerRoi = voi->GetInnerRoi();
    const std::string sSeriesUid = innerRoi->get_seriesuid();
    const std::string sStructuresetUid = innerRoi->get_structuresetuid();
    if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(mVoiUid, RtDbDef::TYPE_VOI)){
        TPS_LOG_DEV_ERROR<<"Failed to delete voi from database";
        return TPS_ER_DB_ERROR;
    }

    //more for deleting a voi
    int VoiCount = 0;
    int iRet = mDatabaseWrapper->GetVoiCountByStructuresetuid(sStructuresetUid, &VoiCount);
    if (DB_CODES_SUCCESS == iRet){
        if (0 == VoiCount){//no voi any more
            if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(sStructuresetUid, RtDbDef::TYPE_STRUCTURESET)){
                TPS_LOG_DEV_ERROR<<"Failed to delete Structureset from database";
                return TPS_ER_DB_ERROR;
            }
        }
    }
    else{
        TPS_LOG_DEV_ERROR<<"Failed to GetVoiCount ByStructuresetuid";
        return TPS_ER_DB_ERROR;
    }

    //delete poi or voi from renderProxy!
    mRenderProxy->DeleteVoi(sSeriesUid, mVoiUid);
    // delete voi from manager.
    voiManager->DeleteROI(mVoiUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int DeleteVoiCmd::PostExecute() {

    //delete all related commands in redo/undo list of CommandManager
    auto mng = mDataRepository->GetCommandManager();
    TpsSafeLock<TpsCommandManager> lock(mng);
    mng->RemoveCommands(
        [&](TpsRedoUndoCommand* cmd)->bool{
            auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
            return voiCmd != nullptr && voiCmd->GetVoiUid() == mVoiUid;
    });
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE

