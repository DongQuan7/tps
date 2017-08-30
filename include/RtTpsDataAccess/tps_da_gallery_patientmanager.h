////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_da_gallery_patientmanager.h
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
////////////////////////////////////////////////////////////////
#ifndef TPS_GALLERY_PATIENT_MANAGER_H
#define TPS_GALLERY_PATIENT_MANAGER_H

#include "tps_da_defs.h"
#include <map>

#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE
class RtPatient;
class RtCourse;
class ITpsDataRepository;

class TPS_DA_DECLSPEC TpsGalleryPatientManager : public TpsDataManagerBase
{
public:
    TpsGalleryPatientManager();
    ~TpsGalleryPatientManager();

    typedef std::map<std::string, RtCourse*> RtCourseMap;
    //typedef std::map<std::string, RtPatient*> RtPatientMap;

    //void SetRepository(ITpsDataRepository* repos) {
    //    mDataRepository = repos;
    //}

    ////Patient Operations
    //bool GetPatientMap(std::map<std::string, RtPatient*> *tpsPatientMap);
    //bool GetPatient(const std::string& patientUID, RtPatient *&patient);

    //bool AddPatient(const RtPatient& patient);

    //void SetCurrentPatient(const std::string& sPatientUID);
    //std::string GetCurrentPatient();
    //std::string GetCurrentPatientName();
    void SetPatient(RtPatient* patient);
    RtPatient* GetPatient();

    //Course Operations
    bool GetCourseMap(std::map<std::string, RtCourse*> *tpsCourseMap);
    bool GetCourse(const std::string& courseUID, RtCourse ** pCourse) const;
    bool AddCourse(RtCourse* course);

    //void GetSeriesUIDListByCourseUID(const std::string &sCourseUID, std::vector<std::string>& seriesUIDList);
    //void AddSeriesToCourse(const std::string&, const std::string&);

    void GetCourseUIDListByPatientUID(const std::string& patientUID, std::vector<std::string>& courseUIDList); 
    void GetCourseListByPatientUID(const std::string &patientUID, std::vector<RtCourse*>& courseList);
    //void AddCourseToPatient(const std::string&, const std::string&);

    //std::string GetCurrentCourseUID() const;
    //void SetCurrentCourseUID(const std::string& strCourseUID);

    void SetBeamGroupUIDsByCourseUID(const std::string &courseUID, const std::vector<std::string> &beamGroupUIDs);

    bool GetBeamGroupUIDsByCourseUID(const std::string &courseUID, std::vector<std::string> &beamGroupUIDs);

    //void ClearManager(void);

    void ClearPatientData();

private:
    void Dispose();
    void ClearCourseMap_i();
    void ClearPatientMap_i();

private:
    RtPatient* mPatient;
    //RtPatientMap mTpsPatientMap;
    RtCourseMap mTpsCourseMap;

    // relation between Course and Plan/DeliveryBeamGroup 
    std::map<std::string, std::vector<std::string>> mCourseBeamGroupUIDMap;

    //std::string                         mCurrentPatientUID;
    //std::string                         mCurrentCourseUID;

    ////iterator
    //typedef std::map<std::string, RtPatient*>::iterator mTpsPatientMapIt;
    //typedef std::map<std::string, RtCourse*>::iterator mTpsCourseMapIt;
};

TPS_END_NAMESPACE
#endif //TPS_PATIENT_MANAGER_H
