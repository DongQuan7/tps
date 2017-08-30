////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_smooth_voi.cpp
/// 
///  \brief class SmoothVoiCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_smooth_voi.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

SmoothVoiCmd::SmoothVoiCmd(const std::string& voiUid)
: TpsVOIRedoUndoSubVolumeCmd(voiUid)
{

}

SmoothVoiCmd::~SmoothVoiCmd()
{

}

int SmoothVoiCmd::Execute()
{
    //if(mVoiUid.empty()) {
    //    TPS_LOG_DEV_ERROR<<"voi uid is empty!";
    //    return TPS_ER_FAILURE;
    //}

    //VOIEntity* voi = nullptr;
    //if(!mDataRepository->GetVoiManager()->GetVOI(mVoiUid,&voi)) {
    //    TPS_LOG_DEV_ERROR<<"Failed to get voi by uid:"
    //        <<mVoiUid;
    //    return TPS_ER_FAILURE;
    //}

    //std::string imageUid = voi->GetSeriesUid();
    //if(imageUid.empty()) {
    //    TPS_LOG_DEV_ERROR<<"Failed to get image uid";
    //    return TPS_ER_FAILURE;
    //}

    //TpsImage3DEntity * image = nullptr;
    //TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    //if(!imageMng->GetImage3DData(imageUid,&image)) {
    //    TPS_LOG_DEV_ERROR<<"Failed to get image 3D";
    //    return TPS_ER_FAILURE;
    //}

    //SubVolumeHelperAPI::SmoothVOI(voi,*image);
    //Mcsf::Matrix4f matV2P;
    //image->GetVolumeToPatientMatrix(matV2P);
    ////     boost::thread thrd(boost::bind(&SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct, 
    ////         matV2P, voi, 1.0f));
    //SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct(matV2P, voi);

    ////update voi statistical information
    //RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUid);
    ////if (pCt2Density == nullptr)
    ////{
    ////    TPS_LOG_DEV_ERROR << "Failed to get Ct2Density.";
    ////    return TPS_ER_FAILURE;
    ////}

    //if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image, voi)){
    //    TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
    //    return TPS_ER_FAILURE;
    //}

    ////save voi to database
    //int iRet = mSaveObjects->SaveVoi(*voi);
    //if (TPS_ER_SUCCESS != iRet){
    //    TPS_LOG_DEV_ERROR<<"Failed to save voi to database.";
    //    return iRet;
    //}

    ////与ZYG确认，SmoothVOI不需要修改实虚线-GML
    ///*if (!voi->GetContours().empty())
    //{
    //for (int i = 0; i < voi->GetInterpolationFlag().size(); ++i) {
    //voi->SetIsInterpolate(true, i);
    //}
    //}*/

    //mRenderProxy->UpdateVOI(imageUid,mVoiUid);
    //mRenderProxy->Render();

    ////update voi to front end.
    //EntityConverter::GetInstance()->VOIEntiyToCertifiedVOI(*voi, mCertVoi);
    //if (mResponser != nullptr){
    //    mResponser->Response(*mCertVoi);
    //}
    return TPS_ER_SUCCESS;
}

TpsRedoUndoCommand* SmoothVoiCmd::Clone()
{
    SmoothVoiCmd* cmd = new SmoothVoiCmd(mVoiUid);
    cmd->SetDatabaseWrapper(mDatabaseWrapper);
    cmd->SetDataRepository(mDataRepository);
    cmd->SetLoadObjects(mLoadObjects);
    cmd->SetSaveObjects(mSaveObjects);
    cmd->SetRenderProxy(mRenderProxy);
    cmd->mSubvolume = std::move(mSubvolume);
    cmd->mResponser = mResponser;
    return cmd;
}
TPS_END_NAMESPACE  // end namespace tps