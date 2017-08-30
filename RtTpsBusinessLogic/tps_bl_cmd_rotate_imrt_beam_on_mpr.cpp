////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_rotate_beam_on_mpr.cpp
/// 
///  \brief class RotateImrtBeamOnMprCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_rotate_imrt_beam_on_mpr.h"
#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include <sstream>
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

RotateImrtBeamOnMprCmd::RotateImrtBeamOnMprCmd(
    WINDOW_TYPE wndType, const std::string& wndUid, 
    double currX, double currY, int mouseActionType, 
    const std::string& beamUid, const std::string& segmentUid)
    : mWndType(wndType), mWndUid(wndUid)
    , mCurrX(currX), mCurrY(currY)
    , mMouseActionType(mouseActionType), mBeamUid(beamUid)
    , mSegmentUid(segmentUid), mResponser(nullptr){

}

RotateImrtBeamOnMprCmd::~RotateImrtBeamOnMprCmd(){

}

int RotateImrtBeamOnMprCmd::Execute()
{
    //get editing beam
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"The beam does not exist.";
        return TPS_ER_FAILURE;
    }
    int beamType = beam->get_beamtype();
    if (beamType != RtDbDef::STATICIMRT && beamType != RtDbDef::DYNAMICIMRT){
        TPS_LOG_DEV_ERROR<<"The editing beam is not imrt beam.";
        return TPS_ER_FAILURE;
    }

    TpsIECCoordinateSystem* iecCs = mDataRepository->GetBeamManager()->GetBeamCS(mBeamUid);
    if (iecCs == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get beam iec coordinate system object.";
        return TPS_ER_FAILURE;
    }
    //get editing segment
    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty()){
        TPS_LOG_DEV_ERROR<<"There is no segment in the beam.";
        return TPS_ER_FAILURE;
    }
    //get the target segment
    RtBeamsegment* segment = nullptr;
    for (int i = 0; i < segments.size(); ++i){
        if (mSegmentUid == segments[i]->get_uid()) {
            segment = segments[i];
            break;
        }
    }
    if (segment == nullptr){
        TPS_LOG_DEV_ERROR<<"Can not find the editing segment.";
        return TPS_ER_FAILURE;
    }
    //get plan uid
    std::string planUid, ngUid, imageUid;
    ngUid = beam->get_normgroupuid();
    RtNormgroup* ng = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &ng)){
        TPS_LOG_DEV_ERROR<<"Failed to get normal group.";
        return TPS_ER_FAILURE;
    }
    planUid = ng->get_planuid();
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan) || plan == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get plan.";
        return TPS_ER_FAILURE;
    }
    imageUid = plan->get_imageseriesuid();
    //get machine
    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(ng->get_machineuid(), machine)){
        TPS_LOG_DEV_ERROR<<"Failed to get machine.";
        return TPS_ER_FAILURE;
    }

    if (mMouseActionType == DRAG){
        int iRet = RotateBeam_i(beam, segment, iecCs, machine);
        if (iRet != TPS_ER_SUCCESS) return iRet;
        //update graphic objects
        mRenderProxy->UpdateBeam(ngUid, mBeamUid);
        mRenderProxy->ResetBeamViewCameras(planUid);
        //render
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    else if (mMouseActionType == UP){
        int iRet = RotateBeam_i(beam, segment, iecCs, machine);
        if (iRet != TPS_ER_SUCCESS) return iRet;
        double gantryAngle = segment->get_startgantryangle();
        double tableAngle = segment->get_starttablerotationangle();
        double collimatorAngle = segment->get_startcollimatorangle();
        iecCs->RotateGantry(gantryAngle);
        iecCs->RotateTable(tableAngle);
        iecCs->RotateCollimator(collimatorAngle);
        Mcsf::Matrix4f matBeam2Pat;
        iecCs->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matBeam2Pat);
        //update mlc | block if auto-shape enabled
        SetJawBoundary(machine, beam->get_physicalwedgeuid(), beam->get_virtualwedgeuid());
        ReshapeAperture_i(beam, segment, machine, matBeam2Pat);
        ReshapeShieldBlock_i(beam, segment, machine, matBeam2Pat);
        //ReCreateBolus_i(imageUid, beam);

        //if the beam has been optimized, delete other segments
        if (segments.size() > 1)
        {
            segments.clear();
            segment = new RtBeamsegment(*segment);
            segment->set_sequencenumber(0);
            segments.push_back(segment);
            beam->set_beamsegments(segments);
            TPS_DEL_PTR(segment);
        }


        //response beam changed
        if (mResponser != nullptr){
            mResponser->Response(beam);
        }

        std::vector<RtBeamsegment*> segmentList = beam->get_beamsegments();
        double offset = CalculateMiniGapOffset_i((RtDbDef::PLAN_TYPE)ng->get_plantype(), segmentList, machine);
        if (offset > 0)
        {
            CorrectBeamLeafGap_i(offset, machine, segmentList);
        }

        if (beam->get_isautoshape() 
            && (!beam->get_targetvoiuid().empty() || !beam->get_avoidvoiuid().empty()))
        {
            //update FluenceMap
            if (!mDoseProxy->UpdateFluenceMap(beam))
            {
                TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
                return false;
            }
        }

        //save beam
        iRet = mSaveObjects->SaveBeam(mBeamUid);
        if (iRet != TPS_ER_SUCCESS){
            TPS_LOG_DEV_ERROR<<"Failed to save beam.";
        }
        //update graphic objects
        mRenderProxy->UpdateBeam(ngUid, mBeamUid);
        mRenderProxy->ResetBeamViewCameras(planUid);
        //render
        mRenderProxy->Render();
        return iRet;
    }
    else {
        TPS_LOG_DEV_ERROR<<"Unrecognized mouse action type.";
        return TPS_ER_FAILURE;
    }
}

