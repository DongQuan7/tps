//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  jiandong.zhou     jiandong.zhou@united-imaging.com
/// 
///  \file tps_patientmanager.cpp
/// 
///  \brief  entry of patient operate

///  \version 1.0
///  \date    Aug. 6, 2013
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

//TPS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_course.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_prescription.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_constraint.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_site.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

TpsPatientManager::TpsPatientManager(void) : mPatient(nullptr)
{
}

TpsPatientManager::~TpsPatientManager(void) 
{
    Dispose();
}

void TpsPatientManager::SetPatient(RtPatient* patient)
{
    if (mPatient != nullptr)
    {
        delete mPatient;
    }
    mPatient = patient;
}

RtPatient* TpsPatientManager::GetPatient()
{
    return mPatient;
}

bool TpsPatientManager::GetPlanMap(std::map<std::string, RtPlan*> *tpsPlanMap) {
    *tpsPlanMap = mTpsPlanMap;
    return true;
}

bool TpsPatientManager::AddPlan(RtPlan* tpsPlan)
{
    if (tpsPlan == nullptr) return false;

    std::string planUid = tpsPlan->get_uid();
    std::string seriesUid = tpsPlan->get_imageseriesuid();
    auto it = mTpsPlanMap.find(planUid);
    if (it != mTpsPlanMap.end()) {
        delete it->second;
    }
    mTpsPlanMap[planUid] = tpsPlan;
    return true;
}

bool TpsPatientManager::GetPlan(const std::string& sPlanUID, 
    RtPlan** pPlan) const
{
    auto it = mTpsPlanMap.find(sPlanUID);
    if(it == mTpsPlanMap.end()){
        TPS_LOG_DEV_WARNING<<"GetPlan failed! "<<sPlanUID<<" does not exist.";
        return false;
    }

    *pPlan = it->second;
    return true;
}

std::vector<std::string>
TpsPatientManager::GetPlanUidsBySeriesUid(const std::string& serUid) const
{
    std::vector<std::string> vecPlanUids;
    auto it = mTpsPlanMap.begin();
    for (;it!=mTpsPlanMap.end(); it++) {
        if (it->second->get_imageseriesuid()==serUid) {
            vecPlanUids.push_back(it->first);
        }
    }

    return vecPlanUids;
}

std::string TpsPatientManager::GetSeriesUidByPlanUid(const std::string& planUid) {
    auto it = mTpsPlanMap.find(planUid);
    if(it == mTpsPlanMap.end()){
        TPS_LOG_DEV_WARNING<<"GetPlan failed! "<<planUid<<" does not exist.";
        return "";
    }
    return it->second->get_imageseriesuid();
}

bool TpsPatientManager::RemovePlan(const std::string &planUID) {

    //////////////////////////////////////////////////////////////////////////
    //delete poi moved to TpsPlanningDeleteCmdHandler
    //std::vector<std::string> poiList = TpsPOIManager::GetInstance()->GetPoiUidListByPlanUid(planUID);
    //for (auto itrPoi = poiList.begin(); itrPoi!=poiList.end(); ++itrPoi){
    //    if (!TpsPOIManager::GetInstance()->DeletePOI(*itrPoi)){
    //        TPS_LOG_DEV_ERROR<<"delete Poi error with PoiUid:"<<*itrPoi;
    //        return false;
    //    }
    //} 

    //////////////////////////////////////////////////////////////////////////
    //delete plan
    auto it = mTpsPlanMap.find(planUID);
    if (it == mTpsPlanMap.end()) {
        return true;
    }
    RtPlan* plan = it->second;
    if (nullptr!=plan) {
        delete plan;
        mTpsPlanMap.erase(it);
    }
    return true;
}

bool TpsPatientManager::GetCourseMap(std::map<std::string, RtCourse*> *tpsCourseMap) 
{
    *tpsCourseMap = mTpsCourseMap;
    return true;
}

bool TpsPatientManager::GetCourse(const std::string& courseUID, RtCourse ** pCourse) const{

    auto it = mTpsCourseMap.find(courseUID);
    if (it == mTpsCourseMap.end()) {
        return false;
    }

    *pCourse = it->second;
    return true;
}

bool TpsPatientManager::AddCourse(RtCourse* course) 
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

void TpsPatientManager::GetCourseUIDListByPatientUID(const std::string& patientUID, 
    std::vector<std::string>& courseUIDList) 
{
    courseUIDList.clear();
    for (auto it = mTpsCourseMap.begin(); it != mTpsCourseMap.end(); ++it){
        if (it->second == nullptr) continue;
        if (it->second->get_patientuid() == patientUID)
        {
            courseUIDList.push_back(it->first);
        }
    }
}

std::vector<std::string> TpsPatientManager::GetPlanUIDListByCourseUID(const std::string& courseUid)
{
    std::vector<std::string> planUidList;
    for (auto it = mTpsPlanMap.begin(); it != mTpsPlanMap.end(); ++it){
        if (it->second == nullptr) continue;
        if (it->second->get_courseuid() == courseUid)
        {
            planUidList.push_back(it->first);
        }
    }
    return planUidList;
}

void TpsPatientManager::GetCourseListByPatientUID(const std::string& patientUid, std::vector<RtCourse*>& courseList)
{
    std::vector<std::string> courseUIDList;
    GetCourseUIDListByPatientUID(patientUid, courseUIDList);
    RtCourse* course = nullptr;
    auto itCourseUID = courseUIDList.cbegin();
    for (; itCourseUID != courseUIDList.cend(); ++itCourseUID) {
        if (!GetCourse(*itCourseUID , &course) || course == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get Course["<<*itCourseUID<<"] from TpsPatientManager";
            return ;
        }
        courseList.push_back(course);
    }
}

//bool TpsPatientManager::GetPrescriptionMap(std::map<std::string, RTFWK::RTFWKDLDOPrescription*> *prescriptionMap) {
//    *prescriptionMap = mTpsPrescriptionMap;
//    return true;
//}
//bool TpsPatientManager::GetPrescription(const std::string& prescriptionUID, 
//                                        RTFWK::RTFWKDLDOPrescription *&prescription) {
//    mTpsPrescriptionMapIt it = mTpsPrescriptionMap.find(prescriptionUID);
//    if (it == mTpsPrescriptionMap.end()) {
//        return false;
//    }
//    prescription = mTpsPrescriptionMap[prescriptionUID];
//    return true;
//}
//bool TpsPatientManager::SetPrescription(RTFWK::RTFWKDLDOPrescription *prescription) {
//    mTpsPrescriptionMapIt it = mTpsPrescriptionMap.find(prescription->GetUID());
//    if (it == mTpsPrescriptionMap.end()) {
//        mTpsPrescriptionMap[prescription->GetUID()] = prescription;
//        return true;
//    }
//    return false;
//}

bool TpsPatientManager::GetPrescription(const std::string& courseUid, std::vector<RtPrescription*>& prescriptions)
{
    prescriptions.clear();
    courseUid;
    for (auto it = mTpsPrescriptionMap.begin(); it != mTpsPrescriptionMap.end(); ++it){
        if (it->second == nullptr) continue;
        //if (it->second->get_courseuid() == courseUid)
        {
            prescriptions.push_back(it->second);
        }
    }
    return true;
}

bool TpsPatientManager::GetPrescriptionByUid(const std::string& uid, RtPrescription** prescription)
{
    std::map<std::string, RtPrescription*>::iterator managerptr = 
        mTpsPrescriptionMap.find(uid);

    if (managerptr != mTpsPrescriptionMap.end()) 
    {
        *prescription = managerptr->second;
        return true;
    }
    return false;

}
bool TpsPatientManager::GetPrescriptionBySiteUid(const std::string& siteUid, std::vector<RtPrescription*> &prescriptions)
{
    prescriptions.clear();
    for (auto it = mTpsPrescriptionMap.begin(); it != mTpsPrescriptionMap.end(); ++it)
    {
        if (it->second == nullptr) continue;
        if (it->second->get_siteuid() == siteUid)
        {
            prescriptions.push_back(it->second);
        }
    }
    if (prescriptions.empty())
    {
        return false;
    }
    return true;
}

bool TpsPatientManager::AddPrescription(RtPrescription* prescription)
{
    if (prescription == nullptr) return false;
    std::string uid = prescription->get_uid();
    auto it = mTpsPrescriptionMap.find(uid);
    if (mTpsPrescriptionMap.end() != it)
    {
        delete it->second;
    }
    mTpsPrescriptionMap[uid] = prescription;
    return true;
}

//bool TpsPatientManager::GetCourseListByPatientUID(const std::string &patientUid, 
//    std::vector<std::string> *courseList) {
//     
//    auto it = mTpsPatientMap.find(patientUid);
//    if(it == mTpsPatientMap.end()) {
//        TPS_LOG_DEV_ERROR<<"Failed to find patient by patient uid:"
//            <<patientUid;
//        return false;
//    }
//
//    RtPatient *pat= it->second;
//    std::vector<std::string> courseListTemp;
//    if(!pat->GetCourseUIDList(courseListTemp)) {
//        TPS_LOG_DEV_ERROR<<"Failed to get course list by patient:"
//            <<patientUid;
//        return false;
//    }
//    *courseList = courseListTemp;
//
//
//    return true;
//}

bool TpsPatientManager::GetPlanToTotalGoals(std::map<std::string, float>& tpsPlanToTotalGoals) const{
    tpsPlanToTotalGoals = mTpsPlanToTotalGoals;
    return true;
}

void TpsPatientManager::SetPlanToTotalGoals(std::map<std::string, float>& tpsPlanToTotalGoals){
    mTpsPlanToTotalGoals = tpsPlanToTotalGoals;
}

bool TpsPatientManager::GetTotalGoals(std::string planUid, float& totalGoals) const{

    auto it = mTpsPlanToTotalGoals.find(planUid);
    if (it == mTpsPlanToTotalGoals.end()){
        TPS_LOG_DEV_ERROR<<"can not find total goals for plan["<<planUid<<"].";
        return false;
    }
    totalGoals = it->second;
    return true;
}

void TpsPatientManager::UpdateTotalGoal( std::string planUID, float& totalGoal ) {
    //if already have, then update it
    //if have not, then add it
    mTpsPlanToTotalGoals[planUID] = totalGoal;
}

bool TpsPatientManager::AddConstraint(RtConstraint* constraint) {

    if (constraint == nullptr) {
        return false;
    }

    std::string constraintUid = constraint->get_uid();
    if (mTpsConstraintMap.find(constraintUid) != mTpsConstraintMap.end()) {
        delete mTpsConstraintMap[constraintUid];
        mTpsConstraintMap[constraintUid] = nullptr;
    }
    mTpsConstraintMap[constraintUid] = constraint;
    return true;
}

void TpsPatientManager::GetConstraintByPrescriptionUid(std::string prescriptionUid, std::vector<RtConstraint*>& constraintList) {

    std::for_each(mTpsConstraintMap.begin(), mTpsConstraintMap.end(), [&](std::map<std::string, RtConstraint*>::value_type& pairConstraint) {

        if (pairConstraint.second->get_prescriptionuid() == prescriptionUid) {
            constraintList.push_back(pairConstraint.second);
        }
    });
}

void TpsPatientManager::AddCourseList(const std::vector<RtCourse*>& courseList)
{
    std::string uid;
    TpsCourseMap::iterator it;
    for (int i = 0; i < courseList.size(); ++i){
        if (courseList[i] == nullptr) continue;
        uid = courseList[i]->get_uid();

        it = mTpsCourseMap.find(uid);
        if (it != mTpsCourseMap.end()) delete it->second;
        mTpsCourseMap[uid] = courseList[i];
    }
}

void TpsPatientManager::AddPrescriptionList(const std::vector<RtPrescription*>& prescriptionList)
{
    std::string uid;
    TpsPrescriptionMap::iterator it;
    for (int i = 0; i < prescriptionList.size(); ++i){
        if (prescriptionList[i] == nullptr) continue;
        uid = prescriptionList[i]->get_uid();

        it = mTpsPrescriptionMap.find(uid);
        if (it != mTpsPrescriptionMap.end()) delete it->second;
        mTpsPrescriptionMap[uid] = prescriptionList[i];
    }
}

void TpsPatientManager::AddSiteList(const std::vector<RtSite*>& siteList)
{
    std::string uid;
    TpsSiteMap::iterator it;
    for (int i = 0; i < siteList.size(); ++i)
    {
        if (siteList[i] == nullptr) continue;
        uid = siteList[i]->get_uid();
        it = mTpsSiteMap.find(uid);
        if (it != mTpsSiteMap.end()) delete it->second;
        mTpsSiteMap[uid] = siteList[i];
    }
}

bool TpsPatientManager::GetSiteByUid(const std::string& uid, RtSite** site)
{
    TpsSiteMap::iterator managerptr = mTpsSiteMap.find(uid);

    if (managerptr != mTpsSiteMap.end()) 
    {
        *site = managerptr->second;
        return true;
    }
   return false;

}

bool TpsPatientManager::GetSiteByCourseUid(const std::string& uid, std::vector<RtSite*>& sites)
{
    sites.clear();
    for (auto it = mTpsSiteMap.begin(); it != mTpsSiteMap.end(); ++it)
    {
        if (it->second == nullptr) continue;
        if (it->second->get_courseuid() == uid)
        {
            sites.push_back(it->second);
        }
    }
    return true;
}

void TpsPatientManager::AddConstraintList(const std::vector<RtConstraint*>& constraintList)
{
    std::string uid;
    TpsConstraintMap::iterator it;
    for (int i = 0; i < constraintList.size(); ++i){
        if (constraintList[i] == nullptr) continue;
        uid = constraintList[i]->get_uid();

        it = mTpsConstraintMap.find(uid);
        if (it != mTpsConstraintMap.end()) delete it->second;
        mTpsConstraintMap[uid] = constraintList[i];
    }
}

std::vector<RtCourse*> TpsPatientManager::GetAllCourse() const
{
    std::vector<RtCourse*> vec(mTpsCourseMap.size());
    std::transform(mTpsCourseMap.begin(), mTpsCourseMap.end(), vec.begin(), 
        [](const std::pair<std::string, RtCourse*>& pr){ return pr.second; });
    return vec;
}

std::vector<RtPrescription*> TpsPatientManager::GetAllPrescription() const
{
    std::vector<RtPrescription*> vec(mTpsPrescriptionMap.size());
    std::transform(mTpsPrescriptionMap.begin(), mTpsPrescriptionMap.end(), vec.begin(), 
        [](const std::pair<std::string, RtPrescription*>& pr){ return pr.second; });
    return vec;
}

std::vector<RtConstraint*> TpsPatientManager::GetAllConstraint() const
{
    std::vector<RtConstraint*> vec(mTpsConstraintMap.size());
    std::transform(mTpsConstraintMap.begin(), mTpsConstraintMap.end(), vec.begin(), 
        [](const std::pair<std::string, RtConstraint*>& pr){ return pr.second; });
    return vec;
}

void TpsPatientManager::ClearPlanOrderInformation()
{
    ClearCourseMap_i();
    ClearPrescriptionMap_i();
    ClearConstraintMap_i();
}

void TpsPatientManager::Dispose()
{
    ClearCourseMap_i();
    ClearPlanMap_i();
    ClearPrescriptionMap_i();
    ClearPatientMap_i();
    ClearConstraintMap_i();
    ClearSiteMap_i();
}

void TpsPatientManager::ClearPatientData() {
    Dispose();
}

void TpsPatientManager::ClearCourseMap_i()
{
    for (auto it = this->mTpsCourseMap.begin(); 
        it != this->mTpsCourseMap.end(); ++it) {
            delete it->second;
    }
    mTpsCourseMap.clear();
}

void TpsPatientManager::ClearPlanMap_i()
{
    for (auto it = this->mTpsPlanMap.begin(); 
        it != this->mTpsPlanMap.end(); ++it) {
            delete it->second;
            it->second = nullptr;
    }
    mTpsPlanMap.clear();
}

void TpsPatientManager::ClearPrescriptionMap_i()
{
    for (auto it = this->mTpsPrescriptionMap.begin(); 
        it != this->mTpsPrescriptionMap.end(); ++it) {
        if (it->second == nullptr) continue;
        delete it->second;
        it->second = nullptr;
    }
    mTpsPrescriptionMap.clear();
}

void TpsPatientManager::ClearPatientMap_i()
{
    DEL_PTR(mPatient);
}

void TpsPatientManager::ClearConstraintMap_i()
{
    for (auto it = mTpsConstraintMap.begin(); it != mTpsConstraintMap.end(); ++it){
        if (it->second == nullptr) continue;
        delete it->second;
        it->second = nullptr;
    }
    mTpsConstraintMap.clear();
}


void TpsPatientManager::ClearSiteMap_i()
{
    for (auto it = mTpsSiteMap.begin(); it != mTpsSiteMap.end(); ++it){
        if (it->second == nullptr) continue;
        delete it->second;
        it->second = nullptr;
    }
    mTpsSiteMap.clear();
}


TPS_END_NAMESPACE
