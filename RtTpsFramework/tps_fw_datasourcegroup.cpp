////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_datasourcegroup.cpp
/// 
///  \brief class TpsDataSourceGroup implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/17
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"

#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsDataSourceGroup::TpsDataSourceGroup(){

}

TpsDataSourceGroup::~TpsDataSourceGroup()
{
    if(!mRenderControllerMap.empty()) {
        for_each(mRenderControllerMap.begin(), mRenderControllerMap.end(),
            [&](std::pair<std::string, TpsRenderControllerBase*> pr){
                if (pr.second){
                    delete pr.second;
                    pr.second = nullptr;
                }
        });
        mRenderControllerMap.clear();
    }
}

void TpsDataSourceGroup::AddRenderController(const std::string& wndUid,
    TpsRenderControllerBase* renderController)
{
    if (mRenderControllerMap.find(wndUid) != mRenderControllerMap.end()){
        TPS_LOG_DEV_WARNING << "The render controller of window: " << wndUid
            <<" already exists, replace it.";
    }

    // when new render controller added into the data source group, 
    // the existing data source uid will be set
    mRenderControllerMap[wndUid] = renderController;
    for (auto it = mDataSourceMap.begin(); it != mDataSourceMap.end(); ++it){
        renderController->SetDataSource(it->first, it->second);
    }
}

TpsRenderControllerBase* TpsDataSourceGroup::GetRenderController(const std::string& wndUid)
{
    auto it = mRenderControllerMap.find(wndUid);
    if (it != mRenderControllerMap.end()){
        return it->second;
    }
    return nullptr;
}

void TpsDataSourceGroup::RemoveRenderController(const std::string& wndUid)
{
    auto it = mRenderControllerMap.find(wndUid);
    if (it == mRenderControllerMap.end()){
        TPS_LOG_DEV_INFO<<"[INFO][TpsDataSourceGroup][RemoveRenderController]: "
            << "Render controller to be removed of window: " << wndUid << "does not exists.";
        return;
    }
    delete it->second;
    it->second = nullptr;
    mRenderControllerMap.erase(it);
}

bool TpsDataSourceGroup::HasWindow(const std::string& windowUid)
{
    return mRenderControllerMap.find(windowUid) != mRenderControllerMap.end();
}

void TpsDataSourceGroup::SetDataSource(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    auto it = mDataSourceMap.find(id);
    bool isFound = (it != mDataSourceMap.end());
    if(isFound && it->second == uid){
        TPS_LOG_TRACE_INFO<<"The data source is the same, return.";
        return;
    }
    mDataSourceMap[id] = uid;

    TpsRenderControllerBase* validRC = nullptr;
    for (auto it = mRenderControllerMap.begin(); it != mRenderControllerMap.end(); ++it){
        if (it->second != nullptr) { 
            validRC = it->second;
            break;
        }
    }
    if(validRC == nullptr){
        TPS_LOG_TRACE_INFO<<"Render Controller does not exist, return.";
        return;
    }
    if(isFound){
        //TEST_PERFORMANCE_INIT
        //TEST_PERFORMANCE_BEGIN
        validRC->SynchronizeRenderThread();
        //TEST_PERFORMANCE_END("SynchronizeRenderThread")
    }
    for_each(mRenderControllerMap.begin(), mRenderControllerMap.end(),
        [&](std::pair<std::string, TpsRenderControllerBase*> pr){
        if(pr.second){
            pr.second->SetDataSource(id, uid);
        }
    });
}

void TpsDataSourceGroup::RemoveDataSource(DATA_SOURCE_ID id)
{
    auto it = mDataSourceMap.find(id);
    if (it == mDataSourceMap.end()){
        TPS_LOG_TRACE_INFO<<"The data source to be removed does not exist.";
        return;
    }
    mDataSourceMap.erase(it);

    TpsRenderControllerBase* validRC = nullptr;
    for (auto it = mRenderControllerMap.begin(); it != mRenderControllerMap.end(); ++it){
        if (it->second != nullptr) { 
            validRC = it->second;
            break;
        }
    }
    if (validRC == nullptr) return;
    validRC->SynchronizeRenderThread();

    for_each(mRenderControllerMap.begin(), mRenderControllerMap.end(),
        [&](std::pair<std::string, TpsRenderControllerBase*> pr){
        if (pr.second){
            pr.second->RemoveDataSource(id);
        }
    });
}

bool TpsDataSourceGroup::GetDataSource(DATA_SOURCE_ID id, DATA_SOURCE_UID& uid)
{
    auto it = mDataSourceMap.find(id);
    if(it == mDataSourceMap.end()){
        TPS_LOG_DEV_INFO<<"The data source with id: "<<id<<" does not exist.";
        return false;
    }
    
    uid = it->second;
    return true;
}

void TpsDataSourceGroup::RemoveAllDataSources()
{
    if (mRenderControllerMap.empty())
    {
        mDataSourceMap.clear();
        return;
    }

    if (mDataSourceMap.empty()) return;

    TpsRenderControllerBase* validRC = nullptr;
    for (auto it = mRenderControllerMap.begin(); it != mRenderControllerMap.end(); ++it){
        if (it->second != nullptr) { 
            validRC = it->second;
            break;
        }
    }
    if (validRC == nullptr) return;
    validRC->SynchronizeRenderThread();

    for_each(mRenderControllerMap.begin(), mRenderControllerMap.end(),
        [&](std::pair<std::string, TpsRenderControllerBase*> pr){
        if (pr.second){
            for (auto it = mDataSourceMap.begin(); it != mDataSourceMap.end(); ++it){
                pr.second->RemoveDataSource(it->first);
            }
        }
    });
    mDataSourceMap.clear();
}

bool TpsDataSourceGroup::IsDataSourceEmpty() const
{
    return mDataSourceMap.empty();
}
TPS_END_NAMESPACE  // end namespace tps