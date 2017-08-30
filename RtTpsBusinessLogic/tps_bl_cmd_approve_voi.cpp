//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author linqi.liu  linqi.liu@united-imaging.com
///
///  \file   tps_cca_approvevoidetail_cmd.h 
///  \brief   TpsApproveVoiDetailCmd
///
///  \version 1.0
///  \date  2014/06/23 
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_approve_voi.h"


#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_approverecord.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "tps_logger.h"


TPS_BEGIN_NAMESPACE

TpsApproveVoiDetailCmd::TpsApproveVoiDetailCmd(std::vector<RtApproverecord*> voiApproveRecordList)
    : mVoiApproveRecordList(voiApproveRecordList)
{

}

TpsApproveVoiDetailCmd::~TpsApproveVoiDetailCmd()
{

}

int TpsApproveVoiDetailCmd::Execute()
{
    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();

    for (std::vector<RtApproverecord*>::iterator itVoiApproveRecord = mVoiApproveRecordList.begin();
        itVoiApproveRecord != mVoiApproveRecordList.end(); itVoiApproveRecord++)
    {
        RtApproverecord* approveRecord = *itVoiApproveRecord;
        std::string voiUid = approveRecord->get_objectuid();
        ROIEntity* roi = voiManager->GetROI(voiUid);
        RtVoi* innerRoi = roi->GetInnerRoi();
        innerRoi->set_locktype(RtDbDef::P_LOCK);

        std::vector<bool> interpolates = innerRoi->get_interpolate();
        interpolates.assign(interpolates.size(), false);
        innerRoi->set_interpolate(interpolates);

        int iRet = mSaveObjects->SaveApproveRecord(*approveRecord);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to InsertApproveRecord in TpsApproveVoiDetailCmd";
            return iRet;
        }

        iRet = mSaveObjects->UpdateVoiAttributes(*roi);

        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to UpdateVoiAttributes in TpsApproveVoiDetailCmd";
            return iRet;
        }

        //delete all related commands in redo/undo list of CommandManager
        TpsSafeLock<TpsCommandManager> lock(mDataRepository->GetCommandManager());
        mDataRepository->GetCommandManager()->RemoveCommands(
            [&](TpsRedoUndoCommand* cmd)->bool
        {
            auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
            return voiCmd != nullptr && voiCmd->GetVoiUid() == voiUid;
        });

        std::string imageUID = innerRoi->get_seriesuid();
        mRenderProxy->UpdateVOIAttribute(imageUID, voiUid);

    }

    mRenderProxy->Render();


    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE