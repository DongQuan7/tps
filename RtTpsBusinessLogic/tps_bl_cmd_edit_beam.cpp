////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_edit_beam.cpp
/// 
///  \brief class for edit plan
/// 
///  \version 1.0
/// 
///  \date    Nov.6,2015
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_edit_beam.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"
//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

//TPS
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsAlgorithmProxy/tps_ap_mlc_jaw_limitation_helper.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#define EDIT_BEAM_TYPE_DIF_VALUE 0x10000000

TPS_BEGIN_NAMESPACE;   // begin namespace tps

EditBeamCmd::EditBeamCmd(const TpsCertifiedBeam& pocoBeam, int editType):
    CreateBeamCmd(pocoBeam), mOnTheFly(false), mEditType(editType)
{

}

EditBeamCmd::~EditBeamCmd()
{

}

void EditBeamCmd::GetBeamCreated(const RtBeam*& pBeam)
{
    RtBeam* newBeam = mDataRepository->GetBeamManager()->GetBeam(mCertifiedBeam.mBeamUID);
    pBeam = newBeam;
}

int EditBeamCmd::Execute()
{
    const std::string sBeamUid = mCertifiedBeam.mBeamUID;
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(sBeamUid);
    if (nullptr == beam)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam:" << sBeamUid;
        return TPS_ER_FAILURE;
    }

    //check normgroup
    RtNormgroup* normGroup = nullptr;
    const std::string sNormgroupUid = mCertifiedBeam.mNormGroupUID;
    mDataRepository->GetNormgroupManager()->GetNormGroup(sNormgroupUid, &normGroup);
    if (nullptr == normGroup)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normal group.";
        return TPS_ER_FAILURE;
    }

    RtMachine* machine = nullptr;
    const std::string sMachineUid = normGroup->get_machineuid();
    mDataRepository->GetMachineManager()->GetMachine(sMachineUid, machine);
    if (nullptr == machine)
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine.";
        return TPS_ER_FAILURE;
    }

    RtPlan* plan = nullptr;
    const std::string sPlanUid = normGroup->get_planuid();
    mDataRepository->GetPatientManager()->GetPlan(sPlanUid, &plan);
    if (nullptr == plan)
    {
        TPS_LOG_DEV_ERROR << "The plan does not exist.";
        return TPS_ER_FAILURE;
    }
    
    RtDbDef::PLAN_TYPE planType = (RtDbDef::PLAN_TYPE)normGroup->get_plantype();
    if (planType == RtDbDef::PLAN_UNKNOWN_TYPE)
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << beam->get_uid() << " has wrong plan type:"
            << planType;
        return TPS_ER_FAILURE;
    }
    RtDbDef::RADIATION_TYPE radType = (RtDbDef::RADIATION_TYPE)normGroup->get_radiationtype();
    if (!ValidateData(planType, radType))
    {
        TPS_LOG_DEV_ERROR << "Failed to validate beam parameters.";
        return TPS_ER_FAILURE;
    }

    const std::string imageUid = plan->get_imageseriesuid();
    SetCommonBeamData_i(planType, beam);

    SetJawBoundary(machine, mCertifiedBeam.mPhysicalWedgeUid, mCertifiedBeam.mVirtualWedgeUid);

    int editType = mEditType ^ EDIT_BEAM_TYPE_DIF_VALUE;

    /////////////////////////////////////////////////////////////

    int iRet;
    bool bIsArc = TpsCmdHelper::IsArcBeam(planType);

    if (editType & EDIT_BEAM_GANTRYANGLE
        || editType & EDIT_BEAM_COLLIMATORANGLE
        || editType & EDIT_BEAM_COUCHANGLE
        || editType & EDIT_BEAM_DIRECTION)
    {
        if (!bIsArc)
        {
            iRet = EditBeamAngles_i(beam, imageUid);
            if ((int)TPS_ER_SUCCESS != iRet)
            {
                TPS_LOG_DEV_ERROR << "Failed to EditBeamAngles_i with beamuid:" << sBeamUid;
                return iRet;
            }
        }
    }

    if (editType & EDIT_BEAM_ISOCENTER)
    {
        if (!bIsArc)
        {
            iRet = EditBeamIsoCenter_i(beam, imageUid);
            if ((int)TPS_ER_SUCCESS != iRet)
            {
                TPS_LOG_DEV_ERROR << "Failed to EditBeamIsoCenter_i with beamuid:" << sBeamUid;
                return iRet;
            }
        }
    }

    if (editType & EDIT_BEAM_APERTURE_JAWFOLLOW_ACTIVE
        || editType & EDIT_BEAM_APERTURE_JAWFOLLOW_MARGIN)
    {
        iRet = EditBeamJawFollow_i(planType, beam, machine);
        if ((int)TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to EditBeamJawFollow_i with beamuid:" << sBeamUid;
            return iRet;
        }
    }

    if (editType & EDIT_BEAM_JAW_ISXSYMMETRIC
        || editType & EDIT_BEAM_JAW_ISYSYMMETRIC
        || editType & EDIT_BEAM_JAW_SIZE)
    {
        iRet = EditBeamJawSize_i(beam);
        if ((int)TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to EditBeamJawSize_i with beamuid:" << sBeamUid;
            return iRet;
        }
    }

    if (editType & EDIT_BEAM_USE_SHIELDING_BLOCK
        || editType & EDIT_BEAM_SHIELDING_BLOCK_TARGETVOI
        || editType & EDIT_BEAM_SHIELDING_BLOCK_AVOIDVOI
        || editType & EDIT_BEAM_SHIELDING_BLOCK_TARGETMARGIN
        || editType & EDIT_BEAM_SHIELDING_BLOCK_AVOIDMARGIN)
    {
        iRet = EditBeamShieldBlock_i(beam, machine);
        if ((int)TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to EditBeamShieldBlock_i with beamuid:" << sBeamUid;
            return iRet;
        }
    }

    if (editType & EDIT_BEAM_PHSYICIAL_WEDGE
        || editType & EDIT_BEAM_VIRTURAL_WEDGE)
    {
        EditWedge_i(beam);
    }

    if (editType & EDIT_BEAM_TRAY)
    {
        EditBeamTray_i(beam);
    }

    if (GetReCreateArcSegmentFlag_i(editType, planType))
    {
        iRet = ReCreateArcSegment_i(beam, machine, planType, imageUid);
        if ((int)TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to ReCreateArcSegment_i with beamuid:" << sBeamUid;
            return iRet;
        }
    }

    if (GetAutoShapeForApertureFlag_i(editType, planType))
    {
        iRet = AutoShapeForAperture_i(beam, machine, planType, imageUid);
        if ((int)TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to AutoShapeForAperture_i with beamuid:" << sBeamUid;
            return iRet;
        }
    }

    if (GetAutoShapeForShieldBlockFlag_i(editType, planType))
    {
        iRet = AutoShapeForShieldingBlock_i(beam, machine);
        if ((int)TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to AutoShapeForShieldingBlock_i with beamuid:" << sBeamUid;
            return iRet;
        }
    }

    if (editType & EDIT_BEAM_BOLUS)
    {
        EditBolus_i(beam);
    }

    if (GetUpdateFluenceMapFlag_i(editType, planType))
    {
        if (!mDoseProxy->UpdateFluenceMap(beam))
        {
            TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
            return false;
        }
    }

    // save beam to database
    if (!mOnTheFly)
    {
        if (TPS_ER_SUCCESS != mSaveObjects->UpdateBeam(*beam))
        {
            TPS_LOG_DEV_ERROR << "Failed to save beam:" << sBeamUid;
            return TPS_ER_DB_ERROR;
        }
    }

    if (!mRenderProxy->UpdateBeam(beam->get_normgroupuid(), sBeamUid))
    {
        TPS_LOG_DEV_ERROR << "Failed to update beam in render proxy.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->ResetBeamViewCameras(sPlanUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

bool EditBeamCmd::GetUpdateFluenceMapFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType)
{
    //如果是非Arc，并且不自动适形
    if (!TpsCmdHelper::IsArcBeam(normalgroupType)
        && !IsNeedAutoShapeForAperture_i())
    {
        //那么这四种情况不需要刷新FluenceMap
        if (editType & EDIT_BEAM_GANTRYANGLE 
            || editType & EDIT_BEAM_COUCHANGLE 
            || editType & EDIT_BEAM_ISOCENTER
            || editType & EDIT_BEAM_SSD)
        {
            return false;
        }
    }

    return true;
}

bool EditBeamCmd::GetReCreateArcSegmentFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType)
{
    if (!TpsCmdHelper::IsArcBeam(normalgroupType))
    {
        return false;
    }

    if (editType & EDIT_BEAM_GANTRYANGLE
        || editType & EDIT_BEAM_COLLIMATORANGLE
        || editType & EDIT_BEAM_COUCHANGLE
        || editType & EDIT_BEAM_DIRECTION
        || editType & EDIT_BEAM_ISOCENTER
        || editType & EDIT_BEAM_APERTURETYPE
        || editType & EDIT_BEAM_MLC_EDIT_MODE
        || editType & EDIT_BEAM_APERTURE_TARGET_VOI
        || editType & EDIT_BEAM_APERTURE_AVOID_VOI
        || editType & EDIT_BEAM_APERTURE_TARGET_MARGIN
        || editType & EDIT_BEAM_APERTURE_AVOID_MARGIN)
    {
        return true;
    }

    return false;
}

bool EditBeamCmd::GetAutoShapeForApertureFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType)
{
    if (TpsCmdHelper::IsArcBeam(normalgroupType)) return false;

    if (editType & EDIT_BEAM_APERTURETYPE
        || editType & EDIT_BEAM_MLC_EDIT_MODE
        || editType & EDIT_BEAM_APERTURE_TARGET_VOI
        || editType & EDIT_BEAM_APERTURE_AVOID_VOI
        || editType & EDIT_BEAM_APERTURE_TARGET_MARGIN
        || editType & EDIT_BEAM_APERTURE_AVOID_MARGIN)
    {
        return true;
    }

    if (editType & EDIT_BEAM_GANTRYANGLE
        || editType & EDIT_BEAM_COLLIMATORANGLE
        || editType & EDIT_BEAM_COUCHANGLE
        || editType & EDIT_BEAM_ISOCENTER
        || editType & EDIT_BEAM_JAW_ISXSYMMETRIC
        || editType & EDIT_BEAM_JAW_ISYSYMMETRIC
        || editType & EDIT_BEAM_JAW_SIZE)
    {
        if (IsNeedAutoShapeForAperture_i())
        {
            return true;
        }
    }

    return false;
}

bool EditBeamCmd::GetAutoShapeForShieldBlockFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType)
{
    if (TpsCmdHelper::IsArcBeam(normalgroupType)) return false;

    if (editType & EDIT_BEAM_GANTRYANGLE
        || editType & EDIT_BEAM_COLLIMATORANGLE
        || editType & EDIT_BEAM_COUCHANGLE
        || editType & EDIT_BEAM_DIRECTION
        || editType & EDIT_BEAM_ISOCENTER)
    {
         if (IsNeedAutoShapeForShieldBlock_i(normalgroupType))
         {
             return true;
         }
    }

    return false;
}

bool EditBeamCmd::IsNeedAutoShapeForAperture_i()
{
    if (!mCertifiedBeam.mIsAutoTrackingShape) return false;

    if (mCertifiedBeam.mTargetVoiUid.empty() && mCertifiedBeam.mAvoidVoiUid.empty())
    {
        return false;
    }

    return true;
}

bool EditBeamCmd::IsNeedAutoShapeForShieldBlock_i(RtDbDef::PLAN_TYPE normalgroupType)
{
    if (RtDbDef::PLAN_3D_CRT != normalgroupType) return false;

    if (!mCertifiedBeam.mUseShieldingBlock) return false;

    if (!mCertifiedBeam.mIsShieldingBlockAutoTrackingShape) return false;

    if (mCertifiedBeam.mShieldingBlockTargetVoiUid.empty()
        && mCertifiedBeam.mShieldingBlockAvoidVoiUid.empty())
    {
        return false;
    }

    return true;
}

