﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object.h
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 1.1.0
///
///  \version 1.0
///  \date    12/29/2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_DOSEOPTGOALBASE_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_DOSEOPTGOALBASE_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum DOSEOPTGOALBASE_FIELD
{
    DOSEOPTGOALBASE_UID = 0,
    DOSEOPTGOALBASE_ID,
    DOSEOPTGOALBASE_NORMGROUPUID,
    DOSEOPTGOALBASE_VOIUID,
    DOSEOPTGOALBASE_DOSEOPTGOALTYPE,
    DOSEOPTGOALBASE_ISHARDLIMIT,
    DOSEOPTGOALBASE_VOIWEIGHT,
    DOSEOPTGOALBASE_ISJOINED,
    DOSEOPTGOALBASE_EUD,
    DOSEOPTGOALBASE_A,
    DOSEOPTGOALBASE_MAXDOSE,
    DOSEOPTGOALBASE_MINDOSE,
    DOSEOPTGOALBASE_DOSE,
    DOSEOPTGOALBASE_DVHDOSE,
    DOSEOPTGOALBASE_VOLUME,
    DOSEOPTGOALBASE_ISLOWERBOUND,
    DOSEOPTGOALBASE_EUDRESULT,
    DOSEOPTGOALBASE_UPDATETIME,
    DOSEOPTGOALBASE_FIELD_MAX
};

static const char* ENUM2STRING_DOSEOPTGOALBASE_FIELD[] =
{
    "UID",
    "ID",
    "NORMGROUPUID",
    "VOIUID",
    "DOSEOPTGOALTYPE",
    "ISHARDLIMIT",
    "VOIWEIGHT",
    "ISJOINED",
    "EUD",
    "A",
    "MAXDOSE",
    "MINDOSE",
    "DOSE",
    "DVHDOSE",
    "VOLUME",
    "ISLOWERBOUND",
    "EUDRESULT",
    "UPDATETIME",
    "DOSEOPTGOALBASE_FIELD_MAX"
};

static const char* ENUM2STRING_DOSEOPTGOALBASE_FIELD_NULL = "110000000000000001";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtDoseoptgoalbaseImp;

class RT_DB_EXPORT RtDoseoptgoalbase : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtDoseoptgoalbase(bool bGeneraterUid = false);

    ~RtDoseoptgoalbase();

    RtDoseoptgoalbase(const RtDoseoptgoalbase& doseoptgoalbase);

    RtDoseoptgoalbase& operator = (const RtDoseoptgoalbase& doseoptgoalbase);

    /*uid varchar
      pk*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*id int
      */
    int get_id() const;
    void set_id(const int& id);

    /*normgroupuid varchar
      fk related to normgroup table uid*/
    std::string get_normgroupuid() const;
    void set_normgroupuid(const std::string& normgroupuid);

    /*voiuid varchar
      目标针对器官的UID*/
    std::string get_voiuid() const;
    void set_voiuid(const std::string& voiuid);

    /*doseoptgoaltype int
      目标类型*/
    int get_doseoptgoaltype() const;
    void set_doseoptgoaltype(const int& doseoptgoaltype);

    /*ishardlimit int
      标志位，标明目标是否为硬约束*/
    int get_ishardlimit() const;
    void set_ishardlimit(const int& ishardlimit);

    /*voiweight float
      目标权重*/
    float get_voiweight() const;
    void set_voiweight(const float& voiweight);

    /*isjoined tinyint
      表示当前记录是否参与了剂量计算优化*/
    bool get_isjoined() const;
    void set_isjoined(const bool& isjoined);

    /*eud float
      针对EUD类型目标，优化后填写相应VOI和参数下的EUD值*/
    float get_eud() const;
    void set_eud(const float& eud);

    /*a float
      EUD参数*/
    float get_a() const;
    void set_a(const float& a);

    /*maxdose float
      最大剂量约束*/
    float get_maxdose() const;
    void set_maxdose(const float& maxdose);

    /*mindose float
      最小剂量约束*/
    float get_mindose() const;
    void set_mindose(const float& mindose);

    /*dose float
      single value剂量约束，建议改成singlevaluedose用以区别dvhdose*/
    float get_dose() const;
    void set_dose(const float& dose);

    /*dvhdose float
      DVH剂量约束*/
    float get_dvhdose() const;
    void set_dvhdose(const float& dvhdose);

    /*volume float
      百分比体积，0~100*/
    float get_volume() const;
    void set_volume(const float& volume);

    /*islowerbound tinyint
      /True表示大于，DVH往右拉；false表示小于，dvh往左拉*/
    bool get_islowerbound() const;
    void set_islowerbound(const bool& islowerbound);

    /*eudresult float
      字段表示当为EUD类型goal时，相应VOI的EUD值*/
    float get_eudresult() const;
    void set_eudresult(const float& eudresult);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);


private:
    RtDoseoptgoalbaseImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtDoseoptgoalbaseImp
{

public:
    //default is false to improve performance
    RtDoseoptgoalbaseImp(bool bGeneraterUid = false);

    ~RtDoseoptgoalbaseImp();

    RtDoseoptgoalbaseImp(const RtDoseoptgoalbaseImp& doseoptgoalbase);

    RtDoseoptgoalbaseImp& operator = (const RtDoseoptgoalbaseImp& doseoptgoalbase);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //id
    inline int get_id() const { return m_id;}
    inline void set_id(const int& id) { m_id = id;}

    //normgroupuid
    inline std::string get_normgroupuid() const { return m_normgroupuid;}
    inline void set_normgroupuid(const std::string& normgroupuid) { m_normgroupuid = normgroupuid;}

    //voiuid
    inline std::string get_voiuid() const { return m_voiuid;}
    inline void set_voiuid(const std::string& voiuid) { m_voiuid = voiuid;}

    //doseoptgoaltype
    inline int get_doseoptgoaltype() const { return m_doseoptgoaltype;}
    inline void set_doseoptgoaltype(const int& doseoptgoaltype) { m_doseoptgoaltype = doseoptgoaltype;}

    //ishardlimit
    inline int get_ishardlimit() const { return m_ishardlimit;}
    inline void set_ishardlimit(const int& ishardlimit) { m_ishardlimit = ishardlimit;}

    //voiweight
    inline float get_voiweight() const { return m_voiweight;}
    inline void set_voiweight(const float& voiweight) { m_voiweight = voiweight;}

    //isjoined
    inline bool get_isjoined() const { return m_isjoined;}
    inline void set_isjoined(const bool& isjoined) { m_isjoined = isjoined;}

    //eud
    inline float get_eud() const { return m_eud;}
    inline void set_eud(const float& eud) { m_eud = eud;}

    //a
    inline float get_a() const { return m_a;}
    inline void set_a(const float& a) { m_a = a;}

    //maxdose
    inline float get_maxdose() const { return m_maxdose;}
    inline void set_maxdose(const float& maxdose) { m_maxdose = maxdose;}

    //mindose
    inline float get_mindose() const { return m_mindose;}
    inline void set_mindose(const float& mindose) { m_mindose = mindose;}

    //dose
    inline float get_dose() const { return m_dose;}
    inline void set_dose(const float& dose) { m_dose = dose;}

    //dvhdose
    inline float get_dvhdose() const { return m_dvhdose;}
    inline void set_dvhdose(const float& dvhdose) { m_dvhdose = dvhdose;}

    //volume
    inline float get_volume() const { return m_volume;}
    inline void set_volume(const float& volume) { m_volume = volume;}

    //islowerbound
    inline bool get_islowerbound() const { return m_islowerbound;}
    inline void set_islowerbound(const bool& islowerbound) { m_islowerbound = islowerbound;}

    //eudresult
    inline float get_eudresult() const { return m_eudresult;}
    inline void set_eudresult(const float& eudresult) { m_eudresult = eudresult;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

private:    //18 parameters
    std::string						m_uid;
    int								m_id;
    std::string						m_normgroupuid;
    std::string						m_voiuid;
    int								m_doseoptgoaltype;
    int								m_ishardlimit;
    float							m_voiweight;
    bool							m_isjoined;
    float							m_eud;
    float							m_a;
    float							m_maxdose;
    float							m_mindose;
    float							m_dose;
    float							m_dvhdose;
    float							m_volume;
    bool							m_islowerbound;
    float							m_eudresult;
    DATETIME_BOOST					m_updatetime;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif