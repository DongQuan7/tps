//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_bl_cmd_loadplan.cpp
///  \brief   load plan
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_loadplan.h"

#include "RtTpsBusinessLogic/tps_bl_load_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess\tps_da_voimanager.h"
#include "RtTpsDataAccess\tps_da_dosemanager.h"
#include "RtTpsDataAccess\tps_da_optgoalmanager.h"
#include "RtTpsDataAccess\tps_da_dvhmanager.h"
#include "RtTpsDataAccess\tps_da_poimanager.h"
#include "RtTpsDataAccess\tps_da_imagedatamanager.h"

#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_dvh.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"

#include "tps_logger.h"
#include "tps_performance.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

TPS_BEGIN_NAMESPACE

LoadPlanCommand::LoadPlanCommand(const std::string sPlanUID)
: msPlanUID(sPlanUID)
{

}

LoadPlanCommand::~LoadPlanCommand()
{

}

int LoadPlanCommand::Execute()
{
    TPS_LOG_DEV_INFO<<"LoadSeriesCommand::Execute";

    TEST_PERFORMANCE_INIT
    TEST_PERFORMANCE_BEGIN

    std::vector<std::string> planUIDList(1, msPlanUID);
    int errorCode = mLoadObjects->LoadPlans(planUIDList);
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializePlan();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeNormgroup();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeBeam();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeDoseline();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeDoseoptgoal();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeDvh();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeMaxPoint();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializePoi();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeDrrImage();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    TEST_PERFORMANCE_END("LoadSeriesCommand::Execute")

    return TPS_ER_SUCCESS;
}

void LoadPlanCommand::GetResult(RtPlan*& pRtPlan, std::vector<RtNormgroup*>& vRtNormgroupList,
    std::vector<RtBeam*>& vRtBeamList,
    std::vector<RtDoseline*>& vRtDoselineList,
    std::vector<RtDoseoptgoalbase*>& vRtDoseoptgoalbaseList,
    std::vector<RtDvh*>& vRtDvhList,
    std::vector<RtPoi*>& vRtPoiList,
    std::vector<RtPoi*>& vRtPoiMaxList,
    std::vector<RtRtimage*>& vRtImageList)

{
    pRtPlan = mpRtPlan;
    vRtNormgroupList = mvRtNormgroupList;
    vRtBeamList = mvRtBeamList;
    vRtDoselineList = mvRtDoselineList;
    vRtDoseoptgoalbaseList = mvRtDoseoptgoalbaseList;
    vRtDvhList = mvRtDvhList;
    vRtPoiList = mvRtPoiList;
    vRtPoiMaxList = mvRtPoiMaxList;
    vRtImageList = mvRtImageList;
}

int LoadPlanCommand::InitializePlan()
{
    if (!mDataRepository->GetPatientManager()->GetPlan(msPlanUID, &mpRtPlan)
        || mpRtPlan == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeNormgroup()
{
    if (mpRtPlan == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    mvRtNormgroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(mpRtPlan->get_uid());

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeBeam()
{
    std::vector<RtBeam*> beams;
    for (auto it = mvRtNormgroupList.begin(); it != mvRtNormgroupList.end(); it++)
    {
        beams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup((*it)->get_uid());
        for (auto itBeam = beams.begin(); itBeam != beams.end(); itBeam++)
        {
            mvRtBeamList.push_back(*itBeam);
        }
    }

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeDoseline()
{
    if (mpRtPlan == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    mDataRepository->GetDoseManager()->GetDoseLineList(mpRtPlan->get_uid(), mvRtDoselineList);

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeDoseoptgoal()
{
    for (auto it = mvRtNormgroupList.begin(); it != mvRtNormgroupList.end(); it++)
    {
        std::vector<RtDoseoptgoalbase*> vRtDoseoptgoalbaseList;
        if (mDataRepository->GetOptGoalManager()->GetOptGoalListByNormalizationGroupUID((*it)->get_uid(), vRtDoseoptgoalbaseList))
        {
            mvRtDoseoptgoalbaseList.insert(mvRtDoseoptgoalbaseList.end(), vRtDoseoptgoalbaseList.begin(), vRtDoseoptgoalbaseList.end());
        }
    }

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeDvh()
{
    if (mpRtPlan == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    mvRtDvhList = mDataRepository->GetDVHManager()->GetPlanDvhs(mpRtPlan->get_uid());

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializePoi()
{
    mvRtPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(mpRtPlan->get_imageseriesuid());

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeMaxPoint()
{
    RtPoi* poi = mDataRepository->GetPoiManager()->GetMaxPoint(msPlanUID);
    if (poi != nullptr)
    {
        mvRtPoiMaxList.push_back(poi);
    }

    return TPS_ER_SUCCESS;
}

int LoadPlanCommand::InitializeDrrImage()
{
    if (mpRtPlan == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    mDataRepository->GetImageDataManager()->GetRtRtimageListByPlanUID(mpRtPlan->get_uid(), mvRtImageList);
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
