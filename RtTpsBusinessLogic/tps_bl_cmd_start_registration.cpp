////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_start_registration.cpp
/// 
///  \brief class StartRegistrationCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/10/29
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_start_registration.h"

#include <omp.h>
#include "boost/lexical_cast.hpp"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsAlgorithmProxy/tps_ap_fusion_algorithmproxy.h"
#include "RtTpsFramework/tps_fw_progress.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsCertifiedLib/rt_tps_certified_landmarks_points.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"



#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

StartRegistrationCmd::StartRegistrationCmd(const std::string& fixedUid, 
    const std::string& floatUid, int algorithm) : mFixedSeriesUid(fixedUid), 
    mFloatSeriesUid(floatUid), mAlgorithm(algorithm), mProgressResponser(nullptr)
{
    mLandmarkPoints = new Mcsf::Point3f[LANDMARK_POINT_PAIR_COUNT * 2];
}

StartRegistrationCmd::~StartRegistrationCmd()
{
    TPS_DEL_ARRAY(mLandmarkPoints);
}

void StartRegistrationCmd::SetProgressResponser(
    const std::shared_ptr<ProgressActiveResponser>& responser)
{
    mProgressResponser = responser;
}

int StartRegistrationCmd::SetLandmarkPoints(const CertifiedLandmarkPoints& landmarkPointsList)
    //(vector<Mcsf::Point3f> pLandmarkPoints)
{
    auto points = landmarkPointsList.landmarkPointsList;
    if (points.size() != LANDMARK_POINT_PAIR_COUNT * 2)
    {
        TPS_LOG_DEV_ERROR<<"Landmark point number is not right";
        return TPS_ER_FAILURE;
    }
    for (int i = 0; i < points.size(); i++)
    {
		mLandmarkPoints[i].SetX(points[i].mX);
		mLandmarkPoints[i].SetY(points[i].mY);
		mLandmarkPoints[i].SetZ(points[i].mZ);
    }

    return TPS_ER_SUCCESS;
}

int StartRegistrationCmd::Execute(){
    TPS_LOG_DEV_INFO<<"Start Registration Command ENTER!";

    bool imagesAreReady = true;
    TpsImage3DEntity *fixedImage = nullptr, *floatImage = nullptr;
    auto imageMng = mDataRepository->GetImageDataManager();
    imagesAreReady &= (!mFixedSeriesUid.empty() && imageMng->GetImage3DData(
        mFixedSeriesUid, &fixedImage) && fixedImage != nullptr);
    imagesAreReady &= (!mFloatSeriesUid.empty() && imageMng->GetImage3DData(
        mFloatSeriesUid, &floatImage) && floatImage != nullptr);

    if (!imagesAreReady){
        TPS_LOG_DEV_ERROR<<"The images are not ready.";
        return TPS_ER_FAILURE;
    }
    if (!StartRegister_i(fixedImage, floatImage, mAlgorithm)){
        TPS_LOG_DEV_ERROR<<"Failed to execute image registration.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->Render();

    TPS_LOG_DEV_INFO<<"Start Registration Command LEAVE!";
    return TPS_ER_SUCCESS;
}

CertifiedRegistrationFeedback StartRegistrationCmd::GetFeedback() const{
    return mCertFeedback;
}

bool StartRegistrationCmd::StartRegister_i(TpsImage3DEntity* fixedImage, 
    TpsImage3DEntity* floatImage, int registrationAlgo){
    bool bRes = false;
    switch(registrationAlgo){
    case INIT_SETUP:
        bRes = RegisterInitSetup_i(fixedImage, floatImage);
        break;
    case LANDMARK:
        bRes = RegisterLandmark_i();
        break;
    case ALIGNMENT:
        bRes = RegisterAlignment_i(fixedImage, floatImage);
        break;
    case DEFORMABLE:
        bRes = RegisterDeformable_i(fixedImage, floatImage);
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unrecognized registration algorithm.";
        bRes = false;
        break;
    }
    if (!bRes){
        TPS_LOG_DEV_ERROR<<"StartRegister_i failed!";
    }
    return bRes;
}

bool StartRegistrationCmd::RegisterInitSetup_i(
    TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage){
    TPS_LOG_DEV_INFO<<"RegisterInitSetup_i ENTER!";
    std::string floatImageUID = floatImage->GetUID();

    Mcsf::Matrix4f matrix;
    if (!fixedImage->GetRegistrationMatrix(floatImageUID, matrix)){
        //get the default matrix
        FusionAlgorithmProxy::Register_InitSetup(*fixedImage, *floatImage, matrix);
    }
    mDataRepository->GetFusionSecondaryDataManager()->SetRigidResult(matrix);
    mRenderProxy->StartRegister();
    TPS_LOG_DEV_INFO<<"RegisterInitSetup_i LEAVE!";
    return true;
}

bool StartRegistrationCmd::RegisterLandmark_i()
{
    TPS_LOG_DEV_INFO<<"RegisterLandmark_i ENTER!";

    TpsProgress progress;
    if (mProgressResponser) 
    {
        progress.SetProgressResponser(mProgressResponser);
    }

    progress.SetCurrent(0.0);
	//把判断是否共面的逻辑放前端
    if (IsLandmarkPointsInSamePlane()){
        TPS_LOG_DEV_ERROR<<"The landmark points are in same plane.";
        mCertFeedback.mFeedbackType = LANDMARK_POINTS_IN_SAME_PLANE;
        progress.SetCancelFlag(true);
        return false;
    }

    const Mcsf::Point3f* landmarks;
    unsigned int pairCount;
    landmarks = mLandmarkPoints;
    pairCount = LANDMARK_POINT_PAIR_COUNT;

    Mcsf::Matrix4f matrix;
    progress.SetCurrent(0.5);
    if (!FusionAlgorithmProxy::Register_Landmark(landmarks, landmarks + pairCount, 
        pairCount, matrix)){
            TPS_LOG_DEV_ERROR<<"Failed to invoke FusionAlgorithmProxy::Register_Landmark.";
            progress.SetCancelFlag(true);
            return false;
    }

    mDataRepository->GetFusionSecondaryDataManager()->SetRigidResult(matrix);

    progress.SetCurrent(0.8);

    mCertFeedback.mFeedbackType = LANDMARK_POINTS_MARKED_FLAG_CHANGED;
    mRenderProxy->RefreshRegistrationResult();
    progress.SetCurrent(1.0);
    TPS_LOG_DEV_INFO<<"RegisterLandmark_i LEAVE!";
    return true;
}

bool StartRegistrationCmd::IsLandmarkPointsInSamePlane() const{
    bool isFixedInSamePlane = true;
    Mcsf::Point3f point1 = mLandmarkPoints[0];
    Mcsf::Point3f point2, point3;
    int i = 1;
    //find second point not equal to first point
    for (; i < LANDMARK_POINT_PAIR_COUNT && point1 == mLandmarkPoints[i]; ++i);
    if (i == LANDMARK_POINT_PAIR_COUNT) return true;
    point2 = mLandmarkPoints[i];
    Mcsf::Vector3f vec1 = point2 - point1;
    //find third point not in the same line with point1 & 2
    Mcsf::Vector3f vec2;
    for (; i < LANDMARK_POINT_PAIR_COUNT; ++i){
        vec2 = mLandmarkPoints[i] - point1;
        if (abs(vec2.CrossProduct(vec1).Magnitude()) > 1e-6) break;
    }
    if (i == LANDMARK_POINT_PAIR_COUNT) return true;
    Mcsf::Vector3f normal = vec2.CrossProduct(vec1);
    for (; i <LANDMARK_POINT_PAIR_COUNT; ++i){
        if (abs(normal.DotProduct(mLandmarkPoints[i] - point1)) > 1e-6){
            isFixedInSamePlane = false;
            break;
        }
    }
    if (isFixedInSamePlane) return true;

    bool isFloatInSamePlane = true;
    point1 = mLandmarkPoints[LANDMARK_POINT_PAIR_COUNT];
    i = LANDMARK_POINT_PAIR_COUNT + 1;
    //find second point not equal to first point
    for (; i < 2 * LANDMARK_POINT_PAIR_COUNT && point1 == mLandmarkPoints[i]; ++i);
    if (i == 2 * LANDMARK_POINT_PAIR_COUNT) return true;
    point2 = mLandmarkPoints[i];
    vec1 = point2 - point1;
    //find third point not in the same line with point1 & 2
    for (; i < 2 * LANDMARK_POINT_PAIR_COUNT; ++i){
        vec2 = mLandmarkPoints[i] - point1;
        if (abs(vec2.CrossProduct(vec1).Magnitude()) > 1e-6) break;
    }
    if (i == 2 * LANDMARK_POINT_PAIR_COUNT) return true;
    normal = vec2.CrossProduct(vec1);
    for (; i < 2 * LANDMARK_POINT_PAIR_COUNT; ++i){
        if (abs(normal.DotProduct(mLandmarkPoints[i] - point1)) > 1e-6){
            isFloatInSamePlane = false;
            break;
        }
    }
    return isFloatInSamePlane;
}




bool StartRegistrationCmd::RegisterAlignment_i(
    TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage){

    TPS_LOG_DEV_INFO<<"RegisterAlignment_i ENTER!";
    auto fsMng = mDataRepository->GetFusionSecondaryDataManager();
    std::string floatImageUID = floatImage->GetUID();
    TpsProgress progress;
    if (mProgressResponser) 
    {
        progress.SetProgressResponser(mProgressResponser);
    }
    progress.SetCurrent(0.0);
    auto header = fixedImage->GetRtSeries()->get_header();
    int fixedDim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
    int fixedCount = fixedDim[0] * fixedDim[1] * fixedDim[2];
    unsigned char* fixedRoiMask = new unsigned char[fixedCount];
    
    header = floatImage->GetRtSeries()->get_header();
    int floatDim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
    int floatCount = floatDim[0] * floatDim[1] * floatDim[2];
    unsigned char* floatRoiMask = new unsigned char[floatCount];
    
    if (fsMng->IsRegionReady()){
        memset(fixedRoiMask, 0x0, fixedCount * sizeof(unsigned char));
        memset(floatRoiMask, 0x0, floatCount * sizeof(unsigned char));
        SetupRoiMask_i(fixedDim, floatDim, fixedRoiMask, floatRoiMask);
    }
    else{
        memset(fixedRoiMask, 0xff, fixedCount * sizeof(unsigned char));
        memset(floatRoiMask, 0xff, floatCount * sizeof(unsigned char));
    }

    /*FILE *fp3;
    fopen_s(&fp3,"D:/FixedRoiMask.dat","wb");
    fwrite(fixedRoiMask,sizeof(unsigned char),fixedCount,fp3);
    fclose(fp3);

    FILE *fp1;
    fopen_s(&fp1,"D:/FloatRoiMask.dat","wb");
    fwrite(floatRoiMask,sizeof(unsigned char),floatCount,fp1);
    fclose(fp1);*/
    progress.SetCurrent(0.1);

    Mcsf::Matrix4f matrix;
    bool bIterative = fixedImage->GetRegistrationMatrix(floatImageUID, matrix);
    progress.SetProgressWeight(0.8);
    progress.SetProgressStartValue(0.1);

    if (!FusionAlgorithmProxy::Register_Alignment(*fixedImage, *floatImage, 
        fixedRoiMask, floatRoiMask, SOFT_TISSUE, (Mcsf::IProgress*)&progress, matrix, bIterative)){
        TPS_LOG_DEV_ERROR<<"Failed to invoke FusionAlgorithmProxy::Register_Alignment.";
        if (fixedRoiMask) delete[] fixedRoiMask; 
        if (floatRoiMask) delete[] floatRoiMask;
        progress.SetCancelFlag(true);
        return false;
    }

    progress.Reset();
    progress.SetCurrent(0.9);
    fsMng->SetRigidResult(matrix);
//     if (!fixedImage->SetRegistrationMatrix(floatImageUID, matrix)){
//         TPS_LOG_DEV_ERROR<<"Failed to set registration matrix.";
//         if (fixedRoiBB) delete[] fixedRoiBB; 
//         if (floatRoiBB) delete[] floatRoiBB;
//         progress.SetCancelFlag(true);
//         return false;
//     }
    if (fixedRoiMask) delete[] fixedRoiMask; 
    if (floatRoiMask) delete[] floatRoiMask;

    mRenderProxy->RefreshRegistrationResult();
    if (fsMng->IsRegionVisible(FUSION_ROLE_FIXED) || 
        fsMng->IsRegionVisible(FUSION_ROLE_FLOAT)){
        mCertFeedback.mFeedbackType = HIDE_REGISTRATION_REGION;

        fsMng->SetRegionVisible(FUSION_ROLE_FIXED, false);
        fsMng->SetRegionVisible(FUSION_ROLE_FLOAT, false);
        mRenderProxy->UpdateFusionRegion();
    }
    progress.SetCurrent(1.0);
    TPS_LOG_DEV_INFO<<"RegisterAlignment_i LEAVE!";
    return true;
}

bool StartRegistrationCmd::RegisterDeformable_i(
    TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage){

    auto fsMng = mDataRepository->GetFusionSecondaryDataManager();
    std::string floatImageUID = floatImage->GetUID();
    TpsProgress progress;
    if (mProgressResponser) 
    {
        progress.SetProgressResponser(mProgressResponser);
    }
    progress.SetCurrent(0.0);

    const int* region;
    fsMng->GetRegion(region);
    unsigned int *fixedRoiBB(nullptr), *floatRoiBB(nullptr);
    if (fsMng->IsRegionReady()){
        fixedRoiBB = new unsigned int[6];
        floatRoiBB = new unsigned int[6];
        for (int i = 0; i < 6; ++i){
            fixedRoiBB[i] = region[i];
            floatRoiBB[i] = region[6 + i];
        }
    }
    progress.SetCurrent(0.1);

    void* resultBuffer = nullptr;
    bool alreadyExisting = true;
    if (!fixedImage->GetDeformableRegistrationResult(floatImageUID, resultBuffer)){
        resultBuffer = fixedImage->GenerateEmptyDeformableRegistrationResultBuffer();
        alreadyExisting = false;
    }

    progress.SetProgressWeight(0.8);
    progress.SetProgressStartValue(0.1);
    if (!FusionAlgorithmProxy::Register_NonRigid(*fixedImage, *floatImage, 
        fixedRoiBB, floatRoiBB, (Mcsf::IProgress*)&progress, resultBuffer)){
        TPS_LOG_DEV_ERROR<<"Failed to invoke FusionAlgorithmProxy::Register_NonRigid.";
        if (fixedRoiBB) delete[] fixedRoiBB; 
        if (floatRoiBB) delete[] floatRoiBB;
        if (alreadyExisting) fixedImage->RemoveDeformableRegistrationResult(floatImageUID);
        progress.SetCancelFlag(true);
        return false;
    }
    progress.Reset();
    progress.SetCurrent(0.9);
    if (!alreadyExisting) fixedImage->SetDeformableRegistrationResult(floatImageUID, resultBuffer);
    if (fixedRoiBB) delete[] fixedRoiBB; 
    if (floatRoiBB) delete[] floatRoiBB;
    mRenderProxy->RefreshRegistrationResult();
    if (fsMng->IsRegionVisible(FUSION_ROLE_FIXED) || 
        fsMng->IsRegionVisible(FUSION_ROLE_FLOAT)){
        mCertFeedback.mFeedbackType = HIDE_REGISTRATION_REGION;

        fsMng->SetRegionVisible(FUSION_ROLE_FIXED, false);
        fsMng->SetRegionVisible(FUSION_ROLE_FLOAT, false);
        mRenderProxy->UpdateFusionRegion();
    }
    progress.SetCurrent(1.0);
    return true;
}

void StartRegistrationCmd::SetupRoiMask_i(const int* fixedDim, const int* floatDim, 
    unsigned char* fixedRoiMask, unsigned char* floatRoiMask)
{
    const int* region;
    mDataRepository->GetFusionSecondaryDataManager()->GetRegion(region);

    for (int z = region[4]; z <= region[5]; ++z)
    {
        for (int y = region[2]; y <= region[3]; ++y)
        {
            memset(fixedRoiMask + z * fixedDim[0] * fixedDim[1] + y * fixedDim[0],
                1, (region[1] - region[0] + 1) * sizeof(unsigned char));
        }
    }

    for (int z = region[10]; z <= region[11]; ++z)
    {
        for (int y = region[8]; y <= region[9]; ++y)
        {
            memset(floatRoiMask + z * floatDim[0] * floatDim[1] + y * floatDim[0],
                1, (region[7] - region[6] + 1) * sizeof(unsigned char));
        }
    }
}
TPS_END_NAMESPACE  // end namespace tps