////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_da_gallery_patientmanager.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_course.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

//TPS
#include "tps_logger.h"


TPS_BEGIN_NAMESPACE

//TpsGalleryPatientManager::TpsGalleryPatientManager(void) : mCurrentPatientUID(""), mCurrentCourseUID(""){
//    mTpsCourseMap.clear(); 
//}

TpsGalleryPatientManager::TpsGalleryPatientManager(void) : mPatient(nullptr)
{
}

TpsGalleryPatientManager::~TpsGalleryPatientManager(void) 
{
    Dispose();
}

void TpsGalleryPatientManager::SetPatient(RtPatient* patient)
{
    if (mPatient != nullptr)
    {
        delete mPatient;
    }
    mPatient = patient;
}

RtPatient* TpsGalleryPatientManager::GetPatient()
{
    return mPatient;
}

//void TpsGalleryPatientManager::ClearManager(void)
//{
//    for (mTpsCourseMapIt it = this->mTpsCourseMap.begin(); 
//        it != this->mTpsCourseMap.end(); ++it) {
//            delete it->second;
//    }
//    mTpsCourseMap.clear();
//
//    for (auto it = this->mTpsPatientMap.begin(); 
//        it != this->mTpsPatientMap.end(); ++it) {
//            delete it->second;
//            it->second = nullptr;
//    }
//    mTpsPatientMap.clear(); 
//}


//TpsGalleryPatientManager::~TpsGalleryPatientManager(void) {
//
//    for (mTpsCourseMapIt it = this->mTpsCourseMap.begin(); 
//        it != this->mTpsCourseMap.end(); ++it) {
//            delete it->second;
//    }
//    mTpsCourseMap.clear();  
//
//    //delete patients
//    for (auto it = this->mTpsPatientMap.begin(); 
//        it != this->mTpsPatientMap.end(); ++it) {
//            delete it->second;
//            it->second = nullptr;
//    }
//    mTpsPatientMap.clear();
//}

//bool TpsGalleryPatientManager::GetPatientMap(std::map<std::string, 
//    RtPatient*> *tpsPatientMap) {
//    *tpsPatientMap = mTpsPatientMap;
//    return true;
//}
//
//bool TpsGalleryPatientManager::GetPatient(const std::string& patientUID, 
//    RtPatient *&patient) {
//    mTpsPatientMapIt it = mTpsPatientMap.find(patientUID);
//    if (it == mTpsPatientMap.end()) {
//        return false;
//    }
//    patient = mTpsPatientMap[patientUID];
//    return true;
//}
//
//bool TpsGalleryPatientManager::AddPatient(const RtPatient& patient) {
//
//    mTpsPatientMapIt it = mTpsPatientMap.find(patient.get_uid());
//    if (it == mTpsPatientMap.end()) {
//
//        //need to be deleted later
//        auto pPatient = new RtPatient(patient);
//
//        mTpsPatientMap[patient.get_uid()] = pPatient;
//        return true;
//    }
//    return false;
//}

bool TpsGalleryPatientManager::GetCourseMap(std::map<std::string, RtCourse*> *tpsCourseMap) 
{
    *tpsCourseMap = mTpsCourseMap;
    return true;
}

bool TpsGalleryPatientManager::GetCourse( const std::string& courseUID, RtCourse ** pCourse) const
{
    auto it = mTpsCourseMap.find(courseUID);
    if (it == mTpsCourseMap.end()) {
        return false;
    }

    *pCourse = it->second;
    return true;
}

bool TpsGalleryPatientManager::AddCourse(RtCourse* course) 
{
    std::string courseUid = course->get_uid();
    auto itCourse = mTpsCourseMap.find(courseUid);
    // If course exists, remove it firstly
    if (itCourse != mTpsCourseMap.end()) {
        TPS_LOG_DEV_WARNING<<"course["<<course->get_uid()
            <<"] already exist. The old one will removed firstly.";
        delete itCourse->second;
        mTpsCourseMap.erase(itCourse);
    }

    mTpsCourseMap[courseUid] = course;
    return true;
}   

////////////////////////////////////////////////////////////////////////////
////// Series Map
////void TpsGalleryPatientManager::GetSeriesUIDListByCourseUID(const std::string &sCourseUID, std::vector<std::string>& seriesUIDList)
////{
////    //auto it = mTpsCourseMap.find(sCourseUID);
////    //if (it == mTpsCourseMap.end()){
////    //    return false;
////    //}
////    //std::vector<RtSeries*> vSeries;
////    ////todo waiting for new interface, ask qq.z
////    ////if (it->second->GetSeriesObjectList(vSeries))
////    //{
////    //    for (auto seriesIt = vSeries.begin();
////    //        seriesIt != vSeries.end(); ++seriesIt){
////    //            seriesUIDList->push_back((*seriesIt)->get_uid());
////    //    }
////
////    //    // delete
////    //    for (auto seriesIt = vSeries.begin();
////    //        seriesIt != vSeries.end(); ++seriesIt){
////    //            delete *seriesIt;
////    //            *seriesIt = nullptr;
////    //    }
////    //    vSeries.clear();
////    //}
////
////    //return true;
////
////    auto itCourse2SeriesList = mCourse2SeriesListMap.find(sCourseUID);
////    // Course2SeriesList exists.
////    if (itCourse2SeriesList != mCourse2SeriesListMap.end()) {
////        seriesUIDList = itCourse2SeriesList->second;
////    }
////    else {
////        TPS_LOG_DEV_WARNING<<"courseUID NOT exist in Course2SerList map.";
////    }
////}
//
////void TpsGalleryPatientManager::AddSeriesToCourse(const std::string& seriesUID, 
////    const std::string& courseUID) 
////{
////    auto itCourse2Series = mCourse2SeriesListMap.find(courseUID);
////    // Course2SeriesList exists.
////    if (itCourse2Series != mCourse2SeriesListMap.end()) {
////        itCourse2Series->second.push_back(seriesUID);
////        return;
////    }
////    else {
////        std::vector<std::string> seriesUIDList;
////        seriesUIDList.push_back(seriesUID);
////        mCourse2SeriesListMap[courseUID] = seriesUIDList;
////    }
////}
//
//void TpsGalleryPatientManager::SetCurrentPatient(const std::string& sPatientUID){
//    mCurrentPatientUID = sPatientUID;
//}
//
//std::string TpsGalleryPatientManager::GetCurrentPatient(){
//    return mCurrentPatientUID;
//}
//
//std::string TpsGalleryPatientManager::GetCurrentPatientName(){
//    auto it = mTpsPatientMap.find(mCurrentPatientUID);
//    if(it == mTpsPatientMap.end()) {
//        TPS_LOG_DEV_ERROR<<"Failed to find patient by patient uid:"
//            <<mCurrentPatientUID;
//        return "";
//    }
//    return it->second->get_firstname() + " " + 
//           it->second->get_middlename() + " " + 
//           it->second->get_lastname();
//}

void TpsGalleryPatientManager::GetCourseUIDListByPatientUID(const std::string& patientUID, 
    std::vector<std::string>& courseUIDList) 
{
    courseUIDList.clear();
    for (auto it = mTpsCourseMap.begin(); it != mTpsCourseMap.end(); ++it) {
        if (it->second == nullptr) continue;
        if (it->second->get_patientuid() == patientUID)
        {
            courseUIDList.push_back(it->second->get_uid());
        }
    }
}

void TpsGalleryPatientManager::GetCourseListByPatientUID(const std::string &patientUID, 
    std::vector<RtCourse*>& courseList) 
{
    courseList.clear();
    for (auto it = mTpsCourseMap.begin(); it != mTpsCourseMap.end(); ++it) {
        if (it->second == nullptr) continue;
        if (it->second->get_patientuid() == patientUID)
        {
            courseList.push_back(it->second);
        }
    }
}

//void TpsGalleryPatientManager::SetCurrentCourseUID(const std::string& strCourseUID) {
//    mCurrentCourseUID = strCourseUID;
//}
//
//std::string TpsGalleryPatientManager::GetCurrentCourseUID() const {
//    return mCurrentCourseUID;
//}

void TpsGalleryPatientManager::SetBeamGroupUIDsByCourseUID(const std::string &courseUID, 
    const std::vector<std::string> &beamGroupUIDs)
{
    
        std::vector<std::string> vBeamGroupUIDs(beamGroupUIDs.begin(), beamGroupUIDs.end());
    mCourseBeamGroupUIDMap[courseUID] = vBeamGroupUIDs;
}

bool TpsGalleryPatientManager::GetBeamGroupUIDsByCourseUID(const std::string &courseUID, 
    std::vector<std::string> &vBeamGroupUIDs)
{

        vBeamGroupUIDs.clear();    
        auto it = mCourseBeamGroupUIDMap.find(courseUID);
        if (it == mCourseBeamGroupUIDMap.end()) {
            
            return false;
        }
        vBeamGroupUIDs.insert(vBeamGroupUIDs.begin(), it->second.begin(), it->second.end());
        return true;  
}

void TpsGalleryPatientManager::ClearPatientData() {
    Dispose();
}

void TpsGalleryPatientManager::Dispose()
{
    ClearCourseMap_i();
    ClearPatientMap_i();
}

void TpsGalleryPatientManager::ClearCourseMap_i()
{
    for (auto it = this->mTpsCourseMap.begin(); 
        it != this->mTpsCourseMap.end(); ++it) {
            delete it->second;
    }
    mTpsCourseMap.clear();
}

void TpsGalleryPatientManager::ClearPatientMap_i()
{
    DEL_PTR(mPatient);
}

TPS_END_NAMESPACE
