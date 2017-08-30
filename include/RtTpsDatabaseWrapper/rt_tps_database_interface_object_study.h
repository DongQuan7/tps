﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object.h
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 1.1.2
///
///  \version 1.0
///  \date    2/21/2017
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_STUDY_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_STUDY_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum STUDY_FIELD
{
    STUDY_UID = 0,
    STUDY_SOPINSTANCEUID,
    STUDY_PATIENTUID,
    STUDY_PATIENTAGE,
    STUDY_PATIENTSIZE,
    STUDY_PATIENTWEIGHT,
    STUDY_OCCUPATION,
    STUDY_ADDITIONALPATIENTHISTORY,
    STUDY_STUDYID,
    STUDY_STUDYDATE,
    STUDY_STUDYTIME,
    STUDY_MODALITIESINSTUDY,
    STUDY_ACCESSIONNUMBER,
    STUDY_STUDYDESCRIPTION,
    STUDY_INSTANCEAVAILABILITY,
    STUDY_ADMITTINGDIAGNOSESDESCRIPTION,
    STUDY_STUDYCOMPLETIONDATE,
    STUDY_STUDYCOMPLETIONTIME,
    STUDY_REQUESTINGPHYSICIAN,
    STUDY_REQUESTEDPROCEDUREDESCRIPTION,
    STUDY_REQUESTEDCONTRASTAGENT,
    STUDY_REFERRINGPHYSICIANNAME,
    STUDY_PREGNANCYSTATUS,
    STUDY_SMOKINGSTATUS,
    STUDY_MEDICALALERTS,
    STUDY_PATIENTINSTITUTIONRESIDENCE,
    STUDY_STUDYSTATUSID,
    STUDY_ADMISSIONID,
    STUDY_STUDYPRINTSTATUS,
    STUDY_STUDYSENDSTATUS,
    STUDY_STUDYSTOREDINDVD,
    STUDY_STUDYSTOREDINUSB,
    STUDY_IMPORTSOURCE,
    STUDY_STUDYFLAG,
    STUDY_STUDYPROTECT,
    STUDY_STUDYCONFIRMSTATUS,
    STUDY_STUDYREJECTREASON,
    STUDY_STUDYCONFIRMDATETIME,
    STUDY_STUDYIDISSUER,
    STUDY_STUDYCOMMENTS,
    STUDY_SPECIALNEEDS,
    STUDY_PERFORMEDPROCEDURESTEPUID,
    STUDY_PERFORMEDPROCEDURESTEPSTATUS,
    STUDY_PERFORMEDPROCEDURESTEPDISCONTINUATIONREASON,
    STUDY_SOURCEISOTOPENAME,
    STUDY_RADIONUCLIDETOTALDOSE,
    STUDY_RADIOPHARMACEUTICALSTARTDATETIME,
    STUDY_RADIOPHARMACEUTICAL,
    STUDY_SCHEDULEDSCANTIME,
    STUDY_SAFETYMODESAR,
    STUDY_SAFETYMODEGRADIENT,
    STUDY_STUDYACCUMULATEDDAP,
    STUDY_STUDYTYPE,
    STUDY_MODIFYFLAG,
    STUDY_MERGEFLAG,
    STUDY_SPLITFLAG,
    STUDY_REPEATFLAG,
    STUDY_PROCEDURESTEPID,
    STUDY_PROCEDURESTEPDESCRIPTION,
    STUDY_PROCEDURESTEPLOCATION,
    STUDY_PREMEDICATION,
    STUDY_PERFORMINGPHYSICIAN,
    STUDY_OPERATORSNAME,
    STUDY_STUDYIMPORTSTATUS,
    STUDY_REQUESTEDPROCEDUREID,
    STUDY_FOLDERPATH,
    STUDY_STUDYRESERVED0,
    STUDY_STUDYRESERVED1,
    STUDY_UPDATETIME,
    STUDY_FIELD_MAX
};

static const char* ENUM2STRING_STUDY_FIELD[] =
{
    "UID",
    "SOPINSTANCEUID",
    "PATIENTUID",
    "PATIENTAGE",
    "PATIENTSIZE",
    "PATIENTWEIGHT",
    "OCCUPATION",
    "ADDITIONALPATIENTHISTORY",
    "STUDYID",
    "STUDYDATE",
    "STUDYTIME",
    "MODALITIESINSTUDY",
    "ACCESSIONNUMBER",
    "STUDYDESCRIPTION",
    "INSTANCEAVAILABILITY",
    "ADMITTINGDIAGNOSESDESCRIPTION",
    "STUDYCOMPLETIONDATE",
    "STUDYCOMPLETIONTIME",
    "REQUESTINGPHYSICIAN",
    "REQUESTEDPROCEDUREDESCRIPTION",
    "REQUESTEDCONTRASTAGENT",
    "REFERRINGPHYSICIANNAME",
    "PREGNANCYSTATUS",
    "SMOKINGSTATUS",
    "MEDICALALERTS",
    "PATIENTINSTITUTIONRESIDENCE",
    "STUDYSTATUSID",
    "ADMISSIONID",
    "STUDYPRINTSTATUS",
    "STUDYSENDSTATUS",
    "STUDYSTOREDINDVD",
    "STUDYSTOREDINUSB",
    "IMPORTSOURCE",
    "STUDYFLAG",
    "STUDYPROTECT",
    "STUDYCONFIRMSTATUS",
    "STUDYREJECTREASON",
    "STUDYCONFIRMDATETIME",
    "STUDYIDISSUER",
    "STUDYCOMMENTS",
    "SPECIALNEEDS",
    "PERFORMEDPROCEDURESTEPUID",
    "PERFORMEDPROCEDURESTEPSTATUS",
    "PERFORMEDPROCEDURESTEPDISCONTINUATIONREASON",
    "SOURCEISOTOPENAME",
    "RADIONUCLIDETOTALDOSE",
    "RADIOPHARMACEUTICALSTARTDATETIME",
    "RADIOPHARMACEUTICAL",
    "SCHEDULEDSCANTIME",
    "SAFETYMODESAR",
    "SAFETYMODEGRADIENT",
    "STUDYACCUMULATEDDAP",
    "STUDYTYPE",
    "MODIFYFLAG",
    "MERGEFLAG",
    "SPLITFLAG",
    "REPEATFLAG",
    "PROCEDURESTEPID",
    "PROCEDURESTEPDESCRIPTION",
    "PROCEDURESTEPLOCATION",
    "PREMEDICATION",
    "PERFORMINGPHYSICIAN",
    "OPERATORSNAME",
    "STUDYIMPORTSTATUS",
    "REQUESTEDPROCEDUREID",
    "FOLDERPATH",
    "STUDYRESERVED0",
    "STUDYRESERVED1",
    "UPDATETIME",
    "STUDY_FIELD_MAX"
};

static const char* ENUM2STRING_STUDY_FIELD_NULL = "100000000000000000000000000000000000000000000000000000000000000000001";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtStudyImp;

class RT_DB_EXPORT RtStudy : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtStudy(bool bGeneraterUid = false);

    ~RtStudy();

    RtStudy(const RtStudy& study);

    RtStudy& operator = (const RtStudy& study);

    /*uid varchar
      (0020,000D) Study Instance UID StudyInstanceUID UI 1,The primary key*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*sopinstanceuid varchar
      Dicom检查Uid*/
    std::string get_sopinstanceuid() const;
    void set_sopinstanceuid(const std::string& sopinstanceuid);

    /*patientuid varchar
      (0061,0001),Patient UID ,Defined by MCSF .UI, The primary key*/
    std::string get_patientuid() const;
    void set_patientuid(const std::string& patientuid);

    /*patientage varchar
      (0010,1010) Patient Age PatientAge AS 1,A string of characters with one of the following formats -- nnnD, nnnW, nnnM, nnnY; where nnn shall contain the number of days for D, weeks for W, months for M, or years for Y. Example: 018M would represent an age of 18 months.*/
    std::string get_patientage() const;
    void set_patientage(const std::string& patientage);

    /*patientsize varchar
      (0010,1020) Patient Size PatientSize DS 1*/
    std::string get_patientsize() const;
    void set_patientsize(const std::string& patientsize);

    /*patientweight varchar
      (0010,1030) Patient Weight PatientWeight DS 1*/
    std::string get_patientweight() const;
    void set_patientweight(const std::string& patientweight);

    /*occupation varchar
      (0010,2180) Occupation Occupation SH 1*/
    std::string get_occupation() const;
    void set_occupation(const std::string& occupation);

    /*additionalpatienthistory varchar
      (0010,21B0) Additional Patient History AdditionalPatientHistory LT 1,Additional information about the patient medical history,10240 max length*/
    std::string get_additionalpatienthistory() const;
    void set_additionalpatienthistory(const std::string& additionalpatienthistory);

    /*studyid varchar
      (0020,0010) Study ID StudyID SH 1, user or equipment generated study identifier*/
    std::string get_studyid() const;
    void set_studyid(const std::string& studyid);

    /*studydate date
      (0008,0020) Study Date StudyDate DA 1.A string of characters of the formatYYYYMMDD where YYYY shall contain year,MM shall contain the month, and DD shallcontain the day, interpreted as a date of theGregorian calendar system.Example:19930822 would represent August 22,1993. 8 bytes fixed*/
    DATE_BOOST get_studydate() const;
    void set_studydate(const DATE_BOOST& studydate);

    /*studytime time
      (0008,0030) Study Time StudyTime TM 1.A string of characters of the formatHHMMSS.FFFFFF; where HH contains hours(range 00 - 23), MM contains minutes(range 00 - 59), SS contains seconds(range 00 - 60), and FFFFFF contains a fractional part of a second as small as 1 millionth of a second (range 000000 -999999). A 24-hour clock is used. Midnight shall be represented by only 0000 since 2400 would violate the hour range. The string may be padded with trailing spaces. Leading and embedded spaces are not allowed. 16 bytes maximum*/
    TIME_BOOST get_studytime() const;
    void set_studytime(const TIME_BOOST& studytime);

    /*modalitiesinstudy varchar
      (0008,0061) Modalities in Study ModalitiesInStudy CS 1-n n=8,eg.[PTCT]*/
    std::string get_modalitiesinstudy() const;
    void set_modalitiesinstudy(const std::string& modalitiesinstudy);

    /*accessionnumber varchar
      (0008,0050) Accession Number AccessionNumber SH 1, The accession number that the study was performed under*/
    std::string get_accessionnumber() const;
    void set_accessionnumber(const std::string& accessionnumber);

    /*studydescription varchar
      (0008,1030) Study Description StudyDescription LO 1*/
    std::string get_studydescription() const;
    void set_studydescription(const std::string& studydescription);

    /*instanceavailability varchar
      (0008,0056) Instance Availability InstanceAvailability CS 1, Instance Availability which defines how rapidly all composite object instances of this study become available for transmission after a C-MOVE retrieval request. ONLINE = 0,NEARLINE = 1,OFFLINE = 2,UNAVAILABLE = 3. A string of characters with leading or trailing spaces (20H) being non-significant.*/
    std::string get_instanceavailability() const;
    void set_instanceavailability(const std::string& instanceavailability);

    /*admittingdiagnosesdescription varchar
      (0008,1080) Admitting Diagnoses Description AdmittingDiagnosesDescription LO 1-n,n=4*/
    std::string get_admittingdiagnosesdescription() const;
    void set_admittingdiagnosesdescription(const std::string& admittingdiagnosesdescription);

    /*studycompletiondate date
      (0032,1050) Study Completion Date StudyCompletionDate DA 8 bytes fixed*/
    DATE_BOOST get_studycompletiondate() const;
    void set_studycompletiondate(const DATE_BOOST& studycompletiondate);

    /*studycompletiontime time
      (0032,1051) Study Completion Time StudyCompletionTime TM,16 bytes maximum*/
    TIME_BOOST get_studycompletiontime() const;
    void set_studycompletiontime(const TIME_BOOST& studycompletiontime);

    /*requestingphysician varchar
      (0032,1032) Requesting Physician RequestingPhysician PN 1*/
    std::string get_requestingphysician() const;
    void set_requestingphysician(const std::string& requestingphysician);

    /*requestedproceduredescription varchar
      (0032,1060) Requested Procedure Description LO 1*/
    std::string get_requestedproceduredescription() const;
    void set_requestedproceduredescription(const std::string& requestedproceduredescription);

    /*requestedcontrastagent varchar
      (0032,1070) Requested Contrast Agent RequestedContrastAgent LO 1*/
    std::string get_requestedcontrastagent() const;
    void set_requestedcontrastagent(const std::string& requestedcontrastagent);

    /*referringphysicianname varchar
      (0008,0090) Referring Physician Name ReferringPhysicianName PN 1*/
    std::string get_referringphysicianname() const;
    void set_referringphysicianname(const std::string& referringphysicianname);

    /*pregnancystatus int
      (0010,21C0) Pregnancy Status PregnancyStatus US 1 .Unsigned binary integer 6 bits long. Represents integer n in the range: 0 <= n < 2^16.*/
    int get_pregnancystatus() const;
    void set_pregnancystatus(const int& pregnancystatus);

    /*smokingstatus varchar
      (0010,21A0) Smoking Status SmokingStatus CS 1*/
    std::string get_smokingstatus() const;
    void set_smokingstatus(const std::string& smokingstatus);

    /*medicalalerts varchar
      (0010,2000) Medical Alerts MedicalAlerts LO 1-n,n=4*/
    std::string get_medicalalerts() const;
    void set_medicalalerts(const std::string& medicalalerts);

    /*patientinstitutionresidence varchar
      (0038,0400) Patient Institution Residence PatientInstitutionResidence LO 1,Primary location where patient resides ward, floor, room, etc. or outpatient*/
    std::string get_patientinstitutionresidence() const;
    void set_patientinstitutionresidence(const std::string& patientinstitutionresidence);

    /*studystatusid varchar
      (0032,000A) Study Status ID StudyStatusID CS 1 RET*/
    std::string get_studystatusid() const;
    void set_studystatusid(const std::string& studystatusid);

    /*admissionid varchar
      (0038,0010) Admission ID AdmissionID LO 1*/
    std::string get_admissionid() const;
    void set_admissionid(const std::string& admissionid);

    /*studyprintstatus varchar
      PrintStatus,0 Unprinted,1 total  printed ,2 partly printed*/
    std::string get_studyprintstatus() const;
    void set_studyprintstatus(const std::string& studyprintstatus);

    /*studysendstatus varchar
      StudySendStatus,0 unsent 1 total sent 2 partly sent*/
    std::string get_studysendstatus() const;
    void set_studysendstatus(const std::string& studysendstatus);

    /*studystoredindvd varchar
      StudyStoredInDVD,0  Un-stored In DVD,1  Stored In DVD,2 partly Stored In DVD*/
    std::string get_studystoredindvd() const;
    void set_studystoredindvd(const std::string& studystoredindvd);

    /*studystoredinusb varchar
      StudyStoredInUSB,0  Un-stored In USB,1  Stored In USB, 2 partly Stored In USB*/
    std::string get_studystoredinusb() const;
    void set_studystoredinusb(const std::string& studystoredinusb);

    /*importsource int
      ImportSource  SL:0-Localhost, 1-DVD Storage, 2-USB Storage, 3--Network*/
    int get_importsource() const;
    void set_importsource(const int& importsource);

    /*studyflag varchar
      0--Scheduled,1--Examining,2--Suspended,3--Completed*/
    std::string get_studyflag() const;
    void set_studyflag(const std::string& studyflag);

    /*studyprotect varchar
      StudyProtect*/
    std::string get_studyprotect() const;
    void set_studyprotect(const std::string& studyprotect);

    /*studyconfirmstatus int
      StudyConfirmStatus :0-Accepted, 1-Un-accepted, 2-Rejected,3-Deleted*/
    int get_studyconfirmstatus() const;
    void set_studyconfirmstatus(const int& studyconfirmstatus);

    /*studyrejectreason varchar
      StudyRejectReason*/
    std::string get_studyrejectreason() const;
    void set_studyrejectreason(const std::string& studyrejectreason);

    /*studyconfirmdatetime datetime
      StudyConfirmDatetime*/
    DATETIME_BOOST get_studyconfirmdatetime() const;
    void set_studyconfirmdatetime(const DATETIME_BOOST& studyconfirmdatetime);

    /*studyidissuer varchar
      (0032,0012) Study ID Issuer StudyIDIssuer LO 1 RET*/
    std::string get_studyidissuer() const;
    void set_studyidissuer(const std::string& studyidissuer);

    /*studycomments varchar
      (0032,4000) Study Comments StudyComments LT 1 RET*/
    std::string get_studycomments() const;
    void set_studycomments(const std::string& studycomments);

    /*specialneeds varchar
      (0038,0050) Special Needs SpecialNeeds LO 1*/
    std::string get_specialneeds() const;
    void set_specialneeds(const std::string& specialneeds);

    /*performedprocedurestepuid varchar
      PerformedProcedureStepUID, UI, 1*/
    std::string get_performedprocedurestepuid() const;
    void set_performedprocedurestepuid(const std::string& performedprocedurestepuid);

    /*performedprocedurestepstatus int
      (0040,0252) Performed Procedure Step Status PerformedProcedureStepStatus CS 1,Contains the state of the Performed Procedure Step.Enumerated Values: 0:null 1:IN  PROGRESS = Started but not complete 2:CONTINUED = Canceled or  unsuccessfully terminated 3:MPLETED = Successfully completed*/
    int get_performedprocedurestepstatus() const;
    void set_performedprocedurestepstatus(const int& performedprocedurestepstatus);

    /*performedprocedurestepdiscontinuationreason varchar
      (0040,0281) Performed Procedure StepDiscontinuation Reason Code Sequence PerformedProcedureStepDiscontinuationReasonCodeSequence SQ 1*/
    std::string get_performedprocedurestepdiscontinuationreason() const;
    void set_performedprocedurestepdiscontinuationreason(const std::string& performedprocedurestepdiscontinuationreason);

    /*sourceisotopename varchar
      (300A,0226) Source Isotope Name SourceIsotopeName LO 1*/
    std::string get_sourceisotopename() const;
    void set_sourceisotopename(const std::string& sourceisotopename);

    /*radionuclidetotaldose varchar
      (0018,1074) Radionuclide Total Dose RadionuclideTotalDose DS 1,The radiopharmaceutical dose administered to the patient measured in Becquerels (Bq) at the Radiopharmaceutical Start Time (0018,1072). Note: In other IODs, such as the NM IOD, this same attribute is specified in MegaBecquerels (MBq).*/
    std::string get_radionuclidetotaldose() const;
    void set_radionuclidetotaldose(const std::string& radionuclidetotaldose);

    /*radiopharmaceuticalstartdatetime datetime
      (0018,1078) Radiopharmaceutical Start DateTime RadiopharmaceuticalStartDateTime DT 1*/
    DATETIME_BOOST get_radiopharmaceuticalstartdatetime() const;
    void set_radiopharmaceuticalstartdatetime(const DATETIME_BOOST& radiopharmaceuticalstartdatetime);

    /*radiopharmaceutical varchar
      (0018,0031) Radiopharmaceutical Radiopharmaceutical LO 1*/
    std::string get_radiopharmaceutical() const;
    void set_radiopharmaceutical(const std::string& radiopharmaceutical);

    /*scheduledscantime datetime
      ScheduledScanTime DT 1 ScheduledScanTime*/
    DATETIME_BOOST get_scheduledscantime() const;
    void set_scheduledscantime(const DATETIME_BOOST& scheduledscantime);

    /*safetymodesar int
      0--NormalMode,1--FirstLevelMode,2--SecondLevelMode*/
    int get_safetymodesar() const;
    void set_safetymodesar(const int& safetymodesar);

    /*safetymodegradient int
      0--NormalMode,1--FirstLevelMode,2--SecondLevelMode*/
    int get_safetymodegradient() const;
    void set_safetymodegradient(const int& safetymodegradient);

    /*studyaccumulateddap varchar
      StudyAccumulatedDAP DS 1*/
    std::string get_studyaccumulateddap() const;
    void set_studyaccumulateddap(const std::string& studyaccumulateddap);

    /*studytype int
      0-Normal 1-Test Study*/
    int get_studytype() const;
    void set_studytype(const int& studytype);

    /*modifyflag int
      ModifyFlag*/
    int get_modifyflag() const;
    void set_modifyflag(const int& modifyflag);

    /*mergeflag int
      MergeFlag*/
    int get_mergeflag() const;
    void set_mergeflag(const int& mergeflag);

    /*splitflag int
      SplitFlag*/
    int get_splitflag() const;
    void set_splitflag(const int& splitflag);

    /*repeatflag int
      1=New, 2=Repeat*/
    int get_repeatflag() const;
    void set_repeatflag(const int& repeatflag);

    /*procedurestepid varchar
      (0040,0009) Scheduled Procedure Step ID ScheduledProcedureStepID SH 1*/
    std::string get_procedurestepid() const;
    void set_procedurestepid(const std::string& procedurestepid);

    /*procedurestepdescription varchar
      (0040,0007) Scheduled Procedure Step Description ScheduledProcedureStepDescription LO 1*/
    std::string get_procedurestepdescription() const;
    void set_procedurestepdescription(const std::string& procedurestepdescription);

    /*proceduresteplocation varchar
      (0040,0011) Scheduled Procedure Step Location ScheduledProcedureStepLocation SH 1*/
    std::string get_proceduresteplocation() const;
    void set_proceduresteplocation(const std::string& proceduresteplocation);

    /*premedication varchar
      (0040,0012) Pre-Medication PreMedication LO 1*/
    std::string get_premedication() const;
    void set_premedication(const std::string& premedication);

    /*performingphysician varchar
      (0008,1050) Performing Physician's Name PerformingPhysicianName PN 1-n, n=4*/
    std::string get_performingphysician() const;
    void set_performingphysician(const std::string& performingphysician);

    /*operatorsname varchar
      (0008,1070) Operators Name OperatorsName PN 1-n n=4*/
    std::string get_operatorsname() const;
    void set_operatorsname(const std::string& operatorsname);

    /*studyimportstatus int
      0 = not complete, 1 = complete*/
    int get_studyimportstatus() const;
    void set_studyimportstatus(const int& studyimportstatus);

    /*requestedprocedureid varchar
      (0040,1001) Requested Procedure ID RequestedProcedureID SH 1*/
    std::string get_requestedprocedureid() const;
    void set_requestedprocedureid(const std::string& requestedprocedureid);

    /*folderpath varchar
      FolderPath*/
    std::string get_folderpath() const;
    void set_folderpath(const std::string& folderpath);

    /*studyreserved0 varchar
      StudyReserved0*/
    std::string get_studyreserved0() const;
    void set_studyreserved0(const std::string& studyreserved0);

    /*studyreserved1 varchar
      StudyReserved1*/
    std::string get_studyreserved1() const;
    void set_studyreserved1(const std::string& studyreserved1);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);


private:
    RtStudyImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtStudyImp
{

public:
    //default is false to improve performance
    RtStudyImp(bool bGeneraterUid = false);

    ~RtStudyImp();

    RtStudyImp(const RtStudyImp& study);

    RtStudyImp& operator = (const RtStudyImp& study);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //sopinstanceuid
    inline std::string get_sopinstanceuid() const { return m_sopinstanceuid;}
    inline void set_sopinstanceuid(const std::string& sopinstanceuid) { m_sopinstanceuid = sopinstanceuid;}

    //patientuid
    inline std::string get_patientuid() const { return m_patientuid;}
    inline void set_patientuid(const std::string& patientuid) { m_patientuid = patientuid;}

    //patientage
    inline std::string get_patientage() const { return m_patientage;}
    inline void set_patientage(const std::string& patientage) { m_patientage = patientage;}

    //patientsize
    inline std::string get_patientsize() const { return m_patientsize;}
    inline void set_patientsize(const std::string& patientsize) { m_patientsize = patientsize;}

    //patientweight
    inline std::string get_patientweight() const { return m_patientweight;}
    inline void set_patientweight(const std::string& patientweight) { m_patientweight = patientweight;}

    //occupation
    inline std::string get_occupation() const { return m_occupation;}
    inline void set_occupation(const std::string& occupation) { m_occupation = occupation;}

    //additionalpatienthistory
    inline std::string get_additionalpatienthistory() const { return m_additionalpatienthistory;}
    inline void set_additionalpatienthistory(const std::string& additionalpatienthistory) { m_additionalpatienthistory = additionalpatienthistory;}

    //studyid
    inline std::string get_studyid() const { return m_studyid;}
    inline void set_studyid(const std::string& studyid) { m_studyid = studyid;}

    //studydate
    inline DATE_BOOST get_studydate() const { return m_studydate;}
    inline void set_studydate(const DATE_BOOST& studydate) { m_studydate = studydate;}

    //studytime
    inline TIME_BOOST get_studytime() const { return m_studytime;}
    inline void set_studytime(const TIME_BOOST& studytime) { m_studytime = studytime;}

    //modalitiesinstudy
    inline std::string get_modalitiesinstudy() const { return m_modalitiesinstudy;}
    inline void set_modalitiesinstudy(const std::string& modalitiesinstudy) { m_modalitiesinstudy = modalitiesinstudy;}

    //accessionnumber
    inline std::string get_accessionnumber() const { return m_accessionnumber;}
    inline void set_accessionnumber(const std::string& accessionnumber) { m_accessionnumber = accessionnumber;}

    //studydescription
    inline std::string get_studydescription() const { return m_studydescription;}
    inline void set_studydescription(const std::string& studydescription) { m_studydescription = studydescription;}

    //instanceavailability
    inline std::string get_instanceavailability() const { return m_instanceavailability;}
    inline void set_instanceavailability(const std::string& instanceavailability) { m_instanceavailability = instanceavailability;}

    //admittingdiagnosesdescription
    inline std::string get_admittingdiagnosesdescription() const { return m_admittingdiagnosesdescription;}
    inline void set_admittingdiagnosesdescription(const std::string& admittingdiagnosesdescription) { m_admittingdiagnosesdescription = admittingdiagnosesdescription;}

    //studycompletiondate
    inline DATE_BOOST get_studycompletiondate() const { return m_studycompletiondate;}
    inline void set_studycompletiondate(const DATE_BOOST& studycompletiondate) { m_studycompletiondate = studycompletiondate;}

    //studycompletiontime
    inline TIME_BOOST get_studycompletiontime() const { return m_studycompletiontime;}
    inline void set_studycompletiontime(const TIME_BOOST& studycompletiontime) { m_studycompletiontime = studycompletiontime;}

    //requestingphysician
    inline std::string get_requestingphysician() const { return m_requestingphysician;}
    inline void set_requestingphysician(const std::string& requestingphysician) { m_requestingphysician = requestingphysician;}

    //requestedproceduredescription
    inline std::string get_requestedproceduredescription() const { return m_requestedproceduredescription;}
    inline void set_requestedproceduredescription(const std::string& requestedproceduredescription) { m_requestedproceduredescription = requestedproceduredescription;}

    //requestedcontrastagent
    inline std::string get_requestedcontrastagent() const { return m_requestedcontrastagent;}
    inline void set_requestedcontrastagent(const std::string& requestedcontrastagent) { m_requestedcontrastagent = requestedcontrastagent;}

    //referringphysicianname
    inline std::string get_referringphysicianname() const { return m_referringphysicianname;}
    inline void set_referringphysicianname(const std::string& referringphysicianname) { m_referringphysicianname = referringphysicianname;}

    //pregnancystatus
    inline int get_pregnancystatus() const { return m_pregnancystatus;}
    inline void set_pregnancystatus(const int& pregnancystatus) { m_pregnancystatus = pregnancystatus;}

    //smokingstatus
    inline std::string get_smokingstatus() const { return m_smokingstatus;}
    inline void set_smokingstatus(const std::string& smokingstatus) { m_smokingstatus = smokingstatus;}

    //medicalalerts
    inline std::string get_medicalalerts() const { return m_medicalalerts;}
    inline void set_medicalalerts(const std::string& medicalalerts) { m_medicalalerts = medicalalerts;}

    //patientinstitutionresidence
    inline std::string get_patientinstitutionresidence() const { return m_patientinstitutionresidence;}
    inline void set_patientinstitutionresidence(const std::string& patientinstitutionresidence) { m_patientinstitutionresidence = patientinstitutionresidence;}

    //studystatusid
    inline std::string get_studystatusid() const { return m_studystatusid;}
    inline void set_studystatusid(const std::string& studystatusid) { m_studystatusid = studystatusid;}

    //admissionid
    inline std::string get_admissionid() const { return m_admissionid;}
    inline void set_admissionid(const std::string& admissionid) { m_admissionid = admissionid;}

    //studyprintstatus
    inline std::string get_studyprintstatus() const { return m_studyprintstatus;}
    inline void set_studyprintstatus(const std::string& studyprintstatus) { m_studyprintstatus = studyprintstatus;}

    //studysendstatus
    inline std::string get_studysendstatus() const { return m_studysendstatus;}
    inline void set_studysendstatus(const std::string& studysendstatus) { m_studysendstatus = studysendstatus;}

    //studystoredindvd
    inline std::string get_studystoredindvd() const { return m_studystoredindvd;}
    inline void set_studystoredindvd(const std::string& studystoredindvd) { m_studystoredindvd = studystoredindvd;}

    //studystoredinusb
    inline std::string get_studystoredinusb() const { return m_studystoredinusb;}
    inline void set_studystoredinusb(const std::string& studystoredinusb) { m_studystoredinusb = studystoredinusb;}

    //importsource
    inline int get_importsource() const { return m_importsource;}
    inline void set_importsource(const int& importsource) { m_importsource = importsource;}

    //studyflag
    inline std::string get_studyflag() const { return m_studyflag;}
    inline void set_studyflag(const std::string& studyflag) { m_studyflag = studyflag;}

    //studyprotect
    inline std::string get_studyprotect() const { return m_studyprotect;}
    inline void set_studyprotect(const std::string& studyprotect) { m_studyprotect = studyprotect;}

    //studyconfirmstatus
    inline int get_studyconfirmstatus() const { return m_studyconfirmstatus;}
    inline void set_studyconfirmstatus(const int& studyconfirmstatus) { m_studyconfirmstatus = studyconfirmstatus;}

    //studyrejectreason
    inline std::string get_studyrejectreason() const { return m_studyrejectreason;}
    inline void set_studyrejectreason(const std::string& studyrejectreason) { m_studyrejectreason = studyrejectreason;}

    //studyconfirmdatetime
    inline DATETIME_BOOST get_studyconfirmdatetime() const { return m_studyconfirmdatetime;}
    inline void set_studyconfirmdatetime(const DATETIME_BOOST& studyconfirmdatetime) { m_studyconfirmdatetime = studyconfirmdatetime;}

    //studyidissuer
    inline std::string get_studyidissuer() const { return m_studyidissuer;}
    inline void set_studyidissuer(const std::string& studyidissuer) { m_studyidissuer = studyidissuer;}

    //studycomments
    inline std::string get_studycomments() const { return m_studycomments;}
    inline void set_studycomments(const std::string& studycomments) { m_studycomments = studycomments;}

    //specialneeds
    inline std::string get_specialneeds() const { return m_specialneeds;}
    inline void set_specialneeds(const std::string& specialneeds) { m_specialneeds = specialneeds;}

    //performedprocedurestepuid
    inline std::string get_performedprocedurestepuid() const { return m_performedprocedurestepuid;}
    inline void set_performedprocedurestepuid(const std::string& performedprocedurestepuid) { m_performedprocedurestepuid = performedprocedurestepuid;}

    //performedprocedurestepstatus
    inline int get_performedprocedurestepstatus() const { return m_performedprocedurestepstatus;}
    inline void set_performedprocedurestepstatus(const int& performedprocedurestepstatus) { m_performedprocedurestepstatus = performedprocedurestepstatus;}

    //performedprocedurestepdiscontinuationreason
    inline std::string get_performedprocedurestepdiscontinuationreason() const { return m_performedprocedurestepdiscontinuationreason;}
    inline void set_performedprocedurestepdiscontinuationreason(const std::string& performedprocedurestepdiscontinuationreason) { m_performedprocedurestepdiscontinuationreason = performedprocedurestepdiscontinuationreason;}

    //sourceisotopename
    inline std::string get_sourceisotopename() const { return m_sourceisotopename;}
    inline void set_sourceisotopename(const std::string& sourceisotopename) { m_sourceisotopename = sourceisotopename;}

    //radionuclidetotaldose
    inline std::string get_radionuclidetotaldose() const { return m_radionuclidetotaldose;}
    inline void set_radionuclidetotaldose(const std::string& radionuclidetotaldose) { m_radionuclidetotaldose = radionuclidetotaldose;}

    //radiopharmaceuticalstartdatetime
    inline DATETIME_BOOST get_radiopharmaceuticalstartdatetime() const { return m_radiopharmaceuticalstartdatetime;}
    inline void set_radiopharmaceuticalstartdatetime(const DATETIME_BOOST& radiopharmaceuticalstartdatetime) { m_radiopharmaceuticalstartdatetime = radiopharmaceuticalstartdatetime;}

    //radiopharmaceutical
    inline std::string get_radiopharmaceutical() const { return m_radiopharmaceutical;}
    inline void set_radiopharmaceutical(const std::string& radiopharmaceutical) { m_radiopharmaceutical = radiopharmaceutical;}

    //scheduledscantime
    inline DATETIME_BOOST get_scheduledscantime() const { return m_scheduledscantime;}
    inline void set_scheduledscantime(const DATETIME_BOOST& scheduledscantime) { m_scheduledscantime = scheduledscantime;}

    //safetymodesar
    inline int get_safetymodesar() const { return m_safetymodesar;}
    inline void set_safetymodesar(const int& safetymodesar) { m_safetymodesar = safetymodesar;}

    //safetymodegradient
    inline int get_safetymodegradient() const { return m_safetymodegradient;}
    inline void set_safetymodegradient(const int& safetymodegradient) { m_safetymodegradient = safetymodegradient;}

    //studyaccumulateddap
    inline std::string get_studyaccumulateddap() const { return m_studyaccumulateddap;}
    inline void set_studyaccumulateddap(const std::string& studyaccumulateddap) { m_studyaccumulateddap = studyaccumulateddap;}

    //studytype
    inline int get_studytype() const { return m_studytype;}
    inline void set_studytype(const int& studytype) { m_studytype = studytype;}

    //modifyflag
    inline int get_modifyflag() const { return m_modifyflag;}
    inline void set_modifyflag(const int& modifyflag) { m_modifyflag = modifyflag;}

    //mergeflag
    inline int get_mergeflag() const { return m_mergeflag;}
    inline void set_mergeflag(const int& mergeflag) { m_mergeflag = mergeflag;}

    //splitflag
    inline int get_splitflag() const { return m_splitflag;}
    inline void set_splitflag(const int& splitflag) { m_splitflag = splitflag;}

    //repeatflag
    inline int get_repeatflag() const { return m_repeatflag;}
    inline void set_repeatflag(const int& repeatflag) { m_repeatflag = repeatflag;}

    //procedurestepid
    inline std::string get_procedurestepid() const { return m_procedurestepid;}
    inline void set_procedurestepid(const std::string& procedurestepid) { m_procedurestepid = procedurestepid;}

    //procedurestepdescription
    inline std::string get_procedurestepdescription() const { return m_procedurestepdescription;}
    inline void set_procedurestepdescription(const std::string& procedurestepdescription) { m_procedurestepdescription = procedurestepdescription;}

    //proceduresteplocation
    inline std::string get_proceduresteplocation() const { return m_proceduresteplocation;}
    inline void set_proceduresteplocation(const std::string& proceduresteplocation) { m_proceduresteplocation = proceduresteplocation;}

    //premedication
    inline std::string get_premedication() const { return m_premedication;}
    inline void set_premedication(const std::string& premedication) { m_premedication = premedication;}

    //performingphysician
    inline std::string get_performingphysician() const { return m_performingphysician;}
    inline void set_performingphysician(const std::string& performingphysician) { m_performingphysician = performingphysician;}

    //operatorsname
    inline std::string get_operatorsname() const { return m_operatorsname;}
    inline void set_operatorsname(const std::string& operatorsname) { m_operatorsname = operatorsname;}

    //studyimportstatus
    inline int get_studyimportstatus() const { return m_studyimportstatus;}
    inline void set_studyimportstatus(const int& studyimportstatus) { m_studyimportstatus = studyimportstatus;}

    //requestedprocedureid
    inline std::string get_requestedprocedureid() const { return m_requestedprocedureid;}
    inline void set_requestedprocedureid(const std::string& requestedprocedureid) { m_requestedprocedureid = requestedprocedureid;}

    //folderpath
    inline std::string get_folderpath() const { return m_folderpath;}
    inline void set_folderpath(const std::string& folderpath) { m_folderpath = folderpath;}

    //studyreserved0
    inline std::string get_studyreserved0() const { return m_studyreserved0;}
    inline void set_studyreserved0(const std::string& studyreserved0) { m_studyreserved0 = studyreserved0;}

    //studyreserved1
    inline std::string get_studyreserved1() const { return m_studyreserved1;}
    inline void set_studyreserved1(const std::string& studyreserved1) { m_studyreserved1 = studyreserved1;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

private:    //69 parameters
    std::string						m_uid;
    std::string						m_sopinstanceuid;
    std::string						m_patientuid;
    std::string						m_patientage;
    std::string						m_patientsize;
    std::string						m_patientweight;
    std::string						m_occupation;
    std::string						m_additionalpatienthistory;
    std::string						m_studyid;
    DATE_BOOST						m_studydate;
    TIME_BOOST						m_studytime;
    std::string						m_modalitiesinstudy;
    std::string						m_accessionnumber;
    std::string						m_studydescription;
    std::string						m_instanceavailability;
    std::string						m_admittingdiagnosesdescription;
    DATE_BOOST						m_studycompletiondate;
    TIME_BOOST						m_studycompletiontime;
    std::string						m_requestingphysician;
    std::string						m_requestedproceduredescription;
    std::string						m_requestedcontrastagent;
    std::string						m_referringphysicianname;
    int								m_pregnancystatus;
    std::string						m_smokingstatus;
    std::string						m_medicalalerts;
    std::string						m_patientinstitutionresidence;
    std::string						m_studystatusid;
    std::string						m_admissionid;
    std::string						m_studyprintstatus;
    std::string						m_studysendstatus;
    std::string						m_studystoredindvd;
    std::string						m_studystoredinusb;
    int								m_importsource;
    std::string						m_studyflag;
    std::string						m_studyprotect;
    int								m_studyconfirmstatus;
    std::string						m_studyrejectreason;
    DATETIME_BOOST					m_studyconfirmdatetime;
    std::string						m_studyidissuer;
    std::string						m_studycomments;
    std::string						m_specialneeds;
    std::string						m_performedprocedurestepuid;
    int								m_performedprocedurestepstatus;
    std::string						m_performedprocedurestepdiscontinuationreason;
    std::string						m_sourceisotopename;
    std::string						m_radionuclidetotaldose;
    DATETIME_BOOST					m_radiopharmaceuticalstartdatetime;
    std::string						m_radiopharmaceutical;
    DATETIME_BOOST					m_scheduledscantime;
    int								m_safetymodesar;
    int								m_safetymodegradient;
    std::string						m_studyaccumulateddap;
    int								m_studytype;
    int								m_modifyflag;
    int								m_mergeflag;
    int								m_splitflag;
    int								m_repeatflag;
    std::string						m_procedurestepid;
    std::string						m_procedurestepdescription;
    std::string						m_proceduresteplocation;
    std::string						m_premedication;
    std::string						m_performingphysician;
    std::string						m_operatorsname;
    int								m_studyimportstatus;
    std::string						m_requestedprocedureid;
    std::string						m_folderpath;
    std::string						m_studyreserved0;
    std::string						m_studyreserved1;
    DATETIME_BOOST					m_updatetime;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif