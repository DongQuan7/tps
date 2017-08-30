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

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_VOISETTING_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_VOISETTING_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum VOISETTING_FIELD
{
    VOISETTING_UID = 0,
    VOISETTING_VOINAME,
    VOISETTING_ROLENAME,
    VOISETTING_DESCRIPTION,
    VOISETTING_CHNDESCRIPTION,
    VOISETTING_PLANNINGROLE,
    VOISETTING_BIOLOGYROLE,
    VOISETTING_PAIRED,
    VOISETTING_CALCULATEDOSE,
    VOISETTING_MATFLAG,
    VOISETTING_MAT,
    VOISETTING_COLOREFFECT,
    VOISETTING_SPO,
    VOISETTING_ISDEFAULT,
    VOISETTING_UPDATETIME,
    VOISETTING_SITE,
    VOISETTING_FIELD_MAX
};

static const char* ENUM2STRING_VOISETTING_FIELD[] =
{
    "UID",
    "VOINAME",
    "ROLENAME",
    "DESCRIPTION",
    "CHNDESCRIPTION",
    "PLANNINGROLE",
    "BIOLOGYROLE",
    "PAIRED",
    "CALCULATEDOSE",
    "MATFLAG",
    "MAT",
    "COLOREFFECT",
    "SPO",
    "ISDEFAULT",
    "UPDATETIME",
    "SITE",
    "VOISETTING_FIELD_MAX"
};

static const char* ENUM2STRING_VOISETTING_FIELD_NULL = "1000000000000010";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtVoisettingImp;

class RT_DB_EXPORT RtVoisetting : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtVoisetting(bool bGeneraterUid = false);

    ~RtVoisetting();

    RtVoisetting(const RtVoisetting& voisetting);

    RtVoisetting& operator = (const RtVoisetting& voisetting);

    /*uid varchar
      每条VOI的uid*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*voiname varchar
      VOI名称*/
    std::string get_voiname() const;
    void set_voiname(const std::string& voiname);

    /*rolename varchar
      角色名称*/
    std::string get_rolename() const;
    void set_rolename(const std::string& rolename);

    /*description varchar
      中文描述*/
    std::string get_description() const;
    void set_description(const std::string& description);

    /*chndescription varchar
      中文描述*/
    std::string get_chndescription() const;
    void set_chndescription(const std::string& chndescription);

    /*planningrole int
      计划角色*/
    int get_planningrole() const;
    void set_planningrole(const int& planningrole);

    /*biologyrole int
      生物角色*/
    int get_biologyrole() const;
    void set_biologyrole(const int& biologyrole);

    /*paired int
      是否为成对器官*/
    int get_paired() const;
    void set_paired(const int& paired);

    /*calculatedose tinyint
      是否自动计算剂量*/
    bool get_calculatedose() const;
    void set_calculatedose(const bool& calculatedose);

    /*matflag tinyint
      是否为自定义材料密度*/
    bool get_matflag() const;
    void set_matflag(const bool& matflag);

    /*mat double
      材料密度*/
    double get_mat() const;
    void set_mat(const double& mat);

    /*coloreffect varchar
      默认颜色*/
    std::string get_coloreffect() const;
    void set_coloreffect(const std::string& coloreffect);
    void get_color(float* color);

    /*spo int
      是否为成并行器官或其他*/
    int get_spo() const;
    void set_spo(const int& spo);

    /*isdefault tinyint
      是否为默认值*/
    bool get_isdefault() const;
    void set_isdefault(const bool& isdefault);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);

    /*site varchar
      部位*/
    std::string get_site() const;
    void set_site(const std::string& site);


private:
    RtVoisettingImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtVoisettingImp
{

public:
    //default is false to improve performance
    RtVoisettingImp(bool bGeneraterUid = false);

    ~RtVoisettingImp();

    RtVoisettingImp(const RtVoisettingImp& voisetting);

    RtVoisettingImp& operator = (const RtVoisettingImp& voisetting);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //voiname
    inline std::string get_voiname() const { return m_voiname;}
    inline void set_voiname(const std::string& voiname) { m_voiname = voiname;}

    //rolename
    inline std::string get_rolename() const { return m_rolename;}
    inline void set_rolename(const std::string& rolename) { m_rolename = rolename;}

    //description
    inline std::string get_description() const { return m_description;}
    inline void set_description(const std::string& description) { m_description = description;}

    //chndescription
    inline std::string get_chndescription() const { return m_chndescription;}
    inline void set_chndescription(const std::string& chndescription) { m_chndescription = chndescription;}

    //planningrole
    inline int get_planningrole() const { return m_planningrole;}
    inline void set_planningrole(const int& planningrole) { m_planningrole = planningrole;}

    //biologyrole
    inline int get_biologyrole() const { return m_biologyrole;}
    inline void set_biologyrole(const int& biologyrole) { m_biologyrole = biologyrole;}

    //paired
    inline int get_paired() const { return m_paired;}
    inline void set_paired(const int& paired) { m_paired = paired;}

    //calculatedose
    inline bool get_calculatedose() const { return m_calculatedose;}
    inline void set_calculatedose(const bool& calculatedose) { m_calculatedose = calculatedose;}

    //matflag
    inline bool get_matflag() const { return m_matflag;}
    inline void set_matflag(const bool& matflag) { m_matflag = matflag;}

    //mat
    inline double get_mat() const { return m_mat;}
    inline void set_mat(const double& mat) { m_mat = mat;}

    //coloreffect
    inline std::string get_coloreffect() const { return m_coloreffect;}
    void set_coloreffect(const std::string& coloreffect);
    void get_color(float* color);

    //spo
    inline int get_spo() const { return m_spo;}
    inline void set_spo(const int& spo) { m_spo = spo;}

    //isdefault
    inline bool get_isdefault() const { return m_isdefault;}
    inline void set_isdefault(const bool& isdefault) { m_isdefault = isdefault;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

    //site
    inline std::string get_site() const { return m_site;}
    inline void set_site(const std::string& site) { m_site = site;}

private:    //16 parameters
    std::string						m_uid;
    std::string						m_voiname;
    std::string						m_rolename;
    std::string						m_description;
    std::string						m_chndescription;
    int								m_planningrole;
    int								m_biologyrole;
    int								m_paired;
    bool							m_calculatedose;
    bool							m_matflag;
    double							m_mat;
    std::string						m_coloreffect;
    float                           m_color[4];
    int								m_spo;
    bool							m_isdefault;
    DATETIME_BOOST					m_updatetime;
    std::string						m_site;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif