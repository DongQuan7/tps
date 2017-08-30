////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Zhendong (mailto:zhendong.wang@united-imaging.com)
/// 
///  \file tps_bl_cmd_opposite_beam.cpp
/// 
///  \brief class  Create Opposite beam
/// 
///  \version 1.0
/// 
///  \date    June.16,2017
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_opposite_beam.h"
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
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"


#include "boost/algorithm/string.hpp"



TPS_BEGIN_NAMESPACE;

OppositeBeamCmd::OppositeBeamCmd(const RtBeam& srcBeam) : mSrcBeam(srcBeam)
{

}

OppositeBeamCmd::~OppositeBeamCmd(void)
{

}

void OppositeBeamCmd::GetOppositeBeam(const RtBeam*& pBeam)
{
    pBeam = mOppositeBeam;
}

int OppositeBeamCmd::Execute()
{
    std::string srcBeamUid = mSrcBeam.get_uid();

    if (mSrcBeam.get_aperturetype() == RtDbDef::Aperture_Block)
    {
        TPS_LOG_DEV_ERROR << "Opposite beam do not support aperture block[" << srcBeamUid << "]";
        return TPS_ER_FAILURE;
    }

    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mSrcBeam.get_uid());

    RtNormgroup* normGroup = nullptr;
    std::string ngUid = mSrcBeam.get_normgroupuid();

    mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &normGroup);
    if (normGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normalgroup[" << ngUid << "]";
        return TPS_ER_FAILURE;
    }

    if (normGroup->get_plantype() != RtDbDef::PLAN_3D_CRT && normGroup->get_plantype() != RtDbDef::PLAN_2D)
    {
        TPS_LOG_DEV_ERROR << "Only 3d_crt beam or 2d beam can opposite" ;
        return TPS_ER_FAILURE;
    }

    RtPlan* plan = nullptr;
    std::string planUid = normGroup->get_planuid();
    mDataRepository->GetPatientManager()->GetPlan(normGroup->get_planuid(), &plan);
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

    std::string isoCenterUID = mSrcBeam.get_isocenterpoiuid();
    if (isoCenterUID.empty()) isoCenterUID = beam->get_isocenterpoiuid();
    RtPoi* isoCenter = mDataRepository->GetPoiManager()->GetPOI(isoCenterUID);
    if (isoCenter == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist of uid: " << isoCenterUID;
        return TPS_ER_FAILURE;
    }

    Mcsf::Point3f isoCenter3f(isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(), isoCenter->get_coordinate3dz());


    const boost::posix_time::ptime ptimeLocalTimeInstance = boost::posix_time::microsec_clock::local_time();
    mOppositeBeam = new RtBeam(*beam);
    mOppositeBeam->setdirty_all_fields(true);
    mOppositeBeam->set_uid(RtUidGenerater::GeneraterUid());
    mOppositeBeam->set_beamnumber(mSrcBeam.get_beamnumber());
    mOppositeBeam->set_name(mSrcBeam.get_name());
    mOppositeBeam->set_updatetime(ptimeLocalTimeInstance);
    mOppositeBeam->set_normgroupuid(ngUid);
    mOppositeBeam->set_beamweight(mSrcBeam.get_beamweight());

    vector<RtBeamsegment*> segmentsInOppositeBeam = mOppositeBeam->get_beamsegments();
    for (auto segItr = segmentsInOppositeBeam.begin(); segItr != segmentsInOppositeBeam.end(); ++segItr)
    {
        double temp;
        RtBeamsegment* pSegment = *segItr;
        pSegment->setdirty_all_fields(true);
        pSegment->set_uid(RtUidGenerater::GeneraterUid());
        pSegment->set_beamuid(mOppositeBeam->get_uid());
        pSegment->set_updatetime(ptimeLocalTimeInstance);
        // modify gantry and collimator angle
        double srcGantryAngle = pSegment->get_startgantryangle();
        if (srcGantryAngle + 180 < 360)
        {
            pSegment->set_startgantryangle(srcGantryAngle + 180);
        }
        else
        {
             pSegment->set_startgantryangle(srcGantryAngle - 180);
        }
        double srcCollimatorAngle = pSegment->get_startcollimatorangle();
        if (srcCollimatorAngle == 0)
        {
            pSegment->set_startcollimatorangle(0);
        }
        else
        {
            pSegment->set_startcollimatorangle(360 - srcCollimatorAngle);
        }

        // modify iec
        TpsIECCoordinateSystem* iec = mDataRepository->GetBeamManager()->GetBeamCS(srcBeamUid);
        if (nullptr == iec)
        {
            TPS_LOG_DEV_ERROR << "The beam with UID: " << srcBeamUid << " has no IEC C.S. object.";
            return TPS_ER_FAILURE;
        }

        iec->RotateGantry(pSegment->get_startgantryangle());
        iec->RotateCollimator(pSegment->get_startcollimatorangle());
        iec->RotateTable(pSegment->get_starttablerotationangle());

        Mcsf::Matrix4f matB2P;
        if (!iec->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P))
        {
            TPS_LOG_DEV_ERROR << "Failed to get beam to patient matrix.";
            return TPS_ER_FAILURE;
        }
        float mat[16] ={0.f};
        matB2P.GetElements(mat);
        pSegment->set_t_beam_to_pat(mat);

        // flip the start mlc shape
        tps::RtMlcshape* pStartMlcShape = nullptr;
        pStartMlcShape = pSegment->get_startmlcshape();
        if (nullptr != pStartMlcShape)
        {
            pStartMlcShape->setdirty_all_fields(true);
            pStartMlcShape->set_uid(RtUidGenerater::GeneraterUid());
            pStartMlcShape->set_segmentuid(pSegment->get_uid());
            pStartMlcShape->set_updatetime(ptimeLocalTimeInstance);

            std::vector<db_Point2d> startMlcLeafPositions = pStartMlcShape->get_leafpositions();
            for (int i = 0; i < startMlcLeafPositions.size(); i++)
            {
                temp = startMlcLeafPositions[i].x;
                startMlcLeafPositions[i].x = -startMlcLeafPositions[i].y;
                startMlcLeafPositions[i].y = -temp;
            }
            pStartMlcShape->set_leafpositions(startMlcLeafPositions);
            temp = pStartMlcShape->get_xlowerjawpos();
            pStartMlcShape->set_xlowerjawpos(-pStartMlcShape->get_xupperjawpos());
            pStartMlcShape->set_xupperjawpos(-temp);

        }

        // flip the end mlc shape
        tps::RtMlcshape* pEndMlcShape = nullptr;
        pEndMlcShape = pSegment->get_endmlcshape();
        if (nullptr != pEndMlcShape)
        {
            pEndMlcShape->setdirty_all_fields(true);
            pEndMlcShape->set_uid(RtUidGenerater::GeneraterUid());
            pEndMlcShape->set_segmentuid(mOppositeBeam->get_uid());
            pEndMlcShape->set_updatetime(ptimeLocalTimeInstance);

            std::vector<db_Point2d> endMlcLeafPositions = pEndMlcShape->get_leafpositions();
            for (int i = 0; i < endMlcLeafPositions.size(); i++)
            {
                int temp = endMlcLeafPositions[i].x;
                endMlcLeafPositions[i].x = -endMlcLeafPositions[i].y;
                endMlcLeafPositions[i].y = -temp;
            }
            pEndMlcShape->set_leafpositions(endMlcLeafPositions);
            temp = pEndMlcShape->get_xlowerjawpos();
            pEndMlcShape->set_xlowerjawpos(-pEndMlcShape->get_xupperjawpos());
            pEndMlcShape->set_xupperjawpos(-temp);

        }

        if (segItr == segmentsInOppositeBeam.begin())
        {
            TpsIECCoordinateSystem* iecCS = CreateIECCS_i(imageUid,
                sad, mOppositeBeam->get_uid(),
                pSegment->get_startgantryangle(),
                pSegment->get_starttablerotationangle(),
                pSegment->get_startcollimatorangle(),
                isoCenter3f);

            if (iecCS == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to create IEC coordinate system.";
                TPS_DEL_PTR(iecCS);
                TPS_DEL_PTR(mOppositeBeam);
                return TPS_ER_FAILURE;
            }
        }
    }

    // modify the shield block
    RtBlock* beamShieldBlock = mOppositeBeam->get_shield_block();
    if (nullptr != beamShieldBlock)
    {
        beamShieldBlock->setdirty_all_fields(true);
        beamShieldBlock->set_uid(RtUidGenerater::GeneraterUid());
        beamShieldBlock->set_beamuid(mOppositeBeam->get_uid());
        beamShieldBlock->set_updatetime(ptimeLocalTimeInstance);
    }

    // modify the dose grid
    RtDosegrid* beamDoseGrid = mOppositeBeam->get_dosegrid();
    if (nullptr != beamDoseGrid)
    {
        beamDoseGrid->setdirty_all_fields(true);
        beamDoseGrid->set_uid(RtUidGenerater::GeneraterUid());
        beamDoseGrid->set_beamuid(mOppositeBeam->get_uid());
        beamDoseGrid->set_updatetime(ptimeLocalTimeInstance);
    }

    // creat the FluenceMap
    FluenceMapEntity* fluenceMap = new FluenceMapEntity();
    if (!mDoseProxy->CalculateFluenceMap(beam, fluenceMap))
    {
        TPS_LOG_DEV_ERROR << "Failed to calculate fluence map.";
        DEL_PTR(fluenceMap);
        DEL_PTR(mOppositeBeam);
        return TPS_ER_FAILURE;
    }
    mDataRepository->GetBeamManager()->AddFluenceMap(beam->get_uid(), fluenceMap);

    // save beam to DB
    if (TPS_ER_SUCCESS != mSaveObjects->InsertBeam(*mOppositeBeam))
    {
        TPS_LOG_DEV_ERROR << "InsertBeam failed! uid:" << mOppositeBeam->get_uid();
        TPS_PRINTF_DEBUG("InsertBeam failed uid:%s\n", mOppositeBeam->get_uid().c_str());
        TPS_DEL_PTR(mOppositeBeam);
        return TPS_ER_DB_ERROR;
    }
    // save beam to local memory
    if (!mDataRepository->GetBeamManager()->AddBeam2Map(mOppositeBeam))
    {
        TPS_LOG_DEV_ERROR << "AddBeam2Map Failed with beam UID:" << mOppositeBeam->get_uid();
        TPS_DEL_PTR(mOppositeBeam);
        return TPS_ER_FAILURE;
    }

    mRenderProxy->SetDataSource(SOURCE_GROUP_COMMON, IMAGE, imageUid);
    mRenderProxy->SetDataSource(SOURCE_GROUP_COMMON, PLAN, planUid);

    // refresh view
    if (!mRenderProxy->AddBeam(ngUid, mOppositeBeam->get_uid()))
    {
        TPS_LOG_DEV_ERROR << "Failed to add beam in render proxy.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->ResetBeamViewCameras(planUid);
    mRenderProxy->Render();


    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE
