﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object.h
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 1.1.11
///
///  \version 1.0
///  \date    7/26/2017
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_APPLICATOR_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_APPLICATOR_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/date_time/posix_time/time_serialize.hpp"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum APPLICATOR_FIELD
{
    APPLICATOR_UID = 0,
    APPLICATOR_MACHINEUID,
    APPLICATOR_NAME,
    APPLICATOR_SOURCETOBOTTOMDISTANCE,
    APPLICATOR_APPLICATORID,
    APPLICATOR_APPLICATORTYPE,
    APPLICATOR_APPLICATORAPERTURESHAPE,
    APPLICATOR_APPLICATOROPENING,
    APPLICATOR_APPLICATOROPENINGX,
    APPLICATOR_APPLICATOROPENINGY,
    APPLICATOR_APPLICATORDESCRIPTION,
    APPLICATOR_MLCDIRECTIONTYPE,
    APPLICATOR_JAWXISSYMMETRY,
    APPLICATOR_JAWYISSYMMETRY,
    APPLICATOR_XUPPERJAWPOS,
    APPLICATOR_XLOWERJAWPOS,
    APPLICATOR_YUPPERJAWPOS,
    APPLICATOR_YLOWERJAWPOS,
    APPLICATOR_ACCURACY,
    APPLICATOR_LEAFPOSITIONS,
    APPLICATOR_ISSTARTMLCSHAPE,
    APPLICATOR_CODE,
    APPLICATOR_CRC,
    APPLICATOR_UPDATETIME,
    APPLICATOR_FIELD_MAX
};

static const char* ENUM2STRING_APPLICATOR_FIELD[] =
{
    "UID",
    "MACHINEUID",
    "NAME",
    "SOURCETOBOTTOMDISTANCE",
    "APPLICATORID",
    "APPLICATORTYPE",
    "APPLICATORAPERTURESHAPE",
    "APPLICATOROPENING",
    "APPLICATOROPENINGX",
    "APPLICATOROPENINGY",
    "APPLICATORDESCRIPTION",
    "MLCDIRECTIONTYPE",
    "JAWXISSYMMETRY",
    "JAWYISSYMMETRY",
    "XUPPERJAWPOS",
    "XLOWERJAWPOS",
    "YUPPERJAWPOS",
    "YLOWERJAWPOS",
    "ACCURACY",
    "LEAFPOSITIONS",
    "ISSTARTMLCSHAPE",
    "CODE",
    "CRC",
    "UPDATETIME",
    "APPLICATOR_FIELD_MAX"
};

static const char* ENUM2STRING_APPLICATOR_FIELD_NULL = "100000000000000000000001";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtApplicatorImp;

class RT_DB_EXPORT RtApplicator : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtApplicator(bool bGeneraterUid = false);

    ~RtApplicator();

    RtApplicator(const RtApplicator& applicator);

    RtApplicator& operator = (const RtApplicator& applicator);

    /*uid varchar
      pk*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*machineuid varchar
      fk related to machine uid */
    std::string get_machineuid() const;
    void set_machineuid(const std::string& machineuid);

    /*name varchar
      名字*/
    std::string get_name() const;
    void set_name(const std::string& name);

    /*sourcetobottomdistance float
      sourcet obottom distance*/
    float get_sourcetobottomdistance() const;
    void set_sourcetobottomdistance(const float& sourcetobottomdistance);

    /*applicatorid varchar
      Applicator ID*/
    std::string get_applicatorid() const;
    void set_applicatorid(const std::string& applicatorid);

    /*applicatortype int
      见DICOM标准11_03 p620*/
    int get_applicatortype() const;
    void set_applicatortype(const int& applicatortype);

    /*applicatorapertureshape int
      见DICOM标准11_03 p621*/
    int get_applicatorapertureshape() const;
    void set_applicatorapertureshape(const int& applicatorapertureshape);

    /*applicatoropening float
      applicatoropening*/
    float get_applicatoropening() const;
    void set_applicatoropening(const float& applicatoropening);

    /*applicatoropeningx float
      applicatoropeningx*/
    float get_applicatoropeningx() const;
    void set_applicatoropeningx(const float& applicatoropeningx);

    /*applicatoropeningy float
      applicatoropeningy*/
    float get_applicatoropeningy() const;
    void set_applicatoropeningy(const float& applicatoropeningy);

    /*applicatordescription varchar
      applicator description*/
    std::string get_applicatordescription() const;
    void set_applicatordescription(const std::string& applicatordescription);

    /*mlcdirectiontype int
      mlc运动方向*/
    int get_mlcdirectiontype() const;
    void set_mlcdirectiontype(const int& mlcdirectiontype);

    /*jawxissymmetry tinyint
      关于X-Jaw对称*/
    bool get_jawxissymmetry() const;
    void set_jawxissymmetry(const bool& jawxissymmetry);

    /*jawyissymmetry tinyint
      关于Y-Jaw对称*/
    bool get_jawyissymmetry() const;
    void set_jawyissymmetry(const bool& jawyissymmetry);

    /*xupperjawpos double
      X-Jaw上表面的位置*/
    double get_xupperjawpos() const;
    void set_xupperjawpos(const double& xupperjawpos);

    /*xlowerjawpos double
      X-Jaw下表面的位置*/
    double get_xlowerjawpos() const;
    void set_xlowerjawpos(const double& xlowerjawpos);

    /*yupperjawpos double
      Y-Jaw上表面的位置*/
    double get_yupperjawpos() const;
    void set_yupperjawpos(const double& yupperjawpos);

    /*ylowerjawpos double
      Y-Jaw下表面的位置*/
    double get_ylowerjawpos() const;
    void set_ylowerjawpos(const double& ylowerjawpos);

    /*accuracy int
      精度要求*/
    int get_accuracy() const;
    void set_accuracy(const int& accuracy);

    /*leafpositions varchar
      存放leaf位置信息*/
    //dLeafLowerPos dLeafUpperPos
    std::vector<db_Point2d> get_leafpositions() const;
    std::vector<db_Point2d> get_leafpositions();
    void set_leafpositions(const std::vector<db_Point2d>& vLeafPos);

    /*isstartmlcshape tinyint
      mlc开始的形状*/
    bool get_isstartmlcshape() const;
    void set_isstartmlcshape(const bool& isstartmlcshape);

    /*code varchar
       An identifier for the accessory intended to*/
    std::string get_code() const;
    void set_code(const std::string& code);

    /*crc varchar
      校验*/
    std::string get_crc() const;
    void set_crc(const std::string& crc);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);


private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize( Archive &ar,const unsigned int version) {
  	  version;
	  ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(RtDatabaseObject);
  	  ar & BOOST_SERIALIZATION_NVP(m_pImp);
    }
    RtApplicatorImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtApplicatorImp
{

public:
    //default is false to improve performance
    RT_DB_EXPORT RtApplicatorImp(bool bGeneraterUid = false);

    virtual ~RtApplicatorImp();

    RtApplicatorImp(const RtApplicatorImp& applicator);

    RtApplicatorImp& operator = (const RtApplicatorImp& applicator);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //machineuid
    inline std::string get_machineuid() const { return m_machineuid;}
    inline void set_machineuid(const std::string& machineuid) { m_machineuid = machineuid;}

    //name
    inline std::string get_name() const { return m_name;}
    inline void set_name(const std::string& name) { m_name = name;}

    //sourcetobottomdistance
    inline float get_sourcetobottomdistance() const { return m_sourcetobottomdistance;}
    inline void set_sourcetobottomdistance(const float& sourcetobottomdistance) { m_sourcetobottomdistance = sourcetobottomdistance;}

    //applicatorid
    inline std::string get_applicatorid() const { return m_applicatorid;}
    inline void set_applicatorid(const std::string& applicatorid) { m_applicatorid = applicatorid;}

    //applicatortype
    inline int get_applicatortype() const { return m_applicatortype;}
    inline void set_applicatortype(const int& applicatortype) { m_applicatortype = applicatortype;}

    //applicatorapertureshape
    inline int get_applicatorapertureshape() const { return m_applicatorapertureshape;}
    inline void set_applicatorapertureshape(const int& applicatorapertureshape) { m_applicatorapertureshape = applicatorapertureshape;}

    //applicatoropening
    inline float get_applicatoropening() const { return m_applicatoropening;}
    inline void set_applicatoropening(const float& applicatoropening) { m_applicatoropening = applicatoropening;}

    //applicatoropeningx
    inline float get_applicatoropeningx() const { return m_applicatoropeningx;}
    inline void set_applicatoropeningx(const float& applicatoropeningx) { m_applicatoropeningx = applicatoropeningx;}

    //applicatoropeningy
    inline float get_applicatoropeningy() const { return m_applicatoropeningy;}
    inline void set_applicatoropeningy(const float& applicatoropeningy) { m_applicatoropeningy = applicatoropeningy;}

    //applicatordescription
    inline std::string get_applicatordescription() const { return m_applicatordescription;}
    inline void set_applicatordescription(const std::string& applicatordescription) { m_applicatordescription = applicatordescription;}

    //mlcdirectiontype
    inline int get_mlcdirectiontype() const { return m_mlcdirectiontype;}
    inline void set_mlcdirectiontype(const int& mlcdirectiontype) { m_mlcdirectiontype = mlcdirectiontype;}

    //jawxissymmetry
    inline bool get_jawxissymmetry() const { return m_jawxissymmetry;}
    inline void set_jawxissymmetry(const bool& jawxissymmetry) { m_jawxissymmetry = jawxissymmetry;}

    //jawyissymmetry
    inline bool get_jawyissymmetry() const { return m_jawyissymmetry;}
    inline void set_jawyissymmetry(const bool& jawyissymmetry) { m_jawyissymmetry = jawyissymmetry;}

    //xupperjawpos
    inline double get_xupperjawpos() const { return m_xupperjawpos;}
    inline void set_xupperjawpos(const double& xupperjawpos) { m_xupperjawpos = xupperjawpos;}

    //xlowerjawpos
    inline double get_xlowerjawpos() const { return m_xlowerjawpos;}
    inline void set_xlowerjawpos(const double& xlowerjawpos) { m_xlowerjawpos = xlowerjawpos;}

    //yupperjawpos
    inline double get_yupperjawpos() const { return m_yupperjawpos;}
    inline void set_yupperjawpos(const double& yupperjawpos) { m_yupperjawpos = yupperjawpos;}

    //ylowerjawpos
    inline double get_ylowerjawpos() const { return m_ylowerjawpos;}
    inline void set_ylowerjawpos(const double& ylowerjawpos) { m_ylowerjawpos = ylowerjawpos;}

    //accuracy
    inline int get_accuracy() const { return m_accuracy;}
    inline void set_accuracy(const int& accuracy) { m_accuracy = accuracy;}

    //leafpositions
    //dLeafLowerPos dLeafUpperPos
    inline std::vector<db_Point2d> get_leafpositions() const { return m_leafpositions;}
    inline std::vector<db_Point2d> get_leafpositions() { return m_leafpositions;}
    inline void set_leafpositions(const std::vector<db_Point2d>& vLeafPos) { m_leafpositions = vLeafPos;}

    //isstartmlcshape
    inline bool get_isstartmlcshape() const { return m_isstartmlcshape;}
    inline void set_isstartmlcshape(const bool& isstartmlcshape) { m_isstartmlcshape = isstartmlcshape;}

    //code
    inline std::string get_code() const { return m_code;}
    inline void set_code(const std::string& code) { m_code = code;}

    //crc
    inline std::string get_crc() const { return m_crc;}
    inline void set_crc(const std::string& crc) { m_crc = crc;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

private:    //24 parameters
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		version;
		ar & BOOST_SERIALIZATION_NVP(m_uid);
		ar & BOOST_SERIALIZATION_NVP(m_machineuid);
		ar & BOOST_SERIALIZATION_NVP(m_name);
		ar & BOOST_SERIALIZATION_NVP(m_sourcetobottomdistance);
		ar & BOOST_SERIALIZATION_NVP(m_applicatorid);
		ar & BOOST_SERIALIZATION_NVP(m_applicatortype);
		ar & BOOST_SERIALIZATION_NVP(m_applicatorapertureshape);
		ar & BOOST_SERIALIZATION_NVP(m_applicatoropening);
		ar & BOOST_SERIALIZATION_NVP(m_applicatoropeningx);
		ar & BOOST_SERIALIZATION_NVP(m_applicatoropeningy);
		ar & BOOST_SERIALIZATION_NVP(m_applicatordescription);
		ar & BOOST_SERIALIZATION_NVP(m_mlcdirectiontype);
		ar & BOOST_SERIALIZATION_NVP(m_jawxissymmetry);
		ar & BOOST_SERIALIZATION_NVP(m_jawyissymmetry);
		ar & BOOST_SERIALIZATION_NVP(m_xupperjawpos);
		ar & BOOST_SERIALIZATION_NVP(m_xlowerjawpos);
		ar & BOOST_SERIALIZATION_NVP(m_yupperjawpos);
		ar & BOOST_SERIALIZATION_NVP(m_ylowerjawpos);
		ar & BOOST_SERIALIZATION_NVP(m_accuracy);
		ar & BOOST_SERIALIZATION_NVP(m_leafpositions);
		ar & BOOST_SERIALIZATION_NVP(m_isstartmlcshape);
		ar & BOOST_SERIALIZATION_NVP(m_code);
		ar & BOOST_SERIALIZATION_NVP(m_crc);
		ar & BOOST_SERIALIZATION_NVP(m_updatetime);
	}
    std::string						m_uid;
    std::string						m_machineuid;
    std::string						m_name;
    float							m_sourcetobottomdistance;
    std::string						m_applicatorid;
    int								m_applicatortype;
    int								m_applicatorapertureshape;
    float							m_applicatoropening;
    float							m_applicatoropeningx;
    float							m_applicatoropeningy;
    std::string						m_applicatordescription;
    int								m_mlcdirectiontype;
    bool							m_jawxissymmetry;
    bool							m_jawyissymmetry;
    double							m_xupperjawpos;
    double							m_xlowerjawpos;
    double							m_yupperjawpos;
    double							m_ylowerjawpos;
    int								m_accuracy;
    std::vector<db_Point2d>			m_leafpositions;//dLeafLowerPos dLeafUpperPos
    bool							m_isstartmlcshape;
    std::string						m_code;
    std::string						m_crc;
    DATETIME_BOOST					m_updatetime;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif