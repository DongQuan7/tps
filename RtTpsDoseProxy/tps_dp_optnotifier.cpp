//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_simpleoptfactory.h
/// 
///  \version 1.0
/// 
///  \date    2014/04/14
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tps_dp_optnotifier.h"

#include "RtTpsDoseProxy/tps_dp_optobserver.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptoutputbase.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_beam.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_poi.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_voi.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_normgroup.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptoutputbase.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dvh.h"


#include "alg/alg_common_algreturn.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedoptoutput.h"
#include "alg/alg_common_algstatus.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dvh.h"

#include "poi_converter.h"
#include "voi_converter.h"
#include "beam_converter.h"
#include "normgroup_converter.h"
#include "dosegrid_converter.h"
#include "image3d_converter.h"
#include "optgoal_converter.h"
#include "dvh_converter.h"
#include "enum_converter.h"



#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsOptNotifier::TpsOptNotifier()
{

}

TpsOptNotifier::~TpsOptNotifier()
{

}

void TpsOptNotifier::NotifyProgress()
{

}

void TpsOptNotifier::SetObservers(TpsOptObserver& observers)
{
    mObserver = &observers;
}

void TpsOptNotifier::NotifyOutput(RTFWK::RTFWKDLDODoseOptOutputBase* pOptOutput)
{
    try
    {
        ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][NotifyOutput]: " 
            << "tps NotifyOutput start";

        float ngMu = 0.0f;
        int ngSegmentCount = 0;
        ALG_RETURN_TYPE optResult = (ALG_RETURN_TYPE)pOptOutput->GetOPTReturnValue();
        float iterationNumber = pOptOutput->GetIterationNumber();
        float iterationScore = pOptOutput->GetIterationScore();
        float progressValue = pOptOutput->GetProgressValue();
        int algStatus = pOptOutput->GetAlgStatusValue();
        std::string normgoupUid = pOptOutput->GetNormgroup()->GetUID();
        std::map<std::string, float> eudResult = pOptOutput->GetEUDGoalValue();
        std::map<std::string, float> objectValue = pOptOutput->GetObjValue();

        printf("\n====================================================");
        printf("\n[TPS][TpsOptCmd] optResult:%.2f", optResult);
        printf("\n[TPS][TpsOptCmd] iterationNumber:%.2f", iterationNumber);
        printf("\n[TPS][TpsOptCmd] iterationScore:%.2f", iterationScore);
        printf("\n[TPS][TpsOptCmd] progressValue:%.2f", progressValue);
        printf("\n====================================================");

        if (algStatus == ALG_STATUS_END)
        {
            if (optResult == ALG_OK)
            {
                RTFWK::RTFWKDLDODoseGrid* algDosegrid = pOptOutput->GetDosegrid();
                std::vector<RTFWK::RTFWKDLDOBeam*> algBeams = pOptOutput->GetBeamList();
                std::vector<RTFWK::RTFWKDLDOPOI*> algPois = pOptOutput->GetPOIList();
                std::vector<RTFWK::RTFWKDLDOVOI*> algVois = pOptOutput->GetVOIList();
                RTFWK::RTFWKDLDONormgroup* algNg = pOptOutput->GetNormgroup();
                auto poiDoseMap = pOptOutput->GetBeamDoseContributionPerMuToPOI();
                std::string planUid = algNg->GetPlanUID();

                // step 1. update norm group
                RtNormgroup* tpsNg = nullptr;
                mDataRepository->GetNormgroupManager()->GetNormGroup(normgoupUid, &tpsNg);
                NormGroupConverter::ConvertToTPS(*algNg, tpsNg);

                // Step 2. update beams
                std::vector<RtBeam*> vBeams;
                for (auto itAlgBeam = algBeams.begin(); itAlgBeam != algBeams.end(); ++itAlgBeam)
                {
                    RTFWK::RTFWKDLDOBeam* algBeam = *itAlgBeam;
                    ngMu += algBeam->GetBeamMU();
                    ngSegmentCount += algBeam->GetSegments().size();
                    RtBeam* tpsBeam = mDataRepository->GetBeamManager()->GetBeam(algBeam->GetUID());
                    vBeams.push_back(tpsBeam);
                }
                BeamConverter::ConvertToTPSBeamList(algBeams, &vBeams);

                // Step 3. update dosegrid
                RtDosegrid* pNormgroupDosegrid = mDataRepository->GetDoseManager()->GetDoseGrid(normgoupUid, DOSE_GRID_NORMGROUP);
                if (algDosegrid != nullptr)
                {
                    DosegridConverter::ConvertToTPS(*algDosegrid, pNormgroupDosegrid);
                }

                // Step 4 update POI
                std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
                std::vector<RtPoi*> tpsPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
                if (algPois.size() > 0)
                {
                    PoiConverter::ConvertToTPSPoiList(algPois, poiDoseMap, &tpsPoiList);
                    mDataRepository->GetPoiManager()->UpdatePois(tpsPoiList);
                }

                ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][NotifyOutput]: " 
                    << "tps NotifyOutput success";
            }
            else
            {
                ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][NotifyOutput]: "
                    << "Alg opt return error" << optResult;
            }
        }


        // Step 5. Call observer to update data
        CertifiedOptOutput cOptOutput;
        cOptOutput.mNormalGroupUid = normgoupUid;
        cOptOutput.mIterationNumber = iterationNumber;
        cOptOutput.mIterationResult = iterationScore;
        cOptOutput.mProgressValue = progressValue;
        cOptOutput.mPlanMU = ngMu;
        cOptOutput.mPlanSegmentCount = ngSegmentCount;
        cOptOutput.mEudResult = eudResult;
        cOptOutput.mObjectValue = objectValue;
        cOptOutput.mOptReturnValue = EnumConverter::AlgConvertToTPS(optResult);
        cOptOutput.mAlgStatus = algStatus;

        mObserver->optUpdated(cOptOutput);


    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
    }
}

TPS_END_NAMESPACE
