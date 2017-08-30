//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_cca_smartcontour_cmd.cpp
///  \brief   SmartContour
///
///  \version 1.0
///  \date    06. 10, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_operatecontourslices.h"

//TPS
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE

OperateContourCmd::OperateContourCmd(CertifiedContoursSlices certifiedContourSlices) 
    : mCertifiedContourSlices(certifiedContourSlices) {
}

OperateContourCmd::~OperateContourCmd(void) {
}


int OperateContourCmd::Execute() {

    //if (mRenderProxy == nullptr){
    //    TPS_LOG_DEV_ERROR<<"The contouring render proxy is empty.";
    //    return TPS_ER_FAILURE;
    //}
    //if(mCertifiedContourSlices.mEditMode == JUDGECOMMONPART) {
    //    VOIEntity *voiEntity = nullptr;
    //    SubVolume *subVolume = nullptr;
    //    char* subData = nullptr;
    //    int slice = 0, totalSlice = 0, startSliceNumber = 0;
    //    mDataRepository->GetVoiManager()->GetVOI(mCertifiedContourSlices.mVOIUid,&voiEntity);
    //    subVolume = voiEntity->GetSubVolume();
    //    subData = subVolume->GetData();
    //    int offX = 0, offY = 0, offZ = 0;
    //    int sizeX = 0, sizeY = 0, sizeZ = 0;
    //    subVolume->GetOffSet(offX, offY, offZ);
    //    subVolume->GetSize(sizeX, sizeY, sizeZ);
    //    mRenderProxy->GetCurrentMprSlice((WINDOW_TYPE)mCertifiedContourSlices.mWindowType, mCertifiedContourSlices.mWindowUid, slice, startSliceNumber, totalSlice);
    //    slice--;
    //    int cutSlice = 0;
    //    cutSlice = voiEntity->GetCutOrCopySlice();
    //    int countInPlane = sizeX * sizeY;
    //    int flag = 0;
    //    mCertifiedContourSlices.mSubEditMode = COMBINE;
    //    if (cutSlice >= offZ && cutSlice < offZ + sizeZ &&
    //        slice >=offZ && slice < offZ + sizeZ) {
    //            for (int i = 0; i < sizeX; ++i) {
    //                for (int j = 0; j < sizeY; ++j) {
    //                    flag = subData[countInPlane * (slice - offZ) + i * sizeY + j];
    //                    if (flag == 1) {
    //                        mCertifiedContourSlices.mSubEditMode = REPLACE;
    //                        return TPS_ER_SUCCESS;
    //                    }
    //                }
    //            }
    //    }
    //}
    //else {
    //    if (mCertifiedContourSlices.mEditMode == CUT || mCertifiedContourSlices.mEditMode == COPY) {
    //        VOIEntity *voiEntity = nullptr;
    //        if(!mDataRepository->GetVoiManager()->GetVOI(mCertifiedContourSlices.mVOIUid,&voiEntity)) {
    //            return TPS_ER_FAILURE;
    //        }
    //        voiEntity->SetCutOrCopySlice(mCertifiedContourSlices.mFirstSlice);
    //    }
    //    mRenderProxy->OperateContourSlices((WINDOW_TYPE)mCertifiedContourSlices.mWindowType,
    //        mCertifiedContourSlices.mWindowUid,mCertifiedContourSlices.mVOIUid,
    //        mCertifiedContourSlices.mFirstSlice,
    //        mCertifiedContourSlices.mSecondSlice,
    //        mCertifiedContourSlices.mEditMode,
    //        mCertifiedContourSlices.mSubEditMode);
    //    if(mCertifiedContourSlices.mEditMode == PASTE || mCertifiedContourSlices.mEditMode == DELETEMode) {
    //        UpdateVOI_i(mCertifiedContourSlices.mVOIUid);
    //    }
    //}
    //mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

bool OperateContourCmd::UpdateVOI_i(const std::string &/*voiUID*/)
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
    return true;
}

CertifiedContoursSlices OperateContourCmd::GetCertifiedContoursSlices() {
    return mCertifiedContourSlices;
}

int OperateContourCmd::PostExecute() {

    //delete all related commands in redo/undo list of CommandManager
    auto mng = mDataRepository->GetCommandManager();
    TpsSafeLock<TpsCommandManager> lock(mng);
    mng->RemoveCommands(
        [&](TpsRedoUndoCommand* cmd)->bool{
            auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
            return voiCmd != nullptr && voiCmd->GetVoiUid() == mCertifiedContourSlices.mVOIUid;
    });
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE
