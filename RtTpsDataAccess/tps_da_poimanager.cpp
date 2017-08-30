//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  xiaoqing.shangguan xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_poi_manager.cpp
/// 
///  \brief

///  \version 1.0
///  \date    Jan. 17, 2014
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

TPS_BEGIN_NAMESPACE;
TpsPOIManager::TpsPOIManager(void) 
{

}

TpsPOIManager::~TpsPOIManager(void) {
    Dispose();
}

void TpsPOIManager::Dispose(bool isPatientDataOnly/* = false*/) {

    if(!isPatientDataOnly)
    {
        auto itPoi = mPoiMap.begin();
        for (; itPoi != mPoiMap.end(); ++itPoi) {
            delete itPoi->second;
            itPoi->second = nullptr;
        }

        mPoiMap.clear();
    }
    else
    {
        RtSeries *series = nullptr;
        auto imgMng = mDataRepository->GetImageDataManager();
        for (auto it = mPoiMap.begin(); it != mPoiMap.end(); )
        {
            if (it->second != nullptr && imgMng->GetImageSeries(
                it->second->get_seriesuid(), &series) && 
                series != nullptr && series->get_isphantom())
            {
                ++it;
                continue;
            }
            delete it->second;
            it = mPoiMap.erase(it);
        }

        for (auto it = mMaxPointMap.begin(); it != mMaxPointMap.end(); )
        {
            delete it->second;
            it = mMaxPointMap.erase(it);
        }
    }

    auto itPos = mAllPOIPosInfo.begin();
    while(itPos != mAllPOIPosInfo.end())
    {
        DEL_ARRAY(itPos->second.TxtBuffer);
        itPos++;
    }

    mAllPOIPosInfo.clear();
}

void TpsPOIManager::ClearPatientData() {
    Dispose(true);
}

void TpsPOIManager::ClearPoiPosMap(){
    auto it_pos = mAllPOIPosInfo.begin();
    while(it_pos != mAllPOIPosInfo.end())
    {                      
        DEL_ARRAY(it_pos->second.TxtBuffer);
        mAllPOIPosInfo.erase(it_pos++);
    }
}

bool TpsPOIManager::AddPOI(RtPoi* poi)
{
    if (poi==nullptr) {
        TPS_LOG_DEV_ERROR<<"poi is nullptr.";
        return false;
    }
    const std::string poiUID = poi->get_uid();
    if (poiUID.empty()){
        TPS_LOG_DEV_ERROR<<"poi UID is empty!";
        return false;
    }

    auto it = mPoiMap.find(poiUID);
    if (it != mPoiMap.end()) {
        TPS_LOG_DEV_WARNING<<"The poi with UID: "<<poiUID<<" already exists.";
        //return true; // fix bug of add qa plan poi(added by zuo kai)
    }

    mPoiMap[poiUID] = poi;

    Mcsf::Point3f p3f(poi->get_coordinate3dx(),poi->get_coordinate3dy(),poi->get_coordinate3dz());
    float col[4] ={poi->get_red(),poi->get_green(),poi->get_blue(), poi->get_alpha()};

    AddPoiPos(poiUID, Mcsf::MedViewer3D::Point2D(p3f.GetX(), p3f.GetY()), poi->get_name(), col);

    return true;
}

bool TpsPOIManager::AddPoiPos(const std::string& uid, Mcsf::MedViewer3D::Point2D pos, const std::string& poiName, const float col[4]){
    auto itext = mAllPOIPosInfo.find(uid);
    if (itext != mAllPOIPosInfo.end()) {
        mAllPOIPosInfo.erase(itext);
    }
    POIPosInfo poi_info;
    poi_info.Uid = uid;
    poi_info.Pos.x = pos.x;
    poi_info.Pos.y = pos.y;
    poi_info.Counter = 0;

    for(auto it = mAllPOIPosInfo.begin(); it != mAllPOIPosInfo.end(); ++it){
        if(std::abs(it->second.Pos.x - pos.x) <= 80 && std::abs(it->second.Pos.y - pos.y) <= 3){
            poi_info.Counter = it->second.Counter + 1;
        }
    }

    std::wstring wName(poiName.begin(), poiName.end());

    unsigned char* pStrBuffer = nullptr;
    int strBufHeight = 0;
    int strBufWidth = 0;
    Pixel32 poi_col(255*col[0], 255*col[1], 255*col[2]);
    if(!FTStringBufferGenerator::GetInstance()->GenerateRasterBuffer(wName,
        (tps::Pixel32**)&pStrBuffer, strBufWidth, strBufHeight, 12, poi_col)){
            TPS_LOG_DEV_ERROR<<"Failed to GetImageTextData";
            return false;
    }
    poi_info.TxtBuffer = pStrBuffer;
    poi_info.TxtWidth = strBufWidth;
    poi_info.TxtHeight = strBufHeight;

    mAllPOIPosInfo[uid] = poi_info;

    return true;
}

bool TpsPOIManager::GetPoiPosText(const std::string& poiUid, POIPosInfo& poiPos) const{
    auto it = mAllPOIPosInfo.find(poiUid);
    if(it != mAllPOIPosInfo.end()){
        poiPos.Uid = it->first;
        poiPos.Pos.x = it->second.Pos.x;
        poiPos.Pos.y = it->second.Pos.y;
        poiPos.Counter = it->second.Counter;

        poiPos.TxtBuffer = it->second.TxtBuffer;
        poiPos.TxtWidth = it->second.TxtWidth;
        poiPos.TxtHeight = it->second.TxtHeight;

        return true;
    }
    return false;
}

bool TpsPOIManager::DeletePOI(const std::string &poiUID){
    //delete normal points
    string mPoiUID = poiUID;
    auto managerptr = mPoiMap.find(poiUID);
    if (managerptr != mPoiMap.end()) {
        RtPoi* poi = managerptr->second;
        delete poi;
        poi = nullptr;
        mPoiMap.erase(managerptr);
    }

    auto it_pos = mAllPOIPosInfo.find(mPoiUID);
    if(it_pos == mAllPOIPosInfo.end()){
        return false;
    }

    auto it = mAllPOIPosInfo.begin();
    for(;it != mAllPOIPosInfo.end(); ++it){
        if(std::abs(it->second.Pos.x - it_pos->second.Pos.x) <= 80 && std::abs(it->second.Pos.y - it_pos->second.Pos.y) <= 3){
            if(it->second.Counter > it_pos->second.Counter) it->second.Counter -=1;
        }
    }

    DEL_ARRAY(it_pos->second.TxtBuffer);
    mAllPOIPosInfo.erase(it_pos++);

    TPS_LOG_DEV_WARNING << "poi " << mPoiUID << " is not exist";
    return true;
}

RtPoi* TpsPOIManager::GetPOI(const std::string &poiUID){
    auto managerptr = mPoiMap.find(poiUID);
    if (managerptr == mPoiMap.end()) {
        TPS_LOG_DEV_ERROR<<"The poi with UID: "<<poiUID<<" does not exist.";
        return nullptr;
    }
    return managerptr->second;
}

RtPoi* TpsPOIManager::GetPoiViaName(const std::string &strPoiName, const std::string &seriesUid){
    for (auto iter = mPoiMap.begin(); iter != mPoiMap.end(); ++iter)
    {
        if (iter->second->get_name() == strPoiName && iter->second->get_seriesuid() == seriesUid)
        {
            return iter->second;
        }
    }
    return nullptr;
}

void TpsPOIManager::UpdatePois(std::vector<RtPoi*>& poiList){

    for(auto it = poiList.begin(); it != poiList.end(); ++it){
        RtPoi* newPoi = *it;
        auto managerptr = mPoiMap.find((*it)->get_uid());
        if (managerptr != mPoiMap.end()) {
            RtPoi* oldPoi = managerptr->second;
            //oldPoi->SetDose(newPoi->GetDose());
            //Deepcopy here!
            *oldPoi = *newPoi;
        }
    }
}

std::vector<RtPoi*> TpsPOIManager::GetAllPoiInSeries(const std::string& sSeriesUid) const
{
    std::vector<RtPoi*> vecPoi;
    for (auto iter=mPoiMap.cbegin(); iter != mPoiMap.cend(); ++iter) {
        if (iter->second->get_seriesuid() == sSeriesUid) {
            vecPoi.push_back(iter->second);
        }
    }
    return vecPoi;
}

std::vector<std::string> TpsPOIManager::GetPoiUidListBySeriesUid(const std::string &sSeriesUID) const{

    std::vector<std::string> PoiUIDList;
    for (auto iter = mPoiMap.begin(); iter != mPoiMap.end(); ++iter) {
        if (nullptr == iter->second){
            TPS_LOG_DEV_ERROR<<"Poi is null:"<<iter->first;
            return PoiUIDList;
        }
        if (iter->second->get_seriesuid() == sSeriesUID) {
            PoiUIDList.push_back(iter->first);
        }
    }

    return PoiUIDList;
}

bool TpsPOIManager::AddMaxPoint(RtPoi* poi)
{
    if (poi==nullptr) {
        TPS_LOG_DEV_ERROR<<"poi is nullptr.";
        return false;
    }
    const std::string poiUID = poi->get_uid();
    if (poiUID.empty()){
        TPS_LOG_DEV_ERROR<<"poi UID is empty!";
        return false;
    }

    auto it = mMaxPointMap.find(poiUID);
    if (it != mMaxPointMap.end()) {
        TPS_LOG_DEV_WARNING<<"TpsPOIManager::AddMaxPoint The poi with UID: "<<poiUID<<" already exists.";
        if (it->second != poi) delete it->second;
    }

    mMaxPointMap[poiUID] = poi;
    return true;
}

bool TpsPOIManager::DeleteMaxPoint(const std::string &poiUID){
    auto managerptr = mMaxPointMap.find(poiUID);
    if (managerptr != mMaxPointMap.end()) {
        RtPoi* poi = managerptr->second;
        delete poi;
        poi = nullptr;
        mMaxPointMap.erase(managerptr);
    }
    return true;
}

RtPoi* TpsPOIManager::GetMaxPoint(const std::string &poiUID){
    auto managerptr = mMaxPointMap.find(poiUID);
    if (managerptr == mMaxPointMap.end()) {
        TPS_LOG_DEV_WARNING<<"TpsPOIManager::GetMaxPoint The poi with UID: "<<poiUID<<" does not exist.";
        return nullptr;
    }
    return managerptr->second;
}
TPS_END_NAMESPACE
