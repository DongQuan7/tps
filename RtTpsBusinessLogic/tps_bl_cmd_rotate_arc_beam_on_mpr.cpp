////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_rotate_arc_beam_on_mpr.cpp
/// 
///  \brief class RotateArcBeamOnMprCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_rotate_arc_beam_on_mpr.h"
#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"
#include "tps_logger.h"

//2 degrees
#define INTERVAL_DEGREE 2
#define MIN_ANGLE 0.0
#define MAX_ANGLE 359.9

TPS_BEGIN_NAMESPACE   // begin namespace tps

RotateArcBeamOnMprCmd::RotateArcBeamOnMprCmd(
    WINDOW_TYPE wndType, const std::string& wndUid, 
    double preX, double preY, double currX, double currY, int mouseActionType, 
    const std::string& beamUid, bool isToRotateStart)
    : mWndType(wndType), mWndUid(wndUid), mPreX(preX)
    , mPreY(preY), mCurrX(currX), mCurrY(currY)
    , mMouseActionType(mouseActionType), mBeamUid(beamUid)
    , mIsToRotateStart(isToRotateStart), mResponser(nullptr)
{

}

RotateArcBeamOnMprCmd::~RotateArcBeamOnMprCmd()
{

}

int RotateArcBeamOnMprCmd::Execute()
{
    //get editing beam
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"The beam does not exist.";
        return TPS_ER_FAILURE;
    }
    int beamType = beam->get_beamtype();
    if (beamType != RtDbDef::BURST_ARC && beamType != RtDbDef::VMAT){
        TPS_LOG_DEV_ERROR<<"The editing beam is not imrt beam.";
        return TPS_ER_FAILURE;
    }

    //get plan uid
    std::string planUid, ngUid;
    ngUid = beam->get_normgroupuid();
    RtNormgroup* ng = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &ng)){
        TPS_LOG_DEV_ERROR<<"Failed to get normal group.";
        return TPS_ER_FAILURE;
    }
    planUid = ng->get_planuid();
    int planType = ng->get_plantype();

    //get machine
    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(ng->get_machineuid(), machine)){
        TPS_LOG_DEV_ERROR<<"Failed to get machine.";
        return TPS_ER_FAILURE;
    }

    TpsIECCoordinateSystem* iecCs = mDataRepository->GetBeamManager()->GetBeamCS(mBeamUid);
    if (iecCs == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get beam iec coordinate system object.";
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty()){
        TPS_LOG_DEV_ERROR<<"There is no segment in the beam.";
        return TPS_ER_FAILURE;
    }
    //get the target segments
    RtBeamsegment* startSegment = segments.front();
    RtBeamsegment* endSegment = segments.back();
    if (startSegment == nullptr || endSegment == nullptr){
        TPS_LOG_DEV_ERROR<<"The start or end segment is null.";
        return TPS_ER_FAILURE;
    }
    bool isClockwise = startSegment->get_gantryrotationdirection() == RtDbDef::CW;
    double collimatorAngle = startSegment->get_startcollimatorangle();
    double tableAngle = startSegment->get_starttablerotationangle();

    if (mMouseActionType == DOWN)
    {
        double minAngle = machine->get_gantrymin();
        //minAngle -= floor(minAngle / 360) * 360;
        double maxAngle = machine->get_gantrymax();
        //maxAngle -= floor(maxAngle / 360) * 360;
        if (maxAngle < minAngle){
            TPS_LOG_DEV_ERROR<<"Gantry limits error: the max angle is less than the min.";
            return TPS_ER_FAILURE;
        }
        double cwRange = machine->get_gantryclockwiserange();
        double ccwRange = machine->get_gantrycounterclockwiserange();
        if (isClockwise && cwRange <= 0){
            TPS_LOG_DEV_ERROR<<"The max clockwise rotation range of the gantry is 0.";
            return TPS_ER_FAILURE;
        }
        else if (!isClockwise && ccwRange <= 0){
            TPS_LOG_DEV_ERROR<<"The max counter-clockwise rotation range of the gantry is 0.";
            return TPS_ER_FAILURE;
        }
        bool isSimpleArc = planType == RtDbDef::PLAN_Simple_Arc;
        double startAngle = startSegment->get_startgantryangle();
        double arcLength = 0.0, start, end;
        int sign = isClockwise ? 1 : -1;
        for (int i = 0; i < segments.size(); ++i)
        {
            arcLength += segments[i]->get_arclength();
            if (i == segments.size() - 1) break;
            start = segments[i]->get_startgantryangle() + sign * segments[i]->get_arclength();
            end = segments[i+1]->get_startgantryangle();
            arcLength += TpsUtility::ToClockPosition(sign * (end - start));
        }
        auto endAngle = endSegment->get_startgantryangle() + sign * endSegment->get_arclength();
        endAngle = TpsUtility::ToClockPosition(endAngle);

        Mcsf::Matrix4f matStartG2P, matEndG2P, matP2OrgG;
        iecCs->RotateCollimator(collimatorAngle);
        iecCs->RotateTable(tableAngle);
        iecCs->RotateGantry(startAngle);
        iecCs->GetTransformMatrix(_GANTRY, _PATIENT, matStartG2P);
        iecCs->RotateGantry(endAngle);
        iecCs->GetTransformMatrix(_GANTRY, _PATIENT, matEndG2P);
        iecCs->RotateGantry(0.0);
        iecCs->GetTransformMatrix(_PATIENT, _GANTRY, matP2OrgG);
        mRenderProxy->SetArcEditCache(planUid, ngUid, mBeamUid, 
            isClockwise, isSimpleArc, mIsToRotateStart, minAngle, 
            maxAngle, cwRange, ccwRange, startAngle, arcLength, 
            TpsArithmeticConverter::ConvertToMatrix4x4(matStartG2P), 
            TpsArithmeticConverter::ConvertToMatrix4x4(matEndG2P), 
            TpsArithmeticConverter::ConvertToMatrix4x4(matP2OrgG));
        mRenderProxy->EnableArcEditModeOnMprPlane(planUid, ngUid, mBeamUid, true);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    else if(mMouseActionType == DRAG)
    {
        mRenderProxy->EditArcBeamCache(mWndType, mWndUid, mPreX, mPreY, 
            mCurrX, mCurrY, planUid, ngUid, mBeamUid);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    else if (mMouseActionType == UP)
    {
        mRenderProxy->EditArcBeamCache(mWndType, mWndUid, mPreX, mPreY, 
            mCurrX, mCurrY, planUid, ngUid, mBeamUid);
        mRenderProxy->EnableArcEditModeOnMprPlane(planUid, ngUid, mBeamUid, false);
        
        double start, arcLength;
        if (!mRenderProxy->GetArcGantryAnglesFromCache(planUid, ngUid, mBeamUid, start, arcLength)){
            TPS_LOG_DEV_ERROR<<"Failed to get arc gantry angles from cache.";
            return TPS_ER_FAILURE;
        }

        if (arcLength < 2){
            TPS_LOG_DEV_ERROR<<"The size of arc gantry angles is invalid.";
            return TPS_ER_FAILURE;
        }
        iecCs->RotateCollimator(collimatorAngle);
        iecCs->RotateTable(tableAngle);

        SetJawBoundary(machine, beam->get_physicalwedgeuid(), beam->get_virtualwedgeuid());

        std::vector<double> gantryAngles;
        std::vector<double> arcLengths;
        double len = 0.0;
        int sign = isClockwise ? 1 : -1;
        for (; len - arcLength < -1e-6; len += INTERVAL_DEGREE)
        {
            gantryAngles.push_back(TpsUtility::ToClockPosition(start + sign * len));
            arcLengths.push_back(TPS_MIN(INTERVAL_DEGREE, arcLength - len));
        }
        gantryAngles.push_back(TpsUtility::ToClockPosition(
            gantryAngles.back() + sign * arcLengths.back()));

        //create mlc shape for every control point
        std::vector<RtMlcshape*> vecCPMlcShapes;
        std::vector<Mcsf::Matrix4f> vecC2PMatrices;
        vecCPMlcShapes.reserve(gantryAngles.size());
        vecC2PMatrices.reserve(gantryAngles.size());
        Mcsf::Matrix4f matC2P;
        RtMlcshape* shape = nullptr;
        RtMlcshape* oldStartShape = startSegment->get_startmlcshape();
        MLC_EDIT_TYPE mlcEditType = (MLC_EDIT_TYPE)beam->get_mlcedittype();
        for (int i = 0; i < gantryAngles.size(); ++i){
            iecCs->RotateGantry(gantryAngles[i]);
            if (!iecCs->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matC2P)){
                TPS_LOG_DEV_ERROR<<"Failed to get beam to patient matrix.";
                continue;
            }
            vecC2PMatrices.push_back(matC2P);
            shape = new RtMlcshape(true);
            CopyMlcShapeEditParameters_i(oldStartShape, shape);
            CreateArcSegmentShape_i(*beam, machine, matC2P, shape, mlcEditType);
            vecCPMlcShapes.push_back(shape);
        }
        
        //create segments
        float arrayC2P[16]; 
        std::vector<RtBeamsegment*> segmentList;
        RtBeamsegment* segment = nullptr;
        if (planType == RtDbDef::PLAN_Simple_Arc){
            matC2P = vecC2PMatrices[0];
            matC2P.GetElements(arrayC2P);
            segment = CreateBasicBeamSegmentObject_i(
                0, start, arcLength, 
                tableAngle,collimatorAngle, arrayC2P, 1.0f, 
                isClockwise ? RtDbDef::CW : RtDbDef::CC, mBeamUid);
            shape = new RtMlcshape(true);
            CopyMlcShapeEditParameters_i(oldStartShape, shape);
            CombineMlcShapes_i(vecCPMlcShapes, shape, machine->get_minleafendgap());
            UpdateSegmentMlcShapes_i(shape, shape, segment);
            delete shape;
            segmentList.push_back(segment);
        }
        else {
            //Conformal arc & VMAT & Burst arc
            bool isBurstArc = planType == RtDbDef::PLAN_Burst_Arc;
            for (int i = 0; i < vecCPMlcShapes.size() - 1; ++i){
                matC2P = vecC2PMatrices[i];
                matC2P.GetElements(arrayC2P);
                segment = CreateBasicBeamSegmentObject_i(i,
                    gantryAngles[i], arcLengths[i], tableAngle, 
                    collimatorAngle, arrayC2P, 1.0f, 
                    isClockwise ? RtDbDef::CW : RtDbDef::CC, mBeamUid);
                UpdateSegmentMlcShapes_i(vecCPMlcShapes[i], 
                    //burst arc segment has same mlc shape on start & end position
                    isBurstArc ? vecCPMlcShapes[i] : vecCPMlcShapes[i+1], segment);
                segmentList.push_back(segment);
            }
        }
        //clear temp mlc shapes
        for (int i = 0; i < vecCPMlcShapes.size(); ++i){
            delete vecCPMlcShapes[i];
        }
        vecCPMlcShapes.clear();

        //set new segment list

        double offset = CalculateMiniGapOffset_i((RtDbDef::PLAN_TYPE)planType, segmentList, machine);
        if (offset > 0)
        {
            CorrectBeamLeafGap_i(offset, machine, segmentList);
        }

        beam->set_beamsegments(segmentList);
        for (auto itr=segmentList.begin(); itr!=segmentList.end(); ++itr) DEL_PTR(*itr);

        //generate new default name
        beam->set_description(this->GenerateDefaultBeamDescription_i(gantryAngles.front(), 
            arcLength, gantryAngles.back(), collimatorAngle, tableAngle));

        //update FluenceMap
        if (!mDoseProxy->UpdateFluenceMap(beam))
        {
            TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
            return false;
        }

        //save beam
        int iRet = mSaveObjects->SaveBeam(mBeamUid);
        if (iRet != TPS_ER_SUCCESS){
            TPS_LOG_DEV_ERROR<<"Failed to save beam.";
        }
        //response beam changed
        if (mResponser != nullptr){
            mResponser->Response(beam);
        }
        //update graphic objects
        mRenderProxy->UpdateBeam(ngUid, mBeamUid);
        mRenderProxy->ResetBeamViewCameras(planUid);
        mRenderProxy->Render();
        return iRet;
    }
    else {
        TPS_LOG_DEV_ERROR<<"Unrecognized mouse action type.";
        return TPS_ER_FAILURE;
    }
}

void RotateArcBeamOnMprCmd::SetResponser(const std::shared_ptr<BeamActiveResponser>& responser)
{
    mResponser = responser;
}

void RotateArcBeamOnMprCmd::CopyMlcShapeEditParameters_i(
    RtMlcshape* const source, RtMlcshape* target)
{
    //jaw
    //target->set_jawtype(source->get_jawtype());
    target->set_jawxissymmetry(source->get_jawxissymmetry());
    target->set_jawyissymmetry(source->get_jawyissymmetry());
    //target->set_jawmarginx(source->get_jawmarginx());
    //target->set_jawmarginy(source->get_jawmarginy());
    target->set_xlowerjawpos(source->get_xlowerjawpos());
    target->set_xupperjawpos(source->get_xupperjawpos());
    target->set_ylowerjawpos(source->get_ylowerjawpos());
    target->set_yupperjawpos(source->get_yupperjawpos());
    //leaf
    //target->set_isautoshape(source->get_isautoshape());
    //target->set_targetvoiuid(source->get_targetvoiuid());
    //target->set_targetmarginx(source->get_targetmarginx());
    //target->set_targetmarginy(source->get_targetmarginy());
    //target->set_avoidvoiuid(source->get_avoidvoiuid());
    //target->set_avoidmarginx(source->get_avoidmarginx());
    //target->set_avoidmarginy(source->get_avoidmarginy());
    target->set_mlcdirectiontype(source->get_mlcdirectiontype());
}

std::string RotateArcBeamOnMprCmd::GenerateDefaultBeamDescription_i(double startGantryAngle, 
    double arcLength, double endGantryAngle, double collimatorAngle, double tableAngle)
{
    stringstream ss;
    ss.precision(1);
    ss.setf(std::ios::fixed, std:: ios::floatfield);
    ss<<"g"<<startGantryAngle<<"To"<<endGantryAngle<<"length"<<arcLength<<"_t"<<tableAngle<<"_c"<<collimatorAngle;
    return ss.str();
}
TPS_END_NAMESPACE  // end namespace tps