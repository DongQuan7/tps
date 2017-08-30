////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WENLONG ZHAO wenlong.zhao@united-imaging.com
/// 
///  \file tps_bl_cmd_planqa_createnormalgroup.cpp
/// 
///  \brief class Create a new Norm Group(including beams) declaration for PLAN QA 
/// 
///  \version 1.0
/// 
///  \date    JUL 21,2016
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_planqa_createnormalgroup.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"



//TPS
#include "tps_logger.h"
#include "boost/algorithm/string.hpp"
//#include "boost/lexical_cast.hpp"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"


// uid generator
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"

// render proxy
#include "RtTpsRenderController/tps_rc_renderproxy.h"

// proto
#include "RtTpsProtoLib/rt_tps_planqaaddnormalgroupconfigtable.pb.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"

TPS_BEGIN_NAMESPACE

CreateQANormGroupCmd::CreateQANormGroupCmd(const std::string planuid, const proto::RtTpsProtoPlanQAAddNormalGroupConfigTable& protoAddNGConfigTable)
    : mPlanUID(planuid), mProtoAddNGConfigTable(protoAddNGConfigTable), mCreatedNormalizationGroupUID("")
{
}

CreateQANormGroupCmd::~CreateQANormGroupCmd(void) 
{
}

int CreateQANormGroupCmd::Execute()
{
    TPS_LOG_DEV_INFO<<"[INFO][CreateQANormGroupCmd][Execute]: "<<"CreateQANormGroupCmd execute!";
    
    std::string strQANormgroupUID = "";
    std::string strOriginalNormgroupUID = mProtoAddNGConfigTable.treatmentplannormalgroupuid();

    // create Plan QA normalizaiton group and save to DB and local memory.
    if(TPS_ER_SUCCESS != CreateQANormalgroup()){
       return TPS_ER_FAILURE;
    }

    // create Plan QA beams and save to DB and local memory.
    if(TPS_ER_SUCCESS != CreateQABeams()){
        return TPS_ER_FAILURE;
    }

    TPS_LOG_DEV_INFO<<"[INFO][CreateQANormGroupCmd][Execute]: "<<"CreateQANormGroupCmd leave!";
    return TPS_ER_SUCCESS;
}

