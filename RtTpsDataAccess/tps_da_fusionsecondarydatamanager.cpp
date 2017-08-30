////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_da_fusionsecondarydatamanager.cpp
/// 
///  \brief class FusionSecondaryDataManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/24
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps
TpsFusionSecondaryDataManager::TpsFusionSecondaryDataManager()
: mFixedImageUID(""), mFloatImageUID("")
, mIsRegionReady(false), mCurrentRegistrationAlgorithm(LANDMARK)
{
    memset(mLimitedRegion, 0, 12 * sizeof (int));
    memset(mIsRegionVisible, false, 2 * sizeof(bool));
    memset(mEditingPlaneFlag, 0, 2 * sizeof(char));
    mRigidResult.SetIdentity();
}

TpsFusionSecondaryDataManager::~TpsFusionSecondaryDataManager(){

}

void TpsFusionSecondaryDataManager::SetFixedImageUID(const std::string& imageUID){
    if(mFixedImageUID != imageUID){
        mFixedImageUID = imageUID;
        mRigidResult.SetIdentity();
        mIsRegionReady = false;
    }
}

void TpsFusionSecondaryDataManager::SetFloatImageUID(const std::string& imageUID){
    if (mFloatImageUID != imageUID){
        mFloatImageUID = imageUID;
        mRigidResult.SetIdentity();
        mIsRegionReady = false;
    }
}

std::string TpsFusionSecondaryDataManager::GetFixedImageUID() const{
    return mFixedImageUID;
}

std::string TpsFusionSecondaryDataManager::GetFloatImageUID() const{
    return mFloatImageUID;
}

int TpsFusionSecondaryDataManager::ImagesStatus() const{
    int status = 0;
    if (!mFixedImageUID.empty()) status |= FIXED_IMAGE_READY;
    if (!mFloatImageUID.empty()) status |= FLOAT_IMAGE_READY;
    return status;
}

void TpsFusionSecondaryDataManager::SetRigidResult(const Mcsf::Matrix4f& matrix)
{
    mRigidResult = matrix;
}

Mcsf::Matrix4f TpsFusionSecondaryDataManager::GetRigidResult() const
{
    return mRigidResult;
}


void TpsFusionSecondaryDataManager::SetRegion(const int* region){
    memcpy(mLimitedRegion, region, 12 * sizeof(int));
}

void TpsFusionSecondaryDataManager::ClearRegion(){
    mIsRegionReady = false;
    memset(mEditingPlaneFlag, 0, 2 * sizeof(char));
    memset(mLimitedRegion, 0, 12 * sizeof(int));
    memset(mIsRegionVisible, 0, 2 * sizeof(bool));
}
TPS_END_NAMESPACE  // end namespace tps