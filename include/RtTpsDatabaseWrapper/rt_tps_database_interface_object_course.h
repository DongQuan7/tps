﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object.h
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 2016-9-2 16:01:15
///
///  \version 1.0
///  \date    10/13/2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_COURSE_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_COURSE_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum COURSE_FIELD
{
    COURSE_UID = 0,
    COURSE_PATIENTUID,
    COURSE_COURSENAME,
    COURSE_CREATIONDATETIME,
    COURSE_SCOPEINDICATOR,
    COURSE_PRESCRIPTIONPRESENCEFLAG,
    COURSE_RADIATIONSETPRESENCEFLAG,
    COURSE_ISDEFAULT,
    COURSE_UPDATETIME,
    COURSE_FIELD_MAX
};

static const char* ENUM2STRING_COURSE_FIELD[] =
{
    "UID",
    "PATIENTUID",
    "COURSENAME",
    "CREATIONDATETIME",
    "SCOPEINDICATOR",
    "PRESCRIPTIONPRESENCEFLAG",
    "RADIATIONSETPRESENCEFLAG",
    "ISDEFAULT",
    "UPDATETIME",
    "COURSE_FIELD_MAX"
};

static const char* ENUM2STRING_COURSE_FIELD_NULL = "110000011";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtCourseImp;

class RT_DB_EXPORT RtCourse : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtCourse(bool bGeneraterUid = false);

    ~RtCourse();

    RtCourse(const RtCourse& course);

    RtCourse& operator = (const RtCourse& course);

    /*uid varchar
      PK*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*patientuid varchar
      (0061,0001),Patient UID ,Defined by MCSF .UI, The primary key*/
    std::string get_patientuid() const;
    void set_patientuid(const std::string& patientuid);

    /*coursename varchar
      course name*/
    std::string get_coursename() const;
    void set_coursename(const std::string& coursename);

    /*creationdatetime date
      (30xx,0800) The date and time when this RT Course definition was created. */
    DATE_BOOST get_creationdatetime() const;
    void set_creationdatetime(const DATE_BOOST& creationdatetime);

    /*scopeindicator int
      (30xx,0804) Definition of the scope of this RT Course. Enumerated Values: PARTIAL = RT Course SOP Instance contains a subset of all data related to the  treatment course at the time it was created. GLOBAL = RT Course SOP Instance  intended to contain all data pertinent to the course at the time it was created. See  C.8.A.A3.1.1. */
    int get_scopeindicator() const;
    void set_scopeindicator(const int& scopeindicator);

    /*prescriptionpresenceflag tinyint
      (30xx,0805) Defines whether an RT Prescription Reference is present. Enumerated Values: YES NO */
    bool get_prescriptionpresenceflag() const;
    void set_prescriptionpresenceflag(const bool& prescriptionpresenceflag);

    /*radiationsetpresenceflag tinyint
      (30xx,0807) Defines whether a RT Radiation Set Reference is present. Enumerated Values: YES NO */
    bool get_radiationsetpresenceflag() const;
    void set_radiationsetpresenceflag(const bool& radiationsetpresenceflag);

    /*isdefault tinyint
      是否是默认的*/
    bool get_isdefault() const;
    void set_isdefault(const bool& isdefault);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);


private:
    RtCourseImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtCourseImp
{

public:
    //default is false to improve performance
    RtCourseImp(bool bGeneraterUid = false);

    ~RtCourseImp();

    RtCourseImp(const RtCourseImp& course);

    RtCourseImp& operator = (const RtCourseImp& course);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //patientuid
    inline std::string get_patientuid() const { return m_patientuid;}
    inline void set_patientuid(const std::string& patientuid) { m_patientuid = patientuid;}

    //coursename
    inline std::string get_coursename() const { return m_coursename;}
    inline void set_coursename(const std::string& coursename) { m_coursename = coursename;}

    //creationdatetime
    inline DATE_BOOST get_creationdatetime() const { return m_creationdatetime;}
    inline void set_creationdatetime(const DATE_BOOST& creationdatetime) { m_creationdatetime = creationdatetime;}

    //scopeindicator
    inline int get_scopeindicator() const { return m_scopeindicator;}
    inline void set_scopeindicator(const int& scopeindicator) { m_scopeindicator = scopeindicator;}

    //prescriptionpresenceflag
    inline bool get_prescriptionpresenceflag() const { return m_prescriptionpresenceflag;}
    inline void set_prescriptionpresenceflag(const bool& prescriptionpresenceflag) { m_prescriptionpresenceflag = prescriptionpresenceflag;}

    //radiationsetpresenceflag
    inline bool get_radiationsetpresenceflag() const { return m_radiationsetpresenceflag;}
    inline void set_radiationsetpresenceflag(const bool& radiationsetpresenceflag) { m_radiationsetpresenceflag = radiationsetpresenceflag;}

    //isdefault
    inline bool get_isdefault() const { return m_isdefault;}
    inline void set_isdefault(const bool& isdefault) { m_isdefault = isdefault;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

private:    //9 parameters
    std::string						m_uid;
    std::string						m_patientuid;
    std::string						m_coursename;
    DATE_BOOST						m_creationdatetime;
    int								m_scopeindicator;
    bool							m_prescriptionpresenceflag;
    bool							m_radiationsetpresenceflag;
    bool							m_isdefault;
    DATETIME_BOOST					m_updatetime;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif