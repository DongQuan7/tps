////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_voiedtingcache.h
/// 
///  \brief class TpsVOIEdtingCache declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/22
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_VOIEDTINGCACHE_H_
#define TPS_FW_VOIEDTINGCACHE_H_

#include "tps_fw_defs.h"
#include <vector>

#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct PlaneCache{
    unsigned char* buffer;
    int width;
    int height;
    //patient to normal plane c.s. when redo|undo,
    //world to normal plane c.s. when copy|paste
    Mcsf::Matrix4f transformMatrix;
};

/// \class TpsVOIEdtingCache
/// 
/// \brief *****
class TPS_FW_DECLSPEC TpsVOIEditingCache {
public:
    TpsVOIEditingCache();
    ~TpsVOIEditingCache();

    void AddCache(unsigned char* buffer, 
        int width, int height, const Mcsf::Matrix4f& transformMatrix);
    void GetCacheList(std::vector<PlaneCache*>& cacheList) const;

    void SetVoiUid(const std::string& uid) { mVoiUid = uid; }
    std::string GetVoiUid() const { return mVoiUid; }

    void SetSliceCount(size_t sliceCount);
    size_t GetSliceCount();

    void SetDirtyFlags(char* dirtyFlags);
    char* GetDirtyFlags();

    void SetInterpolationFlags(bool* flagArray);
    bool* GetInterpolationFlags();

    void Clear();
private:
    std::vector<PlaneCache*> mCacheList;
    
    char*           mDirtyFlags;
    bool*           mInterpolationFlags;
    size_t          mSliceCount;
    std::string     mVoiUid;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIEditingCache);
};

TPS_END_NAMESPACE  // end namespace tps

#endif