int EditBeamCmd::EditBeamAngles_i(RtBeam* beam, std::string sImageUid)
{
    std::string sBeamUid = beam->get_uid();
    TpsIECCoordinateSystem* iec = mDataRepository->GetBeamManager()->GetBeamCS(sBeamUid);
    if (nullptr == iec)
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << sBeamUid<< " has no IEC C.S. object.";
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << sBeamUid << " has no segment.";
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeamsegment*> segmentsNew;

    // 非ArcBeam（step-shoot，sliding-window）存在多个Segment时，编辑角度，保留当前Segment其余清空
    RtBeamsegment* pModelSegment = nullptr;
    std::vector<RtBeamsegment*>::iterator itSegment = segments.begin();
    for (; itSegment != segments.end(); ++itSegment)
    {
        if ((*itSegment)->get_uid() == mCertifiedBeam.mCurrentSegmentUid)
        {
            pModelSegment = *itSegment;
            break;
        }
    }
    if (nullptr == pModelSegment)
    {
        pModelSegment = segments[0];
        TPS_LOG_DEV_WARNING << "Cannot Find the segmentuid" << 
            mCertifiedBeam.mCurrentSegmentUid << "in beam" << sBeamUid;
    }

    RtBeamsegment* pSegment = new RtBeamsegment(*pModelSegment);
    pSegment->set_sequencenumber(0);
    segmentsNew.push_back(pSegment);

    //check gantry
    double angle = pSegment->get_startgantryangle();
    if (abs(angle - mCertifiedBeam.mGantryStartAngle) > 1e-6)
    {
        pSegment->set_startgantryangle(mCertifiedBeam.mGantryStartAngle);
        pSegment->set_arclength(0.f);//ignore the EndAngel
        iec->RotateGantry(mCertifiedBeam.mGantryStartAngle);
    }

    //check couch
    angle = pSegment->get_starttablerotationangle();
    if (abs(angle - mCertifiedBeam.mCouchAngle) > 1e-6)
    {
        pSegment->set_starttablerotationangle(mCertifiedBeam.mCouchAngle);
        pSegment->set_endtablerotationangle(mCertifiedBeam.mCouchAngle);
        iec->RotateTable(mCertifiedBeam.mCouchAngle);
    }

    //check collimator
    angle = pSegment->get_startcollimatorangle();
    if (abs(angle - mCertifiedBeam.mCollimatorAngle) > 1e-6)
    {
        pSegment->set_startcollimatorangle(mCertifiedBeam.mCollimatorAngle);
        pSegment->set_endcollimatorangle(mCertifiedBeam.mCollimatorAngle);
        iec->RotateCollimator(mCertifiedBeam.mCollimatorAngle);
    }
    //iec无法保证当前get到的矩阵正确，因此每次get矩阵之前要重新rotate一下角度。
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
    beam->set_beamsegments(segmentsNew);

    for (auto itr = segmentsNew.begin(); itr != segmentsNew.end(); ++itr) DEL_PTR(*itr);

    return TPS_ER_SUCCESS;
}

