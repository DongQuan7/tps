//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_pca_updategridsizecmd.cpp
///  \brief   update gridsize command
///
///  \version 1.0
///  \date    Apr. 9, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_updategridsize.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
//Rt
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

TPS_BEGIN_NAMESPACE

TpsUpdateGridSizeCMD::TpsUpdateGridSizeCMD(CertifiedGridSize *pocoGridSize) 
    : mPocoGridSize(pocoGridSize) {
}


TpsUpdateGridSizeCMD::~TpsUpdateGridSizeCMD(void) {
}

int TpsUpdateGridSizeCMD::Execute() {
    std::string planUid = mPocoGridSize->mPlanUid;
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan) || plan == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<planUid;
        return TPS_ER_FAILURE;
    }

    RtDosegrid* dosegrid = plan->get_dosegrid();
    if (dosegrid == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<planUid;
        return TPS_ER_FAILURE;
    }

    std::vector<RtDosegrid*> vNgDosegrids;
    std::vector<std::string> ngUids = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUid);
    std::vector<std::string> beamUids;
    for(std::vector<std::string>::iterator ngUidIt = ngUids.begin(); ngUidIt != ngUids.end(); ++ngUidIt)
    {
        beamUids = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(*ngUidIt);
        for (auto beamUidIt = beamUids.begin(); beamUidIt != beamUids.end(); ++beamUidIt)
        {
            RtDosegrid* beamDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(*beamUidIt, DOSE_GRID_BEAM);
            vNgDosegrids.push_back(beamDoseGrid);
        }
        RtDosegrid* ngDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(*ngUidIt, DOSE_GRID_NORMGROUP);
        vNgDosegrids.push_back(ngDoseGrid);
    }
    vNgDosegrids.push_back(dosegrid);

    std::string imageUID = plan->get_imageseriesuid();
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &image))
    {
        TPS_LOG_DEV_ERROR<<"The image entity with UID: "<<imageUID<<" does not exist.";
        return TPS_ER_FAILURE;
    }

    RtImage3DHeader* header = nullptr;
    if (!image->GetImage3DHeader(header))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get the image header.";
        return TPS_ER_FAILURE;
    }
    if (vNgDosegrids.empty())
    {
        TPS_LOG_DEV_ERROR<<"There is no normal groud dose grid.";
        return TPS_ER_FAILURE;
    }

    //Dose Grid各个轴方向不改变
    RtDosegrid* doseGrid = vNgDosegrids.front();
    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(doseGrid->get_grid_to_pat_t());
    Mcsf::Vector3f axis[3] = 
    {
        matG2P.Transform(Mcsf::Vector3f(1.f, 0.f, 0.f)), 
        matG2P.Transform(Mcsf::Vector3f(0.f, 1.f, 0.f)), 
        matG2P.Transform(Mcsf::Vector3f(0.f, 0.f, 1.f))
    };
    for (int i = 0; i < 3; ++i)
    {
        axis[i].Normalize();
    }

    Mcsf::Point3f startPoint = Mcsf::Point3f(mPocoGridSize->mstartx * 10, mPocoGridSize->mstarty * 10, mPocoGridSize->mstartz * 10);
    float spacing[3];
    int dim[3];
    spacing[0] = mPocoGridSize->mx * 10;
    spacing[1] = mPocoGridSize->my * 10;
    spacing[2] = mPocoGridSize->mz * 10;
    dim[0] = mPocoGridSize->msizex / mPocoGridSize->mx + 0.5;
    dim[1] = mPocoGridSize->msizey / mPocoGridSize->my + 0.5;
    dim[2] = mPocoGridSize->msizez / mPocoGridSize->mz + 0.5;
    TpsDoseManager* doseManager = mDataRepository->GetDoseManager();

    for(std::vector<RtDosegrid*>::iterator dosegridIt = vNgDosegrids.begin(); 
        dosegridIt != vNgDosegrids.end(); ++ dosegridIt)
    {
        doseManager->UpdateDoseGrid(*dosegridIt,startPoint,axis,spacing,dim);
    }

    mSaveObjects->SaveDosegridsForPlan(planUid);

    doseManager->SetDoseGridSpacing(spacing);

    mRenderProxy->ShowDoseGridBoundary(mPocoGridSize->mPlanUid, mPocoGridSize->mIsShowBoundary);
    //update boundary 
    float xCount = dosegrid->get_xcount() - 0.5f;
    float yCount = dosegrid->get_ycount() - 0.5f;
    float zCount = dosegrid->get_zcount() - 0.5f;
    Mcsf::Point3f leftBottomPoint;
    Mcsf::Point3f increase(dosegrid->get_xcoordinate3dinc(), dosegrid->get_ycoordinate3dinc(),dosegrid->get_zcoordinate3dinc());
    matG2P = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid->get_grid_to_pat_t());
    Mcsf::Point3f rightTopPoint = matG2P.Transform(
        Mcsf::Point3f(xCount, yCount, zCount));
    leftBottomPoint = matG2P.Transform(Mcsf::Point3f(-0.5f, -0.5f, -0.5f));
    mRenderProxy->EditDoseGridBoundary(mPocoGridSize->mPlanUid, 
        leftBottomPoint, rightTopPoint, increase);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE