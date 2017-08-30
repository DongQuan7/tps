////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_cca_editregistrationregioncmd.cpp
/// 
///  \brief class EditRegistrationRegionCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/05
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_edit_registration_region.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "tps_logger.h"

#define HIT_TOLERANCE 5
#define MIN_CUBE_SIZE 50.f //unit: mm

TPS_BEGIN_NAMESPACE   // begin namespace tps

EditRegistrationRegionCmd::EditRegistrationRegionCmd(WINDOW_TYPE wndType, 
    const std::string& wndUid, double x, double y, int actionType)
    : mWndType(wndType), mWndUid(wndUid), mX(x), mY(y), mActionType(actionType){

}

EditRegistrationRegionCmd::~EditRegistrationRegionCmd(){

}

int EditRegistrationRegionCmd::Execute(){
    auto mng = mDataRepository->GetFusionSecondaryDataManager();
    if (!mng->IsRegionReady()){
        TPS_LOG_DEV_ERROR<<"The registration region of interested is not ready.";
        return TPS_ER_FAILURE;
    }

    DATA_SOURCE_GROUP_ID cw;
    mRenderProxy->CheckDataSourceGroup(mWndType, cw);
    if (cw != SOURCE_GROUP_FUSION_FIXED && cw != SOURCE_GROUP_FUSION_FLOAT){
        TPS_LOG_DEV_ERROR<<"The window can not editing registration region.";
        return TPS_ER_FAILURE;
    }

    Mcsf::Point3f point;
    if (!mRenderProxy->TransformPoint(mWndType, mWndUid, Mcsf::Point2f(mX, mY), point)){
        TPS_LOG_DEV_ERROR<<"Failed to transform point.";
        return TPS_ER_FAILURE;
    }

    bool isEditingFixed = cw == SOURCE_GROUP_FUSION_FIXED;
    FUSION_ROLE role = isEditingFixed ? FUSION_ROLE_FIXED : FUSION_ROLE_FLOAT;
    char editingFlag = mng->GetEditingPlaneFlag(role);
    char mask = (0x01<<6) - 1;
    const int* region;
    mng->GetRegion(region);
    if (!isEditingFixed) region += 6;

    std::string imageUID;
    if (isEditingFixed) imageUID = mng->GetFixedImageUID();
    else imageUID = mng->GetFloatImageUID();
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        imageUID, &image) || image == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image data with uid: "<<imageUID;
        return TPS_ER_FAILURE;
    }
    Mcsf::Matrix4f matPatient2Volume;
    if (!image->GetVolumeToPatientMatrix(matPatient2Volume)){
        TPS_LOG_DEV_ERROR<<"Failed to volume to patient matrix with uid: "<<imageUID;
        return TPS_ER_FAILURE;
    }
    matPatient2Volume = matPatient2Volume.Inverse();
    point = matPatient2Volume.Transform(point);

    if (((editingFlag & mask) == 0) && mActionType == DOWN){
        HitRegionCube_i(point, region, matPatient2Volume, editingFlag);
        mng->SetEditingPlaneFlag(editingFlag, role);
    }
    else if ((editingFlag & mask) != 0 && mActionType == DRAG){
        UpdateRegionCube_i(*image, point, editingFlag, isEditingFixed, region, matPatient2Volume);
        mRenderProxy->UpdateFusionRegion(isEditingFixed ? OnlyFixedRegion : OnlyFloatRegion);
        mRenderProxy->Render();
    }
    else if(mActionType == UP){
        mng->ClearEditingRegionFlag();
    }
    return TPS_ER_SUCCESS;
}

void EditRegistrationRegionCmd::HitRegionCube_i(
    const Mcsf::Point3f& point, const int* region, 
    const Mcsf::Matrix4f& matP2V, char& hitFlag){
    DISPLAY_SIZE displaySize;
    mRenderProxy->GetDisplaySize(mWndType, mWndUid, displaySize);

    Mcsf::Point2f pt1(0.f, 0.f);
    Mcsf::Point2f pt2(HIT_TOLERANCE / (float)displaySize.width, 0.f);
    Mcsf::Point2f pt3(0.f, HIT_TOLERANCE / (float)displaySize.height);
    Mcsf::Point3f ptInPatient1, ptInPatient2;
    mRenderProxy->TransformPoint(mWndType, mWndUid, pt1, ptInPatient1);
    mRenderProxy->TransformPoint(mWndType, mWndUid, pt2, ptInPatient2);
    float hitToleranceInPatient = Mcsf::Vector3f(ptInPatient2 - ptInPatient1).Magnitude();

    float hitTolerance[3] = {
        matP2V.Transform(
        Mcsf::Vector3f(hitToleranceInPatient, 0.f, 0.f)).Magnitude(), 
        matP2V.Transform(
        Mcsf::Vector3f(0.f, hitToleranceInPatient, 0.f)).Magnitude(), 
        matP2V.Transform(
        Mcsf::Vector3f(0.f, 0.f, hitToleranceInPatient)).Magnitude()};

    hitFlag = 0;
    float distance;
    float hit[6] = {point.GetX(), point.GetX(), 
        point.GetY(), point.GetY(), point.GetZ(), point.GetZ()};
    for (int i=0; i<6; ++i){
        distance = hit[i] - region[i];
        if (i % 2 == 0) distance *= -1;
        if (distance > hitTolerance[i/2]){
            hitFlag = 0;
            break;
        }
        if (abs(distance) <= hitTolerance[i/2]){
            hitFlag |= (0x01<<i);
        }
    }
}

void EditRegistrationRegionCmd::UpdateRegionCube_i(const TpsImage3DEntity& image, 
    const Mcsf::Point3f& point, char editFlag, bool isEditingFixed, 
    const int* region, const Mcsf::Matrix4f& matP2V){

    RtImage3DHeader& header = *image.GetRtSeries()->get_header();
    int dim[3] = { header.m_iXDim, header.m_iYDim, header.m_iSliceCount };
    float dragPos[3] = {TPS_CLAMP(point.GetX(), 0, dim[0] - 1), 
        TPS_CLAMP(point.GetY(), 0, dim[1] - 1), 
        TPS_CLAMP(point.GetZ(), 0, dim[2] - 1)};
    float minSizeInVolume[3] = {
        matP2V.Transform(Mcsf::Vector3f(MIN_CUBE_SIZE, 0.f, 0.f)).Magnitude(), 
        matP2V.Transform(Mcsf::Vector3f(0.f, MIN_CUBE_SIZE, 0.f)).Magnitude(), 
        matP2V.Transform(Mcsf::Vector3f(0.f, 0.f, MIN_CUBE_SIZE)).Magnitude()
    };

    auto mng = mDataRepository->GetFusionSecondaryDataManager();
    float realDragPos;
    int index, arg;
    for (int i=0; i<6; ++i){
        if ((editFlag & (0x01<<i)) == 0) continue;
        index = i/2;
        realDragPos = dragPos[index];
        arg = i % 2 == 0 ? 1 : -1;
        if (arg * (region[i + arg] - realDragPos) < minSizeInVolume[index]){
            realDragPos = region[i + arg] - arg * minSizeInVolume[index];
            realDragPos = TPS_CLAMP(realDragPos, 0, dim[index] - 1);
        }
        mng->UpdateRegion((TpsFusionSecondaryDataManager::EditingPlane)i,
            realDragPos, isEditingFixed ? FUSION_ROLE_FIXED : FUSION_ROLE_FLOAT);
    }
}
TPS_END_NAMESPACE  // end namespace tps