////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_comca_voi_redo_undo_subvolume_cmd.cpp
/// 
///  \brief class TpsVOIRedoUndoSubVolumeCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/09/17
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_redo_undo_subvolume.h"
//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIRedoUndoSubVolumeCmd::TpsVOIRedoUndoSubVolumeCmd(const std::string& uid) 
    : TpsVOIRedoUndoCmdBase(uid), mSubvolume(nullptr), mCertVoi(new CertifiedVOI()){

}

TpsVOIRedoUndoSubVolumeCmd::~TpsVOIRedoUndoSubVolumeCmd(){
    if (mCertVoi) delete mCertVoi;
}

int TpsVOIRedoUndoSubVolumeCmd::RedoUndo_i(){
    //if (mSubvolume.get() == nullptr) {
    //    TPS_LOG_DEV_ERROR<<"Pre sub-volume is empty.";
    //    return TPS_ER_FAILURE;
    //}

    //if(mVoiUid.empty()) {
    //    TPS_LOG_DEV_ERROR<<"voi uid is empty!";
    //    return TPS_ER_FAILURE;
    //}

    //VOIEntity* voi;
    //if(!mDataRepository->GetVoiManager()->GetVOI(mVoiUid,&voi) || voi == nullptr) {
    //    TPS_LOG_DEV_ERROR<<"Failed to get voi by uid:"<<mVoiUid;
    //    return TPS_ER_FAILURE;
    //}
    //McsfGeometry::SubVolume* subVolume = mSubvolume.release();
    //voi->SwapSubVolume(subVolume);
    //mSubvolume.reset(subVolume);

    //std::string imageUID = voi->GetSeriesUid();
    ////get image entity
    //TpsImage3DEntity *image3D = nullptr;
    //TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    //if (!imageMng->GetImage3DData(
    //    imageUID, &image3D) || image3D == nullptr){
    //    TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
    //    return TPS_ER_FAILURE;
    //}

    //{
    //    TpsSafeLock<VOIEntity> lockAna(voi);
    //    voi->SetAllSliceDirtyForAna(true);
    //}
    //Mcsf::Matrix4f matV2P;
    //image3D->GetVolumeToPatientMatrix(matV2P);
    //SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct(matV2P, voi);

    ////update voi statistical information
    //RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUID);
    ////if (pCt2Density == nullptr)
    ////{
    ////    TPS_LOG_DEV_ERROR << "Failed to get Ct2Density.";
    ////    return TPS_ER_FAILURE;
    ////}

    //if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, voi)){
    //    TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
    //    return TPS_ER_FAILURE;
    //}

    //int iRet = mSaveObjects->SaveVoi(*voi);
    //if (TPS_ER_SUCCESS != iRet){
    //    TPS_LOG_DEV_ERROR<<"Failed to save voi to database.";
    //    return iRet;
    //}

    ////update voi
    //mRenderProxy->UpdateVOI(imageUID, mVoiUid);
    //mRenderProxy->Render();

    //EntityConverter::GetInstance()->VOIEntiyToCertifiedVOI(*voi, mCertVoi);
    //if (mResponser != nullptr){
    //    mResponser->Response(*mCertVoi);
    //}
    return TPS_ER_SUCCESS;
}

int TpsVOIRedoUndoSubVolumeCmd::Undo(){
    return RedoUndo_i();
}

int TpsVOIRedoUndoSubVolumeCmd::Redo(){
    return RedoUndo_i();
}

bool TpsVOIRedoUndoSubVolumeCmd::Ready() const{
    return mSubvolume.get() != nullptr;
}

int TpsVOIRedoUndoSubVolumeCmd::PreExecute(){
    /*if (TpsVOIRedoUndoCmdBase::PreExecute() != TPS_ER_SUCCESS){
        return TPS_ER_FAILURE;
    }

    if(mVoiUid.empty()) {
        TPS_LOG_DEV_ERROR<<"voi uid is empty!";
        return TPS_ER_FAILURE;
    }

    VOIEntity* voi;
    if(!mDataRepository->GetVoiManager()->GetVOI(mVoiUid,&voi) || voi == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi by uid:"<<mVoiUid;
        return TPS_ER_FAILURE;
    }
    McsfGeometry::SubVolume* subVolume = voi->GetSubVolume();
    if (subVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"The sub-volume is nullptr;";
        return TPS_ER_FAILURE;
    }

    int offset[3], size[3];
    subVolume->GetOffSet(offset[0], offset[1], offset[2]);
    subVolume->GetSize(size[0], size[1], size[2]);
    char* buffer = subVolume->GetData();

    mSubvolume.reset(new McsfGeometry::SubVolume());
    mSubvolume->SetSize(size[0], size[1], size[2]);
    mSubvolume->SetOffSet(offset[0], offset[1], offset[2]);
    char* newBuffer = mSubvolume->GetData();
    if(buffer != nullptr && newBuffer != nullptr)
    {
        memcpy(newBuffer, buffer, size[0] * size[1] * size[2] * sizeof(char));
    }*/
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE  // end namespace tps