int CreateQANormGroupCmd::CreateQANormalgroup()
{
    RtNormgroup* pOriginalNormalgroup = nullptr;
    RtNormgroup* pQANormgroup = nullptr;

    // Get original normal group
    TpsNormGroupManager *normManager = mDataRepository->GetNormgroupManager();
    if(normManager == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get normalgroup manager when adding QA Normalgroup.";
        return TPS_ER_FAILURE;
    }
    normManager->GetNormGroup(mProtoAddNGConfigTable.treatmentplannormalgroupuid(), &pOriginalNormalgroup);
    if(nullptr == pOriginalNormalgroup) {
        TPS_LOG_DEV_ERROR<<"Failed to get original normalgroup when adding QA Normalgroup.";
        return TPS_ER_FAILURE;
    }
    RtDbDef::PLAN_TYPE planType = (RtDbDef::PLAN_TYPE)pOriginalNormalgroup->get_plantype();
    if (planType == RtDbDef::PLAN_UNKNOWN_TYPE)
    {
        TPS_LOG_DEV_ERROR << "Failed to get original normalgroup with UID: " << mProtoAddNGConfigTable.treatmentplannormalgroupuid() << " has wrong plan type:"
            << planType;
        return TPS_ER_FAILURE;
    }
    bool bIsArc = TpsCmdHelper::IsArcBeam(planType);
    if((!mProtoAddNGConfigTable.isbeamgantryfromtreatment() || !mProtoAddNGConfigTable.isbeamtablefromtreatment() || !mProtoAddNGConfigTable.isbeamcollimatorfromtreatment())
        && bIsArc)
    {
        TPS_LOG_DEV_ERROR<<"Cannot Collapse QA Beam when it is ArcBeam with original normalgroup UID "<< mProtoAddNGConfigTable.treatmentplannormalgroupuid();
        return TPS_ER_FAILURE;
    }

    // Create new QA Normalgroup based on original normalgroup and save to DB and local memory.
    pQANormgroup = new RtNormgroup(*pOriginalNormalgroup);
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();

    // Modify normal group according to config table.
    pQANormgroup->set_uid(RtUidGenerater::GeneraterUid());
    pQANormgroup->set_planuid(mPlanUID);
    // 1: POI, 2: VOI
    if(mProtoAddNGConfigTable.goaltargettype() == 1) { // POI
        pQANormgroup->set_isnorm2voi(false);
        pQANormgroup->set_normalpointpoiuid(mProtoAddNGConfigTable.targetpoiuid());
    }
    else { // VOI
        pQANormgroup->set_isnorm2voi(true);
        pQANormgroup->set_voiuid(mProtoAddNGConfigTable.targetvoiuid());
        // 1:Average, 2:Dose at xx% volume
        if(mProtoAddNGConfigTable.voidosetype() == 1) { // Average
            pQANormgroup->set_isnorm2voimeandose(true);
        }
        else { //Dose at xx% volume
            pQANormgroup->set_isnorm2voimeandose(false);
            pQANormgroup->set_normvoipercentagevolume(mProtoAddNGConfigTable.doseatvolumepercentage() / 100);
        }
    }
    pQANormgroup->set_name("QA_" + pQANormgroup->get_name());
    pQANormgroup->set_plandate(ptimeLocalTimeInstance.date());
    pQANormgroup->set_plantime(ptimeLocalTimeInstance.time_of_day());
    if(!mProtoAddNGConfigTable.isgoalfromtreatment()) {
        pQANormgroup->set_percentage(mProtoAddNGConfigTable.doselinepercentage() / 100);
    }
    pQANormgroup->set_doseperfraction(mProtoAddNGConfigTable.doseperfraction());
    pQANormgroup->set_fraction(mProtoAddNGConfigTable.fractioncount());
    pQANormgroup->set_isjoined(true);
    pQANormgroup->set_isgating(false);
    pQANormgroup->set_updatetime(ptimeLocalTimeInstance);
    //pQANormgroup->set_operatorid();

    // Copy dose grid from QAPlan and assign it to QANormgroup according to config table.
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
    if(patientManager == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get patientManager manager when adding QA Normalgroup.";
        return TPS_ER_FAILURE;
    }
    RtPlan* pQAPlan = nullptr;
    if(!patientManager->GetPlan(mPlanUID, &pQAPlan)
        || nullptr == pQAPlan) {
        TPS_LOG_DEV_ERROR<<"Failed to get qa plan when adding QA Normalgroup.";
        return TPS_ER_FAILURE;
    }

    if(nullptr == pQAPlan->get_dosegrid()) {
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid from qaPlan [" <<pQAPlan->get_uid()<<"]";
        return TPS_ER_FAILURE;
    }
    tps::RtDosegrid* pQADoseGrid = new tps::RtDosegrid(*(pQAPlan->get_dosegrid()));
    pQADoseGrid->set_uid(RtUidGenerater::GeneraterUid());
    pQADoseGrid->set_planuid(mPlanUID);
    pQADoseGrid->set_normgroupuid(pQANormgroup->get_uid());
    pQADoseGrid->set_beamuid("");
    pQADoseGrid->set_updatetime(ptimeLocalTimeInstance);
    pQADoseGrid->setdirty_all_fields(true);
    (*pQANormgroup->get_dosegrid()) = *pQADoseGrid;

    // Save QANormGroup to DB
    pQANormgroup->setdirty_all_fields(true);
    if(TPS_ER_SUCCESS != mSaveObjects->SaveNormgroup(*pQANormgroup)) {
        TPS_LOG_DEV_ERROR<<"Failed to add QANormGroup ["<<pQANormgroup->get_uid()<<"] to Database.";
        if(nullptr != pQANormgroup) {
            delete pQANormgroup;
            pQANormgroup = nullptr;
        }
        return TPS_ER_FAILURE;
    }

    // Save QANormGroup to local memory
    if(!normManager->AddNormGroup(pQANormgroup)) {
        TPS_LOG_DEV_ERROR<<"Failed to add QANormGroup ["<<pQANormgroup->get_uid()<<"] to normalgroup data manager.";
        if(nullptr != pQANormgroup) {
            delete pQANormgroup;
            pQANormgroup = nullptr;
        }
        return TPS_ER_FAILURE;
    }

    if (!normManager->AddNgDoseCalculateMode(pQANormgroup->get_uid(), RtDbDef::BASE_ON_MU))
    {
        TPS_LOG_DEV_ERROR << "Failed to add dose calculate mode.";
        if (nullptr != pQANormgroup)
        {
            delete pQANormgroup;
            pQANormgroup = nullptr;
        }
        return TPS_ER_FAILURE;
    }
    mCreatedNormalizationGroupUID = pQANormgroup->get_uid();

    // refreash view
    mRenderProxy->AddNormalGroup(mPlanUID, mCreatedNormalizationGroupUID);
    mRenderProxy->RefreshDoseLine(mPlanUID);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int CreateQANormGroupCmd::CreateQABeams() const
{
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    TpsNormGroupManager* normGroupMnager = mDataRepository->GetNormgroupManager();
    RtNormgroup* pQANormgroup = nullptr;
    if(!normGroupMnager->GetNormGroup(mCreatedNormalizationGroupUID, &pQANormgroup)
        || nullptr == pQANormgroup) {
            TPS_LOG_DEV_ERROR<<"Failed to get QANormgroup when creating QABeams.";
            return TPS_ER_FAILURE;
    }

    // Copy beam from original normal group and modify it according to config table and add it to DB and local memory.
    TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
    vector<RtBeam*> beamsInOriginalNG = beamManager->GetBeamsByNormGroup(mProtoAddNGConfigTable.treatmentplannormalgroupuid());
    for(auto beamItr = beamsInOriginalNG.begin(); beamItr != beamsInOriginalNG.end(); ++beamItr) {
        // create new QA beam based on oringianl beam.
        RtBeam* pQABeam = new RtBeam(**beamItr);
        pQABeam->setdirty_all_fields(true);
        pQABeam->set_uid(RtUidGenerater::GeneraterUid());
        pQABeam->set_normgroupuid(mCreatedNormalizationGroupUID);
        // 1: POI, 2:VOI
        // if mProtoAddNGConfigTable.isocentertype() == 1 then the isocenterpoiuid is genereated from voi
        pQABeam->set_isocenterpoiuid(mProtoAddNGConfigTable.isocenterpoiuid());
        pQABeam->set_name("QA_" + pQABeam->get_name());
        pQABeam->set_isjoincalc(true);
        pQABeam->set_isjoinopt(false);
        pQABeam->set_relatedrtimageuid("");
        // SSD should be reset
        pQABeam->set_ssd(0);
        // 适形相关的参数应该全部删除
        pQABeam->set_isautoshape(false);
        pQABeam->set_targetvoiuid("");
        pQABeam->set_avoidvoiuid("");
        // 删除Wedge与Bolus参数
        pQABeam->set_physicalwedgeuid("");
        pQABeam->set_virtualwedgeuid("");
        pQABeam->set_bolusvoiuid("");
        pQABeam->set_bolusthickness(0);

        pQABeam->set_updatetime(ptimeLocalTimeInstance);
        pQABeam->set_beamweight(0); //设置内存中初始值
        pQABeam->disable_field(BEAM_BEAMWEIGHT);//默认添加的beam weight为空

        //update IEC matrix for beam.
        if(TPS_ER_SUCCESS != UpdateIECCSForBeam(pQABeam, 0, 0, 0)){
                if(nullptr != pQABeam) {
                    delete pQABeam;
                    pQABeam = nullptr;
                }
                return TPS_ER_FAILURE;
        }

        TpsIECCoordinateSystem* iecCS = mDataRepository->GetBeamManager()->GetBeamCS(pQABeam->get_uid());
        if(nullptr == iecCS){
            return TPS_ER_FAILURE;
        }

        // modify beam segment according to config table.
        vector<RtBeamsegment*> beamSegmentsInQABeam = pQABeam->get_beamsegments();
        for(auto segItr = beamSegmentsInQABeam.begin(); segItr != beamSegmentsInQABeam.end(); ++segItr) {
            RtBeamsegment* pQABeamSegment = *segItr;
            pQABeamSegment->set_uid(RtUidGenerater::GeneraterUid());
            pQABeamSegment->set_beamuid(pQABeam->get_uid());
            //pQABeamSegment->set_name("QA_" + pQABeamSegment->get_name());
            if(!mProtoAddNGConfigTable.isbeamgantryfromtreatment()) {
                pQABeamSegment->set_startgantryangle(mProtoAddNGConfigTable.gantrystartangle());
            }
            if(!mProtoAddNGConfigTable.isbeamcollimatorfromtreatment()) {
                pQABeamSegment->set_collimatorrotationdirection(RtDbDef::NONE_ROTATION_DIRECTION);//default direction
                pQABeamSegment->set_startcollimatorangle(mProtoAddNGConfigTable.collimatorangle());
                pQABeamSegment->set_endcollimatorangle(mProtoAddNGConfigTable.collimatorangle());
            }
            if(!mProtoAddNGConfigTable.isbeamtablefromtreatment()) {
                pQABeamSegment->set_starttablerotationangle(mProtoAddNGConfigTable.couchangle());
                pQABeamSegment->set_endtablerotationangle(mProtoAddNGConfigTable.couchangle());
            }
            //创建segment的beam_patient矩阵
            Mcsf::Matrix4f matB2P;
            float gantryAngle = pQABeamSegment->get_startgantryangle();
            float collimatorAngle = pQABeamSegment->get_startcollimatorangle();
            float tableAngle = pQABeamSegment->get_starttablerotationangle();
            iecCS->RotateGantry(gantryAngle);
            iecCS->RotateCollimator(collimatorAngle);
            iecCS->RotateTable(tableAngle);

            if (!iecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P)){
                TPS_LOG_DEV_ERROR << "Failed to get B2P matrix for segment[" << pQABeamSegment->get_uid() << "].";
                return false;
            }
            float arrB2P[16];
            matB2P.GetElements(arrB2P);
            pQABeamSegment->set_t_beam_to_pat(arrB2P);
            pQABeamSegment->set_updatetime(ptimeLocalTimeInstance);

            // modify start mlc shape according to config table
            tps::RtMlcshape* pQAStartMlcShape = nullptr;
            pQAStartMlcShape = pQABeamSegment->get_startmlcshape();
            if(nullptr != pQAStartMlcShape) {
                pQAStartMlcShape->set_uid(RtUidGenerater::GeneraterUid());
                pQAStartMlcShape->set_segmentuid(pQABeamSegment->get_uid());
                pQAStartMlcShape->set_updatetime(ptimeLocalTimeInstance);
                pQAStartMlcShape->setdirty_all_fields(true);
            }
            // modify end mlc shape according to config table.
            tps::RtMlcshape* pQAEndMlcShape = nullptr;
            pQAEndMlcShape = pQABeamSegment->get_endmlcshape();
            if(nullptr != pQAEndMlcShape) {
                pQAEndMlcShape->set_uid(RtUidGenerater::GeneraterUid());
                pQAEndMlcShape->set_segmentuid(pQABeamSegment->get_uid());
                pQAEndMlcShape->set_updatetime(ptimeLocalTimeInstance);
                pQAEndMlcShape->setdirty_all_fields(true);
            }
            pQABeamSegment->setdirty_all_fields(true);
        }

        // Modify beam aperture block according to config table.
        RtBlock* beamApertureBlockInQABeam = pQABeam->get_aperture_block();
        if(nullptr != beamApertureBlockInQABeam) {
            beamApertureBlockInQABeam->set_uid(RtUidGenerater::GeneraterUid());
            beamApertureBlockInQABeam->set_blockname("QA_" + beamApertureBlockInQABeam->get_blockname());
            beamApertureBlockInQABeam->set_beamuid(pQABeam->get_uid());
            beamApertureBlockInQABeam->set_targetvoiuid("");
            beamApertureBlockInQABeam->set_avoidvoiuid("");
            beamApertureBlockInQABeam->set_updatetime(ptimeLocalTimeInstance);
            beamApertureBlockInQABeam->setdirty_all_fields(true);
        }

        // Modify beam shield block according to config table.
        RtBlock* beamShieldBlockInQABeam = pQABeam->get_shield_block();
        if(nullptr != beamShieldBlockInQABeam) {
            beamShieldBlockInQABeam->set_uid(RtUidGenerater::GeneraterUid());
            beamShieldBlockInQABeam->set_blockname("QA_" + beamShieldBlockInQABeam->get_blockname());
            beamShieldBlockInQABeam->set_beamuid(pQABeam->get_uid());
            beamShieldBlockInQABeam->set_targetvoiuid("");
            beamShieldBlockInQABeam->set_avoidvoiuid("");
            beamShieldBlockInQABeam->set_updatetime(ptimeLocalTimeInstance);
            beamShieldBlockInQABeam->setdirty_all_fields(true);
        }

        // Copy dose grid from QANormgroup and assign it to QABeam according config table.
        if(nullptr == pQANormgroup->get_dosegrid()) {
            TPS_LOG_DEV_ERROR<<"Failed to get dose grid from normal group [" <<pQANormgroup->get_uid()<<"]";
            return TPS_ER_FAILURE;
        }
        RtDosegrid* beamDoseGridInQABeam = new RtDosegrid(*(pQANormgroup->get_dosegrid()));
        beamDoseGridInQABeam->set_uid(RtUidGenerater::GeneraterUid());
        beamDoseGridInQABeam->set_beamuid(pQABeam->get_uid());
        beamDoseGridInQABeam->set_normgroupuid("");
        beamDoseGridInQABeam->set_planuid("");
        beamDoseGridInQABeam->set_updatetime(ptimeLocalTimeInstance);
        beamDoseGridInQABeam->setdirty_all_fields(true);
        *(pQABeam->get_dosegrid()) = *beamDoseGridInQABeam;

        //creat fluence map
        FluenceMapEntity* fluenceMap = new FluenceMapEntity();
        if (!mDoseProxy->CalculateFluenceMap(pQABeam, fluenceMap))
        {
            TPS_LOG_DEV_ERROR << "Failed to create FluenceMap for beam[" << pQABeam->get_uid() << "].";
            DEL_PTR(fluenceMap);
            DEL_PTR(pQABeam);
            return TPS_ER_FAILURE;
        }
        mDataRepository->GetBeamManager()->AddFluenceMap(pQABeam->get_uid(), fluenceMap);

        // Save QABeam to DB
        pQABeam->setdirty_all_fields(true);
        if(TPS_ER_SUCCESS != mSaveObjects->InsertBeam(*pQABeam)) {
            TPS_LOG_DEV_ERROR<<"Failed to add QABeam ["<<pQABeam->get_uid()<<"] to Database.";
            if(nullptr != pQABeam) {
                delete pQABeam;
                pQABeam = nullptr;
            }
            return TPS_ER_FAILURE;
        }

        // Save QABeam to local memory
        if(!beamManager->AddBeam2Map(pQABeam)) {
            TPS_LOG_DEV_ERROR<<"Failed to add QABeam ["<<pQABeam->get_uid()<<"] to beam data manager.";
            if(nullptr != pQABeam) {
                delete pQABeam;
                pQABeam = nullptr;
            }
            return TPS_ER_FAILURE;
        }

        // refresh view
        if (!mRenderProxy->AddBeam(mCreatedNormalizationGroupUID, pQABeam->get_uid())){
            TPS_LOG_DEV_ERROR<<"Failed to add beam in render proxy.";
            return TPS_ER_FAILURE;
        }
    }
    mRenderProxy->RefreshDoseLine(mPlanUID);
    mRenderProxy->ResetBeamViewCameras(mPlanUID);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

// create a IEC Coordinate system for a specified beam and add into beam data manager.
TpsIECCoordinateSystem* CreateQANormGroupCmd::CreateIECCS_i(
    const std::string& imageUID, float sad, 
    const std::string& beamUID, float gantryAngle, float tableAngle, 
    float collimatorAngle, const Mcsf::Point3f& isoCenter) const
{
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &image)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data which UID: "<<imageUID;
        return nullptr;
    }
    Mcsf::Point3f orgPt = *image->GetSetupPoint();

    RtImage3DHeader* header;
    if (!image->GetImage3DHeader(header)){
        TPS_LOG_DEV_ERROR<<"Failed to get current image 3d header.";
        return nullptr;
    }
    RtSeries* pSeries = image->GetRtSeries();
    if (nullptr == pSeries){
        TPS_LOG_DEV_ERROR<<"pSeries is null";
        return nullptr;
    }
    PatientPosition pp = TpsCmdHelper::GetPatientPosition(pSeries->get_patientposition());

    TpsIECCoordinateSystem* iecCS = new TpsIECCoordinateSystem();
    if (!iecCS->Initialize(orgPt, pp, sad, gantryAngle, collimatorAngle, tableAngle)){
        TPS_LOG_DEV_ERROR<<"Failed to initialize IEC coordinate system.";
        delete iecCS;
        return nullptr;
    }
    iecCS->SetISOCenter(isoCenter);

    mDataRepository->GetBeamManager()->AddBeamCS(beamUID, iecCS);
    return iecCS;
}

