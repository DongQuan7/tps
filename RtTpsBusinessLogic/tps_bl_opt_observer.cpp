//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  xiaoqing.shangguan xiaoqing.shangguan@united-imaging.com
///
///  \file    tps_pca_planningctrloptobserver.h
///  \brief
///
///  \version 1.0
///  \date    Apr.22, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_defs.h"

#include "RtTpsBusinessLogic/tps_bl_opt_observer.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"

#include "RtTpsProtoLib/rt_tps_optoutput.pb.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "alg/alg_common_algreturn.h"
#include "alg/alg_common_algstatus.h"

TPS_BEGIN_NAMESPACE

TpsPlanningCtrlOptObserver::TpsPlanningCtrlOptObserver() : mResponser(nullptr)
{
}

TpsPlanningCtrlOptObserver::~TpsPlanningCtrlOptObserver()
{
}

void TpsPlanningCtrlOptObserver::optUpdated(CertifiedOptOutput optOutput)
{
    try
    {
        if (optOutput.mAlgStatus == ALG_STATUS_END)
        {
            if (optOutput.mOptReturnValue == ALG_OK)
            {
                std::string normgoupUid = optOutput.mNormalGroupUid;

                RtNormgroup* tpsNg = nullptr;
                if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normgoupUid, &tpsNg))
                {
                    TPS_LOG_DEV_ERROR << "Failed to get normgroup by uid:" << normgoupUid;
                    return;
                }

                // 优化后ng处方修改 所以先刷新doseline
                mDoseCalcImpl->GetRenderProxy()->RefreshDoseLine(tpsNg->get_planuid());

                // recompose dose
                if (!mDoseCalcImpl->ReComposeDoseForNormgroup(normgoupUid))
                {
                    TPS_LOG_DEV_ERROR << "Failed to compose dose for plan.";
                    return;
                }

                std::vector<std::string> beamUIDList = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(normgoupUid);
                for (auto itBeamUID = beamUIDList.begin(); itBeamUID != beamUIDList.end(); ++itBeamUID)
                {
                    auto beam = mDataRepository->GetBeamManager()->GetBeam(*itBeamUID);
                    if (!mDoseCalcImpl->GetDoseProxy()->UpdateFluenceMap(beam))
                    {
                        TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
                        return;
                    }

                    if (TPS_ER_SUCCESS != mSaveObjects->SaveBeam(*itBeamUID))
                    {
                        TPS_LOG_DEV_ERROR << "Failed to save beam in database. uid: " << *itBeamUID;
                        return;
                    }
                }
                if (TPS_ER_SUCCESS != mSaveObjects->SaveDosegridsForNormgroup(normgoupUid))
                {
                    TPS_LOG_DEV_ERROR << "Failed to save norm group's dosegrid in database. uid:" << normgoupUid;
                    return;
                }

                //save ng 优化会修改处方剂量 所以Ng需要存数据库 wzd-2016-8-29
                if (TPS_ER_SUCCESS != mSaveObjects->SaveNormgroup(*tpsNg))
                {
                    TPS_LOG_DEV_ERROR << "Failed to save norm group in database. uid:" << normgoupUid;
                    return;
                }
                std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(tpsNg->get_planuid());
                std::vector<RtPoi*> vPois = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
                for (std::vector<RtPoi*>::iterator itPoi = vPois.begin(); itPoi != vPois.end(); ++itPoi)
                {
                    //和强强茂亮讨论，计算完之后应该不需要更新Poi，只需要更新PoiDose lx 2016/12/19
                    if (TPS_ER_SUCCESS != mSaveObjects->UpdateBeamPoiDose(*itPoi))
                    {
                        TPS_LOG_DEV_ERROR << "Failed to update poi dose to database with poi uid" << (*itPoi)->get_uid() <<"].";
                        return;
                    }
                }

                ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][optUpdated]: "
                    << "tps optUpdated success";
            }
            else
            {
                ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][optUpdated]: "
                    << "Alg opt return error" << optOutput.mOptReturnValue;
            }
        }

        if (mResponser != nullptr)
        {
            proto::RT_TPS_OptOutput protoOptOutput;
            optOutput.ConvertBack(&protoOptOutput);
            mResponser->Response(protoOptOutput);
        }
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
    }
}

TPS_END_NAMESPACE
