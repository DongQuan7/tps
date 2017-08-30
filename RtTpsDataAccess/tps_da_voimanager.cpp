//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  jiandong.zhou     jiandong.zhou@united-imaging.com
/// 
///  \file tps_voi_manager.cpp
/// 
///  \brief  entry of VOI operate

///  \version 1.0
///  \date    Aug. 6, 2013
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//TPS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voisetting.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_voiedtingcache.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
TpsVOIManager::TpsVOIManager(void) :  mVOIPlaneCacheInClipboard(nullptr)
{

}

TpsVOIManager::~TpsVOIManager(void) 
{
    Dispose();
}

void TpsVOIManager::Dispose(bool isPatientDataOnly/* = false*/) {
    if(!isPatientDataOnly)
    {
        auto it = mRoiMap.begin();
        while(it != mRoiMap.end()) {
            if(nullptr != it->second) {
                delete it->second;
                it->second = nullptr;
            }
            it++;
        }

        mRoiMap.clear();
    }
    else
    {
        RtSeries *series = nullptr;
        auto imgMng = mDataRepository->GetImageDataManager();
        for (auto it = mRoiMap.begin(); it != mRoiMap.end(); )
        {
            if (it->second != nullptr && imgMng->GetImageSeries(
                it->second->GetInnerRoi()->get_seriesuid(), &series) && 
                series != nullptr && series->get_isphantom())
            {
                ++it;
                continue;
            }
            delete it->second;
            it = mRoiMap.erase(it);
        }
    }

    if (mVOIPlaneCacheInClipboard != nullptr)
    {
        delete[] mVOIPlaneCacheInClipboard->buffer;
        delete mVOIPlaneCacheInClipboard;
        mVOIPlaneCacheInClipboard = nullptr;
    }

    if (!isPatientDataOnly)
    {
        for (int i = 0; i < mVoiSettings.size(); ++i){
            delete mVoiSettings[i];
            mVoiSettings[i] = nullptr;
        }
        mVoiSettings.clear();
    }
}

void TpsVOIManager::ClearPatientData() {
    Dispose(true);
}

std::vector<ROIEntity*> TpsVOIManager::GetROIListViaStructureUids(
    const std::vector<std::string>& structureSetUidList) const
{
    std::string structureSetUid;
    std::vector<ROIEntity*> voiList;
    for (int i = 0; i < structureSetUidList.size(); ++i)
    {
        structureSetUid = structureSetUidList[i];
        for(auto it = mRoiMap.begin(); it != mRoiMap.end(); ++it)
        {
            if (it->second != nullptr && 
                it->second->GetInnerRoi()->get_structuresetuid() == structureSetUid)
            {
                voiList.push_back(it->second);
            }
        }
    }
    return voiList;
}

std::vector<ROIEntity*> TpsVOIManager::GetROIBySeries(const std::string seriesUid) const
{
    std::vector<ROIEntity*> roiList;
    for (auto it = mRoiMap.begin(); it != mRoiMap.end(); it++)
    {
        if(it->second->GetInnerRoi()->get_seriesuid() == seriesUid)
        {
            roiList.push_back(it->second);
        }
    }
    return roiList;
}

std::vector<std::string> TpsVOIManager::GetROIUidListBySeries(const std::string seriesUid) const
{
    std::vector<std::string> roiUidList;
    for (auto it = mRoiMap.begin(); it != mRoiMap.end(); it++)
    {
        if(it->second->GetInnerRoi()->get_seriesuid() == seriesUid)
        {
            roiUidList.push_back(it->first);
        }
    }
    return roiUidList;
}

void TpsVOIManager::SetVOIDoseInfo( const std::string &voiUID, float minDose, float maxDose, float meanDose ) {
    ROIEntity *voi = GetROI(voiUID);
    if (voi == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi!";
        return;
    }

    voi->SetMinDose(minDose);
    voi->SetMaxDose(maxDose);
    voi->SetMeanDose(meanDose);
}

bool TpsVOIManager::SaveContourMaskToClipboard(const PlaneCache& contourMask)
{
    if (mVOIPlaneCacheInClipboard != nullptr)
    {
        delete[] mVOIPlaneCacheInClipboard->buffer;
        delete mVOIPlaneCacheInClipboard;
        mVOIPlaneCacheInClipboard = nullptr;
    }
    mVOIPlaneCacheInClipboard = new PlaneCache();
    mVOIPlaneCacheInClipboard->width = contourMask.width;
    mVOIPlaneCacheInClipboard->height = contourMask.height;
    int count = contourMask.width * contourMask.height * sizeof(unsigned char);
    mVOIPlaneCacheInClipboard->buffer = new unsigned char[count];
    memcpy(mVOIPlaneCacheInClipboard->buffer, contourMask.buffer, count);
    mVOIPlaneCacheInClipboard->transformMatrix = contourMask.transformMatrix;
    return true;
}

bool TpsVOIManager::GetContourMaskFromClipboard(PlaneCache** contourMask)
{
    if (mVOIPlaneCacheInClipboard == nullptr)
    {
        return false;
    }

    *contourMask = mVOIPlaneCacheInClipboard;
    return true;
}

bool TpsVOIManager::Contains(const std::string& voiUid)
{
    return mRoiMap.find(voiUid) != mRoiMap.end();
}

void TpsVOIManager::SetVoiSettings(const std::vector<RtVoisetting*>& voisettings)
{
    mVoiSettings = voisettings;
}

std::vector<RtVoisetting*> TpsVOIManager::GetVoiSettings() const
{
    return mVoiSettings;
}

bool TpsVOIManager::GetVoiSettingFromVoiName(const std::string& voiname, RtVoisetting* voisetting)
{
    for (int i = 0; i < mVoiSettings.size(); ++i)
    {
        if (mVoiSettings[i]->get_voiname() == voiname)
        {
            *voisetting = *mVoiSettings[i];
            return true;
        }
    }
    return false;
}

ROIEntity* TpsVOIManager::CreateROIEntity(RtVoi* voi)
{
    if (voi == nullptr) return nullptr;
    return new ROIEntity(voi);
}

bool TpsVOIManager::AddROI(ROIEntity* pRoiEntity)
{
    if (nullptr == pRoiEntity){
        TPS_LOG_DEV_ERROR<<"pRoiEntity is null";
        return false;
    }
    const std::string sVOIUID = pRoiEntity->GetInnerRoi()->get_uid();
    mRoiMap[sVOIUID] = pRoiEntity;
    return true;
}

ROIEntity* TpsVOIManager::GetROI(const std::string& uid)
{
    auto it = mRoiMap.find(uid);
    return it == mRoiMap.end() ? nullptr : it->second;
}

void TpsVOIManager::DeleteROI(const std::string& sRoiUID)
{
    auto it = mRoiMap.find(sRoiUID);
    if (it != mRoiMap.end())
    {
        delete it->second;
    }
    mRoiMap.erase(it);
}

ROIEntity* TpsVOIManager::GetExternal(const std::string seriesUID) const
{
    for (auto it = mRoiMap.cbegin(); it != mRoiMap.cend(); ++it)
    {
        RtVoi* innerRoi = it->second->GetInnerRoi();
        if (innerRoi->get_planningrole() == RtDbDef::PLANNING_ROLE_EXTERNAL && 
            innerRoi->get_seriesuid() == seriesUID)
        {
            return it->second;
        }
    }
    return nullptr;
}

TPS_END_NAMESPACE
