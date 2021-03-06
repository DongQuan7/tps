﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object.h
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 1.1.5
///
///  \version 1.0
///  \date    4/18/2017
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_NORMGROUP_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_NORMGROUP_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum NORMGROUP_FIELD
{
    NORMGROUP_UID = 0,
    NORMGROUP_PLANUID,
    NORMGROUP_NORMALPOINTPOIUID,
    NORMGROUP_TPSEQUIPMENTUID,
    NORMGROUP_MACHINEUID,
    NORMGROUP_VOIUID,
    NORMGROUP_OPTSETTINGUID,
    NORMGROUP_PRESCRIPTIONUID,
    NORMGROUP_PLANTYPE,
    NORMGROUP_NAME,
    NORMGROUP_PLANLABEL,
    NORMGROUP_PLANDATE,
    NORMGROUP_PLANTIME,
    NORMGROUP_DOSEPERFRACTION,
    NORMGROUP_FRACTION,
    NORMGROUP_PERCENTAGE,
    NORMGROUP_RTPLANGEOMETRY,
    NORMGROUP_ISJOINED,
    NORMGROUP_RADIATIONTYPE,
    NORMGROUP_ESTIMATEDTIME,
    NORMGROUP_DESCRIPTION,
    NORMGROUP_ISNORM2VOI,
    NORMGROUP_ISNORM2VOIMEANDOSE,
    NORMGROUP_NORMVOIPERCENTAGEVOLUME,
    NORMGROUP_TPSVERNO,
    NORMGROUP_TMSVERNO,
    NORMGROUP_ISGATING,
    NORMGROUP_BINMODE,
    NORMGROUP_BEAMONRANGESTART,
    NORMGROUP_BEAMONRANGEEND,
    NORMGROUP_UPDATETIME,
    NORMGROUP_OPERATORID,
    NORMGROUP_FIELD_MAX
};

static const char* ENUM2STRING_NORMGROUP_FIELD[] =
{
    "UID",
    "PLANUID",
    "NORMALPOINTPOIUID",
    "TPSEQUIPMENTUID",
    "MACHINEUID",
    "VOIUID",
    "OPTSETTINGUID",
    "PRESCRIPTIONUID",
    "PLANTYPE",
    "NAME",
    "PLANLABEL",
    "PLANDATE",
    "PLANTIME",
    "DOSEPERFRACTION",
    "FRACTION",
    "PERCENTAGE",
    "RTPLANGEOMETRY",
    "ISJOINED",
    "RADIATIONTYPE",
    "ESTIMATEDTIME",
    "DESCRIPTION",
    "ISNORM2VOI",
    "ISNORM2VOIMEANDOSE",
    "NORMVOIPERCENTAGEVOLUME",
    "TPSVERNO",
    "TMSVERNO",
    "ISGATING",
    "BINMODE",
    "BEAMONRANGESTART",
    "BEAMONRANGEEND",
    "UPDATETIME",
    "OPERATORID",
    "NORMGROUP_FIELD_MAX"
};

static const char* ENUM2STRING_NORMGROUP_FIELD_NULL = "10000000000000000000000000000010";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtNormgroupImp;
class RtDosegrid;

class RT_DB_EXPORT RtNormgroup : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtNormgroup(bool bGeneraterUid = false);

    ~RtNormgroup();

    RtNormgroup(const RtNormgroup& normgroup);

    RtNormgroup& operator = (const RtNormgroup& normgroup);

    /*uid varchar
      主键*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*planuid varchar
      关联的PlanUid*/
    std::string get_planuid() const;
    void set_planuid(const std::string& planuid);

    /*normalpointpoiuid varchar
      关联的归一点Uid*/
    std::string get_normalpointpoiuid() const;
    void set_normalpointpoiuid(const std::string& normalpointpoiuid);

    /*tpsequipmentuid varchar
      */
    std::string get_tpsequipmentuid() const;
    void set_tpsequipmentuid(const std::string& tpsequipmentuid);

    /*machineuid varchar
      related machine*/
    std::string get_machineuid() const;
    void set_machineuid(const std::string& machineuid);

    /*voiuid varchar
      归一化VOI的UID*/
    std::string get_voiuid() const;
    void set_voiuid(const std::string& voiuid);

    /*optsettinguid varchar
      */
    std::string get_optsettinguid() const;
    void set_optsettinguid(const std::string& optsettinguid);

    /*prescriptionuid varchar
      关联的处方UID*/
    std::string get_prescriptionuid() const;
    void set_prescriptionuid(const std::string& prescriptionuid);

    /*plantype int
      技术类型，建议改为Technical，再确认 cao qiuyang suggest*/
    int get_plantype() const;
    void set_plantype(const int& plantype);

    /*name varchar
      名称*/
    std::string get_name() const;
    void set_name(const std::string& name);

    /*planlabel varchar
      Lable*/
    std::string get_planlabel() const;
    void set_planlabel(const std::string& planlabel);

    /*plandate date
      日期*/
    DATE_BOOST get_plandate() const;
    void set_plandate(const DATE_BOOST& plandate);

    /*plantime time
      时间*/
    TIME_BOOST get_plantime() const;
    void set_plantime(const TIME_BOOST& plantime);

    /*doseperfraction double
      每个Fraction的dose值*/
    double get_doseperfraction() const;
    void set_doseperfraction(const double& doseperfraction);

    /*fraction int
      次数*/
    int get_fraction() const;
    void set_fraction(const int& fraction);

    /*percentage float
      对应百分比剂量线*/
    float get_percentage() const;
    void set_percentage(const float& percentage);

    /*rtplangeometry int
      Enum PATIENT, TREATMENT_DEVICE*/
    int get_rtplangeometry() const;
    void set_rtplangeometry(const int& rtplangeometry);

    /*isjoined tinyint
      是否参与运算*/
    bool get_isjoined() const;
    void set_isjoined(const bool& isjoined);

    /*radiationtype int
      放射类型*/
    int get_radiationtype() const;
    void set_radiationtype(const int& radiationtype);

    /*estimatedtime time
      estimated duration per time*/
    TIME_BOOST get_estimatedtime() const;
    void set_estimatedtime(const TIME_BOOST& estimatedtime);

    /*description varchar
      描述*/
    std::string get_description() const;
    void set_description(const std::string& description);

    /*isnorm2voi tinyint
      表明是否归一化到VOI*/
    bool get_isnorm2voi() const;
    void set_isnorm2voi(const bool& isnorm2voi);

    /*isnorm2voimeandose tinyint
      是否归一化到VOI mean dose*/
    bool get_isnorm2voimeandose() const;
    void set_isnorm2voimeandose(const bool& isnorm2voimeandose);

    /*normvoipercentagevolume float
      归一化到VOI 的百分比体积*/
    float get_normvoipercentagevolume() const;
    void set_normvoipercentagevolume(const float& normvoipercentagevolume);

    /*tpsverno int
      tps 版本号*/
    int get_tpsverno() const;
    void set_tpsverno(const int& tpsverno);

    /*tmsverno int
      tms 版本号*/
    int get_tmsverno() const;
    void set_tmsverno(const int& tmsverno);

    /*isgating tinyint
      是否门控*/
    bool get_isgating() const;
    void set_isgating(const bool& isgating);

    /*binmode int
      其中，BinModeEnum取值有两个：Phase=0和Amplitude=1*/
    int get_binmode() const;
    void set_binmode(const int& binmode);

    /*beamonrangestart double
      门控参数*/
    double get_beamonrangestart() const;
    void set_beamonrangestart(const double& beamonrangestart);

    /*beamonrangeend double
      门控参数*/
    double get_beamonrangeend() const;
    void set_beamonrangeend(const double& beamonrangeend);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);

    /*operatorid varchar
      操作者ID*/
    std::string get_operatorid() const;
    void set_operatorid(const std::string& operatorid);

    //////////////////////////////////////////////////////////////////////////
    RtDosegrid* get_dosegrid() const;

private:
    RtNormgroupImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtNormgroupImp
{

public:
    //default is false to improve performance
    RtNormgroupImp(bool bGeneraterUid = false);

    ~RtNormgroupImp();

    RtNormgroupImp(const RtNormgroupImp& normgroup);

    RtNormgroupImp& operator = (const RtNormgroupImp& normgroup);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //planuid
    inline std::string get_planuid() const { return m_planuid;}
    inline void set_planuid(const std::string& planuid) { m_planuid = planuid;}

    //normalpointpoiuid
    inline std::string get_normalpointpoiuid() const { return m_normalpointpoiuid;}
    inline void set_normalpointpoiuid(const std::string& normalpointpoiuid) { m_normalpointpoiuid = normalpointpoiuid;}

    //tpsequipmentuid
    inline std::string get_tpsequipmentuid() const { return m_tpsequipmentuid;}
    inline void set_tpsequipmentuid(const std::string& tpsequipmentuid) { m_tpsequipmentuid = tpsequipmentuid;}

    //machineuid
    inline std::string get_machineuid() const { return m_machineuid;}
    inline void set_machineuid(const std::string& machineuid) { m_machineuid = machineuid;}

    //voiuid
    inline std::string get_voiuid() const { return m_voiuid;}
    inline void set_voiuid(const std::string& voiuid) { m_voiuid = voiuid;}

    //optsettinguid
    inline std::string get_optsettinguid() const { return m_optsettinguid;}
    inline void set_optsettinguid(const std::string& optsettinguid) { m_optsettinguid = optsettinguid;}

    //prescriptionuid
    inline std::string get_prescriptionuid() const { return m_prescriptionuid;}
    inline void set_prescriptionuid(const std::string& prescriptionuid) { m_prescriptionuid = prescriptionuid;}

    //plantype
    inline int get_plantype() const { return m_plantype;}
    inline void set_plantype(const int& plantype) { m_plantype = plantype;}

    //name
    inline std::string get_name() const { return m_name;}
    inline void set_name(const std::string& name) { m_name = name;}

    //planlabel
    inline std::string get_planlabel() const { return m_planlabel;}
    inline void set_planlabel(const std::string& planlabel) { m_planlabel = planlabel;}

    //plandate
    inline DATE_BOOST get_plandate() const { return m_plandate;}
    inline void set_plandate(const DATE_BOOST& plandate) { m_plandate = plandate;}

    //plantime
    inline TIME_BOOST get_plantime() const { return m_plantime;}
    inline void set_plantime(const TIME_BOOST& plantime) { m_plantime = plantime;}

    //doseperfraction
    inline double get_doseperfraction() const { return m_doseperfraction;}
    inline void set_doseperfraction(const double& doseperfraction) { m_doseperfraction = doseperfraction;}

    //fraction
    inline int get_fraction() const { return m_fraction;}
    inline void set_fraction(const int& fraction) { m_fraction = fraction;}

    //percentage
    inline float get_percentage() const { return m_percentage;}
    inline void set_percentage(const float& percentage) { m_percentage = percentage;}

    //rtplangeometry
    inline int get_rtplangeometry() const { return m_rtplangeometry;}
    inline void set_rtplangeometry(const int& rtplangeometry) { m_rtplangeometry = rtplangeometry;}

    //isjoined
    inline bool get_isjoined() const { return m_isjoined;}
    inline void set_isjoined(const bool& isjoined) { m_isjoined = isjoined;}

    //radiationtype
    inline int get_radiationtype() const { return m_radiationtype;}
    inline void set_radiationtype(const int& radiationtype) { m_radiationtype = radiationtype;}

    //estimatedtime
    inline TIME_BOOST get_estimatedtime() const { return m_estimatedtime;}
    inline void set_estimatedtime(const TIME_BOOST& estimatedtime) { m_estimatedtime = estimatedtime;}

    //description
    inline std::string get_description() const { return m_description;}
    inline void set_description(const std::string& description) { m_description = description;}

    //isnorm2voi
    inline bool get_isnorm2voi() const { return m_isnorm2voi;}
    inline void set_isnorm2voi(const bool& isnorm2voi) { m_isnorm2voi = isnorm2voi;}

    //isnorm2voimeandose
    inline bool get_isnorm2voimeandose() const { return m_isnorm2voimeandose;}
    inline void set_isnorm2voimeandose(const bool& isnorm2voimeandose) { m_isnorm2voimeandose = isnorm2voimeandose;}

    //normvoipercentagevolume
    inline float get_normvoipercentagevolume() const { return m_normvoipercentagevolume;}
    inline void set_normvoipercentagevolume(const float& normvoipercentagevolume) { m_normvoipercentagevolume = normvoipercentagevolume;}

    //tpsverno
    inline int get_tpsverno() const { return m_tpsverno;}
    inline void set_tpsverno(const int& tpsverno) { m_tpsverno = tpsverno;}

    //tmsverno
    inline int get_tmsverno() const { return m_tmsverno;}
    inline void set_tmsverno(const int& tmsverno) { m_tmsverno = tmsverno;}

    //isgating
    inline bool get_isgating() const { return m_isgating;}
    inline void set_isgating(const bool& isgating) { m_isgating = isgating;}

    //binmode
    inline int get_binmode() const { return m_binmode;}
    inline void set_binmode(const int& binmode) { m_binmode = binmode;}

    //beamonrangestart
    inline double get_beamonrangestart() const { return m_beamonrangestart;}
    inline void set_beamonrangestart(const double& beamonrangestart) { m_beamonrangestart = beamonrangestart;}

    //beamonrangeend
    inline double get_beamonrangeend() const { return m_beamonrangeend;}
    inline void set_beamonrangeend(const double& beamonrangeend) { m_beamonrangeend = beamonrangeend;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

    //operatorid
    inline std::string get_operatorid() const { return m_operatorid;}
    inline void set_operatorid(const std::string& operatorid) { m_operatorid = operatorid;}
    //////////////////////////////////////////////////////////////////////////
    RtDosegrid* get_dosegrid() const;

private:    //32 parameters
    std::string						m_uid;
    std::string						m_planuid;
    std::string						m_normalpointpoiuid;
    std::string						m_tpsequipmentuid;
    std::string						m_machineuid;
    std::string						m_voiuid;
    std::string						m_optsettinguid;
    std::string						m_prescriptionuid;
    int								m_plantype;
    std::string						m_name;
    std::string						m_planlabel;
    DATE_BOOST						m_plandate;
    TIME_BOOST						m_plantime;
    double							m_doseperfraction;
    int								m_fraction;
    float							m_percentage;
    int								m_rtplangeometry;
    bool							m_isjoined;
    int								m_radiationtype;
    TIME_BOOST						m_estimatedtime;
    std::string						m_description;
    bool							m_isnorm2voi;
    bool							m_isnorm2voimeandose;
    float							m_normvoipercentagevolume;
    int								m_tpsverno;
    int								m_tmsverno;
    bool							m_isgating;
    int								m_binmode;
    double							m_beamonrangestart;
    double							m_beamonrangeend;
    DATETIME_BOOST					m_updatetime;
    std::string						m_operatorid;
    //////////////////////////////////////////////////////////////////////////
    RtDosegrid*                     m_dosegrid;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif