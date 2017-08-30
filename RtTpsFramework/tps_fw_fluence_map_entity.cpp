////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_da_fluence_map_entity.cpp
/// 
///  \brief class FluenceMapEntity implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/05
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_fluence_map_entity.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

FluenceMapEntity::FluenceMapEntity() : mIsValid(false), mIntensityBuffer(nullptr){

}

FluenceMapEntity::~FluenceMapEntity()
{
    Clear();
}

FluenceMapEntity::FluenceMapEntity(const FluenceMapEntity& fluenceMap)
{
    mIntensityBuffer = nullptr;
    mIsValid = fluenceMap.mIsValid;
    if (mIsValid)
    {
        SetIntensityMap(fluenceMap.mStartPoint[0], fluenceMap.mStartPoint[1], 
            fluenceMap.mSpacing[0], fluenceMap.mSpacing[1], fluenceMap.mDim[0], 
            fluenceMap.mDim[1], fluenceMap.mIntensityBuffer);
    }
}

FluenceMapEntity& FluenceMapEntity::operator=(const FluenceMapEntity& fluenceMap)
{
    Clear();
    mIsValid = fluenceMap.mIsValid;
    if (mIsValid)
    {
        SetIntensityMap(fluenceMap.mStartPoint[0], fluenceMap.mStartPoint[1], 
            fluenceMap.mSpacing[0], fluenceMap.mSpacing[1], fluenceMap.mDim[0], 
            fluenceMap.mDim[1], fluenceMap.mIntensityBuffer);
    }
    return *this;
}

bool FluenceMapEntity::IsValid() const
{
    return mIsValid;
}

void FluenceMapEntity::SetIntensityMap(float startX, float startY, float xSpacing, 
    float ySpacing, int width, int height, float* buffer)
{
    if (width <= 0 || height <= 0 || buffer == nullptr){
        TPS_LOG_DEV_ERROR<<"The parameters are not valid.";
        mIsValid = false;
        return;
    }

    if (mIntensityBuffer == nullptr || width * height != mDim[0] * mDim[1]){
        if (mIntensityBuffer) delete[] mIntensityBuffer;
        mIntensityBuffer = new float[width * height];
    }
    memcpy(mIntensityBuffer, buffer, sizeof(float) * width * height);
    mDim[0] = width; mDim[1] = height;
    mSpacing[0] = xSpacing; mSpacing[1] = ySpacing;
    mStartPoint[0] = startX; mStartPoint[1] = startY;

    mMax = mMin = mIntensityBuffer[0];
    for (int i = 1; i < mDim[0] * mDim[1]; ++i){
        mMin = TPS_MIN(mIntensityBuffer[i], mMin);
        mMax = TPS_MAX(mIntensityBuffer[i], mMax);
    }
    mIsValid = true;
}

void FluenceMapEntity::Clear()
{
    if (mIntensityBuffer){
        delete[] mIntensityBuffer;
        mIntensityBuffer = nullptr;
    }
    mIsValid = false;
}

//notice that this method will malloc memory & copy the map out
//you need release the memory by yourself
bool FluenceMapEntity::GetIntensifyBuffer(int& width, int& height, float*& buffer) const
{
    if (!mIsValid){
        TPS_LOG_DEV_ERROR<<"The intensity map is invalid.";
        return false;
    }
    width = mDim[0];
    height = mDim[1];
    buffer = new float[width * height];
    memcpy(buffer, mIntensityBuffer, sizeof(float) * width * height);
    return true;
}

bool FluenceMapEntity::GetSpacing(float& spacingX, float& spacingY) const
{
    if (!mIsValid){
        TPS_LOG_DEV_ERROR<<"The intensity map is invalid.";
        return false;
    }

    spacingX = mSpacing[0];
    spacingY = mSpacing[1];
    return true;
}

bool FluenceMapEntity::GetStart(float& startX, float& startY) const
{
    if (!mIsValid){
        TPS_LOG_DEV_ERROR<<"The intensity map is invalid.";
        return false;
    }
    
    startX = mStartPoint[0];
    startY = mStartPoint[1];
    return true;
}

bool FluenceMapEntity::GetMaxValue(float& max)
{
    if (!mIsValid){
        TPS_LOG_DEV_ERROR<<"The intensity map is invalid.";
        return false;
    }
    max = mMax;
    return true;
}

bool FluenceMapEntity::GetMinValue(float& min)
{
    if (!mIsValid){
        TPS_LOG_DEV_ERROR<<"The intensity map is invalid.";
        return false;
    }
    min = mMin;
    return true;
}
TPS_END_NAMESPACE  // end namespace tps