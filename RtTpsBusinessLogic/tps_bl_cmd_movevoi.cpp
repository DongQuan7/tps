////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_cca_movevoi_cmd.cpp
/// 
///  \brief class MoveVOICmd  
/// 
///  \version 1.0
/// 
///  \date    2014/6/19
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_movevoi.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/point2d.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_image3d.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_voi.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_safelock.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

MoveVOICmd::MoveVOICmd(const CertifiedMoveVOI &certified):mCertifiedMoveVOI(certified) {

}

MoveVOICmd::~MoveVOICmd() {

}

int MoveVOICmd::Execute() 
{
    if(MoveVOI_i())
    {
        return TPS_ER_SUCCESS;
    }
    return TPS_ER_FAILURE;
}

bool MoveVOICmd::MoveVOI_i()
{
    //std::string voiUid = mCertifiedMoveVOI.mVoiUID;
    //if(voiUid.empty()) {
    //    TPS_LOG_DEV_ERROR<<"voi uid is empty!";
    //    return false;
    //}
    //VOIEntity *voi = nullptr;
    //if(!mDataRepository->GetVoiManager()->GetVOI(voiUid,&voi)) {
    //    TPS_LOG_DEV_ERROR<<"Failed to get voi by uid:"
    //        <<voiUid;
    //    return false;
    //}

    //McsfGeometry::SubVolume *subvolume = voi->GetSubVolume();
    //if(nullptr == subvolume) {
    //    TPS_LOG_DEV_ERROR<<"subvolume is null!";
    //    return false;
    //}
    //int offsetX,offsetY,offsetZ;
    //subvolume->GetOffSet(offsetX,offsetY,offsetZ);
    //int sizeX, sizeY, sizeZ;
    //subvolume->GetSize(sizeX,sizeY,sizeZ);
    //if(offsetX <0 || offsetY <0 || offsetZ <0 ||
    //    sizeX <0 || sizeY <0 || sizeZ <0) {
    //    TPS_LOG_DEV_ERROR<<"subvolume size or offset wrong!";
    //    return TPS_ER_FAILURE;
    //}
    ////空voi不处理
    //if (sizeX == 0 || sizeY == 0 || sizeZ == 0)
    //{
    //    return true;
    //}

    //mCertifiedMoveVOI.mIsEditEnd = mCertifiedMoveVOI.mIsEditByOffset || 
    //    mCertifiedMoveVOI.mMouseActionType == UP;
    //auto stateMng = mDataRepository->GetDisplayStateManager();
    ////get image 3D;
    //std::string imageUID;
    //mRenderProxy->GetDataSource(SOURCE_GROUP_COMMON, IMAGE, imageUID);
    //if(imageUID.empty()) {
    //    TPS_LOG_DEV_ERROR<<"failed to get image uid";
    //    return false;
    //}
    //TpsImage3DEntity* image = nullptr;
    //if(!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID,&image)) {
    //    TPS_LOG_DEV_ERROR<<"failed to get image 3D";
    //    return false;
    //}
    //RtImage3DHeader *header = nullptr;
    //if(!image->GetImage3DHeader(header)) {
    //    TPS_LOG_DEV_ERROR<<"Failed to get image 3D header";
    //    return false;
    //}

    //Mcsf::Matrix4f matV2P;
    //if (!image->GetVolumeToPatientMatrix(matV2P)){
    //    TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix of image:";
    //    return false;
    //}

    //Mcsf::Matrix4f matP2V = matV2P.Inverse();
    //Mcsf::Vector3f vecTranslate;

    //if(nullptr == mRenderProxy) {
    //    TPS_LOG_DEV_ERROR<<"failed to get render proxy!";
    //    return false;
    //}

    //if(mCertifiedMoveVOI.mIsEditByOffset) {
    //    vecTranslate = matP2V.Transform(Mcsf::Vector3f(mCertifiedMoveVOI.mOffsetX, 
    //        mCertifiedMoveVOI.mOffsetY, mCertifiedMoveVOI.mOffsetZ));
    //}
    //else {
    //    Mcsf::Point2f preCoord(mCertifiedMoveVOI.mPreCoordX,mCertifiedMoveVOI.mPreCoordY);
    //    Mcsf::Point2f curCoord(mCertifiedMoveVOI.mCurCoordX,mCertifiedMoveVOI.mCurCoordY);
    //    Mcsf::Point3f preCoordInPat;
    //    Mcsf::Point3f curCoordInPat;
    //    mRenderProxy->TransformPoint((WINDOW_TYPE)mCertifiedMoveVOI.mWindowType, mCertifiedMoveVOI.mWindowUid,preCoord,preCoordInPat);
    //    mRenderProxy->TransformPoint((WINDOW_TYPE)mCertifiedMoveVOI.mWindowType, mCertifiedMoveVOI.mWindowUid,curCoord,curCoordInPat);
    //    vecTranslate= matP2V.Transform(Mcsf::Vector3f(curCoordInPat - preCoordInPat));
    //}
    //double tranLen = vecTranslate.Magnitude();
    //if (tranLen > 1e-6)
    //{
    //    vecTranslate.Normalize();
    //}

    //size_t dim[3] = {header->m_iXDim, header->m_iYDim, header->m_iSliceCount};
    //SubVolumeHelperAPI::RefineSubvolume(dim, voi);

    //int boundingBox[6] = {offsetX, offsetX + sizeX - 1, 
    //    offsetY, offsetY + sizeY - 1, offsetZ, offsetZ + sizeZ - 1};

    //bool outOfRange = false;
    //double tmp;
    //if (vecTranslate.GetX() < 0){
    //    tmp = boundingBox[0] / fabs(vecTranslate.GetX());
    //    outOfRange |= tmp < tranLen;
    //    if (tmp < tranLen) tranLen = tmp;
    //}
    //if (vecTranslate.GetX() > 0){
    //    tmp = (dim[0] - 1 - boundingBox[1]) / fabs(vecTranslate.GetX());
    //    outOfRange |= tmp < tranLen;
    //    if (tmp < tranLen) tranLen = tmp;
    //}
    //if (vecTranslate.GetY() < 0){
    //    tmp = boundingBox[2] / fabs(vecTranslate.GetY());
    //    outOfRange |= tmp < tranLen;
    //    if (tmp < tranLen) tranLen = tmp;
    //}
    //if (vecTranslate.GetY() > 0){
    //    tmp = (dim[1] - 1 - boundingBox[3]) / fabs(vecTranslate.GetY());
    //    outOfRange |= tmp < tranLen;
    //    if (tmp < tranLen) tranLen = tmp;
    //}
    //if (vecTranslate.GetZ() < 0){
    //    tmp = boundingBox[4] / fabs(vecTranslate.GetZ());
    //    outOfRange |= tmp < tranLen;
    //    if (tmp < tranLen) tranLen = tmp;
    //}
    //if (vecTranslate.GetZ() > 0){
    //    tmp = (dim[2] - 1 - boundingBox[5]) / fabs(vecTranslate.GetZ());
    //    outOfRange |= tmp < tranLen;
    //    if (tmp < tranLen) tranLen = tmp;
    //}
    //vecTranslate *= tranLen;

    //int newOffsetX = offsetX + TPS_ROUND(vecTranslate.GetX());
    //int newOffsetY = offsetY + TPS_ROUND(vecTranslate.GetY());
    //int newOffsetZ = offsetZ + TPS_ROUND(vecTranslate.GetZ());
    //if (newOffsetX == offsetX && newOffsetY == offsetY && 
    //    newOffsetZ == offsetZ)
    //{
    //    if(mCertifiedMoveVOI.mIsEditEnd && stateMng->GetMoveVoiFlag())
    //    {
    //        UpdateVOI_i(voiUid);
    //        stateMng->SetMoveVoiFlag(false);
    //    }
    //    return true;
    //}

    ////暂时复用mIsEditCurrentSlice表示超出范围，对原设定数据进行了截取
    //if(outOfRange) {
    //    if (mCertifiedMoveVOI.mIsEditByOffset){
    //        // use this flag to check if the voi is out of range(show warning only when edit by offset)
    //        mCertifiedMoveVOI.mIsShowWarning = true;
    //    }
    //}

    //// update the interpolates
    //if (newOffsetZ != offsetZ) {
    //    std::vector<bool> interpolates = voi->GetInterpolationFlag();
    //    if(newOffsetZ - offsetZ > 0) {
    //        for (auto it = interpolates.begin() + offsetZ + sizeZ; it > interpolates.begin() + offsetZ; --it) {
    //            *(it + (newOffsetZ - offsetZ)) = *it;
    //            *it = true;
    //        }
    //    }
    //    else if(newOffsetZ - offsetZ < 0){
    //        for (auto it = interpolates.begin() + offsetZ; it < interpolates.begin() + offsetZ + sizeZ; ++it) {
    //            *(it + (newOffsetZ - offsetZ)) = *it;
    //            *it = true;
    //        }
    //    }
    //    voi->SetInterpolationFlag(interpolates);
    //}

    //subvolume->SetOffSet(newOffsetX,newOffsetY,newOffsetZ);
    //voi->SetSlicesDirtyForAna(offsetZ, offsetZ + sizeZ);
    //voi->SetSlicesDirtyForAna(newOffsetZ, newOffsetZ + sizeZ);
    ////     boost::thread thrd(boost::bind(&SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct, 
    ////         matV2P, voi, 1.0f));
    //SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct(matV2P, voi);
    //mRenderProxy->UpdateVOI(imageUID,voiUid);
    //mRenderProxy->Render();
    //stateMng->SetMoveVoiFlag(true);
    //if(mCertifiedMoveVOI.mIsEditEnd)
    //{
    //    UpdateVOI_i(voiUid);
    //    stateMng->SetMoveVoiFlag(false);
    //}
    return true;
}

bool MoveVOICmd::UpdateVOI_i(const std::string &/*voiUID*/)
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
    ////update voi statistical information
    //if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, voiEntity)){
    //    TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
    //    return false;
    //}

    //if (mSaveObjects->SaveVoi(*voiEntity) != TPS_ER_SUCCESS){
    //    TPS_LOG_DEV_ERROR << "SaveVOItoTMS failed VOI UID:" << voiUID;
    //    return false;
    //}
    return true;
}

int MoveVOICmd::PostExecute()
{
    //delete all related commands in redo/undo list of CommandManager
    auto mng = mDataRepository->GetCommandManager();
    TpsSafeLock<TpsCommandManager> lock(mng);
    mng->RemoveCommands(
        [&](TpsRedoUndoCommand* cmd)->bool{
            auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
            return voiCmd != nullptr && voiCmd->GetVoiUid() == mCertifiedMoveVOI.mVoiUID;
    });
    return TPS_ER_SUCCESS;
}

CertifiedMoveVOI MoveVOICmd::GetCertifiedVOI(){

    return mCertifiedMoveVOI;
}

TPS_END_NAMESPACE