int EditBeamCmd::EditBeamIsoCenter_i(RtBeam* beam, std::string sImageUid)
{
    std::string sBeamUid = beam->get_uid();
    TpsIECCoordinateSystem* iec = mDataRepository->GetBeamManager()->GetBeamCS(sBeamUid);
    if (nullptr == iec)
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << sBeamUid << " has no IEC C.S. object.";
        return TPS_ER_FAILURE;
    }
    if (mCertifiedBeam.mIsocenterUid.empty())
    {
        TPS_LOG_DEV_ERROR << "IsocenterUid is empty.";
        return TPS_ER_FAILURE;
    }

    beam->set_isocenterpoiuid(mCertifiedBeam.mIsocenterUid);
    RtPoi* isoCenter = mDataRepository->GetPoiManager()->GetPOI(mCertifiedBeam.mIsocenterUid);
    if (nullptr == isoCenter)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist which uid: " << mCertifiedBeam.mIsocenterUid;
        return TPS_ER_FAILURE;
    }

    Mcsf::Point3f pt3d(isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(),
        isoCenter->get_coordinate3dz());
    iec->SetISOCenter(pt3d);

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << sBeamUid<<" has no segment.";
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeamsegment*> segmentsNew;

    RtBeamsegment* pModelSegment = nullptr;
    std::vector<RtBeamsegment*>::iterator itSegment = segments.begin();
    for (; itSegment != segments.end(); ++itSegment)
    {
        if ((*itSegment)->get_uid() == mCertifiedBeam.mCurrentSegmentUid)
        {
            pModelSegment = *itSegment;
            break;
        }
    }
    if (nullptr == pModelSegment)
    {
        pModelSegment = segments[0];
        TPS_LOG_DEV_WARNING << "Cannot Find the segmentuid" << 
            mCertifiedBeam.mCurrentSegmentUid << "in beam" << sBeamUid;
    }

    RtBeamsegment* pSegment = new RtBeamsegment(*pModelSegment);
    pSegment->set_sequencenumber(0);
    segmentsNew.push_back(pSegment);

    Mcsf::Matrix4f matB2P;
    if (!iec->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P))
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam to patient matrix.";
        return TPS_ER_FAILURE;
    }
    float mat[16] ={0.f};
    matB2P.GetElements(mat);
    pSegment->set_t_beam_to_pat(mat);
    beam->set_beamsegments(segmentsNew);

    for (auto itr = segmentsNew.begin(); itr != segmentsNew.end(); ++itr) DEL_PTR(*itr);

    return TPS_ER_SUCCESS;
}

int EditBeamCmd::EditBeamJawFollow_i(RtDbDef::PLAN_TYPE planType, RtBeam* beam, RtMachine* machine)
{
    std::string sBeamUid = beam->get_uid();

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << sBeamUid << " has no segment.";
        return TPS_ER_FAILURE;
    }

    beam->set_jawtype(mCertifiedBeam.mIsJawFollow ? RtDbDef::JAWTYPE_Follow: RtDbDef::JAWTYPE_Manual);
    beam->set_jawmarginx(mCertifiedBeam.mJawFollowMarginX);
    beam->set_jawmarginy(mCertifiedBeam.mJawFollowMarginY);

    double offset = CalculateMiniGapOffset_i(planType, segments, machine);
    if (offset < 0)
    {
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeamsegment*>::iterator itTpsSegment = segments.begin();
    for (; itTpsSegment != segments.end(); ++itTpsSegment)
    {
        RtBeamsegment* pSegment = *itTpsSegment;
        RtMlcshape* pStartShape = pSegment->get_startmlcshape();
        TpsMlcJawLimitationHelper::JawFollowMlc(machine, offset, pStartShape,
            mCertifiedBeam.mJawFollowMarginX, mCertifiedBeam.mJawFollowMarginY);
        RtMlcshape* pEndShape = pSegment->get_endmlcshape();
        TpsMlcJawLimitationHelper::JawFollowMlc(machine, offset, pEndShape,
            mCertifiedBeam.mJawFollowMarginX, mCertifiedBeam.mJawFollowMarginY);
    }

    return TPS_ER_SUCCESS;
}

int EditBeamCmd::EditBeamJawSize_i(RtBeam* beam)
{
    std::string sBeamUid = beam->get_uid();
    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << sBeamUid << " has no segment.";
        return TPS_ER_FAILURE;
    }

    TpsCertifiedSegment certSegment;
    std::vector<TpsCertifiedSegment>::iterator itCertifiSegment = mCertifiedBeam.mSegmentList.begin();
    for (; itCertifiSegment != mCertifiedBeam.mSegmentList.end(); ++itCertifiSegment)
    {
        if ((*itCertifiSegment).segmentUID == mCertifiedBeam.mCurrentSegmentUid)
        {
            certSegment = *itCertifiSegment;
            break;
        }
    }
    // 编辑jawsize 需要更新所有segment的startmlcshape和endmlcshape；
    std::vector<RtBeamsegment*>::iterator itSegment = segments.begin();
    for (; itSegment != segments.end(); ++itSegment)
    {
        RtMlcshape* pStartShape = (*itSegment)->get_startmlcshape();
        RtMlcshape* pEndShape = (*itSegment)->get_endmlcshape();
        pStartShape->set_xlowerjawpos(certSegment.startJaw.x1);
        pStartShape->set_xupperjawpos(certSegment.startJaw.x2);
        pStartShape->set_ylowerjawpos(certSegment.startJaw.y1);
        pStartShape->set_yupperjawpos(certSegment.startJaw.y2);
        pStartShape->set_jawxissymmetry(mCertifiedBeam.mIsXSymmetric);
        pStartShape->set_jawyissymmetry(mCertifiedBeam.mIsYSymmetric);
        pEndShape->set_xlowerjawpos(certSegment.startJaw.x1);
        pEndShape->set_xupperjawpos(certSegment.startJaw.x2);
        pEndShape->set_ylowerjawpos(certSegment.startJaw.y1);
        pEndShape->set_yupperjawpos(certSegment.startJaw.y2);
        pEndShape->set_jawxissymmetry(mCertifiedBeam.mIsXSymmetric);
        pEndShape->set_jawyissymmetry(mCertifiedBeam.mIsYSymmetric);
    }

    return TPS_ER_SUCCESS;
}

int EditBeamCmd::EditBeamShieldBlock_i(RtBeam* beam, RtMachine* machine)
{
    RtBlock* shieldBlock = beam->get_shield_block();
    if (shieldBlock)
    {
        mSaveObjects->EraseObject(shieldBlock->get_uid(), RtDbDef::TYPE_BLOCK);
        beam->remove_shield_block();
    }

    if (!mCertifiedBeam.mUseShieldingBlock) return TPS_ER_SUCCESS;

    std::string sBeamUid = beam->get_uid();


    if (!CreateShieldingBlock_i(beam, machine))
    {
        TPS_LOG_DEV_ERROR << "Create sheild block error for beam " << sBeamUid;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

void EditBeamCmd::EditBolus_i(RtBeam* beam)
{
    beam->set_bolusvoiuid(mCertifiedBeam.mBolusVoiUid);
}

void EditBeamCmd::EditWedge_i(RtBeam* beam)
{
    beam->set_physicalwedgeuid(mCertifiedBeam.mPhysicalWedgeUid);
    beam->set_virtualwedgeuid(mCertifiedBeam.mVirtualWedgeUid);

    // applicator
    if (mCertifiedBeam.mApplicatorUid.empty())
    {
        beam->disable_field(BEAM_APPLICATORUID);
    }
    else
    {
        beam->set_applicatoruid(mCertifiedBeam.mApplicatorUid);
    }
}

void EditBeamCmd::EditBeamTray_i(RtBeam* beam)
{
    beam->set_trayuid(mCertifiedBeam.mTrayUid);
}

int EditBeamCmd::ReCreateArcSegment_i(RtBeam* beam, RtMachine* machine,
    RtDbDef::PLAN_TYPE normalgroupType, std::string sImageUid)
{
    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "The beam with UID: " << beam->get_uid() << " has no segment.";
        return TPS_ER_FAILURE;
    }

    RtPoi* isoCenter = nullptr;
    isoCenter = mDataRepository->GetPoiManager()->GetPOI(mCertifiedBeam.mIsocenterUid);
    if (nullptr == isoCenter)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist which uid: " << mCertifiedBeam.mIsocenterUid;
        return TPS_ER_FAILURE;
    }

    Mcsf::Point3f ptIsoCenter(
        isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(),
        isoCenter->get_coordinate3dz());

    // multi-segments
    for (auto itr = segments.begin(); itr != segments.end(); ++itr)
    {
        if (TPS_ER_SUCCESS != mSaveObjects->EraseObject((*itr)->get_uid(), RtDbDef::TYPE_BEAMSEGMENT))
        {
            TPS_LOG_DEV_ERROR << "Failed to delete segment with uid: " << (*itr)->get_uid();
            return TPS_ER_FAILURE;
        }
    }
    segments.clear();

    //create new beam segments
    std::vector<RtBeamsegment*> segmentsNew;
    this->CreateArcSegments_i(*beam,
        sImageUid,
        mCertifiedBeam.mIsClockWise,
        mCertifiedBeam.mGantryStartAngle,
        mCertifiedBeam.mGantryArcLength,
        mCertifiedBeam.mCouchAngle,
        mCertifiedBeam.mCollimatorAngle,
        machine,
        beam->get_uid(),
        ptIsoCenter,
        normalgroupType,
        segmentsNew);

    double offset = CalculateMiniGapOffset_i(normalgroupType, segmentsNew, machine);
    if (offset > 0)
    {
        CorrectBeamLeafGap_i(offset, machine, segmentsNew);
    }

    beam->set_beamsegments(segmentsNew);
    for (auto itr = segmentsNew.begin(); itr != segmentsNew.end(); ++itr) DEL_PTR(*itr);

    return TPS_ER_SUCCESS;
}

int EditBeamCmd::AutoShapeForAperture_i(RtBeam* beam, RtMachine* machine,
    RtDbDef::PLAN_TYPE normalgroupType, std::string sImageUid)
{
    RtPoi* isoCenter = nullptr;
    isoCenter = mDataRepository->GetPoiManager()->GetPOI(mCertifiedBeam.mIsocenterUid);
    if (nullptr == isoCenter)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist which uid: " << mCertifiedBeam.mIsocenterUid;
        return TPS_ER_FAILURE;
    }

    std::string sBeamUid = beam->get_uid();

    //clear blocks before segments creation
    RtBlock* apertureBlock = beam->get_aperture_block();
    if (apertureBlock)
    {
        mSaveObjects->EraseObject(apertureBlock->get_uid(), RtDbDef::TYPE_BLOCK);
        beam->remove_aperture_block();
    }
    //IMRT beam
    if (!CreateBeamSegments(sImageUid, beam, normalgroupType, machine, isoCenter))
    {
        TPS_LOG_DEV_ERROR << "Failed to create segments for beam[" << sBeamUid << "].";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int EditBeamCmd::AutoShapeForShieldingBlock_i(RtBeam* beam, RtMachine* machine)
{
    std::string sBeamUid = beam->get_uid();

    RtBlock* shieldBlock = beam->get_shield_block();
    if (shieldBlock)
    {
        mSaveObjects->EraseObject(shieldBlock->get_uid(), RtDbDef::TYPE_BLOCK);
        beam->remove_shield_block();

        if (!CreateShieldingBlock_i(beam, machine))
        {
            TPS_LOG_DEV_ERROR << "Create sheild block error for beam " << sBeamUid;
            return TPS_ER_FAILURE;
        }
    }

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE  // end namespace tps
