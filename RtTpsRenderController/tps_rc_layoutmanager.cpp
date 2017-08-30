////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_layoutmanager.cpp
/// 
///  \brief class TpsLayoutManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include"StdAfx.h"

#include "RtTpsRenderController/tps_rc_layoutmanager.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsLayoutManager::TpsLayoutManager(){

}

TpsLayoutManager::~TpsLayoutManager(){

}

void TpsLayoutManager::RegisterLayoutUnit(
    WINDOW_TYPE type, LAYOUT_UNIT unit){

    TPS_LOG_TRACE_INFO<<"[INFO][TpsLayoutManager][RegisterLayoutUnit]: "
        <<"Window: "<<GET_ENUM_STRING(type)<<" registered on ViewControl: "
        <<unit.viewerControlID<<" Cell: "<<unit.cellID;
    mLayoutUnitMap[type] = unit;
}

void TpsLayoutManager::RemoveLayoutUnit(WINDOW_TYPE type){

    TPS_LOG_TRACE_INFO<<"[INFO][TpsLayoutManager][RemoveLayoutUnit]: "
        <<"Window: "<<GET_ENUM_STRING(type)<<"'s layout information has been removed.";
    mLayoutUnitMap.erase(type);
}

bool TpsLayoutManager::GetWindowtype(
    LAYOUT_UNIT unit, WINDOW_TYPE& type) const{

    for (auto it = mLayoutUnitMap.begin(); it != mLayoutUnitMap.end(); ++it)
    {
        if (it->second.viewerControlID == unit.viewerControlID && 
            it->second.cellID == unit.cellID)
        {
            type = it->first;
            return true;
        }
    }
    return false;
}

bool TpsLayoutManager::GetLayoutUnit(
    WINDOW_TYPE type, LAYOUT_UNIT& unit) const{

    auto it = mLayoutUnitMap.find(type);
    if (it == mLayoutUnitMap.end()){
        TPS_LOG_DEV_WARNING<<"[WARNING][TpsLayoutManager][GetLayoutUnit]: "
            <<"Failed to find Window: "<<GET_ENUM_STRING(type)<<"'s layout information";
        return false;
    }

    unit = it->second;
    return true;
}

bool TpsLayoutManager::GetLayoutUnit(
    DISPLAYCACHE_UID uid, LAYOUT_UNIT& unit) const{

    auto it = mRenderContentMap.find(uid);
    if (it == mRenderContentMap.end()){
        TPS_LOG_DEV_WARNING<<"[WARNING][TpsLayoutManager][GetLayoutUnit]: "
            <<"Failed to find display cache: "<<uid<<"'s rendering window type.";
        return false;
    }

    return GetLayoutUnit(it->second, unit);
}

void TpsLayoutManager::RegisterView(
    DISPLAYCACHE_UID uid, WINDOW_TYPE type){

    TPS_LOG_TRACE_INFO<<"[INFO][TpsLayoutManager][RegisterView]: "
        <<"Display cache: "<<uid<<" registered on Window: "<<GET_ENUM_STRING(type);
    mRenderContentMap[uid] = type;
}

void TpsLayoutManager::Lock(){
    mMutex.lock();
}

void TpsLayoutManager::Unlock(){
    mMutex.unlock();
}

TPS_END_NAMESPACE  // end namespace tps