int RotateImrtBeamOnMprCmd::RotateBeam_i(RtBeam* beam, 
    RtBeamsegment* segment, TpsIECCoordinateSystem* iecCs, RtMachine* machine)
{
    double minAngle = machine->get_gantrymin();
    //minAngle -= floor(minAngle / 360) * 360;
    double maxAngle = machine->get_gantrymax();
    //maxAngle -= floor(maxAngle / 360) * 360;
    if (maxAngle < minAngle){
        TPS_LOG_DEV_ERROR<<"Gantry limits error: the max angle is less than the min.";
        return TPS_ER_FAILURE;
    }

    //locate to current segment
    double gantryAngle = segment->get_startgantryangle();
    double tableAngle = segment->get_starttablerotationangle();
    double collimatorAngle = segment->get_startcollimatorangle();
    iecCs->RotateGantry(0.0);
    iecCs->RotateTable(tableAngle);
    iecCs->RotateCollimator(collimatorAngle);
    //check existing angle status
    bool isAlreadyOnEdge = fabs(gantryAngle - minAngle) <= 1e-6 || 
        fabs(gantryAngle - maxAngle) <= 1e-6;
    //transform points to beam c.s.
    Mcsf::Point2f curScrPt(mCurrX, mCurrY);
    Mcsf::Point3f curPatPt;
    if(!mRenderProxy->TransformPoint(mWndType, mWndUid, curScrPt, curPatPt)){
        TPS_LOG_DEV_ERROR<<"Failed to transform points.";
        return TPS_ER_FAILURE;
    }
    //get normal direction of current mpr plane
    Mcsf::Vector3f vecNormal;
    if (!mRenderProxy->GetNormalOfMPR(mWndType, mWndUid, vecNormal)){
        TPS_LOG_DEV_ERROR<<"Failed to get normal vector of MPR.";
        return TPS_ER_FAILURE;
    }
    //calculate rotation angles
    Mcsf::Matrix4f matPat2Gantry;
    if (!iecCs->GetTransformMatrix(_PATIENT, _GANTRY, matPat2Gantry)){
        TPS_LOG_DEV_ERROR<<"Failed to get transform matrix for iec c.s. object.";
        return TPS_ER_FAILURE;
    }
    vecNormal = matPat2Gantry.Transform(vecNormal);
    vecNormal.Normalize();
    float angleNorm2GantryY = AngleBetween_i(vecNormal, Mcsf::Vector3f(0.f, 1.f, 0.f));
    //tolerance is 20 degree, if current plane is not parallel enough with the 
    //gantry rotation plane, do nothing
    if (fabs(angleNorm2GantryY - PI/2) <= 20.f * PI / 180.f){
        return TPS_ER_SUCCESS;
    }
    //gantry angle
    Mcsf::Point3f curGantryPt = matPat2Gantry.Transform(curPatPt);
    Mcsf::Vector3f vecGantry(0.0, 0.0, 1.0);
    Mcsf::Vector3f vecStayPt(curGantryPt); 
    //in-project to gantry rotation plane
    if (fabs(vecStayPt.GetY()) > 1e-6){
        float lamda = (0.f - vecStayPt.GetY()) / vecNormal.GetY();
        vecStayPt += lamda * vecNormal;
    }
    //printf("\nstart: %f, %f", vecStart.GetX(), vecStart.GetZ());
    //printf("\nend: %f, %f", vecEnd.GetX(), vecEnd.GetZ());
    //Mcsf::Vector3f::AngleBetween有bug，当浮点数精度问题导致acos输入超出有效范围时，会返回无效值
    //测试数据:
    //start: 0.967934, 351.031464
    //end: 1.001617, 352.608276
    double currentGantryAngle = TPS_FLOAT_SIGN(vecStayPt.GetX()) * 
        AngleBetween_i(vecGantry, vecStayPt);
    currentGantryAngle *= 180.f / PI;
    //transform to 0-360
    currentGantryAngle -= floor(currentGantryAngle / 360) * 360;
    if (currentGantryAngle - maxAngle > 1e-6 || currentGantryAngle - minAngle < -1e-6){
        if (isAlreadyOnEdge) return TPS_ER_SUCCESS;
        currentGantryAngle = TPS_CLAMP(currentGantryAngle, minAngle, maxAngle);
    }
    //if nothing need to change, return
    if (fabs(currentGantryAngle - gantryAngle) <= 1e-6){
        return TPS_ER_SUCCESS;
    }
    gantryAngle = currentGantryAngle;

    segment->set_startgantryangle(gantryAngle);
    segment->set_arclength(0.f);
    iecCs->RotateGantry(gantryAngle);
    Mcsf::Matrix4f matBeam2Pat;
    iecCs->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matBeam2Pat);
    float arrayMat[16];
    matBeam2Pat.GetElements(arrayMat);
    segment->set_t_beam_to_pat(arrayMat);
    segment->set_sequencenumber(0);
    beam->set_description(this->GenerateDefaultBeamDescription_i(gantryAngle, 
        collimatorAngle, tableAngle));
    return TPS_ER_SUCCESS;
}

void RotateImrtBeamOnMprCmd::ReshapeAperture_i(RtBeam* beam, 
    RtBeamsegment* segment, RtMachine* machine, const Mcsf::Matrix4f& matB2P)
{
    if (segment == nullptr || machine == nullptr) return;
    RtMlcshape* startShape = segment->get_startmlcshape();
    if (startShape == nullptr || !beam->get_isautoshape()) return;
    RtBlock* apertureBlock = nullptr;
    int apertureType = beam->get_aperturetype();
    if (apertureType == RtDbDef::Aperture_Block){
        apertureBlock = beam->get_aperture_block();
        if (apertureBlock == nullptr || !apertureBlock->get_isautoshape()) return;
    }

    RtMlcshape* shape = new RtMlcshape(*startShape);
    switch(apertureType){
    case RtDbDef::Mlc:
        this->AutoShapeMlc_i(*beam, machine, matB2P, shape);
        break;
    case RtDbDef::JawOnly:
        this->AutoShapeJaw_i(*beam, machine, matB2P, shape);
        break;
    case RtDbDef::Aperture_Block:
        this->AutoShapeBlock_i(*beam, machine, matB2P, shape, apertureBlock);
        break;
    }
    this->UpdateSegmentMlcShapes_i(shape, shape, segment);
    delete shape;
}

