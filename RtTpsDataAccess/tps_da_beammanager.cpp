//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  jiandong.zhou     jiandong.zhou@united-imaging.com
/// 
///  \file tps_beam_manager.cpp
/// 
///  \brief  entry of beam operate

///  \version 1.0
///  \date    Aug. 6, 2013
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsBeamManager::TpsBeamManager(void)
{
    mMutex = new boost::mutex();
}


TpsBeamManager::~TpsBeamManager(void) 
{
    delete mMutex; mMutex = nullptr;
    Dispose();
}

void TpsBeamManager::Dispose()
{
    if (!mBeams.empty()){
        for (auto itr = mBeams.begin(); itr != mBeams.end(); ++itr){
            if (itr->second != nullptr){
                delete itr->second;
                itr->second = nullptr;
            }
        }
        mBeams.clear();
    }

    if (!mBeamCS.empty()){
        for (auto itrCS = mBeamCS.begin(); itrCS != mBeamCS.end(); ++itrCS){
            if (itrCS->second != nullptr){
                delete itrCS->second;
                itrCS->second = nullptr;
            }
        }
        mBeamCS.clear();
    }

    if (!mBeamFluenceMapCollection.empty()) {
        for (auto itr = mBeamFluenceMapCollection.begin(); 
            itr != mBeamFluenceMapCollection.end(); ++itr){
            if (itr->second != nullptr){
                delete itr->second;
                itr->second = nullptr;
            }
        }
        mBeamFluenceMapCollection.clear();
    }

    //mBeamAccossory.clear();
}

void TpsBeamManager::ClearPatientData() {
    Dispose();
}

bool TpsBeamManager::AddBeam2Map(RtBeam* pbeam)
{
    if (pbeam==nullptr) {
        TPS_LOG_DEV_ERROR<<"pbeam is nullptr";
        return false;
    }

    boost::lock_guard<boost::mutex> lg(*mMutex);
    std::string sBeamUID = pbeam->get_uid();
    mTpsBeamMapIt it = mBeams.find(sBeamUID);
    if (it != mBeams.end()) {
        if (it->second != pbeam)
        {
            delete it->second;
            it->second = nullptr;
        }
    }
    mBeams[sBeamUID] = pbeam;
    return true;
}

int TpsBeamManager::AddBeamCS(const std::string& beamUID, 
    TpsIECCoordinateSystem* const beamCS)
{
    if (beamCS==nullptr) {
        TPS_LOG_DEV_ERROR<<"beamCS is nullptr";
        return TPS_ER_FAILURE;
    }

    boost::lock_guard<boost::mutex> lg(*mMutex);
    auto iter = mBeamCS.find(beamUID);
    if (iter != mBeamCS.end()) {
        if (iter->second != beamCS)
        {
            delete iter->second;
            iter->second = nullptr;
        }
    }
    mBeamCS[beamUID] = beamCS;
    return TPS_ER_SUCCESS;
}

int TpsBeamManager::DeleteBeam(const std::string& beamUID)
{
     std::map<std::string, RtBeam*>::iterator it = mBeams.find(beamUID);
     if (it != mBeams.end())
     {
         DEL_PTR(it->second);
         mBeams.erase(it);
     }
     std::map<std::string, TpsIECCoordinateSystem*>::iterator itCS = mBeamCS.find(beamUID);
     if (itCS != mBeamCS.end())
     {
         DEL_PTR(itCS->second);
         mBeamCS.erase(itCS);
     }
     std::map<std::string, FluenceMapEntity*>::iterator itFluenceMap = mBeamFluenceMapCollection.find(beamUID);
     if (itFluenceMap != mBeamFluenceMapCollection.end())
     {
         DEL_PTR(itFluenceMap->second);
         mBeamFluenceMapCollection.erase(itFluenceMap);
     }
     return TPS_ER_SUCCESS;
}

int TpsBeamManager::UpdateBeam(RtBeam* const newBeam)
{
    if (newBeam==nullptr) {
        TPS_LOG_DEV_ERROR<<"newBeam is nullptr";
        return TPS_ER_FAILURE;
    }
    boost::lock_guard<boost::mutex> lg(*mMutex);
    std::string beamUid = newBeam->get_uid();
    std::map<std::string, RtBeam*>::iterator it = mBeams.find(beamUid);
    if (it != mBeams.end()){
        //it->second = newBeam;WRONG!
        *(it->second) = *newBeam;//deep copy
        //to be removed,IsJoined flag will miss after deep copy, this is a work around
        //it->second->SetIsJoinCalc(isJoined);
    }

    return TPS_ER_SUCCESS;
}

int TpsBeamManager::UpdateBeams(std::vector<RtBeam*>& beamList){
    for (std::vector<RtBeam*>::iterator itBeam = beamList.begin(); itBeam != beamList.end(); ++itBeam){
        RtBeam* beam = *itBeam;
        UpdateBeam(beam);
    }
    return TPS_ER_SUCCESS;
}

RtBeam* TpsBeamManager::GetBeam(const std::string& beamUID){
    boost::lock_guard<boost::mutex> lg(*mMutex);
    std::map<std::string, RtBeam*>::iterator it = mBeams.find(beamUID);
    if (it != mBeams.end()){
        return it->second;
    }
    else{
        return nullptr;
    }
}


std::vector<std::string> TpsBeamManager::GetBeamUIDListByNormgroupUID(const std::string& sNormgroupUid)
{
    std::vector<std::pair<std::string, int>> beamListNumberMap;
    for (auto itr=mBeams.begin(); itr!=mBeams.end(); ++itr)
    {
        if (nullptr != (*itr).second)
        {
            if ((*itr).second->get_normgroupuid() == sNormgroupUid)
            {
                beamListNumberMap.push_back(std::make_pair((*itr).first, (*itr).second->get_beamnumber()));
            }
        }
    }
    auto cmp =[](std::pair<std::string, int> const & a, std::pair<std::string, int> const & b)
    {
        return a.second != b.second ? a.second < b.second : a.first < b.first;
    };

    std::sort(beamListNumberMap.begin(), beamListNumberMap.end(), cmp);
    std::vector<std::string> beamUidList;
    for (int i=0; i<beamListNumberMap.size(); ++i)
    {
        beamUidList.push_back(beamListNumberMap[i].first);
    }
    return beamUidList;
}

std::vector<RtBeam*> TpsBeamManager::GetBeamsByNormGroup(const std::string& sNormgroupUid)
{
    std::vector<RtBeam*> beamList;
    for (auto itr=mBeams.begin(); itr!=mBeams.end(); ++itr){
        if (nullptr != (*itr).second){
            if ((*itr).second->get_normgroupuid() == sNormgroupUid){
                beamList.push_back((*itr).second);
            }
        }
    }
    return beamList;
}

std::vector<RtBeam*> TpsBeamManager::GetBeamsByPlan(const std::string& sPlanUid)
{
    std::vector<RtBeam*> beamList;

    std::vector<std::string> ngUids = 
        GetNormgroupManager()->GetNormGroupUIDListByPlan(sPlanUid);

    for (auto itrNg=ngUids.begin(); itrNg!=ngUids.end(); ++itrNg){
        for (auto itr=mBeams.begin(); itr!=mBeams.end(); ++itr){
            if (nullptr != (*itr).second){

                if ((*itr).second->get_normgroupuid() == (*itrNg)){
                    beamList.push_back((*itr).second);
                }
            }
        }
    }
    return beamList;
}

//TODO: Need update after TMS provide functions to access gantry angle directly
int TpsBeamManager::RotateNonArcBeam(const std::string& beamName, float degree)
{
    beamName;
    degree;
    //boost::lock_guard<boost::mutex> lg(mMutex);
    //(void)beamName, (void)degree;
    //RtBeam* beam = GetBeam(beamName);
    //if (beam != nullptr){
    //    std::vector<RtBeamsegment*> segments = beam->GetSegments();

    //    RtBeamsegment* segment;
    //    std::vector<RtBeamsegment*>::iterator it = segments.begin();
    //    for(; it != segments.end(); it++){
    //        segment = *it;
    //        //segment->SetStartMachineAngle()
    //    }
    //    return TPS_ER_SUCCESS;
    //}
    //else
    //    return TPS_ER_FAILURE;
    return TPS_ER_FAILURE;
}

TpsIECCoordinateSystem* TpsBeamManager::GetBeamCS(const std::string& beamUID){
    auto it = mBeamCS.find(beamUID);
    if (it == mBeamCS.end()){
        //LOG
        return nullptr;
    }
    return it->second;
}

int TpsBeamManager::UpdateSegmentOutline(
    const std::string& beamUID, 
    const std::string& segmentUID, 
    RtContour& beamOutline){
        (void)beamUID, (void)segmentUID, (void)beamOutline;
        return TPS_ER_SUCCESS;
}

int TpsBeamManager::UpdateSegmentMLC(
    const std::string& beamUID, 
    const std::string& segmentUID, 
    RtMlcshape& MLCShape){
        (void)beamUID, (void)segmentUID, (void)MLCShape;
        return TPS_ER_SUCCESS;
}

void TpsBeamManager::AddFluenceMap(const std::string& beamUid, FluenceMapEntity* fluenceMap)
{
    auto it = mBeamFluenceMapCollection.find(beamUid);
    if (it != mBeamFluenceMapCollection.end())
    {
        TPS_LOG_DEV_INFO<<"The fluence map of beam: "<<beamUid
            <<" already exists, delete it firstly.";
        delete it->second;
    }
    mBeamFluenceMapCollection[beamUid] = fluenceMap;
}

FluenceMapEntity* TpsBeamManager::GetFluenceMap(const std::string& beamUid) const
{
    auto it = mBeamFluenceMapCollection.find(beamUid);
    if (it == mBeamFluenceMapCollection.end())
    {
        TPS_LOG_DEV_WARNING<<"The fluence map of beam: "<<beamUid<<" does not exist.";
        return nullptr;
    }
    return it->second;
}

void TpsBeamManager::RemoveFluenceMap(const std::string& beamUid)
{
    auto it = mBeamFluenceMapCollection.find(beamUid);
    if (it == mBeamFluenceMapCollection.end())
    {
        TPS_LOG_DEV_INFO<<"The fluence map of beam: "<<beamUid<<" does not exist.";
        return;
    }
    DEL_PTR(it->second);
    mBeamFluenceMapCollection.erase(it);
}

std::string TpsBeamManager::GetNormalGroupUIDViaBeamUID(const std::string& beamUid) const
{
    auto it = mBeams.find(beamUid);
    if (it == mBeams.end() || it->second == nullptr){
        return "";
    }
    return it->second->get_normgroupuid();
}

std::vector<std::string> TpsBeamManager::FilterLoadedDoseGridUid(const std::vector<std::string>& doseGridUidList)
{
    std::vector<std::string> unloadedUids;
    bool hasLoaded = false;
    RtDosegrid* dosegrid = nullptr;
    for (int i = 0; i < doseGridUidList.size(); ++i)
    {
        hasLoaded = false;
        for (auto it = mBeams.begin(); it != mBeams.end(); ++it)
        {
            dosegrid = it->second->get_dosegrid();
            if (dosegrid != nullptr && dosegrid->get_uid() == doseGridUidList[i])
            {
                hasLoaded = true;
                break;
            }
        }
        if (!hasLoaded) unloadedUids.push_back(doseGridUidList[i]);
    }
    return unloadedUids;
}
TPS_END_NAMESPACE
