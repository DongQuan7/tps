////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_bl_cmd_remove_small_contours.h
/// 
///  \brief class RemoveSmallContoursCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/6/13
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_remove_small_contours.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"

#include "tps_logger.h" 
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"

TPS_BEGIN_NAMESPACE

RemoveSmallContoursCmd::RemoveSmallContoursCmd(const std::string &voiUid, 
    double area)
    :TpsVOIRedoUndoSubVolumeCmd(voiUid), mArea(area), mRemoveCount(0){

}

RemoveSmallContoursCmd::~RemoveSmallContoursCmd() {

}

int RemoveSmallContoursCmd::Execute() {
//    if(mVoiUid.empty()) {
//        TPS_LOG_DEV_ERROR<<"voi uid is empty!";
//        return TPS_ER_FAILURE;
//    }
//
//    VOIEntity *voi = nullptr;
//    if(!mDataRepository->GetVoiManager()->GetVOI(mVoiUid,&voi)) {
//        TPS_LOG_DEV_ERROR<<"Failed to get voi by uid:"
//            <<mVoiUid;
//        return TPS_ER_FAILURE;
//    }
//
//    std::string seriesUid = voi->GetSeriesUid();
//    if(seriesUid.empty()) {
//        TPS_LOG_DEV_ERROR<<"Failed to get image uid";
//        return TPS_ER_FAILURE;
//    }
//
//    TpsImage3DEntity *image = nullptr;
//    if(!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid,&image)) {
//        TPS_LOG_DEV_ERROR<<"Failed to get image 3D";
//        return TPS_ER_FAILURE;
//    }
//
//    RtImage3DHeader *header = nullptr;
//    if(!mDataRepository->GetImageDataManager()->GetImage3DHeader(seriesUid,header)) {
//        TPS_LOG_DEV_ERROR<<"Failed to get image 3D header";
//        return TPS_ER_FAILURE;
//    }
//
//    if(nullptr == header || nullptr == image) {
//        TPS_LOG_DEV_ERROR<<"image3D or header is null";
//        return TPS_ER_FAILURE;
//    }
//
//    Mcsf::Matrix4f matV2P;
//    if (!image->GetVolumeToPatientMatrix(matV2P)){
//        TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix of image:";
//        return TPS_ER_FAILURE;
//    }
//
//    McsfGeometry::SubVolume *subvolume = voi->GetSubVolume();
//    int offsetX,offsetY,offsetZ;
//    subvolume->GetOffSet(offsetX,offsetY,offsetZ);
//    int sizeX, sizeY, sizeZ;
//    subvolume->GetSize(sizeX,sizeY,sizeZ);
//    if(offsetX <0 || offsetY <0 || offsetZ <0 ||
//        sizeX <0 || sizeY <0 || sizeZ <0) {
//        TPS_LOG_DEV_ERROR<<"subvolume size or offset wrong!";
//        return TPS_ER_FAILURE;
//    }
//    //¿Õvoi²»´¦Àí
//    if (sizeX == 0 || sizeY == 0 || sizeZ == 0)
//    {
//        return true;
//    }
//
//    double xSize = header->m_dXSize;
//    double ySize = header->m_dYSize;
//    char *data = subvolume->GetData();
//
//    bool isDirty = false;
//    for(int i = 0; i < sizeZ; ++ i) {
//        std::vector<std::vector<Mcsf::Point2f>> vvContour;
//        char *tempMask = data + i * sizeX * sizeY;
//        SubVolumeHelperAPI::GetContoursFromMask((unsigned char*)tempMask,
//            (unsigned int)sizeX,(unsigned int)sizeY,vvContour);
//      /*  if(! SubVolumeHelperAPI::GetContoursFromMask((unsigned char*)tempMask,
//            (unsigned int)sizeX,(unsigned int)sizeY,vvContour) ) {
//                TPS_LOG_DEV_ERROR<<"Failed to get mask from contour!";
//                return false;
//        }*/
//        isDirty = false;
//        std::vector<std::vector<Mcsf::Point2f>> contourResult;
//        for(auto itr = vvContour.begin(); itr != vvContour.end(); ++itr) {
//            if(mArea < GetArea_i(*itr,xSize,ySize,sizeX,sizeY)) {
//                contourResult.push_back(*itr);
//                isDirty = true;
//            }
//            else {
//                mRemoveCount++;
//            }
//        }
//        if(isDirty) {
//           voi->SetSliceDirtyForAna(offsetZ + i);
//        }
//
//        memset(tempMask,0,sizeof(unsigned char)*sizeY*sizeX);
//        SubVolumeHelperAPI::GetMaskFromContours(contourResult,sizeX,sizeY,(unsigned char*)tempMask);
//    }
//
////     boost::thread thrd(boost::bind(&SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct, 
////         matV2P, voi, 1.0f));
//    SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct(matV2P, voi);
//    mRenderProxy->UpdateVOI(seriesUid,mVoiUid);
//    mRenderProxy->Render();
//    UpdateVOI_i(mVoiUid);
    //update voi to front end.
    return TPS_ER_SUCCESS;
}

double RemoveSmallContoursCmd::GetArea_i(const std::vector<Mcsf::Point2f> &contour, 
    double xSize, double ySize , int width, int height) {

    std::vector<std::vector<Mcsf::Point2f>> vvContour;
    vvContour.push_back(contour);

    unsigned char* buffer = new unsigned char[width * height];
    memset(buffer,0,width * height * sizeof(unsigned char));
    SubVolumeHelperAPI::GetMaskFromContours(vvContour,width,height,buffer);

    int totalNum = 0;
    for(int i=0; i<width; ++i) {
        for(int j=0; j<height; ++j) {
            if(buffer[j*width + i] == 1) {
                totalNum++;
            }
        }
    }

    delete [] buffer;
    double area = xSize * ySize * totalNum;
    return area;
}

bool RemoveSmallContoursCmd::UpdateVOI_i(const std::string &/*voiUID*/)
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

    //EntityConverter::GetInstance()->VOIEntiyToCertifiedVOI(*voiEntity, mCertVoi);
    //if (mResponser != nullptr){
    //    mResponser->Response(*mCertVoi);
    //}
    return true;
}

TpsRedoUndoCommand* RemoveSmallContoursCmd::Clone(){
    RemoveSmallContoursCmd* cmd = new RemoveSmallContoursCmd(mVoiUid, mArea);
    cmd->SetDatabaseWrapper(mDatabaseWrapper);
    cmd->SetDataRepository(mDataRepository);
    cmd->SetLoadObjects(mLoadObjects);
    cmd->SetSaveObjects(mSaveObjects);
    cmd->SetRenderProxy(mRenderProxy);
    cmd->mSubvolume = std::move(mSubvolume);
    cmd->mResponser = mResponser;
    return cmd;
}
TPS_END_NAMESPACE
