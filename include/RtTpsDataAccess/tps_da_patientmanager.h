//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_da_patientmanager.h
///  \brief   entry of patient operator
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_PATIENT_MANAGER_H
#define TPS_PATIENT_MANAGER_H

#include "tps_da_defs.h"
#include <map>

#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE;

class RtPatient;
class RtCourse;
class RtPlan;
class RtPrescription;
class RtConstraint;
class RtSite;

class TPS_DA_DECLSPEC TpsPatientManager : public TpsDataManagerBase
{
public:
    TpsPatientManager(void);
    virtual ~TpsPatientManager(void);

    typedef std::map<std::string, RtPlan*> RtPlanMap;
    typedef std::map<std::string, RtCourse*> TpsCourseMap;
    typedef std::map<std::string, RtPrescription*> TpsPrescriptionMap;
    typedef std::map<std::string, RtConstraint*> TpsConstraintMap;
    typedef std::map<std::string, RtSite*> TpsSiteMap;

    //Patient Operations
    void SetPatient(RtPatient* patient);
    RtPatient* GetPatient();

    //Try Operations
    bool GetPlanMap(std::map<std::string, RtPlan*> *tpsPlanMap);
    bool AddPlan(RtPlan* tpsPlan);

    //\brief Get try 
    //    \param[in] sPlanUID          UID of Plan
    //    \param[out] pPlan            Plan entity
    //    \return bool  true when succeeded, false otherwise
    bool GetPlan(const std::string& sPlanUID, RtPlan** pPlan) const;
    std::vector<std::string> GetPlanUidsBySeriesUid(const std::string& serUid) const;

    bool RemovePlan(const std::string& planUID); 

    //Course Operations
    bool GetCourseMap(std::map<std::string, RtCourse*> *tpsCourseMap);
    bool GetCourse(const std::string& courseUID, RtCourse ** pCourse) const;
    bool AddCourse(RtCourse* course);

    bool GetPrescription(const std::string& courseUid, std::vector<RtPrescription*>& prescriptions);
    bool AddPrescription(RtPrescription* prescription);
    bool GetPrescriptionBySiteUid(const std::string& siteUid, std::vector<RtPrescription*> &prescriptions);
    bool GetPrescriptionByUid(const std::string& uid, RtPrescription** prescription);

    //bool GetCourseListByPatientUID(const std::string &patientUid, std::vector<std::string> *courseList);

    bool GetPlanToTotalGoals(std::map<std::string, float>& tpsPlanToTotalGoals) const;
    void SetPlanToTotalGoals(std::map<std::string, float>& tpsPlanToTotalGoals);
    bool GetTotalGoals(std::string planUid, float& totalGoals) const;
    void UpdateTotalGoal(std::string planUID, float& totalGoal);

    void GetCourseUIDListByPatientUID(const std::string&, std::vector<std::string>&);
    void GetCourseListByPatientUID(const std::string&, std::vector<RtCourse*>&);
    std::vector<std::string> GetPlanUIDListByCourseUID(const std::string& courseUid);

    std::string GetSeriesUidByPlanUid(const std::string& planUid);

    void ClearPatientData();

    //get sorted plan uid list of specified series
    //the sorting is based on plan creation time
    void GetSortedPlanUidListBySeriesUid(const std::string& sImageUID, 
        std::vector<std::string>& sortedList);
	//\brief    Keep merge result for latest merge operation
	//param[in]  merge result id
	//param[in]  
	//Out        
	inline void SetCurrentResultPlanUID(const std::string mergePlanUID){mCurrentResultPlanUID = mergePlanUID;}
	inline std::string GetCurrentResultPlanUID(){return mCurrentResultPlanUID;}
    inline void SetPrimaryPlanUID(const std::string &planUID) {mPrimaryPlanUID = planUID;}
    inline std::string GetPrimaryPlanUID() {return mPrimaryPlanUID;}
    inline void SetSecondaryPlanUID(const std::string &planUID) {mSecondaryPlanUID = planUID;}
    inline std::string GetSecondaryPlanUID() {return mSecondaryPlanUID;}

    bool AddConstraint(RtConstraint* constraint);
    
    void GetConstraintByPrescriptionUid(std::string prescriptionUid, std::vector<RtConstraint*>& constraintList);

    void ClearPlanOrderInformation();
    void AddCourseList(const std::vector<RtCourse*>& courseList);
    void AddPrescriptionList(const std::vector<RtPrescription*>& prescriptionList);
    void AddConstraintList(const std::vector<RtConstraint*>& constraintList);

    void AddSiteList(const std::vector<RtSite*>& siteList);
    bool GetSiteByUid(const std::string& uid, RtSite** site);
    bool GetSiteByCourseUid(const std::string& uid, std::vector<RtSite*>& sites);

    std::vector<RtCourse*> GetAllCourse() const;
    std::vector<RtPrescription*> GetAllPrescription() const;
    std::vector<RtConstraint*> GetAllConstraint() const;

private:
    void Dispose();
    void ClearCourseMap_i();
    void ClearPlanMap_i();
    void ClearPrescriptionMap_i();
    void ClearPatientMap_i();
    void ClearConstraintMap_i();
    void ClearSiteMap_i();

private:
    RtPatient* mPatient;
    TpsCourseMap mTpsCourseMap;
    TpsPrescriptionMap mTpsPrescriptionMap;
    RtPlanMap mTpsPlanMap;
    TpsConstraintMap mTpsConstraintMap;
    TpsSiteMap mTpsSiteMap;

    std::map<std::string, float> mTpsPlanToTotalGoals;
    std::map<std::string, std::string> mEasyPlanNormGroupMap; //<easyPlanUID, easyNormGroupUID>

    std::string     mCurrentResultPlanUID;
    std::string     mPrimaryPlanUID;
    std::string     mSecondaryPlanUID;
};

TPS_END_NAMESPACE
#endif //TPS_PATIENT_MANAGER_H
