//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface.h
///  \brief     rt tps database wrapper interface 
///
///  \version 1.0
///  \date    July 9,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_H_
#define RT_TPS_DATABASE_INTERFACE_H_

#include <string>
#include <vector>
#include <map>

#include "rt_tps_database_defs.h"
#include "rt_tps_database_common_enums.h"

///////////////////////////////
// The enum declaration for DATABASE_ERROR_CODE
///////////////////////////////
enum RT_DATABASE_DB_CODES
{
    DB_CODES_SUCCESS = 0                        ,
    DB_CODES_CONNECTION_FAILED                  ,
    DB_CODES_VERSION_NOT_MATCHED                ,
    DB_CODES_MEMORY_ALLOCATION_FAILURE          ,
    DB_CODES_UID_NOT_EXIST                      ,
    DB_CODES_UID_NOT_UNIQUE                     ,
    DB_CODES_FK_UID_NOT_EXIST                   ,
    DB_CODES_INPUT_VIOLATION                    ,
    DB_CODES_EXCEPTION                          ,
    DB_CODES_MODALITY_NOT_SUPPORTED             ,
    DB_CODES_UNKNOWN                            
};

static const char * ENUM_TO_STRING_ERROR_CODES[] = 
{
    "DB_CODES_SUCCESS"                          , 
    "DB_CODES_CONNECTION_FAILED"                ,
    "DB_CODES_VERSION_NOT_MATCHED"              , 
    "DB_CODES_MEMORY_ALLOCATION_FAILURE"        ,
    "DB_CODES_UID_NOT_EXIST"                    ,
    "DB_CODES_UID_NOT_UNIQUE"                   ,
    "DB_CODES_FK_UID_NOT_EXIST"                 ,
    "DB_CODES_INPUT_VIOLATION"                  ,
    "DB_CODES_EXCEPTION"                        ,
    "DB_CODES_MODALITY_NOT_SUPPORTED"           ,
    "DB_CODES_UNKNOWN"                          
};

#define RT_TPS_DATABASE_CONFIG_NAME                 "RtTpsDatabaseConfig.xml"
#define RT_TPS_VERSION                              "version.xml"
#define RT_TPS_CLIENT                               "RTClient.xml"
// rt_tps_database_version interface version
// version.
//
//accepted two versions sometimes if no differences for TPS
//#define RT_TPS_DATABASE_VERSION                     112
//#define RT_TPS_DATABASE_VERSION_STR                 "1.1.2"


//////////////////////////////////////////////////////////////////////////
RT_TPS_DATABASE_BEGIN_NAMESPACE;

//images
class IRtPatient;
class IRtDatabaseWrapper;
class RtPatient;
class RtStudy;
class RtSeries;
class RtImage;
class RtImage3D;

class RtImagetransformation;
class RtRtimage;
class RtVoisetting;
class RtVoi;
class RtContour;
class RtPoi;
class RtCt2density;
class RtCommissionedunit_tray;
class RtStructureset;

//planning
class RtCourse;
class RtPrescription;
class RtConstraint;
class RtPlan;
class RtNormgroup;
class RtBeam;
class RtBeamsegment;
class RtMlcshape;
class RtDosegrid;
class RtDoseline;
class RtBeam_poi;
class RtDoseoptgoalbase;
class RtBlock;
class RtDeliverybeamgroup;

//system
class RtMachine;
class RtCommissionedunit;
class RtMcphocommissionedunit;
class RtMcphomonoenergyweight;
class RtMeasuredprofile;
class RtMeasureddata;
class RtWedgefactor;
class RtKerneldata;
class RtKerneldatalist;
class RtKernelweight;
class RtTray;
class RtApplicator;
class RtTemplate;
class RtApproverecord;
class RtAccessory;
class RtAccessorychunk;
class RtGoldenstt;
class RtMonocompressedphasespacedata;
class RtTablecontour;
class RtSeriestable;
class RtDrrsetting;
class RtTcp;
class RtNtcp;
class RtSite;
class RtNorm_voi;
class RtPlan_voi;
class RtEquipment;
class RtSeriesgroup;

class RtDatabaseHelper;

//typedef boost::shared_ptr<tps::IRtDatabaseWrapper> IRtDatabasePtr;

/// \class IRtDatabaseWrapper  
/// \brief IRtDatabaseWrapper 
///
///     IRtDatabaseWrapper is not a security class, please new object in each thread.
///        All interfaces don't throw exceptions.
///
/// \par  Usage of this class:
/// \code
///  IRtDatabaseWrapper *pIDatabase;
///  if(pIDatabase->GetPatient(...))
///  {
///           ...
///  }
/// \endcode
////
class IRtDatabaseWrapper
{
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief constructor
    ///
    ///  \param[in]    None
    ///  \param[out]   None
    ///  \return       None
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    IRtDatabaseWrapper(){}
    virtual ~IRtDatabaseWrapper(){}

    /////////////////////////////////////////////////////////////////
    ///  \brief        
    ///                 As a user I want a new interface of Initialize,
    ///                 so that I can get the status about the connection to DB 
    ///                 before using the other interfaces of DBWapper. 
    ///                 This interface will use McsfFileParser to parse McsfDatabaseConfig.xml and connect to DB 
    ///                  then return the connection status.
    ///                 NOTE: Initialize should be called only once before using RtDatabaseWrapper.
    ///  \param[in]     None
    ///                 
    ///  \param[out]    None
    ///  \return    bool 
    ///                 true: successful connection to DB
    ///                 false: failed connection to DB 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual bool Initialize () = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief        
    ///         Close DB connection
    ///  \return bool
    ///         true:successful close connection
    ///         true:failed to close connection
    /////////////////////////////////////////////////////////////////
    virtual bool Finalize() = 0;

    virtual void BeginTrans() = 0;

    virtual bool Commit() = 0;

    virtual void RollBack() = 0;

    virtual void ClearAllPatientLocks() = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief        CheckValidateString "'"
    ///
    ///  \param[in]     const std::string& sInput
    ///
    ///  \param[out]    None
    ///  \return            bool
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual std::string CheckValidateString(const std::string& sInput) const = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief        Set connection parameters
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
    virtual bool SetConnectionParameters(
        const std::string& sUser,
        const std::string& sPassword,
        const std::string& sDatabaseName,
        const std::string& sHostName,
        const unsigned int& uiPort)=0;

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
    virtual int InsertPatient(RtPatient& patient) const = 0;

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
    virtual int InsertStudy(RtStudy& study) const = 0;

    virtual int InsertCourse(RtCourse& course) const = 0;

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
    virtual int InsertStructureset(RtStructureset& structureset) = 0;
    virtual int InsertVoi(RtVoi& voi) = 0;

    virtual int InsertPoiObject(RtPoi& poi) = 0;
    virtual int InsertBlock(RtBlock& block) = 0;

    virtual int UpdateBlock(RtBlock& block) = 0;

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
    virtual int InsertDoselineObject(RtDoseline* pDoseline) = 0;

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
    virtual int InsertPlanObject(RtPlan* pPlan) const = 0;

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
    virtual int InsertNormgroupObject(RtNormgroup* pNormgroup) const = 0;

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
    virtual int InsertBeamObject(RtBeam& beam) = 0;

    virtual int InsertDosegridObject(RtDosegrid* pDosegrid) = 0;

    virtual int InsertImagetransformationObject(RtImagetransformation& Imagetransformation) const = 0;
    virtual int InsertPoiDose(RtBeam_poi& beam_poi) const = 0;
    virtual int InsertNormgroupVoiDose(RtNorm_voi& norm_voi) const = 0;
    virtual int InsertPlanVoiDose(RtPlan_voi& plan_voi) const = 0;
    virtual int InsertCt2densityObject(RtCt2density& ct2density) const = 0;
    virtual int InsertDoseoptgoalbaseObject(RtDoseoptgoalbase& doseoptgoalbase) const = 0;

    virtual int InsertRtImageObject(RtRtimage& rtimage) = 0;

    virtual int InsertDrrsetting(RtDrrsetting& drrSetting) const = 0;

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
    virtual int InsertBeamsegmentObject(RtBeamsegment& beamsegment) = 0;

    virtual int InsertSeriesObject(RtSeries& series) = 0;

    virtual int InsertImageObject(RtImage& image) = 0;

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
    virtual int EraseObject(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const = 0;
    virtual int EraseObject(const std::string& sFirstUid, const std::string& sSecondUid,
        RtDbDef::OBJECT_TYPE objType) const = 0;

    // sCondition is where for sql
    virtual int EraseObjectUnderCondition(const std::string& sCondition, RtDbDef::OBJECT_TYPE objType) const = 0;

    //return MD5 value
    virtual std::string CalcHash(const void* pObject, RtDbDef::OBJECT_TYPE objType) const = 0;

    virtual bool QueryDBInTable(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const = 0;
    virtual bool QueryDBInTableViaSpecifiedKey(const std::string& sKey, const std::string& sValue, 
        RtDbDef::OBJECT_TYPE objType) const = 0;
    virtual bool QueryDBInTable(const std::string& sFirstUid, const std::string& sSecondUid, 
        RtDbDef::OBJECT_TYPE objType) const = 0;

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
    ///         DB_CODES_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual int UpdatePatientObject(RtPatient* pPatient) const = 0;

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
    virtual int UpdatePoiObject(RtPoi* pPoi) const = 0;

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
    virtual int UpdateDosegridObject(RtDosegrid* pDosegrid) = 0;

    // update voi include contours
    virtual int UpdateVoiObject(RtVoi& voi) = 0;
    // update voi attributes exclude contours
    virtual int UpdateVoiAttributes(RtVoi& voi) const = 0;
    virtual int UpdateContourObject(RtContour& contour) const = 0;

    virtual int UpdateMlcshapeObject(RtMlcshape& mlcshape) const = 0;
    virtual int UpdateCt2densityObject(RtCt2density& ct2density) const = 0;
    virtual int UpdateDoseoptgoalbaseObject(RtDoseoptgoalbase& doseoptgoalbase) const = 0;
    
    virtual int UpdateDrrsetting(RtDrrsetting& drrSetting) const = 0;

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
    ///         DB_CODES_UID_NOT_EXIST-- object not persistent
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    //virtual int UpdatePoiDoses(const std::string sPoiUid, std::string vBeamUid) const = 0;

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
    virtual int GetPatientByUid(const std::string& sPatientUid, RtPatient*& pPatient) = 0;

    virtual int GetDefaultEquipment(RtEquipment*& pEquipment) = 0;
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
    virtual int GetAllPatientObjects(std::vector<RtPatient*>& vRtPatientList) = 0;


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
    virtual int GetAllCommissionedunitObjects(std::vector<RtCommissionedunit*>& vRtCommissionedunitList) = 0;
    virtual int GetAllMachines(std::vector<RtMachine*>& vRtMachineList) = 0;
    virtual int GetAllKerneldata(std::vector<RtKerneldata*>& vKerneldata) = 0;
    virtual int GetAllKerneldatalists(std::vector<RtKerneldatalist*>& vKerneldatalist) = 0;
    virtual int GetAllKernelWeights(std::vector<RtKernelweight*>& vKernelweight) = 0;
    virtual int GetAllTrays(std::vector<RtTray*>& vTray) = 0;
    virtual int GetAllAccessorys(std::vector<RtAccessory*>& vAccessory) = 0;
    virtual int GetAllApplicators(std::vector<RtApplicator*>& vApplicator) = 0;
    virtual int GetAllSeriesgroups(std::vector<RtSeriesgroup*>& vSeriesgroup) = 0;
    virtual int GetAllMcphocommissionedunits(std::vector<RtMcphocommissionedunit*>& vMcphocommissionedunit) = 0;
    virtual int GetAllMeasuredprofile(std::vector<RtMeasuredprofile*>& vMeasuredprofile) = 0;
    virtual int GetAllCommissionedunit_trays(std::vector<RtCommissionedunit_tray*>& vCommissionedunit_tray) = 0;
    virtual int GetAllAccessorychunk(std::vector<RtAccessorychunk*>& vAccessorychunk) = 0;
    virtual int GetAllWedgefactors(std::vector<RtWedgefactor*>& vWedgefactor) = 0;
    virtual int GetAllMeasureDatas(std::vector<RtMeasureddata*>& vMeasureData) = 0;

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
    virtual int GetAllCt2densityObjects(std::vector<RtCt2density*>& vRtCt2densityList) = 0;
    virtual int GetCt2densityByUid(const std::string& uid, RtCt2density*& pCt2Density) = 0;

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
	virtual int GetSeriesByUid(const std::string& seriesuid, RtSeries*& pSeries) = 0;
	virtual int GetSeriesObjectByUid(const std::string& seriesuid, RtSeries*& pSeries) = 0;
    virtual int GetStudyByUid(const std::string& studyuid, RtStudy*& pStudy) = 0;

    virtual bool BuildImage3DFromSeries(RtSeries* pRtSeries, std::vector<RtImage*> imageList, unsigned long ulSize) = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get course list of a patient
    ///  \param[in]     const std::string& sPatientUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtCourse*>& RtCourseList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual int GetCourseListByPatientUid(const std::string& sPatientUid, std::vector<RtCourse*>& RtCourseList) const = 0;

    virtual int GetSeriesList(const std::vector<std::string>& vSeriesList, std::vector<RtSeries*>& seriesList) = 0;
    virtual int GetPatientListByCondition(const std::string& sCondition, std::vector<RtPatient*>& vList) const = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get series list of a course
    ///  \param[in]     const std::string& sCoureseUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtSeries*>& RtSeriesListArray
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual int GetSeriesListByCourseUid(const std::string& sCoureseUid, std::vector<RtSeries*>& RtSeriesList) = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief             get series list of a patient
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
    virtual int GetSeriesListByPatientUid(const std::string& sPatientUid, std::vector<RtSeries*>& RtSeriesList) = 0;

    virtual int GetImagetransformationListBySeriesUid(const std::string& sSeriesUid, std::vector<RtImagetransformation*>& vList) const = 0;
    virtual int GetPoiDoseListByPoiUid(const std::string& poiUid, std::vector<RtBeam_poi*>& vList) const = 0;
    virtual int GetPoiDoseListByBeamUid(const std::string& beamUid, std::vector<RtBeam_poi*>& vList) const = 0;

    virtual int GetPoiDose(const std::string& poiUid, const std::string& beamUid, RtBeam_poi*& beam_poi) const = 0;

    virtual int GetImagetransformation(const std::string& sFixedUid, const std::string& sFloatUid, 
        RtImagetransformation*& pImageTrans) const = 0;

    virtual int GetCommissionedunit_tray(const std::string& sUnitUid, const std::string& sTrayUid, 
        RtCommissionedunit_tray*& pUnitTray) const = 0;
    virtual int GetCommissionedunit_trayListByUnitUid(const std::string& sUnitUid, std::vector<RtCommissionedunit_tray*>& vList) const = 0;

    virtual int UpdateImagetransformation(RtImagetransformation& imageTrans) const = 0;


    virtual int UpdatePoiDose(RtBeam_poi& beam_poi) const = 0;

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
    virtual int GetPlanListByCourseUid(const std::string& sCourseUid, std::vector<RtPlan*>& vRtPlanList) = 0;
    virtual int GetPlanByUid(const std::string& sPlanUid, RtPlan* & pPlan) = 0;
    
    virtual int GetSiteListByCourseUid(const std::string& sCourseUid, std::vector<RtSite*>& vSiteList) = 0;

    virtual int GetPlanList(const std::vector<std::string>& vPlanUidList, std::vector<RtPlan*>& vRtPlanList) = 0;

    virtual int GetPrescriptionListByCourseUid(const std::string& sCourseUid, std::vector<RtPrescription*>& vPresriptionList) = 0;
    virtual int GetDeliverybeamgroupListByPrescriptionUid(const std::string& prescriptionuid, std::vector<RtDeliverybeamgroup*>& vList) = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get normgroup list of a plan
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
    virtual int GetNormgroupListByPlanUid(const std::string& sPlanUid, std::vector<RtNormgroup*>& vRtNormgroupList) = 0;

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
    virtual int GetPoiListBySeriesUid(const std::string& sSeriesUid, std::vector<RtPoi*>& vRtPoiList) const = 0;

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
    virtual int GetDoselineListByPlanUid(const std::string& sPlanUid, std::vector<RtDoseline*>& vRtDoselineList) const = 0;

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
    virtual int GetBeamListByNormgroupUid(const std::string& sNormgroupUid, std::vector<RtBeam*>& vRtBeamList) = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Get beamsegment list of a beam
    ///  \param[in]     const std::string& sBeamUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::vector<RtBeamsegment*>& vRtBeamsegmentList
    ///  \return            int
    ///         DB_CODES_SUCCESS-- success
    ///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
    ///         DB_CODES_EXCEPTION--Exception by ODB such as Versions are not compatible.
    ///         DB_CODES_UNKNOWN-- catch(...)
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual int GetBeamsegmentListByBeamUid(const std::string& sBeamUid, std::vector<RtBeamsegment*>& vRtBeamsegmentList) = 0;

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
    virtual int GetAccessoryListByBeamUid(const std::string& sBeamUid, std::vector<std::string>& vAccessoryUid) = 0;

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
    //virtual int GetMlcshapeListByBeamsegmentUid(const std::string& sBeamsegmentUid, std::vector<RtMlcshape*>& vRtMlcshapeList) = 0;

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
    virtual int GetImageListBySeriesUid(const std::string& seriesUid, std::vector<RtImage*>& RtImageList, bool bWithDcm = false) = 0;

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
    virtual int GetVoiList(const std::vector<std::string>& vStructureSetUids, std::vector<RtVoi*>& RtVoiList) = 0;
    virtual int GetVoiListBySeriesUid(const std::string& seriesuid, std::vector<RtVoi*>& RtVoiList) = 0;
    virtual int GetStructuresetListBySeriesUid(const std::string& seriesuid, std::vector<RtStructureset*>& vList) = 0;
    virtual int GetVoi(const std::string& sVoiUid, RtVoi*& pVoi) = 0;

    virtual int GetVoiSettingList(std::vector<RtVoisetting*>& vVoiSettingList) = 0;

    virtual int GetSeriesCountByPatientUid(const std::string& patientuid, int* count) = 0;
    virtual int GetVoiCountByStructuresetuid(const std::string& structuresetuid, int* count) = 0;

    virtual int GetDrrsettingListByNormgroupUid(const std::string& sNormgroupUid, std::vector<RtDrrsetting*>& vList) = 0;

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
    virtual int GetContourListByVoiUid(const std::string& sVoiUid, std::vector<RtContour*>& RtContourList) = 0;
    virtual int GetContourListByVoiList(const std::vector<RtVoi*>& VoiList, std::vector<RtContour*>& RtContourList) = 0;

    virtual int GetDoseoptgoalbaseListByNormgroupUid(const std::string& sNormgroupUid,
        std::vector<RtDoseoptgoalbase*>& vList) = 0;
    
    virtual std::string GetSavePath() = 0;
    virtual bool CreatePath(const std::string& sPath) = 0;
    
    virtual std::string GetTpsVersion() = 0;
    virtual std::string GetDefaultEquipmentUid() = 0;


    virtual bool GetDICOMImageBySeriesUid(const std::string& seriesuid, char*& pFile, size_t& length) = 0;

    virtual int GetAllPhantomSeries(std::vector<RtSeries*>& RtPhantomList) = 0;

    virtual int GetRtImageListByPlanUid(const std::string& sPlanUid, std::vector<RtRtimage*>& vList) = 0;
	virtual int GetRtImageListByBeamUid(const std::string& sBeamUid, std::vector<RtRtimage*>& vList) = 0;

    //sBeamUid = "1.2.3";
    //sConditon = "beamuid='" + sBeamUid + "'";
    virtual int GetRtImageListByCondition(const std::string& sCondition, std::vector<RtRtimage*>& vList) = 0;
    virtual int GetRtImageByUid(const std::string& sUid, RtRtimage*& pRtImage) = 0;
    virtual int GetContourByUid(const std::string& sContourUid, RtContour*& pContour) = 0;
    virtual int GetNormgroupByUid(const std::string& uid, RtNormgroup*& pNormgroup) = 0;
    virtual int GetAccessoryByUid(const std::string& uid, RtAccessory*& pAccessory) = 0;

    /////////////////////////////////////////////////////////////////
    ///  \brief         GetRelatedPatientCountByCt2DensityUid
    ///  \param[in]     const std::string& ct2DesnsityUid
    ///     NOTE:   If failed, the output will be empty.
    ///  \param[out]    std::string* relatedPatientNames
    ///  \return            int
    ///         Count of related patient count 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual int GetRelatedPatientCountByCt2DensityUid(const std::string& ct2DesnsityUid, std::vector<std::string>& relatedPatientNameList) = 0;

    virtual std::string GeneraterUid() const = 0;
    virtual int GetCourseListByPatientUidList(std::vector<std::string>& vPatientUidList, std::vector<RtCourse*>& vRtCourseList) const = 0;

    virtual bool CopyFileInBinary(const std::string& sFilePathSrc, const std::string& sFilePathDst) const = 0;

    virtual int GetSystemconfigList(std::vector<RtTemplate*>& vRtSystemconfigList) const = 0;

    virtual bool IsObjectApproved(const std::string& uid) const = 0;

    virtual int InsertApproveRecord(RtApproverecord& approveRecord) const = 0;

    virtual int GetApproveRecordList(std::vector<RtApproverecord*>& vRtApproverecordList) const = 0;

    virtual int GetMachineList(std::vector<std::string> vMachineUidList, std::vector<RtMachine*>& vRtMachineList) = 0;

    virtual int GetMachine(const std::string& machineUid, RtMachine*& machine) = 0;

    virtual int InsertContour(RtContour& contour) const = 0;

    //////////////////////////////////////////////////////////////////////////
#pragma region Commissionedunit
    virtual int GetCommissionedunitListByMachineUidList(std::vector<std::string> vMachineUidList, 
        std::vector<RtCommissionedunit*>& vRtCommissionedunitList) = 0;

    virtual int GetMcphocommissionedunitByUnituid(const std::string& commissionunituid, 
        RtMcphocommissionedunit*& mcphocommissionedunit) = 0;
    virtual int GetMeasureddataByUnituid(const std::string& commissionunituid, 
        RtMeasureddata*& measureddata) = 0;

    virtual int GetMeasuredprofileListByUnituid(const std::string& commissionunituid, 
        std::vector<RtMeasuredprofile*>& vList) = 0;
    virtual int GetKernelweightListByUnituid(const std::string& commissionunituid, 
        std::vector<RtKernelweight*>& vList) = 0;
    virtual int GetWedgefactorListByUnituid(const std::string& commissionunituid, 
        std::vector<RtWedgefactor*>& vList) = 0;

    virtual int GetApplicatorListByMachineuid(const std::string& machineuid, 
        std::vector<RtApplicator*>& vList) = 0;
    virtual int GetAccessoryListByMachineuid(const std::string& machineuid, 
        std::vector<RtAccessory*>& vList) = 0;
    virtual int GetTrayListByMachineuid(const std::string& machineuid, 
        std::vector<RtTray*>& vList) = 0;

    virtual int InsertTray(RtTray& tray) const = 0;
    virtual int UpdateTray(RtTray& tray) const = 0;

    virtual int InsertMachine(RtMachine& machine) const= 0;
    virtual int UpdateMachine(RtMachine& machine) const= 0;

    virtual int InsertCommissionedunit(RtCommissionedunit& commissionedunit) const= 0;
    virtual int UpdateCommissionedunit(RtCommissionedunit& commissionedunit) const= 0;

    virtual int InsertAccessory(RtAccessory& accessory) const= 0;
    virtual int UpdateAccessory(RtAccessory& accessory) const= 0;

    //virtual int InsertEquipment(const RtEquipment& equipment) const= 0;

    virtual int InsertAccessorychunk(RtAccessorychunk& accessorychunk) const= 0;
    virtual int UpdateAccessorychunk(RtAccessorychunk& accessorychunk) const= 0;

    virtual int InsertApplicator(RtApplicator& applicator) const= 0;
    virtual int UpdateApplicator(RtApplicator& applicator) const= 0;

    virtual int InsertCommissionedunit_tray(RtCommissionedunit_tray& commissionedunit_tray) const= 0;
    virtual int UpdateCommissionedunit_tray(RtCommissionedunit_tray& commissionedunit_tray) const= 0;

    virtual int InsertMeasuredprofile(RtMeasuredprofile& measuredprofile) const= 0;
    virtual int UpdateMeasuredprofile(RtMeasuredprofile& measuredprofile) const= 0;

    virtual int InsertMeasureddata(RtMeasureddata& measureddata) const= 0;
    virtual int UpdateMeasureddata(RtMeasureddata& measureddata) const= 0;

    virtual int InsertWedgefactor(RtWedgefactor& wedgefactor) const= 0;
    virtual int UpdateWedgefactor(RtWedgefactor& wedgefactor) const= 0;

    virtual int InsertKernelweight(RtKernelweight& kernelweight) const= 0;
    virtual int UpdateKernelweight(RtKernelweight& kernelweight) const= 0;

    virtual int InsertGoldenstt(RtGoldenstt& RtGoldenstt) const= 0;
    virtual int UpdateGoldenstt(RtGoldenstt& RtGoldenstt) const= 0;

    virtual int InsertMcphocommissionedunit(RtMcphocommissionedunit& mcphocommissionedunit) const= 0;
    virtual int UpdateMcphocommissionedunit(RtMcphocommissionedunit& mcphocommissionedunit) const= 0;

    virtual int InsertMcphomonoenergyweight(RtMcphomonoenergyweight& Mcphomonoenergyweight) const= 0;
    virtual int UpdateMcphomonoenergyweight(RtMcphomonoenergyweight& Mcphomonoenergyweight) const= 0;

    virtual int GetAccessorychunkListByAccUidAndUnitUid(
        const std::string& sAccUid, 
        const std::string& sUnitUid,
        std::vector<RtAccessorychunk*>& vList) = 0;

    virtual int GetAccessorychunkListByCommissionedunitUid(std::string commissionUid, std::vector<RtAccessorychunk*>& vRtAccessorychunkList) =0;

    virtual int GetGoldensttListByAccUidAndUnitUid(
        const std::string& sAccUid, 
        const std::string& sUnitUid,
        std::vector<RtGoldenstt*>& vList) = 0;

    virtual int GetAllGoldenstt(std::vector<RtGoldenstt*>& vGoldenstt) = 0;
    virtual int GetAllTcp(std::vector<RtTcp*>& vTcp) = 0;
    virtual int GetAllNtcp(std::vector<RtNtcp*>& vNtcp) = 0;

    virtual int GetMcphomonoenergyweightListByMcphocommissionedunituid(
        const std::string& mcphocommissionedunituid, 
        std::vector<RtMcphomonoenergyweight*>& vList) = 0;

    virtual int GetContourByAccessorychunkuid(
        const std::string& accessorychunkuid, 
        RtContour*& contour) = 0;

    virtual int GetAllMonocompressedphasespacedataList(
        std::vector<RtMonocompressedphasespacedata*>& vList) = 0;

    virtual int SaveDataFromXmltoDB(const std::string& xmlPath, const std::string& tableName) const = 0;


#pragma endregion Commissionedunit

#pragma region Table
    virtual int InsertTablecontour(RtTablecontour& tablecontour) = 0;
    virtual int GetTablecontourByUid(const std::string& uid, RtTablecontour*& pTablecontour) = 0;
    virtual int GetTablecontourListByTableUid(const std::string& tableuid, std::vector<RtTablecontour*>& vList) = 0;
    virtual int GetAllTablecontourList(std::vector<RtTablecontour*>& vList) = 0;

    virtual int InsertSeriestable(RtSeriestable& seriestable) const = 0;
    virtual int UpdateSeriestable(RtSeriestable& seriestable) const = 0;
    virtual int GetAllSeriestableList(std::vector<RtSeriestable*>& vList) = 0;
    virtual int GetSeriestable(const std::string& seriesUID, RtSeriestable*& seriesTable) = 0;

#pragma endregion Table

    virtual int InsertSeriesgroup(RtSeriesgroup& seriesgroup) const = 0;
    virtual int UpdateSeriesgroup(RtSeriesgroup& seriesgroup) const = 0;
    virtual int GetSeriesgroupByUid(const std::string& uid, RtSeriesgroup*& pSeriesgroup) = 0;

    virtual int GetAccessoryListByAccessoryUidList(std::vector<std::string> vAccessoryUidList, std::vector<RtAccessory*>& vRtAccessoryList) = 0;

    virtual int GetAccessorychunkListByCommissionedunitUidList(
        std::vector<std::string> vCommissionedunitUidList, 
        std::vector<RtAccessorychunk*>& vRtAccessorychunkList) = 0;
    virtual int GetGoldensttListByCommissionedunitUidList(
        std::vector<std::string> vCommissionedunitUidList, 
        std::vector<RtGoldenstt*>& vList) = 0;

    virtual int UpdateSeriesObject(RtSeries& series) const = 0;

    virtual int UpdatePlanObject(RtPlan& plan) const = 0;
    virtual int UpdateNormgroupObject(RtNormgroup& normgroup) const = 0;
    virtual int UpdateBeamObject(RtBeam& beam) = 0;
    //update beam only exclude segments and other objects
    virtual int UpdateBeamAttributes(RtBeam& beam) const = 0;
    virtual int UpdateBeamSegments(std::vector<RtBeamsegment*> beamSegments) const = 0;

    virtual int EraseDoselineByPlanUid(const std::string planUid) = 0;

    virtual int InsertTemplate(RtTemplate& systemconfig) = 0;
    virtual int UpdateTemplate(RtTemplate& systemconfig) = 0;

    virtual int GetConstraintListByPrescriptionUidList(std::vector<std::string> vPrescriptionUidList, 
        std::vector<RtConstraint*>& vRtConstraintList) = 0;

    virtual int GetDosegridObjects(const std::vector<std::string>& dosegridUidList, 
        std::vector<RtDosegrid*>& vRtDosegridList) = 0;

    virtual int UpdateItems(RtDbDef::OBJECT_TYPE objType, 
        //<attribute name, value>
        const std::map<std::string, std::string>& attrToValueMap, 
        //<filter attribute name, filter value>
        const std::map<std::string, std::vector<std::string>>& filtersMap) const= 0;

	virtual RtDatabaseHelper& GetDatabaseHelper() = 0;

#pragma region [New Load]
    virtual int GetPartialSeriesListByPatientUid(const std::string& sPatientUid, std::vector<RtSeries*>& vRtSeriesList) = 0;
    virtual int GetPartialPlanListByCourseUidList(const std::vector<std::string>& vCourseUidList, std::vector<RtPlan*>& vRtPlanList) = 0;
    virtual int GetSiteListByCourseUidList(const std::vector<std::string>& vCourseUidList, std::vector<RtSite*>& vSiteList) = 0;
    virtual int GetPrescriptionListBySiteUidList(const std::vector<std::string>& vSiteUidList, std::vector<RtPrescription*>& vPresriptionList) = 0;
#pragma endregion [New Load]

};

RT_TPS_DATABASE_END_NAMESPACE;

#endif  //MCSF_DATABASE_INTERFACE_H
