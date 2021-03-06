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

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_SERIESTABLE_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_SERIESTABLE_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum SERIESTABLE_FIELD
{
    SERIESTABLE_SERIESUID = 0,
    SERIESTABLE_TABLEUID,
    SERIESTABLE_OFFSETX,
    SERIESTABLE_OFFSETY,
    SERIESTABLE_OFFSETZ,
    SERIESTABLE_ANGLEX,
    SERIESTABLE_ANGLEY,
    SERIESTABLE_ANGLEZ,
    SERIESTABLE_UPDATETIME,
    SERIESTABLE_AVERAGEDENSITY,
    SERIESTABLE_ISACTIVE,
    SERIESTABLE_OVERRIDEEDENSITY,
    SERIESTABLE_FIELD_MAX
};

static const char* ENUM2STRING_SERIESTABLE_FIELD[] =
{
    "SERIESUID",
    "TABLEUID",
    "OFFSETX",
    "OFFSETY",
    "OFFSETZ",
    "ANGLEX",
    "ANGLEY",
    "ANGLEZ",
    "UPDATETIME",
    "AVERAGEDENSITY",
    "ISACTIVE",
    "OVERRIDEEDENSITY",
    "SERIESTABLE_FIELD_MAX"
};

static const char* ENUM2STRING_SERIESTABLE_FIELD_NULL = "110000001000";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtSeriestableImp;

class RT_DB_EXPORT RtSeriestable : public RtDatabaseObject
{
public:

    RtSeriestable();

    ~RtSeriestable();

    RtSeriestable(const RtSeriestable& seriestable);

    RtSeriestable& operator = (const RtSeriestable& seriestable);

    /*seriesuid varchar
      关联图像uid 主键*/
    std::string get_seriesuid() const;
    void set_seriesuid(const std::string& seriesuid);

    /*tableuid varchar
      tableuid 主键  */
    std::string get_tableuid() const;
    void set_tableuid(const std::string& tableuid);

    /*offsetx double
      table在关联图像中距离初始摆位在X方向上的偏移量*/
    double get_offsetx() const;
    void set_offsetx(const double& offsetx);

    /*offsety double
      table在关联图像中距离初始摆位在Y方向上的偏移量*/
    double get_offsety() const;
    void set_offsety(const double& offsety);

    /*offsetz double
      table在关联图像中距离初始摆位在Z方向上的偏移量*/
    double get_offsetz() const;
    void set_offsetz(const double& offsetz);

    /*anglex double
       table在关联图像中面对X轴负方向逆时针旋转的角度*/
    double get_anglex() const;
    void set_anglex(const double& anglex);

    /*angley double
      table在关联图像中面对Y轴负方向逆时针旋转的角度*/
    double get_angley() const;
    void set_angley(const double& angley);

    /*anglez double
      table在关联图像中面对Z轴负方向逆时针旋转的角度*/
    double get_anglez() const;
    void set_anglez(const double& anglez);

    /*updatetime timestamp
      数据戳用保证修改表数据时时最新表数据，有数据库自动填写
            */
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);

    /*averagedensity float
      床的平均密度 for TPS*/
    float get_averagedensity() const;
    void set_averagedensity(const float& averagedensity);

    /*isactive tinyint
      床是否是series选中的 for TPS*/
    bool get_isactive() const;
    void set_isactive(const bool& isactive);

    /*overrideedensity float
      该字段用于存储Table对电子密度的重置。*/
    float get_overrideedensity() const;
    void set_overrideedensity(const float& overrideedensity);


private:
    RtSeriestableImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtSeriestableImp
{

public:
    RtSeriestableImp();

    ~RtSeriestableImp();

    RtSeriestableImp(const RtSeriestableImp& seriestable);

    RtSeriestableImp& operator = (const RtSeriestableImp& seriestable);

    //seriesuid
    inline std::string get_seriesuid() const { return m_seriesuid;}
    inline void set_seriesuid(const std::string& seriesuid) { m_seriesuid = seriesuid;}

    //tableuid
    inline std::string get_tableuid() const { return m_tableuid;}
    inline void set_tableuid(const std::string& tableuid) { m_tableuid = tableuid;}

    //offsetx
    inline double get_offsetx() const { return m_offsetx;}
    inline void set_offsetx(const double& offsetx) { m_offsetx = offsetx;}

    //offsety
    inline double get_offsety() const { return m_offsety;}
    inline void set_offsety(const double& offsety) { m_offsety = offsety;}

    //offsetz
    inline double get_offsetz() const { return m_offsetz;}
    inline void set_offsetz(const double& offsetz) { m_offsetz = offsetz;}

    //anglex
    inline double get_anglex() const { return m_anglex;}
    inline void set_anglex(const double& anglex) { m_anglex = anglex;}

    //angley
    inline double get_angley() const { return m_angley;}
    inline void set_angley(const double& angley) { m_angley = angley;}

    //anglez
    inline double get_anglez() const { return m_anglez;}
    inline void set_anglez(const double& anglez) { m_anglez = anglez;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

    //averagedensity
    inline float get_averagedensity() const { return m_averagedensity;}
    inline void set_averagedensity(const float& averagedensity) { m_averagedensity = averagedensity;}

    //isactive
    inline bool get_isactive() const { return m_isactive;}
    inline void set_isactive(const bool& isactive) { m_isactive = isactive;}

    //overrideedensity
    inline float get_overrideedensity() const { return m_overrideedensity;}
    inline void set_overrideedensity(const float& overrideedensity) { m_overrideedensity = overrideedensity;}

private:    //12 parameters
    std::string						m_seriesuid;
    std::string						m_tableuid;
    double							m_offsetx;
    double							m_offsety;
    double							m_offsetz;
    double							m_anglex;
    double							m_angley;
    double							m_anglez;
    DATETIME_BOOST					m_updatetime;
    float							m_averagedensity;
    bool							m_isactive;
    float							m_overrideedensity;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif