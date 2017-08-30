////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_voiedtingcache.cpp
/// 
///  \brief class TpsVOIEditingCache implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/22
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_voiedtingcache.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIEditingCache::TpsVOIEditingCache()
: mDirtyFlags(nullptr), mInterpolationFlags(nullptr), mSliceCount(0){

}

TpsVOIEditingCache::~TpsVOIEditingCache(){
    Clear();
}

void TpsVOIEditingCache::AddCache(unsigned char* buffer, 
    int width, int height, const Mcsf::Matrix4f& transformMatrix){
    if (width <= 0 || height <= 0 || buffer == nullptr) return;

    PlaneCache* cache = new PlaneCache();
    cache->width = width;
    cache->height = height;
    cache->transformMatrix = transformMatrix;
    cache->buffer = new unsigned char[width * height];
    memcpy(cache->buffer, buffer, width * height * sizeof(unsigned char));
    mCacheList.push_back(cache);
}

void TpsVOIEditingCache::GetCacheList(std::vector<PlaneCache*>& cacheList) const{
    cacheList = mCacheList;
}

void TpsVOIEditingCache::Clear(){
    for (int i = 0; i < mCacheList.size(); ++i){
        if (mCacheList[i] == nullptr) continue;
        delete[] mCacheList[i]->buffer;
        delete mCacheList[i];
    }
    mCacheList.clear();

    if (mInterpolationFlags) {
        delete[] mInterpolationFlags;
        mInterpolationFlags = nullptr;
    }

    if (mDirtyFlags) {
        delete[] mDirtyFlags;
        mDirtyFlags = nullptr;
    }
    mSliceCount = 0;
}

void TpsVOIEditingCache::SetSliceCount(size_t sliceCount){
    if (sliceCount == 0) return;

    if (mSliceCount != sliceCount){
        mSliceCount = sliceCount;
        if (mDirtyFlags) delete[] mDirtyFlags;
        mDirtyFlags = new char[mSliceCount];
        memset(mDirtyFlags, 0, sizeof(char) * mSliceCount);

        if (mInterpolationFlags) delete[] mInterpolationFlags;
        mInterpolationFlags = new bool[mSliceCount];
        memset(mInterpolationFlags, 1, sizeof(bool) * mSliceCount);
    }
}

size_t TpsVOIEditingCache::GetSliceCount(){
    return mSliceCount;
}

void TpsVOIEditingCache::SetDirtyFlags(char* dirtyFlags){
    if (dirtyFlags == nullptr || 
        dirtyFlags == mDirtyFlags) return;
    memcpy(mDirtyFlags, dirtyFlags, sizeof(char) * mSliceCount);
}

char* TpsVOIEditingCache::GetDirtyFlags(){
    return mDirtyFlags;
}

void TpsVOIEditingCache::SetInterpolationFlags(bool* flagArray){
    if (flagArray == nullptr || 
        mInterpolationFlags == flagArray) return;
    memcpy(mInterpolationFlags, flagArray, sizeof(bool) * mSliceCount);
}

bool* TpsVOIEditingCache::GetInterpolationFlags(){
    return mInterpolationFlags;
}

TPS_END_NAMESPACE  // end namespace tps