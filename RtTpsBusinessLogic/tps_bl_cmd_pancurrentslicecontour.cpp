////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_cca_pan_slice_contour_cmd.cpp
/// 
///  \brief class PanSliceContourCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/09/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_pancurrentslicecontour.h"

#include "McsfMedViewer3DArithmetic/point2d.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDataAccess/tps_da_entity_converter.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

PanSliceContourCmd::PanSliceContourCmd(const CertifiedMoveVOI& certMoveVoi)
    : TpsVOIRedoUndoCacheCmd(certMoveVoi.mVoiUID)
    , mCertMoveVOI(certMoveVoi), mCertVOI(new CertifiedVOI()){

}

PanSliceContourCmd::~PanSliceContourCmd(){
    if (mCertVOI) delete mCertVOI;
}

int PanSliceContourCmd::Execute(){

//    if (mRenderProxy == nullptr){
//        TPS_LOG_DEV_ERROR<<"The contouring render proxy is empty.";
//        return TPS_ER_FAILURE;
//    }
//
//    Mcsf::MedViewer3D::Point2D currentPoint, prePoint;
//    currentPoint.x = mCertMoveVOI.mCurCoordX;
//    currentPoint.y = mCertMoveVOI.mCurCoordY;
//    prePoint.x = mCertMoveVOI.mPreCoordX;
//    prePoint.y = mCertMoveVOI.mPreCoordY;
//    int actionType = mCertMoveVOI.mMouseActionType;
//    auto wndType = (WINDOW_TYPE)mCertMoveVOI.mWindowType;
//    std::string wndUid = mCertMoveVOI.mWindowUid;
//
////     if (!mRenderProxy->IsInVOI(wndType, wndUid, mVoiUid, prePoint.x, prePoint.y))
////     {
////         return TPS_ER_SUCCESS;
////     }
//    mRenderProxy->MoveCurrentSliceVOI(currentPoint, prePoint, actionType, 
//        mVoiUid, wndType, wndUid, mCertMoveVOI.mIsEditEnd);
//    mRenderProxy->Render();
//
//    if(mCertMoveVOI.mIsEditEnd)
//    {
//        UpdateVOI_i(mCertMoveVOI.mVoiUID);
//    }
//    return TPS_ER_SUCCESS;

    //if (mCertifiedMoveVOI.mMouseActionType == DOWN){
    //    size_t size[3];
    //    for(int i = 0; i < 3; ++i) {
    //        size[i] = dim[i];
    //    }
    //    SubVolumeHelperAPI::ExpandSubVolumeToOriginSize(size,subvolume);
    //}
    //else if(mCertifiedMoveVOI.mMouseActionType == DRAG){
    //    //note: it only supports to move voi slice on AXIAL section.
    //    WINDOW_TYPE type = ANASTRUCTEDITOR_AXIAL;
    //    char *volumeData = nullptr;
    //    volumeData = subvolume->GetData();
    //    int slice = 0, totalSlice = 0;
    //    renderProxy->GetCurrentMprSlice(type, slice, totalSlice);
    //    int startIndex = (slice - offsetZ) * sizeX * sizeY;
    //    int lowerX = 0, upperX = 0, lowerY = 0, upperY = 0;
    //    bool isBreak = false;
    //    for(int i = 0; i < sizeX; ++i) {
    //        if(isBreak) break;
    //        for (int j = 0; j < sizeY; ++j) {
    //            if(volumeData[startIndex + i * sizeY + j] == 1) {
    //                lowerX = i;
    //                isBreak = true;
    //                break;
    //            }
    //        }
    //    }
    //    isBreak = false;
    //    for(int i = sizeX - 1; i >= 0; --i) {
    //        if(isBreak) break;
    //        for (int j = 0; j < sizeY; ++j) {
    //            if(volumeData[startIndex + i * sizeY + j] == 1) {
    //                upperX = i;
    //                isBreak = true;
    //                break;
    //            }
    //        }
    //    }
    //    isBreak = false;
    //    for(int i = 0; i < sizeY; ++i) {
    //        if(isBreak) break;
    //        for (int j = lowerX; j <= upperX; ++j) {
    //            if(volumeData[startIndex + i * sizeX + j] == 1) {
    //                lowerY = i;
    //                isBreak = true;
    //                break;
    //            }
    //        }
    //    }
    //    isBreak = false;
    //    for(int i = sizeY - 1; i >= 0; --i) {
    //        if(isBreak) break;
    //        for (int j = lowerX; j <= upperX; ++j) {
    //            if(volumeData[startIndex + i * sizeX + j] == 1) {
    //                upperY = i;
    //                isBreak = true;
    //                break;
    //            }
    //        }
    //    }

    //    volumeData = subvolume->GetData();
    //    if(lowerX + int(vecOffset.GetX() + 0.5f) + offsetX < 0) {
    //        vecOffset.SetX(-lowerX - offsetX); 
    //    }
    //    if(upperX + int(vecOffset.GetX() + 0.5f) + offsetX >= dim[0]) {
    //        vecOffset.SetX(dim[0] - 1 - upperX - offsetX);
    //    }
    //    if(lowerY + int(vecOffset.GetY() + 0.5f) + offsetY < 0) {
    //        vecOffset.SetY(-lowerY - offsetY); 
    //    }
    //    if(upperY + int(vecOffset.GetY() + 0.5f) + offsetY >= dim[1]) {
    //        vecOffset.SetY(dim[1] - 1 - upperY - offsetY);
    //    }
    //    startIndex = slice * dim[0] * dim[1];
    //    char *tempBuffer = new char[dim[0] * dim[1]];
    //    int dataIndex = 0;
    //    memset(tempBuffer, 0, dim[0] * dim[1] * sizeof(char));
    //    for(int i = 0; i < dim[0]; ++i)
    //        for (int j = 0; j < dim[1]; ++j) {
    //            dataIndex = (i - int(vecOffset.GetY() + 0.5f)) * dim[1] + j - int(vecOffset.GetX() + 0.5f);
    //            if(i - int(vecOffset.GetY() + 0.5f) < 0 || i - int(vecOffset.GetY() + 0.5f) >= dim[0]) continue;
    //            if(j - int(vecOffset.GetX() + 0.5f) < 0 || j - int(vecOffset.GetX() + 0.5f) >= dim[1]) continue;
    //            tempBuffer[i * dim[1] + j] = volumeData[startIndex + dataIndex];
    //        }
    //        memcpy(volumeData + startIndex, tempBuffer, dim[0] * dim[1] * sizeof(char));
    //        renderProxy->UpdateVOI(imageUID,voiUid);
    //        renderProxy->Render();
    //}
    //else if(mCertifiedMoveVOI.mMouseActionType == UP){
    //    //note: it only supports to move voi slice on AXIAL section.
    //    WINDOW_TYPE type = ANASTRUCTEDITOR_AXIAL;
    //    int slice = 0, totalSlice = 0;
    //    renderProxy->GetCurrentMprSlice(type, slice, totalSlice);
    //    SubVolumeHelperAPI::RefineSubvolume(subvolume);
    //    voi->SetSliceDirtyForAna(slice);
    //    boost::thread thrd(boost::bind(&SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct, 
    //        matV2P, voi, 1.0f));
    //    mCertifiedMoveVOI.mIsEditEnd = true;
    //}
    return TPS_ER_FAILURE;
}

int PanSliceContourCmd::Undo(){
    if (mRenderProxy == nullptr){
        TPS_LOG_DEV_ERROR<<"The contouring render proxy is empty.";
        return TPS_ER_FAILURE;
    }

    if (!mRenderProxy->VoiRedoUndo(mPreState)){
        TPS_LOG_DEV_ERROR<<"Failed to execute render proxy voi redo undo.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->Render();

    UpdateVOI_i(mVoiUid);
    return TPS_ER_SUCCESS;
}

int PanSliceContourCmd::Redo(){

    if (!mRenderProxy->VoiRedoUndo(mPostState)){
        TPS_LOG_DEV_ERROR<<"Failed to execute render proxy voi redo undo.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->Render();

    UpdateVOI_i(mVoiUid);
    return TPS_ER_SUCCESS;
}

bool PanSliceContourCmd::UpdateVOI_i(const std::string &/*voiUID*/)
{
    //VOIEntity *voiEntity = nullptr;
    //mDataRepository->GetVoiManager()->GetVOI(voiUID, &voiEntity);
    //if (nullptr == voiEntity){
    //    TPS_LOG_DEV_ERROR<<"no voi with the voi UID:"<<voiUID;
    //    return false;
    //}

    //std::string imageUID = voiEntity->GetSeriesUid();
    //TpsImage3DEntity *image3D = nullptr;
    //TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    //if (!imageMng->GetImage3DData(imageUID, &image3D) || image3D == nullptr){
    //    TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
    //    return false;
    //}

    //RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUID);
    ////if (pCt2Density == nullptr)
    ////{
    ////    TPS_LOG_DEV_ERROR << "Failed to get Ct2Density.";
    ////    return TPS_ER_FAILURE;
    ////}

    ////update voi statistical information
    //if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, voiEntity)){
    //    TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
    //    return false;
    //}

    //if(mSaveObjects->SaveVoi(*voiEntity) != TPS_ER_SUCCESS){
    //    TPS_LOG_DEV_ERROR<<"SaveVOItoTMS failed VOI UID:"<<voiUID;
    //    return false;
    //}

    //EntityConverter::GetInstance()->VOIEntiyToCertifiedVOI(*voiEntity, mCertVOI);
    //if (mResponser != nullptr){
    //    mResponser->Response(*mCertVOI);
    //}
    return true;
}

TpsRedoUndoCommand* PanSliceContourCmd::Clone(){
    PanSliceContourCmd* cmd = new PanSliceContourCmd(mCertMoveVOI);
    cmd->SetDatabaseWrapper(mDatabaseWrapper);
    cmd->SetDataRepository(mDataRepository);
    cmd->SetLoadObjects(mLoadObjects);
    cmd->SetSaveObjects(mSaveObjects);
    cmd->SetRenderProxy(mRenderProxy);
    cmd->mResponser = mResponser;
    return cmd;
}
TPS_END_NAMESPACE  // end namespace tps