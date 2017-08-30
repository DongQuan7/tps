//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  xiaoqing.shangguan (mailto:xiaoqing.shangguan@united-imaging.com)
///
///  \file   tps_cca_approveplan_cmd.h
///  \brief   approve normgroup
///
///  \version 1.0
///  \date  2014/11/11  
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_approve_plan.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"

#include "tps_logger.h"
#include <boost/date_time/posix_time/posix_time.hpp>


TPS_BEGIN_NAMESPACE

TpsApprovePlanCmd::TpsApprovePlanCmd(RtApproverecord* approveRecord)
    : mApproveRecord(approveRecord)
{

}

TpsApprovePlanCmd::~TpsApprovePlanCmd()
{

}


int TpsApprovePlanCmd::Execute()
{
    if (nullptr == mApproveRecord)
    {
        return TPS_ER_FAILURE;
    }

    int iRet = mSaveObjects->SaveApproveRecord(*mApproveRecord);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to InsertApproveRecord in TpsApprovePlanCmd";
        return iRet;
    }

    std::string planUid = mApproveRecord->get_objectuid();

    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan))
    {
        TPS_LOG_DEV_INFO << "plan do not exist with uid: " << planUid;
        return TPS_ER_FAILURE;
    }
    // update poi info
    std::vector<RtPoi*> tpsPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(plan->get_imageseriesuid());
    for (std::vector<RtPoi*>::iterator it = tpsPoiList.begin(); it != tpsPoiList.end(); ++it)
    {
        RtPoi* poi = *it;
        //最新需求是approve plan不锁计算点
        if (poi->get_locktype() != RtDbDef::P_LOCK && RtDbDef::Calculation != poi->get_type())
        {
            poi->set_locktype(RtDbDef::P_LOCK);
            mApprovedPois.push_back(poi);
            iRet = mSaveObjects->UpdatePoi(poi);
            if (iRet != TPS_ER_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "Failed to UpdatePoi in TpsApprovePlanCmd";
                return iRet;
            }
        }
    }
    // update voi info
    std::string seriesUid = plan->get_imageseriesuid();
    std::vector<ROIEntity*> voiList = mDataRepository->GetVoiManager()->GetROIBySeries(seriesUid);
    RtVoi* innerRoi;
    for (int i = 0; i < voiList.size(); ++i)
    {
        innerRoi = voiList[i]->GetInnerRoi();
        if (innerRoi == nullptr) continue;

        if (innerRoi->get_locktype() == RtDbDef::P_LOCK)
        {
            continue;
        }
        innerRoi->set_locktype(RtDbDef::P_LOCK);
        std::vector<bool> interpolates = innerRoi->get_interpolate();
        interpolates.assign(interpolates.size(), false);
        innerRoi->set_interpolate(interpolates);
        iRet = mSaveObjects->UpdateVoiAttributes(*voiList[i]);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to UpdateVoiAttributes in TpsApprovePlanCmd";
            return iRet;
        }

        RtApproverecord approveRecord(true);
        approveRecord.set_objectuid(innerRoi->get_uid());
        approveRecord.set_isapproved(true);
        approveRecord.set_objecttype(RtDbDef::VOI);
        approveRecord.set_approver(mApproveRecord->get_approver());
        approveRecord.set_approvedatetime(mApproveRecord->get_approvedatetime());
        iRet = mSaveObjects->SaveApproveRecord(approveRecord);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to Save Voi ApproveRecord in TpsApproveNormGroupCmd";
            return iRet;
        }

        mApprovedVois.push_back(voiList[i]);
        //delete all related commands in redo/undo list of CommandManager
        TpsSafeLock<TpsCommandManager> lock(mDataRepository->GetCommandManager());
        mDataRepository->GetCommandManager()->RemoveCommands(
            [&](TpsRedoUndoCommand* cmd)->bool{
                auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
                return voiCmd != nullptr && voiCmd->GetVoiUid() == innerRoi->get_uid();
        });

        mRenderProxy->UpdateVOIAttribute(seriesUid, innerRoi->get_uid());
    }

    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE