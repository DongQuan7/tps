////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  XU QAING (mailto:qiang.xu@united-imaging.com)
/// 
///  \file tps_bl_cmd_duplicate_beam.h
/// 
///  \brief class  Create duplicate beam with segment and block
/// 
///  \version 1.0
/// 
///  \date    Dec.01,2016
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_duplicate_beam.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "boost/algorithm/string.hpp"



TPS_BEGIN_NAMESPACE;

DuplicateBeamCmd::DuplicateBeamCmd(const RtBeam& rtBeam) :mRtBeam(rtBeam)
{

}

DuplicateBeamCmd::~DuplicateBeamCmd(void)
{

}

void DuplicateBeamCmd::GetDuplicateBeam(const RtBeam*& pBeam)
{
    if (!newBeamUid.empty())
    {
        RtBeam* newBeam = mDataRepository->GetBeamManager()->GetBeam(newBeamUid);
        pBeam = newBeam;
    }
}

int DuplicateBeamCmd::Execute()
{
    TPS_LOG_DEV_INFO << "[INFO][DuplicateBeamCmd][Execute]: " << "DuplicateBeamCmd execute!";
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    const boost::posix_time::ptime ptimeLocalTimeInstance = boost::posix_time::microsec_clock::local_time();
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mRtBeam.get_uid());

    RtNormgroup* normGroup = nullptr;
    std::string ngUid = mRtBeam.get_normgroupuid();
    mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &normGroup);
    if (normGroup == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    std::string planUid = normGroup->get_planuid();
    RtPlan* plan = nullptr;
    mDataRepository->GetPatientManager()->GetPlan(planUid, &plan);
    if (plan == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    std::string imageUid = plan->get_imageseriesuid();
    RtMachine* machine = nullptr;
    std::string machineUid = normGroup->get_machineuid();
    if (!mDataRepository->GetMachineManager()->GetMachine(machineUid, machine))
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine[" << machineUid << "]";
        return TPS_ER_FAILURE;
    }
    if (machine == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    float sad = machine->get_sad();

    std::string isoCenterUID = mRtBeam.get_isocenterpoiuid();
    if (isoCenterUID.empty()) isoCenterUID = beam->get_isocenterpoiuid();
    RtPoi* isoCenter = mDataRepository->GetPoiManager()->GetPOI(isoCenterUID);
    if (isoCenter == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist of uid: " << isoCenterUID;
        return TPS_ER_FAILURE;
    }

    Mcsf::Point3f isoCenter3f(isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(), isoCenter->get_coordinate3dz());

    RtBeam* pDuplicateBeam = new RtBeam(*beam);
    pDuplicateBeam->setdirty_all_fields(true);
    pDuplicateBeam->set_isocenterpoiuid(isoCenterUID);
    pDuplicateBeam->set_uid(RtUidGenerater::GeneraterUid());
    pDuplicateBeam->set_beamnumber(mRtBeam.get_beamnumber());
    pDuplicateBeam->set_name(mRtBeam.get_name());
    pDuplicateBeam->set_updatetime(ptimeLocalTimeInstance);
    pDuplicateBeam->set_normgroupuid(ngUid);
    pDuplicateBeam->set_bolusvoiuid(beam->get_bolusvoiuid());
    pDuplicateBeam->set_beamweight(mRtBeam.get_beamweight());

    // modify beam aperture block
    RtBlock* beamApertureBolck = pDuplicateBeam->get_aperture_block();
    if (nullptr != beamApertureBolck)
    {
        beamApertureBolck->setdirty_all_fields(true);
        beamApertureBolck->set_uid(RtUidGenerater::GeneraterUid());
        beamApertureBolck->set_beamuid(pDuplicateBeam->get_uid());
        beamApertureBolck->set_updatetime(ptimeLocalTimeInstance);
    }
    // modify the shield block
    RtBlock* beamShieldBlock = pDuplicateBeam->get_shield_block();
    if (nullptr != beamShieldBlock)
    {
        beamShieldBlock->setdirty_all_fields(true);
        beamShieldBlock->set_uid(RtUidGenerater::GeneraterUid());
        beamShieldBlock->set_beamuid(pDuplicateBeam->get_uid());
        beamShieldBlock->set_updatetime(ptimeLocalTimeInstance);
    }
    // modify the dose grid
    RtDosegrid* beamDoseGrid = pDuplicateBeam->get_dosegrid();
    if (nullptr != beamDoseGrid)
    {
        beamDoseGrid->setdirty_all_fields(true);
        beamDoseGrid->set_uid(RtUidGenerater::GeneraterUid());
        beamDoseGrid->set_beamuid(pDuplicateBeam->get_uid());
        beamDoseGrid->set_updatetime(ptimeLocalTimeInstance);
    }

    vector<RtBeamsegment*> segmentsInDuplicateBeam = pDuplicateBeam->get_beamsegments();
    for (auto segItr = segmentsInDuplicateBeam.begin(); segItr != segmentsInDuplicateBeam.end(); ++segItr)
    {
        RtBeamsegment* pbeamSegment = *segItr;
        pbeamSegment->setdirty_all_fields(true);
        pbeamSegment->set_uid(RtUidGenerater::GeneraterUid());
        pbeamSegment->set_beamuid(pDuplicateBeam->get_uid());
        pbeamSegment->set_updatetime(ptimeLocalTimeInstance);
        // modify the start mlc shape
        tps::RtMlcshape* pStartMlcShape = nullptr;
        pStartMlcShape = pbeamSegment->get_startmlcshape();
        if (nullptr != pStartMlcShape)
        {
            pStartMlcShape->setdirty_all_fields(true);
            pStartMlcShape->set_uid(RtUidGenerater::GeneraterUid());
            pStartMlcShape->set_segmentuid(pbeamSegment->get_uid());
            pStartMlcShape->set_updatetime(ptimeLocalTimeInstance);
        }
        // modify the end mlc shape
        tps::RtMlcshape* pEndMlcShape = nullptr;
        pEndMlcShape = pbeamSegment->get_endmlcshape();
        if (nullptr != pEndMlcShape)
        {
            pEndMlcShape->setdirty_all_fields(true);
            pEndMlcShape->set_uid(RtUidGenerater::GeneraterUid());
            pEndMlcShape->set_segmentuid(pDuplicateBeam->get_uid());
            pEndMlcShape->set_updatetime(ptimeLocalTimeInstance);
        }

        if (segItr == segmentsInDuplicateBeam.begin())
        {
            TpsIECCoordinateSystem* iecCS = CreateIECCS_i(imageUid,
                sad, pDuplicateBeam->get_uid(),
                pbeamSegment->get_startgantryangle(),
                pbeamSegment->get_starttablerotationangle(),
                pbeamSegment->get_startcollimatorangle(),
                isoCenter3f);

            if (iecCS == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to create IEC coordinate system.";
                TPS_DEL_PTR(iecCS);
                TPS_DEL_PTR(pDuplicateBeam);
                return TPS_ER_FAILURE;
            }
        }
    }

    // modify the FluenceMap
    FluenceMapEntity* fluenceMap = mDataRepository->GetBeamManager()->GetFluenceMap(beam->get_uid());
    if (fluenceMap != nullptr)
    {
        FluenceMapEntity* duplicateFluenceMap = new FluenceMapEntity(*fluenceMap);
        mDataRepository->GetBeamManager()->AddFluenceMap(pDuplicateBeam->get_uid(), duplicateFluenceMap);
    }

    // save beam to DB
    if (TPS_ER_SUCCESS != mSaveObjects->InsertBeam(*pDuplicateBeam))
    {
        TPS_LOG_DEV_ERROR << "InsertBeam failed! uid:" << pDuplicateBeam->get_uid();
        TPS_PRINTF_DEBUG("InsertBeam failed uid:%s\n", pDuplicateBeam->get_uid().c_str());
        TPS_DEL_PTR(pDuplicateBeam);
        return TPS_ER_DB_ERROR;
    }
    // save beam to local memory
    if (!mDataRepository->GetBeamManager()->AddBeam2Map(pDuplicateBeam))
    {
        TPS_LOG_DEV_ERROR << "AddBeam2Map Failed with beam UID:" << pDuplicateBeam->get_uid();
        TPS_DEL_PTR(pDuplicateBeam);
        return TPS_ER_FAILURE;
    }
    mRenderProxy->SetDataSource(SOURCE_GROUP_COMMON, IMAGE, imageUid);
    mRenderProxy->SetDataSource(SOURCE_GROUP_COMMON, PLAN, planUid);

    // refresh view
    if (!mRenderProxy->AddBeam(ngUid, pDuplicateBeam->get_uid()))
    {
        TPS_LOG_DEV_ERROR << "Failed to add beam in render proxy.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->ResetBeamViewCameras(planUid);
    mRenderProxy->Render();
    newBeamUid = pDuplicateBeam->get_uid();
    TEST_PERFORMANCE_END("DuplicateBeam");
    return TPS_ER_SUCCESS;
}



TPS_END_NAMESPACE
