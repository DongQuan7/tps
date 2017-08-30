////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobjectcollection.cpp
/// 
///  \brief class TpsVOIGOCollection implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"

#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsFramework/tps_fw_block_buffer.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIGOCollection::TpsVOIGOCollection()
{
    
}

TpsVOIGOCollection::~TpsVOIGOCollection()
{
    mVOIGOMap.clear();
}

TpsVOIGOMap TpsVOIGOCollection::GetVOIList() const
{
    return mVOIGOMap;
}

TpsVOIGraphicObjectPtr TpsVOIGOCollection::GetVOI(const std::string& uid) const
{
    auto it = mVOIGOMap.find(uid);
    if (it == mVOIGOMap.end()){
        TPS_LOG_DEV_ERROR<<"The VOI graphic object with UID: "<<uid<<" does not exist";
        return nullptr;
    }
    return it->second;
}

void TpsVOIGOCollection::AddVOI(
    const std::string& uid, TpsVOIGraphicObjectPtr voi)
{
    auto it = mVOIGOMap.find(uid);
    if (it != mVOIGOMap.end()){
        TPS_LOG_DEV_WARNING<<"The VOI graphic object with UID: "<<uid <<" already exists.";
        it->second = voi;
    }
    else{
        mVOIGOMap[uid] = voi;
    }
}

void TpsVOIGOCollection::RemoveVOI(const std::string& uid)
{
    auto it = mVOIGOMap.find(uid);
    if (it == mVOIGOMap.end()){
        TPS_LOG_DEV_WARNING<<"The VOI graphic object to be removed with UID: "
            <<uid <<" does not exist.";
        return;
    }
    mVOIGOMap.erase(it);
}

void TpsVOIGOCollection::UpdateVOI(const std::string& uid)
{
    auto it = mVOIGOMap.find(uid);
    if (it == mVOIGOMap.end())
    {
        TPS_LOG_DEV_ERROR<<"The VOI to be updated does not exist.";
        return;
    }
}

void TpsVOIGOCollection::RemoveAllVOI()
{
    mVOIGOMap.clear();
}

void TpsVOIGOCollection::SetDirty(bool bFlag)
{
    TpsGraphicObjectBase::SetDirty(bFlag);
    if (!bFlag){
        for (auto it = mVOIGOMap.begin(); it != mVOIGOMap.end(); ++it){
            it->second->SetDirty(bFlag);
        }
    }
}

TpsVOIGOCollection& TpsVOIGOCollection::operator=(const TpsVOIGOCollection& src)
{
    this->mVOIGOMap = src.mVOIGOMap;
    return *this;
}
TPS_END_NAMESPACE  // end namespace tps