// get created QANormalizationGroup UID.
std::string CreateQANormGroupCmd::GetCreatedNormGroupUID()
{
    return mCreatedNormalizationGroupUID;
}

// Prepare parameters and create IEC Coordinate system for QA beam and add into beam data manager.
int CreateQANormGroupCmd::UpdateIECCSForBeam(RtBeam* beam, float gantryAngle, float collimatorAngle, float couchAngle) const
{
    RtNormgroup* normgroup = nullptr;
    if(!mDataRepository->GetNormgroupManager()->GetNormGroup(mCreatedNormalizationGroupUID, &normgroup)
        || nullptr == normgroup) {
        TPS_LOG_DEV_ERROR<<"Failed to get normgroup.";
        return TPS_ER_FAILURE;
    }

    RtPlan* plan = nullptr;
    if(!mDataRepository->GetPatientManager()->GetPlan(normgroup->get_planuid(), &plan)
        || nullptr == plan) {
        TPS_LOG_DEV_ERROR<<"Failed to get normgroup.";
        return TPS_ER_FAILURE;
    }

    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(normgroup->get_machineuid(), machine)
        || nullptr == machine){
        TPS_LOG_DEV_ERROR<<"Failed to get machine.";
        return TPS_ER_FAILURE;
    }
    auto isoCenter = mDataRepository->GetPoiManager()->GetPOI(beam->get_isocenterpoiuid());
    if (isoCenter == nullptr){
        TPS_LOG_DEV_ERROR<<"The iso center does not exist of uid: "<<beam->get_isocenterpoiuid();
        return TPS_ER_FAILURE;
    }
    Mcsf::Point3f isoCenter3f(isoCenter->get_coordinate3dx(), isoCenter->get_coordinate3dy(),
        isoCenter->get_coordinate3dz());
    if(0 == beam->get_beamsegments().size() || nullptr == beam->get_beamsegments()[0]) {
        TPS_LOG_DEV_ERROR<<"The new created(Copied) QABeam dose not have any beam segments ";
        return TPS_ER_FAILURE;
    }
    //auto firstBeamSegment = beam->get_beamsegments()[0];

    // create IEC Coordinate system for beam and add into beam data manager.
    TpsIECCoordinateSystem* iecCS = CreateIECCS_i(plan->get_imageseriesuid(), machine->get_sad(), 
        beam->get_uid(), gantryAngle, 
        couchAngle, collimatorAngle, isoCenter3f);
    if (!iecCS){
        TPS_LOG_DEV_ERROR<<"Failed to create IEC coordinate system.";
        if(nullptr != iecCS) {
            delete iecCS;
            iecCS = nullptr;
        }
        return TPS_ER_FAILURE;
    }

    Mcsf::Matrix4f matB2P;
    if (!iecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P)){
        TPS_LOG_DEV_ERROR<<"Failed to get beam to patient matrix.";
        if(nullptr != iecCS) {
            delete iecCS;
            iecCS = nullptr;
        }
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE
