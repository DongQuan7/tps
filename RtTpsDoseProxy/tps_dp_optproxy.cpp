//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_optproxy.h
/// 
///  \version 1.0
/// 
///  \date    2014/04/14
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RtTpsDoseProxy/tps_dp_optproxy.h"

#include <process.h>

#include "alg/RtAlgDataLayer/rt_alg_dl_object_series.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_normgroup.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptgoalbase.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_poi.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_normgroup.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptoutputbase.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptendcriterionbyiteration.h"

#include "alg/alg_opt_factory.h"
#include "alg/alg_common_algreturn.h"
#include "alg/alg_opt_optimizerbase.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseoptgoalbase.h"

#include "RtTpsCertifiedLib/rt_tps_doseoptimizationparam.h"

#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

#include "tps_dp_optnotifier.h"



#include "poi_converter.h"
#include "voi_converter.h"
#include "beam_converter.h"
#include "normgroup_converter.h"
#include "dosegrid_converter.h"
#include "image3d_converter.h"
#include "optgoal_converter.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsOptProxy::TpsOptProxy()
{

}

TpsOptProxy::~TpsOptProxy()
{
    DEL_PTR(mNotifier);
    map<RTFWK::DOSEOPTALGO, RTALG::OptimizerBase*>::iterator itMapOpt = optimizerMap.begin();
    for (; itMapOpt != optimizerMap.end(); ++itMapOpt)
    {
        RTALG::OptimizerBase* optimizer = itMapOpt->second;
        optimizer->Finalize();
        DEL_PTR(optimizer);
    }
}

bool TpsOptProxy::Initialize()
{
    try
    {
        std::vector<RTFWK::DOSEOPTALGO> vOptEnum;
        vOptEnum.push_back(RTFWK::FLUENCE_MAP);
        vOptEnum.push_back(RTFWK::DIRECT_APERTURE);
        vOptEnum.push_back(RTFWK::SEGMENT_WEIGHT);
        vOptEnum.push_back(RTFWK::BURST_MODE_ARC);
        vOptEnum.push_back(RTFWK::VOLUMETRIC_MODULATED_ARC);
        RTALG::OptimizerFactory optFacotry;
        std::vector<RTFWK::DOSEOPTALGO>::iterator itOptimizer = vOptEnum.begin();
        for (; itOptimizer != vOptEnum.end(); ++itOptimizer)
        {
            RTALG::OptimizerBase* optimizer = optFacotry.CreateOptimizer((RTALG::DoseOptAlgo)*itOptimizer);
            if (nullptr != optimizer)
            {
                if (optimizer->Initialize() != TPS_ER_SUCCESS)
                {
                    TPS_LOG_DEV_ERROR << "Failed to initialize the optimizer" << *itOptimizer;
                }
                else
                {
                    optimizerMap.insert(std::make_pair(*itOptimizer, optimizer));
                }
            }
            else
            {
                TPS_LOG_DEV_ERROR << "Failed create the optimizer" << *itOptimizer;
            }
        }

        mNotifier = new TpsOptNotifier();
        mNotifier->SetObservers(*mObserver);
        mNotifier->SetDataRepository(mDataRepository);
        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

bool TpsOptProxy::OptStart(RtDbDef::PLAN_TYPE plan_type,
    const std::string& ngUid,
    const std::vector<std::string>& vecBasedOnNgUids, 
    const std::vector<RtDoseoptgoalbase*>& vecGoals,
    const DoseOptimizationParam& param)
{
    try
    {
        ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][OptStart]: " 
            << "tps optimizer start: [" << ngUid << "]";

        RTFWK::DOSEOPTALGO optType = GetOptType_i(plan_type);
        map<RTFWK::DOSEOPTALGO, RTALG::OptimizerBase*>::iterator itOpt = optimizerMap.begin();
        for (; itOpt != optimizerMap.end(); ++ itOpt)
        {
            if (itOpt->first == optType)
            {
                mOptimizer = itOpt->second;
            }
        }
        if (mOptimizer == nullptr)
        {
            TPS_LOG_DEV_ERROR << "do not support optimize type :" << optType;
            return false;
        }
        mOptimizer->SetOptNotifier(mNotifier);
        RTFWK::RTFWKDLDODoseOptInputBase optInput;
        RtNormgroup* pNormgroup = nullptr;
        if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &pNormgroup))
        {
            if (pNormgroup == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to GetPlan[" << ngUid << "] from TpsPlanManager.";
                return false;
            }
        }
        RtPlan* plan = nullptr;
        if (!mDataRepository->GetPatientManager()->GetPlan(pNormgroup->get_planuid(), &plan)
            || plan == nullptr)
        {
            TPS_LOG_DEV_ERROR << "The Plan does not exist, uid: " << pNormgroup->get_planuid();
            return false;
        }

        std::string sCurrentImageDataUid = plan->get_imageseriesuid();

        if (!PrepareOptNg_i(*pNormgroup, vecBasedOnNgUids, &optInput))
        {
            TPS_LOG_DEV_ERROR << "PrepareOptNg_i failed";
            return false;
        }
        if (!PrepareConceptVolume_i(*plan, &optInput))
        {
            TPS_LOG_DEV_ERROR << "PrepareConceptVolume_i failed";
            return false;
        }
        if (!PrepareOptParams_i(vecGoals, param, &optInput, sCurrentImageDataUid))
        {
            TPS_LOG_DEV_ERROR << "PrepareOptParams_i failed";
            return false;
        }
        if (ALG_OK != mOptimizer->SetOptInput(optInput))
        {
            TPS_LOG_DEV_ERROR << "SetOptInput has error";
            return false;
        }
        mOptimizer->Start();


        ADD_WORKFLOW_KEYPOINT << "[TpsDoseProxy][OptStart]: " 
            << "tps optimizer end: [" << ngUid << "]";

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }

}

bool TpsOptProxy::OptAbort()
{
    if (TPS_ER_SUCCESS == mOptimizer->Abort())
    {
        return true;
    }
    else
    {
        TPS_LOG_DEV_ERROR << "Abort Optimizer failed.";
        return false;
    }
}

bool TpsOptProxy::PrepareConceptVolume_i(
    const RtPlan& plan,
    RTFWK::RTFWKDLDODoseOptInputBase* optInput)
{
    try
    {
        // set image 3d
        TpsImage3DEntity* image3D = nullptr;
        std::string seriesUid = plan.get_imageseriesuid();
        if (!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image3D))
        {
            TPS_LOG_DEV_ERROR << "Failed to GetImageSeries[" << seriesUid << "]";
            return false;
        }
        std::shared_ptr<RTFWK::RTFWKDLDOImage3D> image3dCarrier;
        Image3dConverter::ConvertToALG(*image3D, &image3dCarrier);
        RTFWK::RTFWKDLDOImage3D* algImage3d = image3dCarrier.get();
        optInput->SetCtImage3D(algImage3d);

        // set poi
        std::vector<RtPoi*> tpsPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(plan.get_imageseriesuid());
        if (tpsPoiList.size() > 0)
        {
            std::vector<std::shared_ptr<RTFWK::RTFWKDLDOPOI>> poiCarrier;
            PoiConverter::CreateALGPoiList(tpsPoiList, &poiCarrier);
            std::vector<RTFWK::RTFWKDLDOPOI*> algPoiList;
            for (int i = 0; i < poiCarrier.size(); ++i)
            {
                algPoiList.push_back(poiCarrier[i].get());
            }
            optInput->SetPOIs(algPoiList);
        }

        // set voi
        auto voiMng = mDataRepository->GetVoiManager();
        std::vector<ROIEntity*> roiList = voiMng->GetROIBySeries(seriesUid);
        if (roiList.size() > 0 )
        {
            std::vector<RtVoi*> voiList;
            for (int i = 0; i < roiList.size(); ++i)
            {
                voiList.push_back(roiList[i]->GetInnerRoi());
            }

            size_t dim[3];
            image3D->GetDimension(dim);
            Mcsf::Matrix4f matV2P;
            image3D->GetVolumeToPatientMatrix(matV2P);
            auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
            std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>> voiCarrier;
            VoiConverter::CreateALGVoiList(voiList, &voiCarrier);
            std::vector<RTFWK::RTFWKDLDOVOI*> algVoiList;
            for (int i = 0; i < voiCarrier.size(); ++i)
            {
                RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier[i].get();
                auto roi = mDataRepository->GetVoiManager()->GetROI(algVoi->GetVOIUID());
                auto subVolume = new McsfGeometry::SubVolume();
                ROIShapeConverter::SDFToSubVolume(*roi->GetSDF(), matP2V, dim, *subVolume);

                algVoi->SetReferenceSubVolume(subVolume);
                algVoiList.push_back(algVoi);
                TPS_DEL_PTR(subVolume);
            }
            optInput->SetVOIs(algVoiList);
        }
        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

bool TpsOptProxy::PrepareOptNg_i(
    const RtNormgroup& normgroup,
    const std::vector<std::string>& vBasedOnNgUids,
    RTFWK::RTFWKDLDODoseOptInputBase* optInput)
{
    try
    {
        // set input type
        RtDbDef::PLAN_TYPE normGroupTechniqueType = (RtDbDef::PLAN_TYPE)normgroup.get_plantype();
        RTFWK::DOSEOPTALGO optType = GetOptType_i(normGroupTechniqueType);
        optInput->SetInputType(optType);

        // set dose grid object
        RTFWK::RTFWKDLDODoseGrid* algDoseGrid = nullptr;
        std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
        DosegridConverter::ConvertToALG(*normgroup.get_dosegrid(), &doseGridCarrier);
        algDoseGrid = doseGridCarrier.get();
        optInput->SetDoseGrid(algDoseGrid);

        // set normal group
        std::shared_ptr<RTFWK::RTFWKDLDONormgroup> normgroupCarrier;
        NormGroupConverter::ConvertToALG(normgroup, &normgroupCarrier);
        RTFWK::RTFWKDLDONormgroup* algNormgroup = normgroupCarrier.get();
        optInput->SetNormgroup(algNormgroup);

        // set opt beams
        // 1. get beamList by currentNg
        TpsBeamManager* pBeamMgr = mDataRepository->GetBeamManager();
        std::vector<RtBeam*> vTpsBeams = pBeamMgr ->GetBeamsByNormGroup(normgroup.get_uid());

        // 2.get beamList by BasedOnNg
        std::vector<std::string> vBnU = vBasedOnNgUids;
        std::vector<RtNormgroup*> vBasedOnNg;
        for (std::vector<std::string>::iterator itNgUid = vBnU.begin(); itNgUid != vBnU.end(); ++itNgUid)
        {
            RtNormgroup* pNormgroup = nullptr;
            if (!mDataRepository->GetNormgroupManager()->GetNormGroup(*itNgUid, &pNormgroup))
            {
                if (pNormgroup == nullptr)
                {
                    TPS_LOG_DEV_ERROR << "Failed to GetNoramlGroup[" << *itNgUid << "] from TpsMormalGroupManager.";
                    return false;
                }
            }
            vBasedOnNg.push_back(pNormgroup);
            std::vector<RtBeam*> tpsBeamList = pBeamMgr->GetBeamsByNormGroup(pNormgroup->get_uid());
            vTpsBeams.insert(vTpsBeams.end(), tpsBeamList.begin(), tpsBeamList.end());
        }
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>> beamCarrier;
        BeamConverter::mDataRepository = mDataRepository;
        BeamConverter::CreateALGBeamList(vTpsBeams, &beamCarrier);
        std::vector<RTFWK::RTFWKDLDOBeam*> algBeamList;
        for (int i = 0; i < beamCarrier.size(); ++i)
        {
            algBeamList.push_back(beamCarrier[i].get());
        }
        optInput->SetBeams(algBeamList);

        // set base on normalgroup
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDONormgroup>> basedOnNgCarrier;
        NormGroupConverter::CreateALGNormgroupList(vBasedOnNg, &basedOnNgCarrier);
        std::vector<RTFWK::RTFWKDLDONormgroup*> vAlgNg;
        for (int i = 0; i < basedOnNgCarrier.size(); ++i)
        {
            vAlgNg.push_back(basedOnNgCarrier[i].get());
        }
        optInput->SetOptBasedOnNormgroups(vAlgNg);

        // set algorithmType
        optInput->SetDoseAlgorithmType(algNormgroup->GetDoseAlgorithmType());

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

bool TpsOptProxy::PrepareOptParams_i(
    const std::vector<RtDoseoptgoalbase*>& vGoals,
    const DoseOptimizationParam& param, 
    RTFWK::RTFWKDLDODoseOptInputBase* optInput,
    const std::string& sImageSeriesUid)
{
    try
    {
        // set end criteria
        RTFWK::RTFWKDLDODoseOptEndCriterionByIteration* endCon = new RTFWK::RTFWKDLDODoseOptEndCriterionByIteration(false);
        endCon->SetCriterionType((RTFWK::OPTENDCRITERIONTYPE)param.GetCriterionType());
        endCon->SetIterNum(param.GetIterNum());
        RTFWK::RTFWKDLDODoseOptEndCriterionBase* endConBase = endCon;
        optInput->SetEndCondition(endConBase);

        // set opt goals
        std::vector<RtDoseoptgoalbase*> optGoals = vGoals;
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDODoseOptGoalBase>> optGoalCarrier;
        OptGoalConverter::CreateALGOptGoalList(optGoals, &optGoalCarrier);
        std::vector<RTFWK::RTFWKDLDODoseOptGoalBase*> algOptGoalList;
        for (int i = 0; i < optGoalCarrier.size(); ++i) 
        {
            algOptGoalList.push_back(optGoalCarrier[i].get());
        }
        optInput->SetGoalVec(algOptGoalList);

        // set bid width
        optInput->SetDVHBinWidth(10.0f);

        // set ct2densitytable
        float* pCt2DensityArray = const_cast<float*>(
            mDataRepository->GetImageDataManager()->GetCT2ReferenceEDensityArrayViaImage(sImageSeriesUid));

        if (nullptr == pCt2DensityArray)
        {
            TPS_LOG_DEV_ERROR << "pCt2DensityArray is null when GetCT2ReferenceEDensityArrayViaImage";
            return false;
        }
        optInput->SetCt2DensityTable(pCt2DensityArray);

        // add by wzd 2016-08-24
        DEL_PTR(endCon);

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return false;
    }
}

RTFWK::DOSEOPTALGO TpsOptProxy::GetOptType_i(const RtDbDef::PLAN_TYPE& planType)
{
    RTFWK::DOSEOPTALGO optType = RTFWK::FLUENCE_MAP;
    switch (planType)
    {
        case RTFWK::PLAN_Burst_Arc:
            optType = RTFWK::BURST_MODE_ARC;
            break;
        case RTFWK::PLAN_VMAT_Arc:
            optType = RTFWK::VOLUMETRIC_MODULATED_ARC;
            break;
        case RTFWK::PLAN_Step_Shoot:
            optType = RTFWK::DIRECT_APERTURE;
            break;;
        case RTFWK::PLAN_Sliding_Window:
            optType = RTFWK::FLUENCE_MAP;
            break;
        case RTFWK::PLAN_Simple_Arc:
            TPS_LOG_DEV_ERROR << "TpsPlanOptStartCmd failed with wrong technique type:PLAN_Simple_Arc";
            break;
        case RTFWK::PLAN_3D_CRT:
        case RTFWK::PLAN_Conformal_Arc:
            optType = RTFWK::SEGMENT_WEIGHT;
            break;
        default:
            TPS_LOG_DEV_ERROR << "TpsPlanOptStartCmd failed with wrong technique type:" << (int)optType;
            break;
    }
    return optType;
}


TPS_END_NAMESPACE
