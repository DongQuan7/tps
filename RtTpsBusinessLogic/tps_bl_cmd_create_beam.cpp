////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_pa_planning_createbeam_handler.cpp
/// 
///  \brief class Planning Create new beam handler 
/// 
///  \version 1.0
/// 
///  \date    Dec.11,2013
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"


#include "boost/algorithm/string.hpp"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

//TPS
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"

#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsAlgorithmProxy/tps_pl_make_bolus.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "RtTpsAlgorithmProxy/tps_ap_mlc_jaw_limitation_helper.h"
#include "RtTpsAlgorithmProxy/tps_ap_auto_shape_utility.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "tps_logger.h"

#define DEFAULT_FIELD_SIZE 100//mm
#define INTERVAL_DEGREE 2 //degree

TPS_BEGIN_NAMESPACE;

CreateBeamCmd::CreateBeamCmd(const TpsCertifiedBeam& certifiedBeam, int dataSourceID)
    :mCertifiedBeam(certifiedBeam), mDataSourceId(dataSourceID)
{

}

CreateBeamCmd::CreateBeamCmd()
{

}

CreateBeamCmd::~CreateBeamCmd(void)
{

}

void CreateBeamCmd::SetCertifiedBeam(TpsCertifiedBeam certifiedBeam)
{
    mCertifiedBeam = certifiedBeam;
}

void CreateBeamCmd::GetBeamCreated(const RtBeam*& pBeam)
{
    RtBeam* newBeam = mDataRepository->GetBeamManager()->GetBeam(mCertifiedBeam.mBeamUID);
    pBeam = newBeam;
}

int CreateBeamCmd::Execute()
{
    TPS_LOG_DEV_INFO << "[INFO][CreateBeamCmd][Execute]: " << "CreateBeamCmd execute!";
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    std::string normGroupUID = mCertifiedBeam.mNormGroupUID;

    if (mCertifiedBeam.mCommissionedUnitUID.empty())
    {
        TPS_LOG_DEV_ERROR << "mCertifiedBeam.mUnitUID is empty.";
        return TPS_ER_FAILURE;
    }

    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normGroupUID, &normGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to get plan.";
        return TPS_ER_FAILURE;
    }

    std::string planUID = normGroup->get_planuid();
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan) || plan == nullptr)
    {
        TPS_LOG_DEV_ERROR << "plan does not exist, uid: " << planUID;
        return TPS_ER_FAILURE;
    }
    const std::string imageUID = plan->get_imageseriesuid();

    //create new beam as beam UID created from FE true == false
    RtBeam* beam = nullptr;
    if(RtDbDef::QAPlan == plan->get_plansourcetype() || RtDbDef::MergedPlan == plan->get_plansourcetype())
    {
        TPS_LOG_DEV_ERROR << "CreateBeamCmd can not add beam for plan [" << plan->get_uid() << "] because it is not a TreatmentPlan.";
        return TPS_ER_FAILURE;
    }

    // create Treatment beam
    int ret = CreateTreatmentBeam(beam, normGroup, imageUID);
    if (TPS_ER_SUCCESS != ret)
    {
        TPS_LOG_DEV_ERROR << "Failed to create treatment beam[" << std::string(nullptr == beam ? "" : beam->get_uid()) << "].";
        return TPS_ER_FAILURE;
    }

    //create empty dose grid
    if (!CreateDosegridForBeam_i(plan, normGroup, beam))
    {
        TPS_LOG_DEV_ERROR << "Failed to create empty dosegrid for beam[" << mCertifiedBeam.mBeamUID << "].";
        DEL_PTR(beam);
        return TPS_ER_FAILURE;
    }

    //creat fluence map
    FluenceMapEntity* fluenceMap = new FluenceMapEntity();
    if (!mDoseProxy->CalculateFluenceMap(beam, fluenceMap))
    {
        TPS_LOG_DEV_ERROR << "Failed to create FluenceMap for beam[" << beam->get_uid() << "].";
        DEL_PTR(fluenceMap);
        DEL_PTR(beam);
        return TPS_ER_FAILURE;
    }
    mDataRepository->GetBeamManager()->AddFluenceMap(beam->get_uid(), fluenceMap);

    if (TPS_ER_SUCCESS != mSaveObjects->InsertBeam(*beam))
    {
        TPS_LOG_DEV_ERROR << "InsertBeam failed! uid:" << mCertifiedBeam.mBeamUID;
        TPS_PRINTF_DEBUG("InsertBeam failed uid:%s\n", mCertifiedBeam.mBeamUID.c_str());
        return TPS_ER_DB_ERROR;
    }


    // 将beam加入内存管理
    if (!mDataRepository->GetBeamManager()->AddBeam2Map(beam))
    {
        TPS_LOG_DEV_ERROR << "AddBeam2Map Failed with beam UID:" << mCertifiedBeam.mBeamUID;
        DEL_PTR(beam);
        return TPS_ER_FAILURE;
    }

    if (!mRenderProxy->AddBeam(normGroupUID, mCertifiedBeam.mBeamUID))
    {
        TPS_LOG_DEV_ERROR << "Failed to add beam in render proxy.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->ResetBeamViewCameras(plan->get_uid());

    mRenderProxy->Render();
    TPS_LOG_DEV_INFO << "[INFO][CreateBeamCmd][Execute]: " << "CreateBeamCmd leave!";

    TEST_PERFORMANCE_END("CreateBeamCmd");
    return TPS_ER_SUCCESS;
}

TpsIECCoordinateSystem* CreateBeamCmd::CreateIECCS_i(
    const std::string& imageUID,
    float sad, 
    const std::string& beamUID,
    float gantryAngle,
    float tableAngle,
    float collimatorAngle,
    const Mcsf::Point3f& isoCenter)
{
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &image))
    {
        TPS_LOG_DEV_ERROR << "Failed to get image 3d data which UID: " << imageUID;
        return nullptr;
    }
    Mcsf::Point3f orgPt = *image->GetSetupPoint();

    RtImage3DHeader* header;
    if (!image->GetImage3DHeader(header))
    {
        TPS_LOG_DEV_ERROR << "Failed to get current image 3d header.";
        return nullptr;
    }
    RtSeries* pSeries = image->GetRtSeries();
    if (nullptr == pSeries)
    {
        TPS_LOG_DEV_ERROR << "pSeries is null";
        return nullptr;
    }
    PatientPosition pp = TpsCmdHelper::GetPatientPosition(pSeries->get_patientposition());

    TpsIECCoordinateSystem *iecCS = new TpsIECCoordinateSystem();
    if (!iecCS->Initialize(orgPt, pp, sad, gantryAngle, collimatorAngle, tableAngle))
    {
        TPS_LOG_DEV_ERROR << "Failed to initialize IEC coordinate system.";
        delete iecCS;
        return nullptr;
    }
    iecCS->SetISOCenter(isoCenter);

    mDataRepository->GetBeamManager()->AddBeamCS(beamUID, iecCS);
    return iecCS;
}


void CreateBeamCmd::CreateArcSegments_i(
    RtBeam& beam,
    const std::string& imageUID,
    bool bIsClockWise,
    float gantryStartAngle,
    float gantryArcLength,
    float tableAngle,
    float collimatorAngle,
    RtMachine* const machine,
    std::string beamUid,
    const Mcsf::Point3f& isoCenter,
    RtDbDef::PLAN_TYPE planType,
    std::vector<RtBeamsegment*>& segmentList)
{
    std::vector<float> vecSegmentAngles;
    CalculateBeamsegmentAngles_i(gantryStartAngle, gantryArcLength, bIsClockWise, vecSegmentAngles);

    // 先创建一个ieccs对象，然后通过旋转gantry来更新每一个segment的变换矩阵B2P
    TpsIECCoordinateSystem* iecCS = CreateIECCS_i(imageUID, machine->get_sad(), beamUid,
        gantryStartAngle, tableAngle, collimatorAngle, isoCenter);
    if (iecCS == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to create IEC coordinate system.";
        return;
    }

    //create mlc shape for every control point
    std::vector<RtMlcshape*> vecCPMlcShapes;
    std::vector<Mcsf::Matrix4f> vecC2PMatrices;
    vecCPMlcShapes.reserve(vecSegmentAngles.size());
    vecC2PMatrices.reserve(vecSegmentAngles.size());
    Mcsf::Matrix4f matC2P;
    RtMlcshape* shape = nullptr;
    for (int i = 0; i < vecSegmentAngles.size(); i++)
    {
        iecCS->RotateGantry(vecSegmentAngles[i]);
        if (!iecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matC2P))
        {
            TPS_LOG_DEV_ERROR << "Failed to get beam to patient matrix.";
            return;
        }
        vecC2PMatrices.push_back(matC2P);
        shape = new RtMlcshape(true);
        SetMlcShapeEditParameters_i(machine, &beam, shape, i);
        //create default MLC shape
        CreateArcSegmentShape_i(beam, machine, matC2P, shape, (MLC_EDIT_TYPE)beam.get_mlcedittype());
        vecCPMlcShapes.push_back(shape);
    }

    //create segments
    float arrayC2P[16];
    RtBeamsegment* segment = nullptr;
    if (planType == RtDbDef::PLAN_Simple_Arc)
    {
        matC2P = vecC2PMatrices[0];
        matC2P.GetElements(arrayC2P);
        segment = CreateBasicBeamSegmentObject_i(
            0, vecSegmentAngles.front(), gantryArcLength,
            tableAngle,collimatorAngle, arrayC2P, 1.0f,
            bIsClockWise ? RtDbDef::CW : RtDbDef::CC, beamUid);
        shape = new RtMlcshape(true);
        SetMlcShapeEditParameters_i(machine, &beam, shape);
        CombineMlcShapes_i(vecCPMlcShapes, shape, machine->get_minleafendgap());
        UpdateSegmentMlcShapes_i(shape, shape, segment);
        delete shape;
        segmentList.push_back(segment);
    }
    else 
    {
        //Conformal arc & VMAT & Burst arc
        bool isBurstArc = planType == RtDbDef::PLAN_Burst_Arc;
        float arcLength;
        int sign = bIsClockWise ? 1 : -1;
        for (int i = 0; i < vecCPMlcShapes.size() - 1; ++i)
        {
            arcLength = sign * (vecSegmentAngles[i+1] - vecSegmentAngles[i]);
            arcLength = TpsUtility::ToClockPosition(arcLength);
            matC2P = vecC2PMatrices[i];
            matC2P.GetElements(arrayC2P);
            segment = CreateBasicBeamSegmentObject_i(i,
                vecSegmentAngles[i], arcLength, tableAngle,
                collimatorAngle, arrayC2P, 1.0f,
                bIsClockWise ? RtDbDef::CW : RtDbDef::CC, beamUid);
            UpdateSegmentMlcShapes_i(vecCPMlcShapes[i], 
                //burst arc segment has same mlc shape on start & end position
                isBurstArc ? vecCPMlcShapes[i] : vecCPMlcShapes[i + 1], segment);
            segmentList.push_back(segment);
        }
    }

    for (int i = 0; i < vecCPMlcShapes.size(); ++i)
    {
        DEL_PTR(vecCPMlcShapes[i]);

    }
    vecCPMlcShapes.clear();
}

bool CreateBeamCmd::CreateImrtSegment_i(
    RtBeam* beam,
    const std::string& imageUID,
    RtMachine* const machine,
    float gantryStartAngle,
    float tableAngle,
    float collimatorAngle,
    const Mcsf::Point3f& isoCenter,
    RtDbDef::PLAN_TYPE planType,
    RtDbDef::APERTURE_TYPE apertureType,
    MLC_EDIT_TYPE mlcEditType,
    RtBeamsegment*& segment)
{
    const int iControlPointIndex = 0;
    TpsIECCoordinateSystem* iecCS = CreateIECCS_i(imageUID,
        machine->get_sad(),
        beam->get_uid(),
        gantryStartAngle,
        tableAngle,
        collimatorAngle,
        isoCenter);

    if (iecCS == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to create IEC coordinate system.";
        return false;
    }

    Mcsf::Matrix4f matC2P;
    if (!iecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matC2P))
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam to patient matrix.";
        return false;
    }

    RtDbDef::ROTATION_DIRECTION direction = RtDbDef::NONE_ROTATION_DIRECTION;
    if (RtDbDef::PLAN_Simple_Arc == planType || RtDbDef::PLAN_Burst_Arc == planType)
    {
        direction = mCertifiedBeam.mIsClockWise? RtDbDef::CW: RtDbDef::CC;
    }

    RtMlcshape* mlcShape = new RtMlcshape(true);
    SetMlcShapeEditParameters_i(machine, beam, mlcShape);
    RtBlock* block = nullptr;
    if (apertureType == RtDbDef::Aperture_Block)
    {
        beam->create_aperture_block();
        block = beam->get_aperture_block();
        SetBlockParameters_i(block, machine);
    }

    if (!CreateImrtAperture_i(*beam, machine, matC2P, mlcShape, block, mlcEditType))
    {
        TPS_LOG_DEV_ERROR << "CreateImrtAperture_i error";
        DEL_PTR(mlcShape);
        if (block) beam->remove_aperture_block();
        return false;
    }

    float arrayC2P[16];
    matC2P.GetElements(arrayC2P);
    segment = CreateBasicBeamSegmentObject_i(
        iControlPointIndex,
        gantryStartAngle,
        0.0f,
        tableAngle,
        collimatorAngle,
        arrayC2P,
        1.0f,
        direction,
        beam->get_uid());
    UpdateSegmentMlcShapes_i(mlcShape, mlcShape, segment);
    delete mlcShape;
    return true; 
}

bool CreateBeamCmd::CreateApplicatorShape_i(RtMachine* const machine, RtMlcshape* shape)
{
    RtApplicator* applicator = nullptr;
    if (!mDataRepository->GetMachineManager()->GetApplicator(mCertifiedBeam.mApplicatorUid, applicator))
    {
        TPS_LOG_DEV_ERROR << "Failed to get applicator with uid: "
            << mCertifiedBeam.mApplicatorUid;
        return false;
    }

    //todo waiting for qq.z's interface
    RtMlcshape* existingShape = nullptr;// = applicator->get_shape();
    if (existingShape)
    {
        *shape = *existingShape;
    }
    else
    {
        TPS_LOG_DEV_WARNING << "There is no default mlc shape for applicator.";
        double rect[4] = {0};
        auto shapeType = (RtDbDef::APPLICATOR_APERTURE_SHAPE)applicator->get_applicatorapertureshape();
        if (shapeType == RtDbDef::SYM_CIRCULAR || shapeType == RtDbDef::SYM_SQUARE)
        {
            double openning = applicator->get_applicatoropening();
            rect[0] = -0.5 * openning;
            rect[1] =  0.5 * openning;
            rect[2] = -0.5 * openning;
            rect[3] =  0.5 * openning;
        }
        else if (shapeType == RtDbDef::SYM_RECTANGLE)
        {
            double openningX = applicator->get_applicatoropeningx();
            double openningY = applicator->get_applicatoropeningy();
            rect[0] = -0.5 * openningX;
            rect[1] =  0.5 * openningX;
            rect[2] = -0.5 * openningY;
            rect[3] =  0.5 * openningY;
        }
        else
        {
            TPS_LOG_DEV_ERROR << "Unrecognized applicator aperture shape.";
            return false;
        }

        if (!CreateRectShape_i(machine, shape, rect, Jaw_Followed))
        {
            TPS_LOG_DEV_ERROR << "Failed to create rect mlc shape.";
            return false;
        }
        delete shape;
    }
    return true;
}

//for debug only
void DrawPoints(int index, int pointNum, float* ptX, float* ptY)
{
    if (nullptr == ptX || nullptr == ptY || pointNum < 1)
    {
        return;
    }
    char fileName[256];
    sprintf_s(fileName, "d:\\temp1\\%d.bmp",index);
    int x_dim = 128, y_dim = 128;
    const int iSize = x_dim * y_dim;
    unsigned char* bufferMirror = new unsigned char[iSize];
    memset(bufferMirror,255,iSize);

    int v(0);
    float x_start = -206.50406f;
    float x_scale = 3.2520325f;
    for (int i = 0; i < pointNum; ++i)
    {
        v = ((ptX[i] - x_start) / x_scale) +
            ((ptY[i] - x_start ) / x_scale) * x_dim;
        bufferMirror[v] = 0;
    }

    SubVolumeHelperAPI::SaveToGrayBitmap(fileName, (unsigned char*)bufferMirror, y_dim, x_dim);
    DEL_ARRAY(bufferMirror);
}

bool CreateBeamCmd::AutoShapeMlc_i(
    const RtBeam& beam,
    RtMachine* const machine,
    const Mcsf::Matrix4f& matC2P,
    RtMlcshape* shape,
    MLC_EDIT_TYPE mlcEditType)
{
    if (shape == nullptr || machine == nullptr) return false;
    if (!matC2P.HasInverse())
    {
        TPS_LOG_DEV_ERROR << "matBeam2Pat has no inverse.";
        return false;
    }

    std::string sSeriesUid("");

    std::string targetVoiUid = beam.get_targetvoiuid();
    tps::ROIEntity* pVOI = nullptr;
    if (!targetVoiUid.empty())
    {
        pVOI = mDataRepository->GetVoiManager()->GetROI(targetVoiUid);
        if (nullptr != pVOI)
        {
            sSeriesUid = pVOI->GetSeriesUid();
        }
    }

    std::string avoidVoiUid = beam.get_avoidvoiuid();
    tps::ROIEntity* pVOIAvoid = nullptr;
    if (!avoidVoiUid.empty() && avoidVoiUid != targetVoiUid)
    {
        pVOIAvoid = mDataRepository->GetVoiManager()->GetROI(avoidVoiUid);
        if (nullptr != pVOIAvoid && sSeriesUid.empty())
        {
            sSeriesUid = pVOIAvoid->GetSeriesUid();
        }
    }

    if (pVOI == nullptr && pVOIAvoid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "There is no target voi & avoid voi, can not do auto shape.";
        return false;
    }

    //get image3d
    tps::TpsImage3DEntity* pImage3D = nullptr;
    bool bRet = mDataRepository->GetImageDataManager()->GetImage3DData(sSeriesUid, &pImage3D);
    if (!bRet || nullptr == pImage3D)
    {
        TPS_LOG_DEV_ERROR << "GetImage3DData failed or pImage3D is null.";
        return false;
    }
    //get matrix
    Mcsf::Matrix4f mat4fVol2Pat;
    pImage3D->GetVolumeToPatientMatrix(mat4fVol2Pat);
    float matVol2Pat[4][4];
    mat4fVol2Pat.GetElements(&matVol2Pat[0][0]);

    Mcsf::Matrix4f matPat2Beam = matC2P.Inverse();
    float fPat2Beam[4][4];
    matPat2Beam.GetElements(&fPat2Beam[0][0]);

    float fBeam2Pat[4][4];
    matC2P.GetElements(&fBeam2Pat[0][0]);

    plunc::CONTOUR_STC contour;
    memset(&contour, 0, sizeof(contour));
    float shappingField[4] =
    {
        mJawBoundary[0],
        mJawBoundary[3],
        mJawBoundary[4],
        mJawBoundary[7],
    };
    //Specify the max range of the auto shaped voi which must be inside jaws if not followed.
    bool isJawFollow = beam.get_jawtype() == RtDbDef::JAWTYPE_Follow;
    if (!isJawFollow)
    {
        shappingField[0] = TPS_MAX(shappingField[0], shape->get_xlowerjawpos());
        shappingField[1] = TPS_MIN(shappingField[1], shape->get_xupperjawpos());
        shappingField[2] = TPS_MAX(shappingField[2], shape->get_ylowerjawpos());
        shappingField[3] = TPS_MIN(shappingField[3], shape->get_yupperjawpos());
    }

    plunc::SCAN3D_MAP map;
    memset(&map, 0, sizeof(map));

    if (pVOI == nullptr)
    {
        //IF user didn't specify target voi, use default field 10cm * 10cm.
        // fix dim 48041
        if (isJawFollow)
        {
            shappingField[0] = -0.5 * DEFAULT_FIELD_SIZE;
            shappingField[1] = 0.5 * DEFAULT_FIELD_SIZE;
            shappingField[2] = -0.5 * DEFAULT_FIELD_SIZE;
            shappingField[3] = 0.5 * DEFAULT_FIELD_SIZE;
        }

        AutoShapeUtility::GenerateMapForOnlyAvoid(
            pVOIAvoid, beam.get_avoidmarginx(), beam.get_avoidmarginy(),
            fPat2Beam, shappingField, &map);
    }
    else
    {
        AutoShapeUtility::GenerateMapFromVoi(
            pVOI, beam.get_targetmarginx(), beam.get_targetmarginy(),
            pVOIAvoid, beam.get_avoidmarginx(), beam.get_avoidmarginy(), 
            fPat2Beam, shappingField, &map);
    }

    //create default MLC shape
    int leafNum = machine->get_leafnum();
    const std::vector<double> leafBoundary = machine->get_leafboundaries_list();
    if (leafBoundary.size() != leafNum + 1)
    {
        TPS_LOG_DEV_ERROR << "The size of leaf boundary list does not match leaf number.";
        return false;
    }

    plunc::LEAF_POSITION_STC* pLeaf = new plunc::LEAF_POSITION_STC[leafNum]; 
    for (int i = 0; i < leafNum; i++)
    {
        pLeaf[i].independent = 2;
        pLeaf[i].pos_1 = 0.0;
        pLeaf[i].pos_2 = 0.0;
        pLeaf[i].min_w = (float)leafBoundary[i];
        pLeaf[i].max_w = (float)leafBoundary[i + 1];
    }
    //DrawPoints(2, contour.vertex_count, contour.x, contour.y);

    double minBoundary = machine->get_negativeleafrangemin();
    double maxBoundary = machine->get_positiveleafrangemax();
    std::vector<db_Point2d> vecLeafPos;
    db_Point2d leafPos;
    int iRet = LeafAdaptForProjectionMap(&map, leafNum, pLeaf, minBoundary, maxBoundary, (int)mlcEditType);
    if (iRet != 0)
    {
        TPS_LOG_DEV_ERROR << "LeafAdaptForProjectionMap error in beam " << beam.get_uid();
        delete map.data;
        map.data = nullptr;
        return false;
    }

    delete map.data;
    map.data = nullptr;

    //Jaw positions
    float jawPos[4];
    jawPos[0] = shape->get_xlowerjawpos();
    jawPos[1] = shape->get_xupperjawpos();
    jawPos[2] = shape->get_ylowerjawpos();
    jawPos[3] = shape->get_yupperjawpos();

    TpsMlcJawLimitationHelper::AdjustMlcJawGivenMachine(
        machine, pLeaf, leafNum, isJawFollow, beam.get_jawmarginx(), beam.get_jawmarginy(), jawPos);
    for (int i = 0; i < leafNum; ++i)
    {
        leafPos.x = pLeaf[i].pos_1;
        leafPos.y = pLeaf[i].pos_2;
        vecLeafPos.push_back(leafPos);
    }
    DEL_ARRAY(pLeaf);

    jawPos[0] = TPS_CLAMP(jawPos[0], mJawBoundary[0], mJawBoundary[1]);
    jawPos[1] = TPS_CLAMP(jawPos[1], mJawBoundary[2], mJawBoundary[3]);
    jawPos[2] = TPS_CLAMP(jawPos[2], mJawBoundary[4], mJawBoundary[5]);
    jawPos[3] = TPS_CLAMP(jawPos[3], mJawBoundary[6], mJawBoundary[7]);

    // update shape
    shape->set_xlowerjawpos(jawPos[0]);
    shape->set_xupperjawpos(jawPos[1]);
    shape->set_ylowerjawpos(jawPos[2]);
    shape->set_yupperjawpos(jawPos[3]);
    shape->set_mlcdirectiontype(machine->get_mlcdirectiontype());
    shape->set_leafpositions(vecLeafPos);

    free(contour.x);
    free(contour.y);
    return true;
}


bool CreateBeamCmd::AutoShapeBlock_i(
    const RtBeam& beam,
    RtMachine* const machine,
    const Mcsf::Matrix4f& matC2P,
    RtMlcshape* shape,
    RtBlock* block)
{
    if (block == nullptr || shape == nullptr || machine == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The block or machine is null.";
        return false;
    }

    if (!matC2P.HasInverse())
    {
        TPS_LOG_DEV_ERROR << "matBeam2Pat has no inverse.";
        return false;
    }

    tps::ROIEntity* pVOI = nullptr;
    std::string targetVoiUid = block->get_targetvoiuid();
    pVOI = mDataRepository->GetVoiManager()->GetROI(targetVoiUid);
    if (pVOI == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Target VOI is empty.";
        return false;
    }
    const std::string sSeriesUid = pVOI->GetSeriesUid();

    tps::ROIEntity* pVOIAvoid = nullptr;
    std::string avoidVoiUid = block->get_avoidvoiuid();
    if (!avoidVoiUid.empty() && avoidVoiUid != targetVoiUid)
    {
        pVOIAvoid = mDataRepository->GetVoiManager()->GetROI(avoidVoiUid);
    }

    //get image3d
    tps::TpsImage3DEntity* pImage3D = nullptr;
    bool bRet = mDataRepository->GetImageDataManager()->GetImage3DData(sSeriesUid, &pImage3D);
    if (!bRet || nullptr == pImage3D)
    {
        TPS_LOG_DEV_ERROR << "GetImage3DData failed or pImage3D is null.";
        return false;
    }
    //get matrix
    Mcsf::Matrix4f mat4fVol2Pat;
    pImage3D->GetVolumeToPatientMatrix(mat4fVol2Pat);
    float matVol2Pat[4][4];
    mat4fVol2Pat.GetElements(&matVol2Pat[0][0]);

    Mcsf::Matrix4f matPat2Beam = matC2P.Inverse();
    float fPat2Beam[4][4];
    matPat2Beam.GetElements(&fPat2Beam[0][0]);

    float fBeam2Pat[4][4];
    matC2P.GetElements(&fBeam2Pat[0][0]);

    plunc::CONTOUR_STC contour;
    memset(&contour, 0, sizeof(contour));
    float jawBdry[4] =
    {
        mJawBoundary[0],
        mJawBoundary[3],
        mJawBoundary[4],
        mJawBoundary[7],
    };
    float shappingField[4];
    memcpy(shappingField, jawBdry, 4 * sizeof(float));
    bool isJawFollow = beam.get_jawtype() == RtDbDef::JAWTYPE_Follow;
    if (!isJawFollow)
    {
        shappingField[0] = TPS_MAX(shappingField[0], shape->get_xlowerjawpos());
        shappingField[1] = TPS_MIN(shappingField[1], shape->get_xupperjawpos());
        shappingField[2] = TPS_MAX(shappingField[2], shape->get_ylowerjawpos());
        shappingField[3] = TPS_MIN(shappingField[3], shape->get_yupperjawpos());
    }

    if (pVOI == nullptr)
    {
        AutoShapeUtility::GenerateOutlineForOnlyAvoid(matVol2Pat,
            pVOIAvoid, block->get_avoidmarginx(), block->get_avoidmarginy(), 
            fPat2Beam, shappingField, &contour);
    }
    else
    {
        AutoShapeUtility::GenerateOutlineFromVoi(matVol2Pat,
            pVOI, block->get_targetmarginx(), block->get_targetmarginy(), 
            pVOIAvoid, block->get_avoidmarginx(), block->get_avoidmarginy(), 
            fPat2Beam, shappingField, &contour);
    }

    std::vector<db_Point2f> blockPts;
    blockPts.reserve(contour.vertex_count);
    double boundaries[4] =
    {
        jawBdry[1], jawBdry[0], jawBdry[3], jawBdry[2],
    };
    for (int i = 0; i < contour.vertex_count; i++)
    {
        if (contour.x[i] < boundaries[0]) boundaries[0] = contour.x[i];
        if (contour.x[i] > boundaries[1]) boundaries[1] = contour.x[i];
        if (contour.y[i] < boundaries[2]) boundaries[2] = contour.y[i];
        if (contour.y[i] > boundaries[3]) boundaries[3] = contour.y[i];
        blockPts.push_back(db_Point2f(contour.x[i], contour.y[i]));
    }
    block->set_points(blockPts);
    free(contour.x);
    free(contour.y);
    //if add shielding block, do not need to add mlc shape
    if (block->get_blocktype() == RtDbDef::SHIELDING) return true;

    //the leaves' position set to be all open
    db_Point2d leafPos;
    leafPos.x = machine->get_negativeleafrangemin();
    leafPos.y = machine->get_positiveleafrangemax();
    int leafNum = machine->get_leafnum();
    std::vector<db_Point2d> vecLeafPos(leafNum, leafPos);

    //set jaw position
    float jawPos[4];
    if (isJawFollow)
    {
        jawPos[0] = boundaries[0] - beam.get_jawmarginx();
        jawPos[1] = boundaries[1] + beam.get_jawmarginx();
        jawPos[2] = boundaries[2] - beam.get_jawmarginy();
        jawPos[3] = boundaries[3] + beam.get_jawmarginy();
    }
    else
    {
        jawPos[0] = shape->get_xlowerjawpos();
        jawPos[1] = shape->get_xupperjawpos();
        jawPos[2] = shape->get_ylowerjawpos();
        jawPos[3] = shape->get_yupperjawpos();
    }
    jawPos[0] = TPS_CLAMP(jawPos[0], mJawBoundary[0], mJawBoundary[1]);
    jawPos[1] = TPS_CLAMP(jawPos[1], mJawBoundary[2], mJawBoundary[3]);
    jawPos[2] = TPS_CLAMP(jawPos[2], mJawBoundary[4], mJawBoundary[5]);
    jawPos[3] = TPS_CLAMP(jawPos[3], mJawBoundary[6], mJawBoundary[7]);

    // update shape
    shape->set_xlowerjawpos(jawPos[0]);
    shape->set_xupperjawpos(jawPos[1]);
    shape->set_ylowerjawpos(jawPos[2]);
    shape->set_yupperjawpos(jawPos[3]);
    shape->set_mlcdirectiontype(machine->get_mlcdirectiontype());
    shape->set_leafpositions(vecLeafPos);
    return true;
}

bool CreateBeamCmd::AutoShapeJaw_i(
    const RtBeam& beam,
    RtMachine* const machine,
    const Mcsf::Matrix4f& matC2P,
    RtMlcshape* shape)
{
    if (shape == nullptr || machine == nullptr) return false;
    if (!matC2P.HasInverse())
    {
        TPS_LOG_DEV_ERROR << "matBeam2Pat has no inverse.";
        return false;
    }

    std::string sSeriesUid("");

    std::string targetVoiUid = beam.get_targetvoiuid();
    tps::ROIEntity* pVOI = nullptr;
    if (!targetVoiUid.empty())
    {
        pVOI = mDataRepository->GetVoiManager()->GetROI(targetVoiUid);
        if (nullptr != pVOI)
        {
            sSeriesUid = pVOI->GetSeriesUid();
        }
    }

    std::string avoidVoiUid = beam.get_avoidvoiuid();
    tps::ROIEntity* pVOIAvoid = nullptr;
    if (!avoidVoiUid.empty() && avoidVoiUid != targetVoiUid)
    {
        pVOIAvoid = mDataRepository->GetVoiManager()->GetROI(avoidVoiUid);
        if (nullptr != pVOIAvoid && sSeriesUid.empty())
        {
            sSeriesUid = pVOIAvoid->GetSeriesUid();
        }
    }
    if (pVOI == nullptr && pVOIAvoid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "There is no target voi & avoid voi, can not do auto shape.";
        return false;
    }

    //get image3d
    tps::TpsImage3DEntity* pImage3D = nullptr;
    bool bRet = mDataRepository->GetImageDataManager()->GetImage3DData(sSeriesUid, &pImage3D);
    if (!bRet || nullptr == pImage3D)
    {
        TPS_LOG_DEV_ERROR << "GetImage3DData failed or pImage3D is null.";
        return false;
    }
    //get matrix
    Mcsf::Matrix4f mat4fVol2Pat;
    pImage3D->GetVolumeToPatientMatrix(mat4fVol2Pat);
    float matVol2Pat[4][4];
    mat4fVol2Pat.GetElements(&matVol2Pat[0][0]);

    Mcsf::Matrix4f matPat2Beam = matC2P.Inverse();
    float fPat2Beam[4][4];
    matPat2Beam.GetElements(&fPat2Beam[0][0]);
    float fBeam2Pat[4][4];
    matC2P.GetElements(&fBeam2Pat[0][0]);

    float shappingField[4] =
    {
        mJawBoundary[0],
        mJawBoundary[3],
        mJawBoundary[4],
        mJawBoundary[7]
    };

    float jawPos[4];
    if (pVOI == nullptr)
    {
        //IF user didn't specify target voi, use default field 10cm * 10cm.
        shappingField[0] = -0.5 * DEFAULT_FIELD_SIZE;
        shappingField[1] =  0.5 * DEFAULT_FIELD_SIZE;
        shappingField[2] = -0.5 * DEFAULT_FIELD_SIZE;
        shappingField[3] =  0.5 * DEFAULT_FIELD_SIZE;
        AutoShapeUtility::GenerateBoundingBoxForOnlyAvoid(matVol2Pat,
            pVOIAvoid, beam.get_avoidmarginx(),beam.get_avoidmarginy(),
            fPat2Beam, shappingField, jawPos);
    }
    else
    {
        AutoShapeUtility::GenerateBoundingBoxFromVoi(matVol2Pat,
            pVOI, beam.get_targetmarginx(),beam.get_targetmarginy(),
            pVOIAvoid, beam.get_avoidmarginx(),beam.get_avoidmarginy(), 
            fPat2Beam, shappingField, jawPos);
    }
    double djawPos[4] =
    {
        TPS_CLAMP(jawPos[0], mJawBoundary[0], mJawBoundary[1]),
        TPS_CLAMP(jawPos[1], mJawBoundary[2], mJawBoundary[3]),
        TPS_CLAMP(jawPos[2], mJawBoundary[4], mJawBoundary[5]),
        TPS_CLAMP(jawPos[3], mJawBoundary[6], mJawBoundary[7]),
    };

    //create default MLC shape
    int leafNum = machine->get_leafnum();

    //the leaves' position set to be all open
    db_Point2d leafPos;
    leafPos.x = machine->get_negativeleafrangemin();
    leafPos.y = machine->get_positiveleafrangemax();
    std::vector<db_Point2d> vecLeafPos(leafNum, leafPos);

    // update shape
    shape->set_xlowerjawpos(djawPos[0]);
    shape->set_xupperjawpos(djawPos[1]);
    shape->set_ylowerjawpos(djawPos[2]);
    shape->set_yupperjawpos(djawPos[3]);
    shape->set_mlcdirectiontype(machine->get_mlcdirectiontype());
    shape->set_leafpositions(vecLeafPos);
    return true;
}

bool CreateBeamCmd::CreateRectShape_i(RtMachine* const machine,
    RtMlcshape* shape, double* rectSize/* = nullptr*/, int mode/* = 0*/)
{
    bool isAllOpen = rectSize == nullptr;
    double minBoundary = machine->get_negativeleafrangemin();
    double maxBoundary = machine->get_positiveleafrangemax();
    float fLeafMinGap = machine->get_minleafendgap();
    std::vector<double> leafBounds = machine->get_leafboundaries_list();

    int leafNum = machine->get_leafnum();
    RtDbDef::MLC_LEAF_DIRECTION_TYPE leafDirt = 
        (RtDbDef::MLC_LEAF_DIRECTION_TYPE)machine->get_mlcdirectiontype();
    double leafPos1 = (!isAllOpen && (mode & Leaf_Followed)) ? (leafDirt == RtDbDef::MLCX_DIRECTION ?
        rectSize[0] : rectSize[2]) : minBoundary;
    double leafPos2 = (!isAllOpen && (mode & Leaf_Followed)) ? (leafDirt == RtDbDef::MLCX_DIRECTION ?
        rectSize[1] : rectSize[3]) : maxBoundary;
    std::vector<db_Point2d> vecLeafPos;
    vecLeafPos.reserve(leafNum);
    db_Point2d leafPos;
    for (int i = 0; i < leafNum; i++)
    {
        if (!isAllOpen && (mode & Leaf_Followed))
        {
            if (leafBounds[i] >= rectSize[2] && leafBounds[i] < rectSize[3])
            {
                leafPos.x = leafPos1;
                leafPos.y = leafPos2;
            }
            else
            {
                leafPos.x = -0.5 * fLeafMinGap;
                leafPos.y = 0.5 * fLeafMinGap;
            }
        }
        else
        {
            leafPos.x = leafPos1;
            leafPos.y = leafPos2;
        }

        vecLeafPos.push_back(leafPos);
    }

    double jawBdry[4] =
    {
        mJawBoundary[0],
        mJawBoundary[3],
        mJawBoundary[4],
        mJawBoundary[7],
    };
    double jawPos[4];
    memcpy(jawPos, jawBdry, 4 * sizeof(double));
    if (!isAllOpen && (mode & Jaw_Followed))
    {
        jawPos[0] = TPS_CLAMP(rectSize[0], jawBdry[0], jawBdry[1]);
        jawPos[1] = TPS_CLAMP(rectSize[1], jawBdry[0], jawBdry[1]);
        jawPos[2] = TPS_CLAMP(rectSize[2], jawBdry[2], jawBdry[3]);
        jawPos[3] = TPS_CLAMP(rectSize[3], jawBdry[2], jawBdry[3]);
    }

    shape->set_xlowerjawpos(jawPos[0]);
    shape->set_xupperjawpos(jawPos[1]);
    shape->set_ylowerjawpos(jawPos[2]);
    shape->set_yupperjawpos(jawPos[3]);
    shape->set_mlcdirectiontype(machine->get_mlcdirectiontype());
    shape->set_leafpositions(vecLeafPos);
    return true;
}

bool CreateBeamCmd::ValidateData(RtDbDef::PLAN_TYPE type, RtDbDef::RADIATION_TYPE radType) const
{
    // The new requirements from yige.zhang  Apr\30\2015
    bool res = true;
    switch (type)
    {
        case RtDbDef::PLAN_3D_CRT:
        case RtDbDef::PLAN_Burst_Arc:
        case RtDbDef::PLAN_VMAT_Arc:
        case RtDbDef::PLAN_2D:
            break;
        case RtDbDef::PLAN_Step_Shoot:
        case RtDbDef::PLAN_Sliding_Window:
            if (mCertifiedBeam.mApertureType != RtDbDef::Mlc)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support beam without mlc shape, plan type: " << type;
                res = false;
            }
            if (mCertifiedBeam.mUseShieldingBlock)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support shield block, plan type: " << type;
                res = false;
            }
            if (mCertifiedBeam.mUsePhysicalWedge || mCertifiedBeam.mUseVirtualWedge)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support wedge, plan type: " << type;
                res = false;
            }
            break;
        case RtDbDef::PLAN_Conformal_Arc:
            if (mCertifiedBeam.mApertureType == RtDbDef::Applicator)
            {
                TPS_LOG_DEV_ERROR << "Conformal arc plan does not support beam using Cone.";
                res = false;
            }
            if (mCertifiedBeam.mUsePhysicalWedge)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support physical wedge.";
                res = false;
            }
            if (mCertifiedBeam.mUseShieldingBlock || mCertifiedBeam.mUseEBeamBlock)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support block.";
                res = false;
            }
        case RtDbDef::PLAN_Simple_Arc:
            if (mCertifiedBeam.mApertureType == RtDbDef::Aperture_Block 
                || mCertifiedBeam.mUseShieldingBlock
                || mCertifiedBeam.mUseEBeamBlock)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support block, plan type: "<<type;
                res = false;
            }
            if (mCertifiedBeam.mUsePhysicalWedge)
            {
                TPS_LOG_DEV_ERROR << "Current plan does not support physical wedge.";
                res = false;
            }
            break;
        default:
            TPS_LOG_DEV_ERROR << "Unrecognized plan type:" << type;
            res = false;
    }

    if (radType == RtDbDef::RADIATION_ELETRON && mCertifiedBeam.mApplicatorUid.empty())
    {
        TPS_LOG_DEV_ERROR << "There is no applicator in electron beam.";
        res = false;
    }
    return res;
}

int CreateBeamCmd::CreateTreatmentBeam(RtBeam*& beam, RtNormgroup* normGroup, const std::string& imageUID)
{
    // 先判断必要条件，不满足即返回失败
    // 条件:isocenter
    std::string isoCenterUID = mCertifiedBeam.mIsocenterUid;
    RtPoi* isoCenter = mDataRepository->GetPoiManager()->GetPOI(isoCenterUID);
    if (isoCenter == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist of uid: " << isoCenterUID;
        return TPS_ER_FAILURE;
    }

    // 条件：plan type radType
    RtDbDef::PLAN_TYPE planType = (RtDbDef::PLAN_TYPE)normGroup->get_plantype();
    RtDbDef::RADIATION_TYPE radType = (RtDbDef::RADIATION_TYPE)normGroup->get_radiationtype();
    if (!ValidateData(planType, radType))
    {
        TPS_LOG_DEV_ERROR << "Failed to validate beam parameters.";
        return TPS_ER_FAILURE;
    }

    // 条件：machine
    RtMachine* machine = nullptr;
    std::string machineUid = normGroup->get_machineuid();
    if (!mDataRepository->GetMachineManager()->GetMachine(machineUid, machine))
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine[" << machineUid << "]";
        return TPS_ER_FAILURE;
    }

    // 开始填值
    beam = new RtBeam(true);
    SetCommonBeamData_i(planType, beam);

    // wedge
    if (!CreateWedge_i(beam))
    {
        TPS_LOG_DEV_ERROR << "Failed to create wedge for beam[" << mCertifiedBeam.mBeamUID << "].";
        DEL_PTR(beam);
        return TPS_ER_FAILURE;
    }

    // bolus
    beam->set_bolusvoiuid(mCertifiedBeam.mBolusVoiUid);

    // set jaw boudary according machine config and wedge
    SetJawBoundary(machine, mCertifiedBeam.mPhysicalWedgeUid, mCertifiedBeam.mVirtualWedgeUid);

    // IMRT and ARC
    if (!CreateBeamSegments(imageUID, beam, planType, machine, isoCenter))
    {
        TPS_LOG_DEV_ERROR << "Failed to create segments for beam[" << mCertifiedBeam.mBeamUID << "].";
        DEL_PTR(beam);
        return TPS_ER_FAILURE;
    }

    if (mCertifiedBeam.mUseShieldingBlock && planType == RtDbDef::PLAN_3D_CRT)
    {
        if (!CreateShieldingBlock_i(beam, machine))
        {
            TPS_LOG_DEV_ERROR << "Failed to create sheidingblock for beam[" << mCertifiedBeam.mBeamUID << "].";
            DEL_PTR(beam);
            return TPS_ER_FAILURE;
        }
    }
    return TPS_ER_SUCCESS;
}

void CreateBeamCmd::SetJawBoundary(
    const RtMachine* machine,
    const std::string& sPhyicalUid,
    const std::string& sVirtualUid)
{
    mJawBoundary[0] = machine->get_xmin_1();
    mJawBoundary[1] = machine->get_xmax_1();
    mJawBoundary[2] = machine->get_xmin_2();
    mJawBoundary[3] = machine->get_xmax_2();
    mJawBoundary[4] = machine->get_ymin_1();
    mJawBoundary[5] = machine->get_ymax_1();
    mJawBoundary[6] = machine->get_ymin_2();
    mJawBoundary[7] = machine->get_ymax_2();

    RtAccessory* pPhysicalWedge = nullptr;
    if (!sPhyicalUid.empty())
    {
        mDataRepository->GetMachineManager()->GetAccessory(sPhyicalUid, pPhysicalWedge);
        mJawBoundary[0] = TPS_MAX(mJawBoundary[0], pPhysicalWedge->get_min1_jaw1());
        mJawBoundary[1] = TPS_MIN(mJawBoundary[1], pPhysicalWedge->get_max1_jaw1());
        mJawBoundary[2] = TPS_MAX(mJawBoundary[2], pPhysicalWedge->get_min2_jaw1());
        mJawBoundary[3] = TPS_MIN(mJawBoundary[3], pPhysicalWedge->get_max2_jaw1());
        mJawBoundary[4] = TPS_MAX(mJawBoundary[4], pPhysicalWedge->get_min2_jaw2());
        mJawBoundary[5] = TPS_MIN(mJawBoundary[5], pPhysicalWedge->get_max2_jaw2());
        mJawBoundary[6] = TPS_MAX(mJawBoundary[6], pPhysicalWedge->get_min1_jaw2());
        mJawBoundary[7] = TPS_MIN(mJawBoundary[7], pPhysicalWedge->get_max1_jaw2());
    }

    RtAccessory* pVirtualWedge = nullptr;
    if (!sVirtualUid.empty())
    {
        mDataRepository->GetMachineManager()->GetAccessory(sVirtualUid, pVirtualWedge);
        mJawBoundary[0] = TPS_MAX(mJawBoundary[0], pVirtualWedge->get_min1_jaw1());
        mJawBoundary[1] = TPS_MIN(mJawBoundary[1], pVirtualWedge->get_max1_jaw1());
        mJawBoundary[2] = TPS_MAX(mJawBoundary[2], pVirtualWedge->get_min2_jaw1());
        mJawBoundary[3] = TPS_MIN(mJawBoundary[3], pVirtualWedge->get_max2_jaw1());
        mJawBoundary[4] = TPS_MAX(mJawBoundary[4], pVirtualWedge->get_min2_jaw2());
        mJawBoundary[5] = TPS_MIN(mJawBoundary[5], pVirtualWedge->get_max2_jaw2());
        mJawBoundary[6] = TPS_MAX(mJawBoundary[6], pVirtualWedge->get_min1_jaw2());
        mJawBoundary[7] = TPS_MIN(mJawBoundary[7], pVirtualWedge->get_max1_jaw2());
    }
}

RtBeamsegment* CreateBeamCmd::CreateBasicBeamSegmentObject_i(
    int iControlPointIndex,
    float segmentStartAngle,
    float segmentArcLength,
    float tableAngle,
    float collimatorAngle,
    float matB2P[16],
    float beamWeight,
    RtDbDef::ROTATION_DIRECTION directionOfRot,
    const std::string& beamUid)
{
    RtBeamsegment* segment = new RtBeamsegment(true);
    segment->set_sequencenumber(iControlPointIndex);
    segment->set_startgantryangle(segmentStartAngle);
    segment->set_arclength(segmentArcLength);
    segment->set_starttablerotationangle(tableAngle);
    segment->set_startcollimatorangle(collimatorAngle);
    segment->set_endtablerotationangle(tableAngle);
    segment->set_endcollimatorangle(collimatorAngle);
    segment->set_t_beam_to_pat(matB2P);
    segment->set_weight(beamWeight);
    segment->set_gantryrotationdirection(directionOfRot);
    segment->set_collimatorrotationdirection(RtDbDef::NONE_ROTATION_DIRECTION);//default direction
    segment->set_beamuid(beamUid);
    return segment;
}

void CreateBeamCmd::CreateArcSegmentShape_i(
    const RtBeam& beam,
    RtMachine* machine,
    const Mcsf::Matrix4f& matC2P,
    RtMlcshape* shape,
    MLC_EDIT_TYPE mlcEditType)
{
    std::string targetVoiUid = beam.get_targetvoiuid();
    std::string avoidVoiUid = beam.get_avoidvoiuid();
    if (!targetVoiUid.empty() || !avoidVoiUid.empty())
    {
        //Mlc Target和Avoid有选中Voi，auto-shape
        //如果只有Avoid，而没有Target，默认10*10(cm)方野并去除avoid voi。
        //其他情况，正常逻辑
        AutoShapeMlc_i(beam, machine, matC2P, shape, mlcEditType);
    }
    else
    {
        //如果没有选中任何voi，则默认10*10(cm)方野
        double defaultShape[4] =
        {
            -0.5 * DEFAULT_FIELD_SIZE,
            0.5 * DEFAULT_FIELD_SIZE,
            -0.5 * DEFAULT_FIELD_SIZE,
            0.5 * DEFAULT_FIELD_SIZE
        };
        //如果手动设定了Jaw位置，使用设定位置产生矩形野
        bool isJawFollow = beam.get_jawtype() == RtDbDef::JAWTYPE_Follow;
        if (!isJawFollow)
        {
            defaultShape[0] = shape->get_xlowerjawpos();
            defaultShape[1] = shape->get_xupperjawpos();
            defaultShape[2] = shape->get_ylowerjawpos();
            defaultShape[3] = shape->get_yupperjawpos();
        }
        CreateRectShape_i(machine, shape, defaultShape, Jaw_Followed | Leaf_Followed);
    }
}

bool CreateBeamCmd::CreateImrtAperture_i(
    const RtBeam& beam,
    RtMachine* const machine,
    const Mcsf::Matrix4f& matC2P,
    RtMlcshape* mlcShape,
    RtBlock* block,
    MLC_EDIT_TYPE mlcEditType)
{
    if (mlcShape == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The mlc shape is null.";
        return false;
    }
    if (mCertifiedBeam.mSegmentList.empty())
    {
        TPS_LOG_DEV_ERROR << "The segment list is empty.";
        return false;
    }
    TpsCertifiedSegment certSegment = mCertifiedBeam.mSegmentList[0];

    bool res = false;
    auto apertureType = mCertifiedBeam.mApertureType;
    std::string targetVoiUid, avoidVoiUid;
    switch (apertureType)
    {
    case RtDbDef::Mlc:
        {
            targetVoiUid = beam.get_targetvoiuid();
            avoidVoiUid = beam.get_avoidvoiuid();
            if (!targetVoiUid.empty() || !avoidVoiUid.empty())
            {
                //Mlc Target和Avoid有选中Voi，auto-shape
                //如果只有Avoid，而没有Target，默认10*10(cm)方野并去除avoid voi。
                //其他情况，正常逻辑
                res = AutoShapeMlc_i(beam, machine, matC2P, mlcShape, mlcEditType);
                break;
            }
            //如果没有选中任何voi，则默认10*10(cm)方野
            double defaultShape[4] =
            {
                -0.5 * DEFAULT_FIELD_SIZE,
                0.5 * DEFAULT_FIELD_SIZE,
                -0.5 * DEFAULT_FIELD_SIZE,
                0.5 * DEFAULT_FIELD_SIZE
            };
            // 如果手动设定了Jaw位置，使用设定位置产生矩形野
            if (!mCertifiedBeam.mIsJawFollow)
            {
                defaultShape[0] = certSegment.startJaw.x1;
                defaultShape[1] = certSegment.startJaw.x2;
                defaultShape[2] = certSegment.startJaw.y1;
                defaultShape[3] = certSegment.startJaw.y2;
            }
            res = CreateRectShape_i(machine, mlcShape, defaultShape, Jaw_Followed | Leaf_Followed);
            break;
        }
    case RtDbDef::JawOnly:
        {
            targetVoiUid = beam.get_targetvoiuid();
            avoidVoiUid = beam.get_avoidvoiuid();
            if (!targetVoiUid.empty() || !avoidVoiUid.empty())
            {
                //Jaw Only, Target或Avoid有选中，则Auto Shape
                //如果仅有Avoid而未设定Target，默认10*10(cm)方野并去除Avoid 部分
                res = AutoShapeJaw_i(beam, machine, matC2P, mlcShape);
                break;
            }
            //如果没有指定任何voi，并且选中Auto Shape，则直接默认10*10(cm)方野
            double defaultShape[4] =
            {
                -0.5 * DEFAULT_FIELD_SIZE,
                0.5 * DEFAULT_FIELD_SIZE,
                -0.5 * DEFAULT_FIELD_SIZE,
                0.5 * DEFAULT_FIELD_SIZE
            };
            // 如果手动设定了Jaw位置，使用设定位置产生矩形野
            if (!mCertifiedBeam.mIsJawFollow) {
                defaultShape[0] = certSegment.startJaw.x1;
                defaultShape[1] = certSegment.startJaw.x2;
                defaultShape[2] = certSegment.startJaw.y1;
                defaultShape[3] = certSegment.startJaw.y2;
            }
            res = CreateRectShape_i(machine, mlcShape, defaultShape, Jaw_Followed);
            break;
        }
    case RtDbDef::Aperture_Block:
        {
            if (block == nullptr)
            {
                TPS_LOG_DEV_ERROR << "The block object is null.";
                return false;
            }
            targetVoiUid = block->get_targetvoiuid();
            //与MLC逻辑类似
            if (!targetVoiUid.empty())
            {
                //Block Target和Avoid有选中Voi，auto-shape
                //这里不考虑只有Avoid没有Target的情况，block默认大小为全闭（Aperture）或全开（Shielding），没有影响。
                //对于这里添加Aperture Block，需要添加Mlc Shape
                res = AutoShapeBlock_i(beam, machine, matC2P, mlcShape, block);
                break;
            }
            //包括两种情况：1.只有Avoid voi没有Target 2.没有选择任何voi做适形
            //创建Mlc shape，Leaf保持全开，Jaw如果已设定，采用设定值，否则也全开
            if (!mCertifiedBeam.mIsJawFollow)
            {
                double defaultShape[4] =
                { 
                    certSegment.startJaw.x1,
                    certSegment.startJaw.x2,
                    certSegment.startJaw.y1,
                    certSegment.startJaw.y2
                };
                res = CreateRectShape_i(machine, mlcShape, defaultShape, Jaw_Followed);
            }
            else
            {
                res = CreateRectShape_i(machine, mlcShape);
            }
            break;
        }
    case RtDbDef::Applicator:
        //adjust jaw to cone
        res = CreateApplicatorShape_i(machine, mlcShape);
        break;
    default:
        TPS_LOG_DEV_ERROR << "Unrecognized aperture type!";
        break;
    }
    return res;
}

void CreateBeamCmd::CalculateBeamsegmentAngles_i(
    float gantryStartAngle,
    float gantryArcLength,
    bool bIsClockWise,
    std::vector<float>& vecSegmentAngles)
{
    double deltaAngle = 0.0, angle;
    int sign = bIsClockWise ? 1 : -1;
    while (deltaAngle - gantryArcLength < -1e-6)
    {
        angle = gantryStartAngle + sign * deltaAngle;
        angle -= floor(angle / 360) * 360;
        vecSegmentAngles.push_back(angle);
        deltaAngle += INTERVAL_DEGREE;
    }
    angle = gantryStartAngle + sign * gantryArcLength;
    angle -= floor(angle / 360) * 360;
    vecSegmentAngles.push_back(angle);
}

void CreateBeamCmd::SetCommonBeamData_i(RtDbDef::PLAN_TYPE planType, RtBeam* beam)
{
    if (nullptr == beam)
    {
        TPS_LOG_DEV_ERROR << "beam is null.";
        return;
    }
    RtDbDef::BEAM_TYPE beamType = TpsCmdHelper::GetBeamTypeViaPlanType(planType);
    beam->set_beamtype(beamType);
    beam->set_uid(mCertifiedBeam.mBeamUID);
    beam->set_beamnumber(mCertifiedBeam.mBeamNumber);
    // 2015/07/14, SGXQ, now set treatment beam as default delivery type, in the future may change
    beam->set_beamdeliverytype(RtDbDef::TREATMENT_BEAM);
    beam->set_doserate(mCertifiedBeam.mDoseRate);
    beam->set_isjoincalc(mCertifiedBeam.mIsJoined);
    beam->set_isjoinopt(mCertifiedBeam.mIsJoined);
    beam->set_isvisible(mCertifiedBeam.mIsVisible);
    beam->set_name(mCertifiedBeam.mBeamName);
    beam->set_isautocreated(mCertifiedBeam.mIsAutoCreated);
    beam->set_description(mCertifiedBeam.mBeamDescription);
    beam->set_mlcedittype(mCertifiedBeam.mmlcedittype);
    if (mCertifiedBeam.mHasBeamWeight)
    {
        beam->set_beamweight(mCertifiedBeam.mBeamWeight);
    } 
    beam->set_beammu(mCertifiedBeam.mBeamMU);
    beam->set_normgroupuid(mCertifiedBeam.mNormGroupUID);
    beam->set_commissionedunituid(mCertifiedBeam.mCommissionedUnitUID);
    beam->set_isocenterpoiuid(mCertifiedBeam.mIsocenterUid);
    beam->set_trayuid(mCertifiedBeam.mTrayUid);
    // Current TMS Import logic check this field, this should remove after TMS update Import/Export logic
    beam->set_finalcumulativemetersetweight(1);
    beam->set_dosealgorithmtype(mCertifiedBeam.mDoseAlgorithmType);
    beam->set_ssd(mCertifiedBeam.mSsd);
    beam->set_aperturetype(mCertifiedBeam.mApertureType);
}

bool CreateBeamCmd::CreateBeamSegments(
    const std::string& imageUID,
    RtBeam* beam,
    RtDbDef::PLAN_TYPE planType,
    RtMachine* machine,
    RtPoi* isoCenter)
{
    if (beam == nullptr || machine == nullptr || isoCenter == nullptr
        || imageUID.empty() || planType == RtDbDef::PLAN_UNKNOWN_TYPE)
    {
        TPS_LOG_DEV_ERROR << "beam/machine/isoCenter/imageUID/planType parameter error";
        return false;
    }

    std::string beamUID = beam->get_uid();
    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    // delete old segments
    for (auto itrSeg = segments.begin(); itrSeg != segments.end(); ++itrSeg)
    {
        if (TPS_ER_SUCCESS != mSaveObjects->EraseObject((*itrSeg)->get_uid(), RtDbDef::TYPE_BEAMSEGMENT))
        {
            TPS_LOG_DEV_ERROR << "Failed to delete segment:" << (*itrSeg)->get_uid();
            return false;
        }
    }
    segments.clear();

    Mcsf::Point3f isoCenter3f(isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(), isoCenter->get_coordinate3dz());

    if (!TpsCmdHelper::IsArcBeam(planType))
    {
        RtBeamsegment* pSegment = nullptr;
        if (!CreateImrtSegment_i(beam,
            imageUID, machine,
            mCertifiedBeam.mGantryStartAngle,
            mCertifiedBeam.mCouchAngle,
            mCertifiedBeam.mCollimatorAngle,
            isoCenter3f,
            planType,
            (RtDbDef::APERTURE_TYPE)mCertifiedBeam.mApertureType,
            (MLC_EDIT_TYPE)mCertifiedBeam.mmlcedittype,
            pSegment) || nullptr == pSegment)
        {
            TPS_LOG_DEV_ERROR << "Failed to create single segment.";
            return false;
        }
        segments.push_back(pSegment);
    }
    else
    {
        CreateArcSegments_i(*beam,
            imageUID,
            mCertifiedBeam.mIsClockWise,
            mCertifiedBeam.mGantryStartAngle,
            mCertifiedBeam.mGantryArcLength,
            mCertifiedBeam.mCouchAngle,
            mCertifiedBeam.mCollimatorAngle,
            machine,
            beamUID,
            isoCenter3f,
            planType,
            segments);
    }

    double offset = CalculateMiniGapOffset_i(planType, segments, machine);
    if (offset > 0)
    {
        CorrectBeamLeafGap_i(offset, machine, segments);
    }

    beam->set_beamsegments(segments);

    for (auto itr = segments.begin(); itr != segments.end(); ++itr) DEL_PTR(*itr);
    return true;
}

double CreateBeamCmd::CalculateMiniGapOffset_i(RtDbDef::PLAN_TYPE planType,
    std::vector<RtBeamsegment*>& segmentList, RtMachine* machine)
{
    if (planType == RtDbDef::PLAN_UNKNOWN_TYPE || segmentList.size() < 1)
    {
        TPS_LOG_DEV_ERROR << "CorrectBeamLeafGap_i: input parameter invalid.";
        return -1;
    }

    if (planType == RtDbDef::PLAN_3D_CRT || planType == RtDbDef::PLAN_Step_Shoot || planType == RtDbDef::PLAN_Simple_Arc)
    {
       return 0;
    }

    double offset;
    auto leafPos = segmentList[0]->get_startmlcshape()->get_leafpositions();
    double leftLeafMin =  leafPos[0].x;
    double rightLeafMax = leafPos[0].y;
    RtMlcshape* pStartMlcShape = nullptr;
    RtMlcshape* pEndMlcShape = nullptr;
    int i, j;
    for (i = 0; i < segmentList.size(); i++)
    {
        pStartMlcShape = segmentList[i]->get_startmlcshape();
        pEndMlcShape = segmentList[i]->get_endmlcshape();
        auto startLeafPosition = pStartMlcShape->get_leafpositions();
        auto endLeafPosition = pEndMlcShape->get_leafpositions();
        if (startLeafPosition.size() != endLeafPosition.size())
        {
            TPS_LOG_DEV_ERROR << "Mlc leaf number is different";
            return -1;
        }
        for (j = 0; j < startLeafPosition.size(); ++j)
        {
            leftLeafMin = TPS_MIN(startLeafPosition[j].x, leftLeafMin);
            leftLeafMin = TPS_MIN(endLeafPosition[j].x, leftLeafMin);
            rightLeafMax = TPS_MAX(startLeafPosition[j].y, rightLeafMax);
            rightLeafMax = TPS_MAX(endLeafPosition[j].y, rightLeafMax);
        }
    }


    const float gapoffset1 = mDataRepository->GetMachineManager()->GetLeafGapOffsetInSmallField();
    const float gapoffset2 = mDataRepository->GetMachineManager()->GetLeafGapOffsetInLargeField();

    if ((rightLeafMax - leftLeafMin) <= machine->get_leafspan())
    {
        offset = gapoffset1;
    }
    else
    {
        offset = gapoffset2;
    }

    return offset;
}

void CreateBeamCmd::CorrectBeamLeafGap_i(
    double offset,
    RtMachine* machine,
    std::vector<RtBeamsegment*>& segmentList)
{
    RtMlcshape* pStartMlcShape = nullptr;
    RtMlcshape* pEndMlcShape = nullptr;
    for (int i = 0; i < segmentList.size(); i++)
    {
        pStartMlcShape = segmentList[i]->get_startmlcshape();
        pEndMlcShape = segmentList[i]->get_endmlcshape();
        std::vector<db_Point2d> startLeafPosition = pStartMlcShape->get_leafpositions();
        std::vector<db_Point2d> endLeafPosition = pEndMlcShape->get_leafpositions();

        for (int j = 0; j < startLeafPosition.size(); j++)
        {
            if (abs(startLeafPosition[j].y - startLeafPosition[j].x - machine->get_minleafendgap()) < 1e-4)
            {
                startLeafPosition[j].x -= 0.5 * offset;
                startLeafPosition[j].y += 0.5 * offset;
            }

            if (abs(endLeafPosition[j].y - endLeafPosition[j].x - machine->get_minleafendgap()) < 1e-4)
            {
                endLeafPosition[j].x -= 0.5 * offset;
                endLeafPosition[j].y += 0.5 * offset;
            }
        }
        pStartMlcShape->set_leafpositions(startLeafPosition);
        pEndMlcShape->set_leafpositions(endLeafPosition);
    }
}

bool CreateBeamCmd::CreateShieldingBlock_i(RtBeam* beam, RtMachine* machine)
{
    if (nullptr == beam || nullptr == machine)
    {
        TPS_LOG_DEV_ERROR << "beam or machine is nullptr";
        return false;
    }

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();

    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << beam->get_uid() << " has no segment.";
        return false;
    }

    RtBeamsegment*& pSegment = segments[0];

    beam->create_shield_block();
    RtBlock* pBlock = beam->get_shield_block();
    SetBlockParameters_i(pBlock, machine);
    Mcsf::Matrix4f matC2P = TpsArithmeticConverter::ConvertToMatrix4f(
        pSegment->get_t_beam_to_pat());
    if (!mCertifiedBeam.mShieldingBlockTargetVoiUid.empty())
    {
        AutoShapeBlock_i(*beam, machine, matC2P, pSegment->get_startmlcshape(), pBlock);
    }

    return true;
}

bool CreateBeamCmd::CreateWedge_i(RtBeam* beam)
{
    if (nullptr == beam)
    {
        TPS_LOG_DEV_ERROR << "beam is nullptr";
        return false;
    }
    beam->set_physicalwedgeuid(mCertifiedBeam.mPhysicalWedgeUid);
    beam->set_virtualwedgeuid(mCertifiedBeam.mVirtualWedgeUid);

    //applicator
    if (mCertifiedBeam.mApplicatorUid.empty())
    {
        beam->disable_field(BEAM_APPLICATORUID);
    }
    else
    {
        beam->set_applicatoruid(mCertifiedBeam.mApplicatorUid);
    }
    return true;
}