void RotateImrtBeamOnMprCmd::ReshapeShieldBlock_i(RtBeam* beam, 
    RtBeamsegment* segment, RtMachine* machine, const Mcsf::Matrix4f& matB2P)
{
    if (beam == nullptr || machine == nullptr || segment == nullptr) return;
    RtBlock* block = beam->get_shield_block();
    if (block == nullptr || !block->get_isautoshape()) return;
    RtMlcshape* shape = segment->get_startmlcshape();
    if (shape == nullptr) return;
    this->AutoShapeBlock_i(*beam, machine, matB2P, shape, block);
}

// wzd-2017-02-14 bolus 重构后 编辑角度不需要重新生成Bolus
//void RotateImrtBeamOnMprCmd::ReCreateBolus_i(const std::string& imageUid, RtBeam* beam)
//{
//    if (beam == nullptr) return;
//    //if no bolus exists, return
//    if (beam->get_bolusvoiuid().empty()) return;
//    //clear current bolus voi firstly
//    std::string voiUid = beam->get_bolusvoiuid();
//    VOIEntity* bolusVoi = nullptr;
//    if (!mDataRepository->GetVoiManager()->GetVOI(voiUid, &bolusVoi) || bolusVoi == nullptr)
//    {
//        return;
//    }
//    float density = bolusVoi->GetAverageDensity();
//    mRenderProxy->DeleteVoi(imageUid, voiUid);
//    mDataRepository->GetVoiManager()->DeleteVOI(voiUid);
//
//    mSaveObjects->EraseObject(voiUid, RtDbDef::TYPE_VOI);
//
//    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
//    if (!segments.empty()) {
//        //draw bolus
//        float bolusMargin[4] = {DEFAULT_BOLUS_MARGIN, DEFAULT_BOLUS_MARGIN, DEFAULT_BOLUS_MARGIN, DEFAULT_BOLUS_MARGIN};
//        float bolusThickness = beam->get_bolusthickness();
//        std::string bolusVOIUID = MakeBolus(imageUid, beam->get_normgroupuid(), segments, 
//            bolusThickness, bolusMargin, density);
//        VOIEntity* pBolusVoi = nullptr;
//        mDataRepository->GetVoiManager()->GetVOI(bolusVOIUID, &pBolusVoi);
//
//        if (!bolusVOIUID.empty() && nullptr != pBolusVoi) {
//            beam->set_bolusvoiuid(bolusVOIUID);
//            mRenderProxy->AddVOI(bolusVOIUID, imageUid);
//        }
//    }
//    else{
//        beam->set_bolusthickness(0.f);
//        beam->set_bolusvoiuid("");
//        // beam->SetBolus(nullptr);
//    }
//}

void RotateImrtBeamOnMprCmd::SetResponser(const std::shared_ptr<BeamActiveResponser>& responser)
{
    mResponser = responser;
}

double RotateImrtBeamOnMprCmd::AngleBetween_i(const Mcsf::Vector3f& vecL, const Mcsf::Vector3f& vecR)
{
    float len = vecL.Magnitude() * vecR.Magnitude();
    len = (len > 1e-6f) ? len : 1e-6f;
    float prod = vecL.DotProduct(vecR) / len;
    prod = prod > 1.f ? 1.f : prod;
    prod = prod < -1.f ? -1.f : prod;
    return acos(prod);
}

std::string RotateImrtBeamOnMprCmd::GenerateDefaultBeamDescription_i(double gantryAngle, 
    double collimatorAngle, double tableAngle)
{
    stringstream ss;
    ss.precision(1);
    ss.setf(std::ios::fixed, std:: ios::floatfield);
    ss<<"g"<<gantryAngle<<"_t"<<tableAngle<<"_c"<<collimatorAngle;
    return ss.str();
}
TPS_END_NAMESPACE  // end namespace tps
