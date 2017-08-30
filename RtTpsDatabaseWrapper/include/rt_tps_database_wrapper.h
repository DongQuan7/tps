//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_wrapper.h
///  \brief     rt tps database wrapper interface 
///
///  \version 1.0
///  \date    July 9,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_WRAPPER_H_
#define RT_TPS_DATABASE_WRAPPER_H_

#include "McsfFileParser/mcsf_file_parser_factory.h"  //for Parser XML file 

#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "rt_tps_database_wrapper_utils.h"


RT_TPS_DATABASE_BEGIN_NAMESPACE;

class RtDatabaseHelper;
class RtDatabaseClient;
class RtDatabaseClientTcp;

enum  enDataType 
{
      DT_DOSEGRID = 0,
      DT_IMAGE,
      DT_SERIES
};
//extern boost::recursive_mutex* g_DBInitRecMutex;

// RtDatabaseWrapper
class RtDatabaseWrapper : public IRtDatabaseWrapper
{
public:

    // constructor
    RtDatabaseWrapper(void);
    // destructor
    virtual ~RtDatabaseWrapper(void);

    /////////////////////////////////////////////////////////////////
    ///  \brief        
    ///         Close DB connection
    ///  \return bool
    ///         true:successful close connection
    ///         true:failed to close connection
    /////////////////////////////////////////////////////////////////
    bool Finalize();

    void BeginTrans();

    bool Commit();

    void AutoTransOff();

    void RollBack();

    void ClearAllPatientLocks();

    /////////////////////////////////////////////////////////////////
    ///  \brief        CheckValidateString "'"
    ///
    ///  \param[in]     const std::string& sInput
    ///
    ///  \param[out]    None
    ///  \return            bool
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    std::string CheckValidateString(const std::string& sInput) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief        Insert persistent object of RtPatient on database
    ///  \param[in]     const RtPatient*& pPatient     
    ///                        PatientUID and PatientID not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertPatient(RtPatient& patient ) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief        Insert persistent object of RtStudy on database
    ///  \param[in]     const RtStudy*& pStudy     
    ///                        StudyUID and StudyID not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertStudy(RtStudy& study) const;

    int InsertCourse(RtCourse& course) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtVoi on database
    ///  \param[in]     const RtVoi* pVoi
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_VERSION_NOT_MATCHED--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertStructureset(RtStructureset& structureset);
    int InsertVoi(RtVoi& voi);
    int InsertContour(RtContour& contour) const;

    int InsertPoiObject(RtPoi& poi);
    int InsertBlock(RtBlock& block);
    int UpdateBlock(RtBlock& block);

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtDoseline on database
    ///  \param[in]     const RtVoi* pVoi
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_VERSION_NOT_MATCHED--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertDoselineObject(RtDoseline* pDoseline);

    /////////////////////////////////////////////////////////////////
    ///  \brief        Insert persistent object of RtPlan on database
    ///  \param[in]     const RtPlan* pPlan 
    ///                        PlanUid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertPlanObject(RtPlan* pPlan) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtNormgroup on database
    ///  \param[in]     const RtNormgroup* pNormgroup 
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertNormgroupObject(RtNormgroup* pNormgroup) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtBeam on database
    ///  \param[in]     const RtBeam* pBeam
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertBeamObject(RtBeam& beam);

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtBeams on database
    ///  \param[in]     const RtBeamsegment* pBeamsegment
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertBeamsegmentObject(RtBeamsegment& beamsegment);

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtMlcshape on database
    ///  \param[in]     const RtMlcshape* pMlcshape
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertMlcshapeObject(RtMlcshape& mlcshape);

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtBeams on database
    ///  \param[in]     const RtBeamsegment* pBeamsegment
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertmlcshapeObject(RtBeamsegment* pBeamsegment);

    /////////////////////////////////////////////////////////////////
    ///  \brief         Insert persistent object of RtDosegrid on database
    ///  \param[in]     const RtDosegrid* pDosegrid
    ///                 uid not null
    ///     NOTE:
    ///                 Check UID in order to insert valid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///  \param[out]    None
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized or input is null
    ///         DB_CODES_INPUT_VIOLATION--invalid UID
    ///         DB_CODES_UID_NOT_UNIQUE--object already persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int InsertDosegridObject(RtDosegrid* pDosegrid);

    int InsertImagetransformationObject(RtImagetransformation& Imagetransformation) const;
    int InsertPoiDose(RtBeam_poi& beam_poi) const;
    int InsertNormgroupVoiDose(RtNorm_voi& norm_voi) const;
    int InsertPlanVoiDose(RtPlan_voi& plan_voi) const;
    int InsertCt2densityObject(RtCt2density& ct2density) const;

    int InsertSeriesObject(RtSeries& series) ;

    int InsertImageObject(RtImage& image);
    int InsertDoseoptgoalbaseObject(RtDoseoptgoalbase& doseoptgoalbase) const;

    int InsertRtImageObject(RtRtimage& rtimage);
    int InsertDrrsetting(RtDrrsetting& drrSetting) const;
    /////////////////////////////////////////////////////////////////
    ///  \brief         Delete object from database
    ///     NOTE:       The function will delete DICOM files related to the RtPatient,
    ///                 if the record doesn't exist, return success.
    ///  \param[in]     const std::string& sPlanUid
    ///
    //  \param[out]     None
    ///  \return        int
    ///         DB_CODES_SUCCESS -- success
    ///         DB_CODES_CONNECTION_FAILED -- RtDatabaseWrapper has not been initialized
    ///         DB_CODES_VERSION_NOT_MATCHED --Exception by ODB such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int EraseObject(const std::string& sUid, RtDbDef::OBJECT_TYPE objTye) const;
    int EraseObject(const std::string& sFirstUid, const std::string& sSecondUid,
        RtDbDef::OBJECT_TYPE objType) const;
    int EraseObjectUnderCondition(const std::string& sCondition, RtDbDef::OBJECT_TYPE objType) const;

    std::string CalcHash(const void* pObject, RtDbDef::OBJECT_TYPE objType) const;

    bool QueryDBInTable(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const;
    bool QueryDBInTableViaSpecifiedKey(const std::string& sKey, const std::string& sValue, 
        RtDbDef::OBJECT_TYPE objType) const;
    bool QueryDBInTable(const std::string& sFirstUid, const std::string& sSecondUid, 
        RtDbDef::OBJECT_TYPE objType) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief        Update persistent object of RtPatient on database
    ///
    ///  \param[in]     const RtPatient*& pPatient
    /// NOTE:           pPatient->set_uid() is invalid
    ///                 pPatient->set_patientid() is invalid
    ///  \param[out]    None
    ///
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         ERROR_DB_PATIENT_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int UpdatePatientObject(RtPatient* pPatient) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         Update persistent object of RtPoi on database
    ///
    ///  \param[in]     const RtPoi* pPoi
    ///     NOTE:       update PK or FK invalid
    ///  \param[out]    None
    ///
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         DB_CODES_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_VERSION_NOT_MATCHED-- Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int UpdatePoiObject(RtPoi* pPoi) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         Update persistent object of RtDosegrid on database
    ///
    ///  \param[in]     const RtDosegrid* pDosegrid
    ///     NOTE:       update PK or FK invalid
    ///  \param[out]    None
    ///
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         DB_CODES_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_VERSION_NOT_MATCHED--Exception by ODB such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int UpdateDosegridObject(RtDosegrid* pDosegrid);

    // update voi include contours
    int UpdateVoiObject(RtVoi& voi);
    // update voi attributes exclude contours    
    int UpdateVoiAttributes(RtVoi& voi) const;
    int UpdateContourObject(RtContour& contour) const;
    int UpdateMlcshapeObject(RtMlcshape& mlcshape) const;
    int UpdateCt2densityObject(RtCt2density& ct2density) const;
    int UpdateDoseoptgoalbaseObject(RtDoseoptgoalbase& doseoptgoalbase) const;
    int UpdateDrrsetting(RtDrrsetting& drrSetting) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief        Update persistent object of RtPatient on database
    ///               eg. std::string sSQL("PatientName='WANG^XIAO^Liu', PatientBirthDate='2011-7-21'");
    ///  \param[in]     const std::string& sPatientUID,const std::string& sSQLStatement               
    /// NOTE:   pPatient->set_uid() is invalid
    ///             pPatient->set_patientid() is invalid
    ///             sSQL should NOT include DROP, DELETE, UPDATE, EXECUTE.
    ///  \param[out]    None
    ///
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         DB_CODES_INPUT_VIOLATION--input violation
    ///         ERROR_DB_PATIENT_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int UpdatePatientObjectByUID(const std::string& sPatientUID, 
        const std::string& sSQLStatement) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief               GetPatientByUid
    ///
    ///  \param[in]       const std::string& sPatientUID, RtPatient*& pPatient 
    ///  \param[out]     None
    ///
    ///  \return             int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         ERROR_DB_PATIENT_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetPatientByUid(const std::string& sPatientUid, RtPatient*& pPatient);

    int GetDefaultEquipment(RtEquipment*& pEquipment);
    /////////////////////////////////////////////////////////////////
    ///  \brief         GetDosegridObjectByUid
    ///
    ///  \param[in]     const std::string& sDosegridUid, RtDosegrid* pDosegrid
    ///  \param[out]    None
    ///
    ///  \return             int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         DB_CODES_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetDosegridObjectByNormgroupUid(const std::string& sNormgroupUid, RtDosegrid* pDosegrid);
    int GetDosegridObjectByBeamUid(const std::string& sBeamUid, RtDosegrid*& pDosegrid);
    int GetDosegridObjects(const std::vector<std::string>& dosegridUidList, 
        std::vector<RtDosegrid*>& vRtDosegridList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get series list of a course
    ///  \param[in]    
    ///     NOTE:   If failed, the output will be empty.
    ///             new inside, should be delete outside!!!
    ///  \param[out]    std::vector<RtPatient*>& vRtPatientList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetAllPatientObjects(std::vector<RtPatient*>& vRtPatientList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get all commissioned units 
    ///  \param[in]    
    ///     NOTE:   If failed, the output will be empty.
    ///             new inside, should be delete outside!!!
    ///  \param[out]    std::vector<RtCommissionedunit*>& vRtCommissionedunitList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetAllCommissionedunitObjects(std::vector<RtCommissionedunit*>& vRtCommissionedunitList);
    int GetAllMachines(std::vector<RtMachine*>& vRtMachineList);
    int GetAllKerneldata(std::vector<RtKerneldata*>& vKerneldata);
    int GetAllKerneldatalists(std::vector<RtKerneldatalist*>& vKerneldatalist);
    int GetAllKernelWeights(std::vector<RtKernelweight*>& vKernelweight);
    int GetAllTrays(std::vector<RtTray*>& vTray);
    int GetAllAccessorys(std::vector<RtAccessory*>& vAccessory);
    int GetAllApplicators(std::vector<RtApplicator*>& vApplicator);
    int GetAllSeriesgroups(std::vector<RtSeriesgroup*>& vSeriesgroup);
    int GetAllMcphocommissionedunits(std::vector<RtMcphocommissionedunit*>& vMcphocommissionedunit);
    int GetAllMeasuredprofile(std::vector<RtMeasuredprofile*>& vMeasuredprofile);
    int GetAllCommissionedunit_trays(std::vector<RtCommissionedunit_tray*>& vCommissionedunit_tray);
    int GetAllAccessorychunk(std::vector<RtAccessorychunk*>& vAccessorychunk);
    int GetAllWedgefactors(std::vector<RtWedgefactor*>& vWedgefactor);
    int GetAllMeasureDatas(std::vector<RtMeasureddata*>& vMeasureData);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get all Ct2density  
    ///  \param[in]    
    ///     NOTE:   If failed, the output will be empty.
    ///             new inside, should be delete outside!!!
    ///  \param[out]    std::vector<RtCt2density*>& vRtCt2densityList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetAllCt2densityObjects(std::vector<RtCt2density*>& vRtCt2densityList);
    int GetCt2densityByUid(const std::string& uid, RtCt2density*& pCt2Density);

    /////////////////////////////////////////////////////////////////
    ///  \brief               GetSeriesByUid
    ///
    ///  \param[in]       const std::string& seriesuid, RtSeries* pSeries
    ///  \param[out]     None
    ///
    ///  \return             int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         ERROR_DB_PATIENT_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
	int GetSeriesByUid(const std::string& seriesuid, RtSeries*& pSeries);
	int GetSeriesObjectByUid(const std::string& seriesuid, RtSeries*& pSeries);
    int GetStudyByUid(const std::string& studyuid, RtStudy*& pStudy);
    bool BuildImage3DFromSeries(RtSeries* pRtSeries, std::vector<RtImage*> imageList, unsigned long ulSize);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get course list of a patient
    ///  \param[in]     const std::string& sPatientUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtCourse*>& StudyPtrArray
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetCourseListByPatientUid(const std::string& sPatientUid, std::vector<RtCourse*>& RtCourseList) const;

    int GetPatientListByCondition(const std::string& sCondition, std::vector<RtPatient*>& vList) const;
    /////////////////////////////////////////////////////////////////
    ///  \brief             Get course list of a patient
    ///  \param[in]     const std::string& sPatientUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtSeries*>& RtSeriesListArray
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetSeriesList(const std::vector<std::string>& vSeriesList, std::vector<RtSeries*>& seriesList);
    int GetSeriesListByCourseUid(const std::string& sCoureseUid, std::vector<RtSeries*>& RtSeriesList);
    int GetSeriesListByPatientUid(const std::string& sPatientUid, std::vector<RtSeries*>& RtSeriesList);
    int GetImagetransformationListBySeriesUid(const std::string& sSeriesUid, std::vector<RtImagetransformation*>& vList) const;
    int GetPoiDoseListByPoiUid(const std::string& sPoiUid, std::vector<RtBeam_poi*>& vList) const;
    int GetPoiDoseListByBeamUid(const std::string& sBeamUid, std::vector<RtBeam_poi*>& vList) const;
    int GetPoiDose(const std::string& poiUid, const std::string& beamUid, RtBeam_poi*& beam_poi) const;
    int GetImagetransformation(const std::string& sFixedUid, const std::string& sFloatUid, 
        RtImagetransformation*& pImageTrans) const;
    int GetCommissionedunit_tray(const std::string& sUnitUid, const std::string& sTrayUid, 
        RtCommissionedunit_tray*& pUnitTray) const;
    int GetCommissionedunit_trayListByUnitUid(const std::string& sUnitUid, std::vector<RtCommissionedunit_tray*>& vList) const;
    int UpdateImagetransformation(RtImagetransformation& imageTrans) const;

    int UpdatePoiDose(RtBeam_poi& beam_poi) const;
    /////////////////////////////////////////////////////////////////
    ///  \brief             Get plan list of a course
    ///  \param[in]     const std::string& sCoureseUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtPlan*>& vRtPlanListy
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetPlanListByCourseUid(const std::string& sCourseUid, std::vector<RtPlan*>& vRtPlanList);
    int GetPlanByUid(const std::string& sPlanUid, RtPlan* & pPlan);

    int GetSiteListByCourseUid(const std::string& sCourseUid, std::vector<RtSite*>& vSiteList);

    int GetPlanList(const std::vector<std::string>& vPlanUidList, std::vector<RtPlan*>& vRtPlanList);

    int GetPrescriptionListByCourseUid(const std::string& sCourseUid, std::vector<RtPrescription*>& vPresriptionList);
    int GetDeliverybeamgroupListByPrescriptionUid(const std::string& prescriptionuid, std::vector<RtDeliverybeamgroup*>& vList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get normgroup list of a course
    ///  \param[in]     const std::string& sPlanUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtNormgroup*>& vRtNormgroupList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetNormgroupListByPlanUid(const std::string& sPlanUid, std::vector<RtNormgroup*>& vRtNormgroupList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get normgroup list of a plan
    ///  \param[in]         const std::string& sPlanUid
    ///         NOTE:       If failed, the output will be empty.
    ///  \param[out]        std::vector<RtPoi*>& vRtPoiList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetPoiListBySeriesUid(const std::string& sSeriesUid, std::vector<RtPoi*>& vRtPoiList) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get RtDoseline list of a plan
    ///  \param[in]         const std::string& sPlanUid
    ///         NOTE:       If failed, the output will be empty.
    ///  \param[out]        std::vector<RtDoseline*>& vRtDoselineListt
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetDoselineListByPlanUid(const std::string& sPlanUid, std::vector<RtDoseline*>& vRtDoselineList) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get beam list of a normgroup
    ///  \param[in]     const std::string& sNormgroupUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtBeam*>& vRtBeamListt
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetBeamListByNormgroupUid(const std::string& sNormgroupUid, std::vector<RtBeam*>& vRtBeamList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get beamsegment list of a beam
    ///  \param[in]     const std::string& sBeamUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtBeamsegment*>& vRtBeamsegmentListt
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetBeamsegmentListByBeamUid(const std::string& sBeamUid, std::vector<RtBeamsegment*>& vRtBeamsegmentList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get AccessoryUid list of a beam
    ///  \param[in]         const std::string& sBeamUid
    ///         NOTE:       If failed, the output will be empty.
    ///  \param[out]        std::vector<std::string>& vAccessoryUid
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetAccessoryListByBeamUid(const std::string& sBeamUid, std::vector<std::string>& vAccessoryUid);

    /////////////////////////////////////////////////////////////////
    ///  \brief         Get mlcshape list of a beamsegment
    ///  \param[in]     const std::string& sBeamsegmentUid
    ///         NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtMlcshape*>& vRtMlcshapeList
    ///  \return        int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetMlcshapeListByBeamsegmentUid(const std::string& sBeamsegmentUid, std::vector<RtMlcshape*>& vRtMlcshapeList);

    int GetBlockListByBeamuid(const std::string& beamuid, std::vector<RtBlock*>& vRtBlockList);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get image list of a series
    ///  \param[in]     const std::string& sPatientUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtSeries*>& RtSeriesListArray
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetImageListBySeriesUid(const std::string& seriesUid, std::vector<RtImage*>& RtImageList, bool bWithDcm = false);

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get voi list of a series
    ///  \param[in]     const std::string& seriesUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtVoi*>& RtVoiList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetVoiList(const std::vector<std::string>& vStructureSetUids, std::vector<RtVoi*>& RtVoiList);
    int GetVoiListBySeriesUid(const std::string& seriesUid, std::vector<RtVoi*>& RtVoiList);
    int GetStructuresetListBySeriesUid(const std::string& seriesuid, std::vector<RtStructureset*>& vList);
    int GetVoi(const std::string& sVoiUid, RtVoi*& pVoi);


    int GetVoiSettingList(std::vector<RtVoisetting*>& vVoiSettingList);

    int GetSeriesCountByPatientUid(const std::string& patientUid, int* count);
    int GetVoiCountByStructuresetuid(const std::string& structuresetuid, int* count);

    int GetDrrsettingListByNormgroupUid(const std::string& sNormgroupUid, std::vector<RtDrrsetting*>& vList);
    /////////////////////////////////////////////////////////////////
    ///  \brief             Get contour list of a voi
    ///  \param[in]     const std::string& sVoiUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtVoi*>& RtContourList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetContourListByVoiUid(const std::string& sVoiUid, std::vector<RtContour*>& RtContourList);
    int GetContourListByVoiList(const std::vector<RtVoi*>& VoiList, std::vector<RtContour*>& RtContourList);

    int GetDoseoptgoalbaseListByNormgroupUid(const std::string& sNormgroupUid,
        std::vector<RtDoseoptgoalbase*>& vList);

    /////////////////////////////////////////////////////////////////
    ///  \brief        
    ///                 As a user I want a new interface of Initialize,
    ///                 so that I can get the status about the connection to DB 
    ///                 before using the other interfaces of DBWapper. 
    ///                 This interface will use McsfFileParser to parse McsfDatabaseConfig.xml and connect to DB 
    //                  then return the connection status.
    ///  \param[in]     None
    ///                 
    ///  \param[out]    None
    ///  \return    bool 
    ///                 true: successful connection to DB
    ///                 false: failed connection to DB 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool Initialize ();

    /////////////////////////////////////////////////////////////////
    ///  \brief        Set connetion parameters
    ///                 true: connect to DB right, false:connect to DB wrong
    ///  \param[in]
    ///                 const std::string& sUser,
    ///                 const std::string& sPassword,
    ///                 const std::string& sDatabaseName,
    ///                 const std::string& sHostName,
    ///                 const unsigned int& uiPort
    ///  \param[out]    None
    ///  \return        bool
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool SetConnectionParameters(
        const std::string& sUser,
        const std::string& sPassword,
        const std::string& sDatabaseName,
        const std::string& sHostName,
        const unsigned int& uiPort);

    /////////////////////////////////////////////////////////////////
    ///  \brief         ReturnCodeToString
    ///
    ///  \param[in]     returnCode
    ///  \param[out]    None
    ///
    ///  \return        string
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    const std::string ReturnCodeToString(const int returnCode) const;

    std::string GetSavePath();
    bool CreatePath(const std::string& sPath);

    std::string GetTpsVersion();
    std::string GetDefaultEquipmentUid();

    bool GetDICOMImageBySeriesUid(const std::string& seriesuid, char*& pFile, size_t& length);
    std::string GeneraterUid() const;

    int GetAllPhantomSeries(std::vector<RtSeries*>& RtPhantomList);

    int GetRtImageListByPlanUid(const std::string& sPlanUid, std::vector<RtRtimage*>& vList);
	int GetRtImageListByBeamUid(const std::string& sBeamUid, std::vector<RtRtimage*>& vList);

    //sBeamUid = "1.2.3";
    //sConditon = "beamuid='" + sBeamUid + "'";
    int GetRtImageListByCondition(const std::string& sCondition, std::vector<RtRtimage*>& vList);

    int GetRtImageByUid(const std::string& sUid, RtRtimage*& pRtImage);
    int GetContourByUid(const std::string& sContourUid, RtContour*& pContour);
    int GetNormgroupByUid(const std::string& uid, RtNormgroup*& pNormgroup);
    int GetAccessoryByUid(const std::string& uid, RtAccessory*& pAccessory);

    /////////////////////////////////////////////////////////////////
    ///  \brief         GetRelatedPatientCountByCt2DensityUid
    ///  \param[in]     const std::string& ct2DesnsityUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::string* relatedPatientNames
    ///  \return            int
    ///         Count of related patient count 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    int GetRelatedPatientCountByCt2DensityUid(const std::string& ct2DesnsityUid, std::vector<std::string>& relatedPatientNameList);
    std::string GetOneItemBySql(const std::string& sSql) const;

    int GetCourseListByPatientUidList(std::vector<std::string>& vPatientUidList, std::vector<RtCourse*>& vRtCourseList) const;

    bool CopyFileInBinary(const std::string& sFilePathSrc, const std::string& sFilePathDst) const;

    int GetSystemconfigList(std::vector<RtTemplate*>& vRtSystemconfigList) const;

    bool IsObjectApproved(const std::string& uid) const;

    int InsertApproveRecord(RtApproverecord& approveRecord) const;

    int GetApproveRecordList(std::vector<RtApproverecord*>& vRtApproverecordList) const;

    int GetMachineList(std::vector<std::string> vMachineUidList, std::vector<RtMachine*>& vRtMachineList);

    int GetMachine(const std::string& machineUid, RtMachine*& machine);
#pragma region Commissionedunit
    int GetCommissionedunitListByMachineUidList(std::vector<std::string> vMachineUidList, 
        std::vector<RtCommissionedunit*>& vRtCommissionedunitList);

    int GetMcphocommissionedunitByUnituid(const std::string& commissionunituid, 
        RtMcphocommissionedunit*& mcphocommissionedunit);
    int GetMeasureddataByUnituid(const std::string& commissionunituid, 
        RtMeasureddata*& measureddata);

    int GetMeasuredprofileListByUnituid(const std::string& commissionunituid, 
        std::vector<RtMeasuredprofile*>& vList);
    int GetKernelweightListByUnituid(const std::string& commissionunituid, 
        std::vector<RtKernelweight*>& vList);
    int GetWedgefactorListByUnituid(const std::string& commissionunituid, 
        std::vector<RtWedgefactor*>& vList);

    int GetApplicatorListByMachineuid(const std::string& machineuid, 
        std::vector<RtApplicator*>& vList);
    int GetAccessoryListByMachineuid(const std::string& machineuid, 
        std::vector<RtAccessory*>& vList);
    int GetTrayListByMachineuid(const std::string& machineuid, 
        std::vector<RtTray*>& vList);

    int InsertTray(RtTray& tray) const;
    int UpdateTray(RtTray& tray) const;

    int InsertMachine(RtMachine& machine) const;
    int UpdateMachine(RtMachine& machine) const;

    int InsertCommissionedunit(RtCommissionedunit& commissionedunit) const;
    int UpdateCommissionedunit(RtCommissionedunit& commissionedunit) const;

    int InsertAccessory(RtAccessory& accessory) const;
    int UpdateAccessory(RtAccessory& accessory) const;

    int InsertEquipment(const RtEquipment& equipment) const;

    int InsertAccessorychunk(RtAccessorychunk& accessorychunk) const;
    int UpdateAccessorychunk(RtAccessorychunk& accessorychunk) const;

    int InsertApplicator(RtApplicator& applicator) const;
    int UpdateApplicator(RtApplicator& applicator) const;

    int InsertCommissionedunit_tray(RtCommissionedunit_tray& commissionedunit_tray) const;
    int UpdateCommissionedunit_tray(RtCommissionedunit_tray& commissionedunit_tray) const;

    int InsertMeasuredprofile(RtMeasuredprofile& measuredprofile) const;
    int UpdateMeasuredprofile(RtMeasuredprofile& measuredprofile) const;

    int InsertMeasureddata(RtMeasureddata& measureddata) const;
    int UpdateMeasureddata(RtMeasureddata& measureddata) const;

    int InsertWedgefactor(RtWedgefactor& wedgefactor) const;
    int UpdateWedgefactor(RtWedgefactor& wedgefactor) const;

    int InsertKernelweight(RtKernelweight& kernelweight) const;
    int UpdateKernelweight(RtKernelweight& kernelweight) const;

    int InsertGoldenstt(RtGoldenstt& Goldenstt) const;
    int UpdateGoldenstt(RtGoldenstt& Goldenstt) const;

    int InsertMcphocommissionedunit(RtMcphocommissionedunit& mcphocommissionedunit) const;
    int UpdateMcphocommissionedunit(RtMcphocommissionedunit& mcphocommissionedunit) const;

    int InsertMcphomonoenergyweight(RtMcphomonoenergyweight& Mcphomonoenergyweight) const;
    int UpdateMcphomonoenergyweight(RtMcphomonoenergyweight& Mcphomonoenergyweight) const;

    int GetAccessorychunkListByAccUidAndUnitUid(
        const std::string& sAccUid, 
        const std::string& sUnitUid,
        std::vector<RtAccessorychunk*>& vList);

    int GetGoldensttListByAccUidAndUnitUid(
        const std::string& sAccUid, 
        const std::string& sUnitUid,
        std::vector<RtGoldenstt*>& vList);

    int GetAllGoldenstt(std::vector<RtGoldenstt*>& vGoldenstt);
    int GetAllTcp(std::vector<RtTcp*>& vTcp);
    int GetAllNtcp(std::vector<RtNtcp*>& vNtcp);

    int GetMcphomonoenergyweightListByMcphocommissionedunituid(
        const std::string& mcphocommissionedunituid, 
        std::vector<RtMcphomonoenergyweight*>& vList);

    int GetContourByAccessorychunkuid(
        const std::string& accessorychunkuid, 
        RtContour*& contour);

    int GetAllMonocompressedphasespacedataList(
        std::vector<RtMonocompressedphasespacedata*>& vList);

#pragma endregion Commissionedunit

#pragma region Table
    int InsertTablecontour(RtTablecontour& tablecontour);
    int GetTablecontourByUid(const std::string& uid, RtTablecontour*& pTablecontour);
    int GetTablecontourListByTableUid(const std::string& tableuid, std::vector<RtTablecontour*>& vList);
    int GetAllTablecontourList(std::vector<RtTablecontour*>& vList);

    int InsertSeriestable(RtSeriestable& seriestable) const;
    int UpdateSeriestable(RtSeriestable& seriestable) const;
    int GetAllSeriestableList(std::vector<RtSeriestable*>& vList);
    int GetSeriestable(const std::string& seriesUID, RtSeriestable*& seriesTable);
#pragma endregion Table

    int InsertSeriesgroup(RtSeriesgroup& seriesgroup) const;
    int UpdateSeriesgroup(RtSeriesgroup& seriesgroup) const;
    int GetSeriesgroupByUid(const std::string& uid, RtSeriesgroup*& pSeriesgroup);

    int GetAccessoryListByAccessoryUidList(std::vector<std::string> vAccessoryUidList, std::vector<RtAccessory*>& vRtAccessoryList);

    int GetAccessorychunkListByCommissionedunitUidList(
        std::vector<std::string> vCommissionedunitUidList, 
        std::vector<RtAccessorychunk*>& vRtAccessorychunkList);
    int GetGoldensttListByCommissionedunitUidList(
        std::vector<std::string> vCommissionedunitUidList, 
        std::vector<RtGoldenstt*>& vList);

    int UpdateSeriesObject(RtSeries& series) const;
    int UpdatePlanObject(RtPlan& plan) const;
    int UpdateNormgroupObject(RtNormgroup& normgroup) const;
    //update beam only exclude segments and other objects
    int UpdateBeamObject(RtBeam& beam);
    int UpdateBeamAttributes(RtBeam& beam) const;
    int UpdateBeamSegments(std::vector<RtBeamsegment*> beamSegments) const;

    int EraseDoselineByPlanUid(const std::string planUid);

    int InsertTemplate(RtTemplate& systemconfig);
    int UpdateTemplate(RtTemplate& systemconfig);

    int GetConstraintListByPrescriptionUidList(std::vector<std::string> vPrescriptionUidList, std::vector<RtConstraint*>& vRtConstraintList);

    int UpdateItems(RtDbDef::OBJECT_TYPE objType, 
        //<attribute name, value>
        const std::map<std::string, std::string>& attrToValueMap, 
        //<filter attribute name, filter value>
        const std::map<std::string, std::vector<std::string>>& filtersMap) const;
private:

    /////////////////////////////////////////////////////////////////
    ///  \brief                  Get all ToBeDeleted record list
    ///  \param[in]          None
    ///  \param[out]        std::vector<IToBeDeletedPtr>& pTBDPtr
    ///  \return                int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- RtDatabaseWrapper has not been initialized
    ///         DB_CODES_INPUT_VIOLATION--empty or contains invalid characters
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool GetConfigFile (Mcsf::IFileParser* const pFileParser);


    /////////////////////////////////////////////////////////////////
    ///  \brief             Check SQL query in order to prevent SQL injection
    ///     NOTE:
    ///     sSQLQuery should NOT include DROP, DELETE, UPDATE, EXECUTE.
    ///
    ///  \param[in]     const std::string& sSQLQuery
    ///
    ///  \param[out]    None
    ///  \return            bool
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    /// \fn bool CheckSQLQuery(const std::string& sSQLQuery)
    /// <key> \n
    /// Name:bool CheckSQLQuery(const std::string& sSQLQuery) \n
    /// PRA:Yes \n
    /// Traced from:SSFS_PRA_DB_UnwantedInput_UT&CodeReview \n
    /// Description:Check the SQL sentence \n
    /// Short Description:CheckSQLQuery \n
    /// Component:DB \n
    /// </key> \n
    /// 
    bool CheckSQLQuery(const std::string& sSQLQuery) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief        Check UID in order to insert invalid UID to DB
    ///                 UID should be "0"~"9" and ".", 
    ///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
    ///
    ///  \param[in]     const std::string& sUID
    ///
    ///  \param[out]    None
    ///  \return            bool
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool CheckUID(const std::string& sUID) const;

    //int BuildCt2densityMapFromFile(RtCt2density* ct2densityMap);
    int BuildDosegridFromFile(RtDosegrid* pDosegrid);
    int BuildLeafPositionsFromFile(RtMlcshape* pMlcshape);

    //void SetupCt2DensityMap_i(const std::string& strLongString, RtCt2density* ct2densityMap);
    //float GetDensitybyCTval_i(int ct, const std::vector<int>& vCT, std::map<int, float>& ct2densMap);

    int FillInContoursIntoVoi_i(RtVoi* pRtVoi);

    int ReadOutLeafBoundaryFromFile(RtMachine* pMachine);
    int ReadOutDoserateFromFile(RtCommissionedunit* pUnit);
    int ReadOutTrayfactorFromFile(RtCommissionedunit* pUnit);
    int SaveDosegridBufferToFile(RtDosegrid* pDose);
    void DeleteFileByPath(const std::string sPath) const;

    int GetSeriesInformation(RtSeries* pSeries);
    int GetAccessorychunkListByCommissionedunitUid(std::string commissionUid, std::vector<RtAccessorychunk*>& vRtAccessorychunkList);
    int SaveDataFromXmltoDB(const std::string& xmlPath, const std::string& tableName) const;

    int SaveFile(enDataType datatype, const std::string& sUid, const char* filebuffer, size_t filesize, std::string* filepath);
    int GetFile(const std::string& path, char*& filebuffer, size_t* filesize);
	RtDatabaseHelper& GetDatabaseHelper();

#pragma region [New Load]
    int GetPartialSeriesListByPatientUid(const std::string& sPatientUid, std::vector<RtSeries*>& vRtSeriesList);
    int GetPartialPlanListByCourseUidList(const std::vector<string>& vCourseUidList, std::vector<RtPlan*>& vRtPlanList);
    int GetSiteListByCourseUidList(const std::vector<std::string>& vCourseUidList, std::vector<RtSite*>& vSiteList);
    int GetPrescriptionListBySiteUidList(const std::vector<std::string>& vSiteUidList, std::vector<RtPrescription*>& vPresriptionList);
#pragma endregion [New Load]

private:
    //////////////////////////////////////////////////////////////////////////
    //user name of database
    std::string                 m_sUser;
    //password of database
    std::string                 m_sPassword;
    //database name
    std::string                 m_sDatabaseName;
    //host name
    std::string                 m_sHostName;
    //port number
    unsigned int                m_uiPort;
    bool                        m_IsLocalhost;

    //Images Folder
    std::string                 m_sImagesFolder;
    std::string                 m_sTpsVersion;

    //mysql
    MYSQL*                      m_pMySql;
    RtDatabaseUtils*            m_pDBUtils;
    RtDatabaseHelper*           m_pHelper;
    RtDatabaseClient*           m_pClient;
    RtDatabaseClientTcp*        m_pClientTcp;
    bool                        m_bIsTcp;

    // boost::unique_lock<Mutex> m_dbmutex;
    //boost::mutex* m_dbmutex;
    boost::recursive_mutex*     m_dbrecmutext;
    //Transaction begin count
    int                         m_iTransCount;
    //If roll back needed
    bool                        m_bRollBack;

    std::string                 m_sDefaultEquipmentUid;
};

RT_TPS_DATABASE_END_NAMESPACE;

#endif  //MCSF_DBWRAPPER_H