bool CreateBeamCmd::CreateDosegridForBeam_i(RtPlan* plan, RtNormgroup* normGroup, RtBeam* beam)
{
    if (beam == nullptr || plan == nullptr || normGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR << "beam/normgroup/plan is nullptr";
        return false;
    }

    RtDosegrid* ngDosegrid = normGroup->get_dosegrid();
    if (nullptr == ngDosegrid)
    {
        return false;
    }

    if (ngDosegrid->get_dosegrid_buffer()==nullptr)
    {
        TPS_LOG_DEV_ERROR << "Invalid dose grid buffer under norm-group[" << normGroup->get_uid()<< "].";
        return false;
    }
    //beam->create_dosegrid();
    RtDosegrid* pDosegrid = beam->get_dosegrid();
    if (nullptr == pDosegrid)
    {
        return false;
    }
    pDosegrid->set_planuid("");
    pDosegrid->set_normgroupuid("");
    pDosegrid->set_beamuid(beam->get_uid());
    pDosegrid->set_uid(beam->get_uid());//same as the beam's uid

    pDosegrid->set_xcoordinate3dstart(ngDosegrid->get_xcoordinate3dstart());
    pDosegrid->set_ycoordinate3dstart(ngDosegrid->get_ycoordinate3dstart());
    pDosegrid->set_zcoordinate3dstart(ngDosegrid->get_zcoordinate3dstart());
    pDosegrid->set_xcoordinate3dinc(ngDosegrid->get_xcoordinate3dinc());
    pDosegrid->set_ycoordinate3dinc(ngDosegrid->get_ycoordinate3dinc());
    pDosegrid->set_zcoordinate3dinc(ngDosegrid->get_zcoordinate3dinc());
    pDosegrid->set_grid_to_pat_t(ngDosegrid->get_grid_to_pat_t());
    pDosegrid->set_xcount(ngDosegrid->get_xcount());
    pDosegrid->set_ycount(ngDosegrid->get_ycount());
    pDosegrid->set_zcount(ngDosegrid->get_zcount());
    pDosegrid->set_accuracy(ngDosegrid->get_accuracy());
    pDosegrid->create_dosegrid_buffer();
    float* buffer = pDosegrid->get_dosegrid_buffer();
    size_t bufferSize = pDosegrid->get_xcount() * 
        pDosegrid->get_ycount() * pDosegrid->get_zcount();
    memset(buffer, 0, bufferSize * sizeof(float));
    pDosegrid->set_isdosevalid(false);
    pDosegrid->set_bitsallocated(TPS_BITSALLOCATED);
    pDosegrid->set_bitsstored(TPS_BITSALLOCATED);
    pDosegrid->set_highbits(TPS_BITSALLOCATED -1);

    return true;
}

//set editing parameters for certified beam
void CreateBeamCmd::SetMlcShapeEditParameters_i(
    RtMachine* machine,
    RtBeam* pBeam,
    RtMlcshape* shape,
    int segmentIndex)
{
    segmentIndex = TPS_MIN(segmentIndex, mCertifiedBeam.mSegmentList.size() - 1);

    auto certSegment = mCertifiedBeam.mSegmentList[segmentIndex];
    shape->set_jawxissymmetry(mCertifiedBeam.mIsXSymmetric);
    shape->set_jawyissymmetry(mCertifiedBeam.mIsYSymmetric);
    shape->set_xlowerjawpos(certSegment.startJaw.x1);
    shape->set_xupperjawpos(certSegment.startJaw.x2);
    shape->set_ylowerjawpos(certSegment.startJaw.y1);
    shape->set_yupperjawpos(certSegment.startJaw.y2);
    shape->set_mlcdirectiontype(machine->get_mlcdirectiontype());

    //leaf
    pBeam->set_jawmarginx(mCertifiedBeam.mJawFollowMarginX);
    pBeam->set_jawmarginy(mCertifiedBeam.mJawFollowMarginY);
    pBeam->set_isautoshape(mCertifiedBeam.mIsAutoTrackingShape);
    pBeam->set_targetvoiuid(mCertifiedBeam.mTargetVoiUid);
    pBeam->set_targetmarginx(mCertifiedBeam.mTargetMarginX);
    pBeam->set_targetmarginy(mCertifiedBeam.mTargetMarginY);
    pBeam->set_avoidvoiuid(mCertifiedBeam.mAvoidVoiUid);
    pBeam->set_avoidmarginx(mCertifiedBeam.mAvoidMarginX);
    pBeam->set_avoidmarginy(mCertifiedBeam.mAvoidMarginY);
    pBeam->set_jawtype(mCertifiedBeam.mIsJawFollow ? 
        RtDbDef::JAWTYPE_Follow: RtDbDef::JAWTYPE_Manual);
}

//set editing parameters for certified beam
void CreateBeamCmd::SetBlockParameters_i(RtBlock* block, RtMachine* machine)
{
    // TPS该参数写死,DICOM导出需要该参数
    block->set_blockdivergence((int)RtDbDef::PRESENT);
    block->set_sourcetotraydistance(machine->get_sourcetoblocktraydistance());
    // machine中只填写了blockheight,没有填写blockthickness;这两个字段应该只保留一个
    block->set_blockthickness(machine->get_blockheight());

    if (RtDbDef::APERTURE == block->get_blocktype())
    {
        block->set_isautoshape(mCertifiedBeam.mIsAutoTrackingShape);
        block->set_targetvoiuid(mCertifiedBeam.mTargetVoiUid);
        block->set_targetmarginx(mCertifiedBeam.mTargetMarginX);
        block->set_targetmarginy(mCertifiedBeam.mTargetMarginY);
        block->set_avoidvoiuid(mCertifiedBeam.mAvoidVoiUid);
        block->set_avoidmarginx(mCertifiedBeam.mAvoidMarginX);
        block->set_avoidmarginy(mCertifiedBeam.mAvoidMarginY);
        // 目前TPSBeam最多只支持两个Block,ApertureBlock Num 1
        block->set_blocknumber(1);
    }
    else
    {
        block->set_blockname(mCertifiedBeam.mShieldingBlock.mBlockName);
        block->set_isautoshape(mCertifiedBeam.mIsShieldingBlockAutoTrackingShape);
        block->set_targetvoiuid(mCertifiedBeam.mShieldingBlockTargetVoiUid);
        block->set_targetmarginx(mCertifiedBeam.mShieldingBlockTargetMarginX);
        block->set_targetmarginy(mCertifiedBeam.mShieldingBlockTargetMarginY);
        block->set_avoidvoiuid(mCertifiedBeam.mShieldingBlockAvoidVoiUid);
        block->set_avoidmarginx(mCertifiedBeam.mShieldingBlockAvoidMarginX);
        block->set_avoidmarginy(mCertifiedBeam.mShieldingBlockAvoidMarginY);
        // 目前TPSBeam最多只支持两个Block,ShieldBlock Num 2
        block->set_blocknumber(2);
    }
}

bool CreateBeamCmd::CombineMlcShapes_i(
    const std::vector<RtMlcshape*>& vecMlcShape,
    RtMlcshape* shape,
    float miniLeafGap)
{
    if (vecMlcShape.empty())
    {
        TPS_LOG_DEV_ERROR << "The mlc shape list to be combined is empty.";
        return false;
    }
    if (shape == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The target mlc shape is null.";
        return false;
    }

    double jawPos[4] =
    {
        vecMlcShape[0]->get_xlowerjawpos(),
        vecMlcShape[0]->get_xupperjawpos(),
        vecMlcShape[0]->get_ylowerjawpos(),
        vecMlcShape[0]->get_yupperjawpos()
    };
    int direction = vecMlcShape[0]->get_mlcdirectiontype();
    auto leafPos = vecMlcShape[0]->get_leafpositions();

    RtMlcshape* tmpShape;
    std::vector<db_Point2d> tmpLeafPos;
    for (int i = 1; i < vecMlcShape.size(); ++i)
    {
        tmpShape = vecMlcShape[i];
        jawPos[0] = TPS_MIN(jawPos[0], tmpShape->get_xlowerjawpos());
        jawPos[1] = TPS_MAX(jawPos[1], tmpShape->get_xupperjawpos());
        jawPos[2] = TPS_MIN(jawPos[2], tmpShape->get_ylowerjawpos());
        jawPos[3] = TPS_MAX(jawPos[3], tmpShape->get_yupperjawpos());
        tmpLeafPos = tmpShape->get_leafpositions();
        if (leafPos.size() != tmpLeafPos.size())
        {
            TPS_LOG_DEV_ERROR << "The leaf position size is not coincident.";
            return false;
        }
        for (int j = 0; j < leafPos.size(); ++j)
        {
            if (abs(tmpLeafPos[j].y - tmpLeafPos[j].x - miniLeafGap) < 1e-4)
            {
                continue;
            }
            leafPos[j].x = TPS_MIN(leafPos[j].x, tmpLeafPos[j].x);
            leafPos[j].y = TPS_MAX(leafPos[j].y, tmpLeafPos[j].y);
        }
    }

    shape->set_xlowerjawpos(jawPos[0]);
    shape->set_xupperjawpos(jawPos[1]);
    shape->set_ylowerjawpos(jawPos[2]);
    shape->set_yupperjawpos(jawPos[3]);
    shape->set_mlcdirectiontype(direction);
    shape->set_leafpositions(leafPos);
    return true;
}

void CreateBeamCmd::UpdateSegmentMlcShapes_i(
    RtMlcshape* const startShape,
    RtMlcshape* const endShape,
    RtBeamsegment* segment)
{
    //start shape
    auto start_shape = segment->get_startmlcshape();
    std::string uid = start_shape->get_uid();
    *start_shape = *startShape;
    start_shape->set_uid(uid);
    start_shape->set_segmentuid(segment->get_uid());
    start_shape->set_isstartmlcshape(true);
    //end shape
    auto end_shape = segment->get_endmlcshape();
    uid = end_shape->get_uid();
    *end_shape = *endShape;
    end_shape->set_uid(uid);
    end_shape->set_segmentuid(segment->get_uid());
    end_shape->set_isstartmlcshape(false);
}

TPS_END_NAMESPACE
