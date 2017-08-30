////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_normalgroupcollection.cpp
/// 
///  \brief class NormalGroupCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

NormalGroupCollection::NormalGroupCollection() : mActiveNormalGroupUID(""){
    
}

NormalGroupCollection::~NormalGroupCollection(){
    ClearAll();
}

std::map<std::string, NormalGroupPtr> NormalGroupCollection::GetNormalGroupMap(){
    return mNormalGroupMap;
}

NormalGroupPtr NormalGroupCollection::GetNormalGroup(const std::string& uid){
    auto it = mNormalGroupMap.find(uid);
    if (it == mNormalGroupMap.end()){
        TPS_LOG_DEV_ERROR<<"The normal group does not exist which uid: "<<uid;
        return nullptr;
    }

    return it->second;
}

NormalGroupPtr NormalGroupCollection::GetActiveNormalGroup(){
    auto it = mNormalGroupMap.find(mActiveNormalGroupUID);
    if (it == mNormalGroupMap.end()){
        TPS_LOG_DEV_ERROR<<"The normal group does not exist which uid: "<<mActiveNormalGroupUID;
        return nullptr;
    }
    return it->second;
}


bool NormalGroupCollection::AddNormalGroup(const std::string& uid, NormalGroupPtr normalGroup){
    auto it = mNormalGroupMap.find(uid);
    if (it != mNormalGroupMap.end()){
        TPS_LOG_DEV_WARNING<<"The normal group which uid: "<<uid<<" has already exist, Replace it.";
    }

    mNormalGroupMap[uid] = normalGroup;

    //SetActiveNormalGroup(uid);
    return true;
}

bool NormalGroupCollection::RemoveNormalGroup(const std::string& uid){
    auto it = mNormalGroupMap.find(uid);
    if (it == mNormalGroupMap.end()){
        TPS_LOG_DEV_WARNING<<"The normal group does not exist which uid: "<<uid;
        return true;
    }

    mNormalGroupMap.erase(it);
    if (mActiveNormalGroupUID == uid) mActiveNormalGroupUID = "";
    return true;
}

void NormalGroupCollection::ClearAll(){

    mNormalGroupMap.clear();
}

bool NormalGroupCollection::SetActiveNormalGroup(const std::string& uid){
    auto it = mNormalGroupMap.find(uid);
    //允许设置为空的Active Normal Group
    if (!uid.empty() && it == mNormalGroupMap.end()){
        TPS_LOG_DEV_ERROR<<"The normal group does not exist which uid: "<<uid;
        return false;
    }

    mActiveNormalGroupUID = uid;
    return true;
}

std::string NormalGroupCollection::GetActiveNormalGroupUID(){
    return mActiveNormalGroupUID;
}

bool NormalGroupCollection::IsDirty(){
    
    bool dirty = TpsGraphicObjectBase::IsDirty();
    for (auto it = mNormalGroupMap.begin(); it != mNormalGroupMap.end(); ++it){
        dirty |= it->second->IsDirty();
    }
    return dirty;
}

void NormalGroupCollection::SetDirty(bool bFlag){

    TpsGraphicObjectBase::SetDirty(bFlag);
    if (!bFlag){
        for (auto it = mNormalGroupMap.begin(); it != mNormalGroupMap.end(); ++it){
            it->second->SetDirty(bFlag);
        }
    }
}

// bool NormalGroupCollection::IsPlanActive(){
//     return mIsPlanActive;
// }
// 
// void NormalGroupCollection::SetPlanActivity(bool flag){
//     mIsPlanActive = flag;
// }

void NormalGroupCollection::SetSeriesUID(const std::string& seriesUID){
    mSeriesUID = seriesUID;
}

std::string NormalGroupCollection::GetSeriesUID() const{
    return mSeriesUID;
}
TPS_END_NAMESPACE  // end namespace tps