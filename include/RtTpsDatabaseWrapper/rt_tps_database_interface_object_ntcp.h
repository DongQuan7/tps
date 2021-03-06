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

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_NTCP_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_NTCP_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum NTCP_FIELD
{
    NTCP_UID = 0,
    NTCP_NAME,
    NTCP_BIOMODEL,
    NTCP_D50,
    NTCP_M,
    NTCP_N,
    NTCP_UPDATETIME,
    NTCP_FIELD_MAX
};

static const char* ENUM2STRING_NTCP_FIELD[] =
{
    "UID",
    "NAME",
    "BIOMODEL",
    "D50",
    "M",
    "N",
    "UPDATETIME",
    "NTCP_FIELD_MAX"
};

static const char* ENUM2STRING_NTCP_FIELD_NULL = "1000001";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtNtcpImp;

class RT_DB_EXPORT RtNtcp : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtNtcp(bool bGeneraterUid = false);

    ~RtNtcp();

    RtNtcp(const RtNtcp& ntcp);

    RtNtcp& operator = (const RtNtcp& ntcp);

    /*uid varchar
      每条ntcp的uid*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*name varchar
      器官名称*/
    std::string get_name() const;
    void set_name(const std::string& name);

    /*biomodel varchar
      生物模型*/
    std::string get_biomodel() const;
    void set_biomodel(const std::string& biomodel);

    /*d50 double
      计算NTCP所需的参数*/
    double get_d50() const;
    void set_d50(const double& d50);

    /*m float
      计算NTCP所需的参数*/
    float get_m() const;
    void set_m(const float& m);

    /*n float
      计算NTCP所需的参数*/
    float get_n() const;
    void set_n(const float& n);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);


private:
    RtNtcpImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtNtcpImp
{

public:
    //default is false to improve performance
    RtNtcpImp(bool bGeneraterUid = false);

    ~RtNtcpImp();

    RtNtcpImp(const RtNtcpImp& ntcp);

    RtNtcpImp& operator = (const RtNtcpImp& ntcp);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //name
    inline std::string get_name() const { return m_name;}
    inline void set_name(const std::string& name) { m_name = name;}

    //biomodel
    inline std::string get_biomodel() const { return m_biomodel;}
    inline void set_biomodel(const std::string& biomodel) { m_biomodel = biomodel;}

    //d50
    inline double get_d50() const { return m_d50;}
    inline void set_d50(const double& d50) { m_d50 = d50;}

    //m
    inline float get_m() const { return m_m;}
    inline void set_m(const float& m) { m_m = m;}

    //n
    inline float get_n() const { return m_n;}
    inline void set_n(const float& n) { m_n = n;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

private:    //7 parameters
    std::string						m_uid;
    std::string						m_name;
    std::string						m_biomodel;
    double							m_d50;
    float							m_m;
    float							m_n;
    DATETIME_BOOST					m_updatetime;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif