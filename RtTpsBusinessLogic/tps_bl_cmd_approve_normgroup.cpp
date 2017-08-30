//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  xiaoqing.shangguan (mailto:xiaoqing.shangguan@united-imaging.com)
///
///  \file   tps_cca_approvenormgroup_cmd.h
///  \brief   approve normgroup
///
///  \version 1.0
///  \date  2014/11/10  
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_approve_normgroup.h"

#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_safelock.h"

#include "RtTpsAlgorithmProxy/tps_ap_drr_algorithm_proxy.h"
#include <boost/date_time/posix_time/posix_time.hpp>


#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsApproveNormGroupCmd::TpsApproveNormGroupCmd(RtApproverecord* approverecord)
    : mApproveRecord(approverecord)
{

}

TpsApproveNormGroupCmd::~TpsApproveNormGroupCmd()
{

}

int TpsApproveNormGroupCmd::Execute()
{
    if (mApproveRecord == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    std::string normalGroupUid = mApproveRecord->get_objectuid();

    int iRet = mSaveObjects->SaveApproveRecord(*mApproveRecord);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to Insert normalgroup ApproveRecord in TpsApproveNormGroupCmd";
        return iRet;
    }

    RtNormgroup* pNormGroup = nullptr;

    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normalGroupUid, &pNormGroup))
    {
        TPS_LOG_DEV_ERROR << "[TpsApproveNormGroupCmd]: " << "Fail to get normalgroup by normgroup uid: " << normalGroupUid;
        return TPS_ER_FAILURE;
    }

    std::string planUid = pNormGroup->get_planuid();
    RtPlan* pPlan = nullptr;
    mDataRepository->GetPatientManager()->GetPlan(planUid, &pPlan);


    // collect isocenter related to this normgroup
    std::set<std::string> isocenterCollection;
    std::vector<RtBeam*> beams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(normalGroupUid);
    for (auto itBeam = beams.begin(); itBeam != beams.end(); ++itBeam)
    {
        isocenterCollection.insert((*itBeam)->get_isocenterpoiuid());
    }

    // update poi info
    std::string seriesUid = pPlan->get_imageseriesuid();
    std::vector<RtPoi*> tpsPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
    for (std::vector<RtPoi*>::iterator it = tpsPoiList.begin(); it != tpsPoiList.end(); ++it)
    {
        RtPoi* poi = *it;
        if (poi->get_locktype() != RtDbDef::P_LOCK)
        {
            poi->set_locktype(RtDbDef::P_LOCK);
            mApprovedPois.push_back(poi);
            iRet = mSaveObjects->UpdatePoi(poi);
            if (iRet != TPS_ER_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "Failed to UpdatePoi in TpsApproveNormGroupCmd";
                return iRet;
            }
        }
    }

    // update voi info
    std::vector<ROIEntity*> voiList = mDataRepository->GetVoiManager()->GetROIBySeries(seriesUid);
    RtVoi* innerRoi;
    for (int i = 0; i < voiList.size(); ++i)
    {
        innerRoi = voiList[i]->GetInnerRoi();
        if (innerRoi == nullptr) continue;

        if (innerRoi->get_locktype() == RtDbDef::P_LOCK)
        {
            continue;
        }
        innerRoi->set_locktype(RtDbDef::P_LOCK);
        std::vector<bool> interpolates = innerRoi->get_interpolate();
        interpolates.assign(interpolates.size(), false);
        innerRoi->set_interpolate(interpolates);
        iRet = mSaveObjects->UpdateVoiAttributes(*voiList[i]);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to UpdateVoiAttributes in TpsApprovePlanCmd";
            return iRet;
        }

        RtApproverecord approveRecord(true);
        approveRecord.set_objectuid(innerRoi->get_uid());
        approveRecord.set_isapproved(true);
        approveRecord.set_objecttype(RtDbDef::VOI);
        approveRecord.set_approver(mApproveRecord->get_approver());
        approveRecord.set_approvedatetime(mApproveRecord->get_approvedatetime());
        iRet = mSaveObjects->SaveApproveRecord(approveRecord);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to Save Voi ApproveRecord in TpsApproveNormGroupCmd";
            return iRet;
        }

        mApprovedVois.push_back(voiList[i]);
        //delete all related commands in redo/undo list of CommandManager
        TpsSafeLock<TpsCommandManager> lock(mDataRepository->GetCommandManager());
        mDataRepository->GetCommandManager()->RemoveCommands(
            [&](TpsRedoUndoCommand* cmd)->bool{
                auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
                return voiCmd != nullptr && voiCmd->GetVoiUid() == innerRoi->get_uid();
        });

        mRenderProxy->UpdateVOIAttribute(seriesUid, innerRoi->get_uid());
    }
    mRenderProxy->Render();

    //generate drr rt image for every beam in the normal group
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image) || image == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image 3d entity: " << seriesUid;
        return TPS_ER_FAILURE;
    }
    //machine
    std::string machineUID = pNormGroup->get_machineuid();
    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(machineUID, machine) || machine == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine: " << machineUID;
        return TPS_ER_FAILURE;
    }
    //ct 2 density
    RtSeries* series = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImageSeries(seriesUid, &series) || series == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get series: " << seriesUid;
        return TPS_ER_FAILURE;
    }
    std::string ct2densityUid = series->get_ct2densityuid();
    if (ct2densityUid.empty())
    {
        TPS_LOG_DEV_ERROR << "There is no Default Ct2Density Table ";
        return TPS_ER_FAILURE;
    }

    float* ct2Density = const_cast<float*>(mDataRepository->GetImageDataManager()->GetCT2DensityArrayByUid(ct2densityUid));
    if (ct2Density == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get ct 2 density: " << ct2densityUid;
        return TPS_ER_FAILURE;
    }

    auto beamList = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(pNormGroup->get_uid());
    RtBeam* beam = nullptr;
    int beamType;
    for (int i = 0; i < beamList.size(); ++i)
    {
        beam = beamList[i];
        if (beam == nullptr) continue;
        beamType = beam->get_beamtype();
        // if the beam is arc beam, continue
        if (beamType == RtDbDef::BURST_ARC || beamType == RtDbDef::VMAT) continue;
        TpsIECCoordinateSystem* iecCs = mDataRepository->GetBeamManager()->GetBeamCS(beam->get_uid());
        if (!GenerateDrrForBeam_i(beam, image, machine, ct2Density, iecCs))
        {
            TPS_LOG_DEV_ERROR << "Failed to generate drr for beam: " << beam->get_uid();
            return TPS_ER_FAILURE;
        }
    }

    return TPS_ER_SUCCESS;
}


bool TpsApproveNormGroupCmd::GenerateDrrForBeam_i(RtBeam* beam, TpsImage3DEntity* image, RtMachine* tpsMachine, float* ct2Density,
    TpsIECCoordinateSystem* iecCs)
{
    //beam to volume
    Mcsf::Matrix4f v2p;
    if (!image->GetVolumeToPatientMatrix(v2p))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix.";
        return false;
    }
    if (iecCs == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get iec coordinate system of beam: "<<beam->get_uid();
        return false;
    }
    Mcsf::Matrix4f b2p;
    if (!iecCs->GetTransformMatrix(_GANTRY, _PATIENT, b2p))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get transform matrix.";
        return false;
    }
    double sad = tpsMachine->get_sad();
    b2p.Append(Mcsf::Matrix4f(
        Mcsf::Vector4f(1.f, 0.f, 0.f, 0.f), 
        Mcsf::Vector4f(0.f, 1.f, 0.f, 0.f), 
        Mcsf::Vector4f(0.f, 0.f, 1.f, 0.f), 
        Mcsf::Vector4f(0.f, 0.f, sad, 1.f)));
    Mcsf::Matrix4f beam2Volume = b2p;
    beam2Volume.Prepend(v2p.Inverse());

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.empty())
    {
        TPS_LOG_DEV_ERROR << "There is no segments in the beam: " << beam->get_uid();
        return false;
    }
    RtBeamsegment* frontSegment = segments.front();
    float gantryAngle = (float)frontSegment->get_startgantryangle();

    RtRtimage* rtImage = nullptr;
    beam->get_uid();
    beam->get_isocenterpoiuid();

    RtCommissionedunit *unit = nullptr;
    TpsMachineManager *machineManager = mDataRepository->GetMachineManager();
    machineManager->GetCommissionedUnit(beam->get_commissionedunituid(), unit);
    if (nullptr == unit) {
        TPS_LOG_DEV_ERROR<<"Failed to get unit. ";
        return false;
    }
    float energy = unit->get_energy();

    if(!DrrAlgorithmProxy::GenerateDrrRtImage(
        image, 
        tpsMachine, 
        beam->get_uid(), 
        beam->get_isocenterpoiuid(), 
        ct2Density, 
        beam2Volume, 
        energy,
        gantryAngle,
        rtImage, 512, 512) || rtImage == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to invoke DrrAlgorithmProxy::GenerateDrrRtImage.";
        return false;
    }
    if (TPS_ER_SUCCESS != mSaveObjects->SaveRtImage(*rtImage))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save DRR image of beam"<<beam->get_uid();
        delete rtImage;
        return false;
    }
    delete rtImage;
    return true;
}

TPS_END_NAMESPACE
