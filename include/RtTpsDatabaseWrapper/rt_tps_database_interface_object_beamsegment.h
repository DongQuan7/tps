﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object.h
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 1.1.12
///
///  \version 1.0
///  \date    8/15/2017
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_BEAMSEGMENT_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_BEAMSEGMENT_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum BEAMSEGMENT_FIELD
{
    BEAMSEGMENT_UID = 0,
    BEAMSEGMENT_BEAMUID,
    BEAMSEGMENT_SEQUENCENUMBER,
    BEAMSEGMENT_NAME,
    BEAMSEGMENT_MU,
    BEAMSEGMENT_WEIGHT,
    BEAMSEGMENT_STARTCOLLIMATORANGLE,
    BEAMSEGMENT_STARTGANTRYANGLE,
    BEAMSEGMENT_STARTTABLEROTATIONANGLE,
    BEAMSEGMENT_ENDCOLLIMATORANGLE,
    BEAMSEGMENT_ARCLENGTH,
    BEAMSEGMENT_ENDTABLEROTATIONANGLE,
    BEAMSEGMENT_T_BEAM_TO_PAT,
    BEAMSEGMENT_ISPRE,
    BEAMSEGMENT_ISPOST,
    BEAMSEGMENT_ISDURING,
    BEAMSEGMENT_ISDOUBLE,
    BEAMSEGMENT_GANTRYROTATIONDIRECTION,
    BEAMSEGMENT_COLLIMATORROTATIONDIRECTION,
    BEAMSEGMENT_STARTSSD,
    BEAMSEGMENT_ENDSSD,
    BEAMSEGMENT_UPDATETIME,
    BEAMSEGMENT_FIELD_MAX
};

static const char* ENUM2STRING_BEAMSEGMENT_FIELD[] =
{
    "UID",
    "BEAMUID",
    "SEQUENCENUMBER",
    "NAME",
    "MU",
    "WEIGHT",
    "STARTCOLLIMATORANGLE",
    "STARTGANTRYANGLE",
    "STARTTABLEROTATIONANGLE",
    "ENDCOLLIMATORANGLE",
    "ARCLENGTH",
    "ENDTABLEROTATIONANGLE",
    "T_BEAM_TO_PAT",
    "ISPRE",
    "ISPOST",
    "ISDURING",
    "ISDOUBLE",
    "GANTRYROTATIONDIRECTION",
    "COLLIMATORROTATIONDIRECTION",
    "STARTSSD",
    "ENDSSD",
    "UPDATETIME",
    "BEAMSEGMENT_FIELD_MAX"
};

static const char* ENUM2STRING_BEAMSEGMENT_FIELD_NULL = "1000000000000000000001";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtBeamsegmentImp;
class RtMlcshape;
class RtContour;
class RtMlcshape;

class RT_DB_EXPORT RtBeamsegment : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtBeamsegment(bool bGeneraterUid = false);

    ~RtBeamsegment();

    RtBeamsegment(const RtBeamsegment& beamsegment);

    RtBeamsegment& operator = (const RtBeamsegment& beamsegment);

    /*uid varchar
      PK*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*beamuid varchar
      Belong to whcih beam*/
    std::string get_beamuid() const;
    void set_beamuid(const std::string& beamuid);

    /*sequencenumber int
      Segment number.Begin at 0*/
    int get_sequencenumber() const;
    void set_sequencenumber(const int& sequencenumber);

    /*name varchar
      Segment name*/
    std::string get_name() const;
    void set_name(const std::string& name);

    /*mu float
      calculate by dose and Cumulative meterset weight*/
    float get_mu() const;
    void set_mu(const float& mu);

    /*weight float
      relative weight of the segment in this beam
            for RT-PH
            removed according Desiree's Advice on 20131211 but Jingjie thought it is useful on 20131217*/
    float get_weight() const;
    void set_weight(const float& weight);

    /*startcollimatorangle double
      Start Collimator Angle*/
    double get_startcollimatorangle() const;
    void set_startcollimatorangle(const double& startcollimatorangle);

    /*startgantryangle double
      Start Gantry Angle*/
    double get_startgantryangle() const;
    void set_startgantryangle(const double& startgantryangle);

    /*starttablerotationangle double
      开始床的旋转角度*/
    double get_starttablerotationangle() const;
    void set_starttablerotationangle(const double& starttablerotationangle);

    /*endcollimatorangle double
      End Collimator Angle*/
    double get_endcollimatorangle() const;
    void set_endcollimatorangle(const double& endcollimatorangle);

    /*arclength double
      segment的弧长(degree)*/
    double get_arclength() const;
    void set_arclength(const double& arclength);

    /*endtablerotationangle double
      End Pedestal Angle*/
    double get_endtablerotationangle() const;
    void set_endtablerotationangle(const double& endtablerotationangle);

    /*t_beam_to_pat varchar
      beam  coordinate to patient coordinate transformation matrix*/
    const float* get_t_beam_to_pat() const;
    void set_t_beam_to_pat(const float* t_beam_to_pat);

    /*ispre tinyint
      是否为Pre类型，不清楚是否人用*/
    bool get_ispre() const;
    void set_ispre(const bool& ispre);

    /*ispost tinyint
      是否为Post类型，不清楚是否人用*/
    bool get_ispost() const;
    void set_ispost(const bool& ispost);

    /*isduring tinyint
      是否为During类型，不清楚是否人用*/
    bool get_isduring() const;
    void set_isduring(const bool& isduring);

    /*isdouble tinyint
      是否双曝光，不清楚是否人用*/
    bool get_isdouble() const;
    void set_isdouble(const bool& isdouble);

    /*gantryrotationdirection int
      Gantry rotation direction*/
    int get_gantryrotationdirection() const;
    void set_gantryrotationdirection(const int& gantryrotationdirection);

    /*collimatorrotationdirection int
      Collimator rotation direction*/
    int get_collimatorrotationdirection() const;
    void set_collimatorrotationdirection(const int& collimatorrotationdirection);

    /*startssd float
      */
    float get_startssd() const;
    void set_startssd(const float& startssd);

    /*endssd float
      */
    float get_endssd() const;
    void set_endssd(const float& endssd);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);

    //////////////////////////////////////////////////////////////////////////
    //if null then create one. Should NOT delete the pointer outside!
    RtContour* get_beamoutline();

    //must have. Should NOT delete the pointer outside!
    RtMlcshape* get_startmlcshape() const;
    RtMlcshape* get_startmlcshape();

    //must have. Should NOT delete the pointer outside!
    RtMlcshape* get_endmlcshape() const;
    RtMlcshape* get_endmlcshape();

private:
    RtBeamsegmentImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtBeamsegmentImp
{

public:
    //default is false to improve performance
    RtBeamsegmentImp(bool bGeneraterUid = false);

    ~RtBeamsegmentImp();

    RtBeamsegmentImp(const RtBeamsegmentImp& beamsegment);

    RtBeamsegmentImp& operator = (const RtBeamsegmentImp& beamsegment);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //beamuid
    inline std::string get_beamuid() const { return m_beamuid;}
    inline void set_beamuid(const std::string& beamuid) { m_beamuid = beamuid;}

    //sequencenumber
    inline int get_sequencenumber() const { return m_sequencenumber;}
    inline void set_sequencenumber(const int& sequencenumber) { m_sequencenumber = sequencenumber;}

    //name
    inline std::string get_name() const { return m_name;}
    inline void set_name(const std::string& name) { m_name = name;}

    //mu
    inline float get_mu() const { return m_mu;}
    inline void set_mu(const float& mu) { m_mu = mu;}

    //weight
    inline float get_weight() const { return m_weight;}
    inline void set_weight(const float& weight) { m_weight = weight;}

    //startcollimatorangle
    inline double get_startcollimatorangle() const { return m_startcollimatorangle;}
    inline void set_startcollimatorangle(const double& startcollimatorangle) { m_startcollimatorangle = startcollimatorangle;}

    //startgantryangle
    inline double get_startgantryangle() const { return m_startgantryangle;}
    inline void set_startgantryangle(const double& startgantryangle) { m_startgantryangle = startgantryangle;}

    //starttablerotationangle
    inline double get_starttablerotationangle() const { return m_starttablerotationangle;}
    inline void set_starttablerotationangle(const double& starttablerotationangle) { m_starttablerotationangle = starttablerotationangle;}

    //endcollimatorangle
    inline double get_endcollimatorangle() const { return m_endcollimatorangle;}
    inline void set_endcollimatorangle(const double& endcollimatorangle) { m_endcollimatorangle = endcollimatorangle;}

    //arclength
    inline double get_arclength() const { return m_arclength;}
    inline void set_arclength(const double& arclength) { m_arclength = arclength;}

    //endtablerotationangle
    inline double get_endtablerotationangle() const { return m_endtablerotationangle;}
    inline void set_endtablerotationangle(const double& endtablerotationangle) { m_endtablerotationangle = endtablerotationangle;}

    //t_beam_to_pat
    inline const float* get_t_beam_to_pat() const { return m_t_beam_to_pat;}
    inline void set_t_beam_to_pat(const float* t_beam_to_pat) { memcpy(m_t_beam_to_pat, t_beam_to_pat, sizeof(float)*16);}

    //ispre
    inline bool get_ispre() const { return m_ispre;}
    inline void set_ispre(const bool& ispre) { m_ispre = ispre;}

    //ispost
    inline bool get_ispost() const { return m_ispost;}
    inline void set_ispost(const bool& ispost) { m_ispost = ispost;}

    //isduring
    inline bool get_isduring() const { return m_isduring;}
    inline void set_isduring(const bool& isduring) { m_isduring = isduring;}

    //isdouble
    inline bool get_isdouble() const { return m_isdouble;}
    inline void set_isdouble(const bool& isdouble) { m_isdouble = isdouble;}

    //gantryrotationdirection
    inline int get_gantryrotationdirection() const { return m_gantryrotationdirection;}
    inline void set_gantryrotationdirection(const int& gantryrotationdirection) { m_gantryrotationdirection = gantryrotationdirection;}

    //collimatorrotationdirection
    inline int get_collimatorrotationdirection() const { return m_collimatorrotationdirection;}
    inline void set_collimatorrotationdirection(const int& collimatorrotationdirection) { m_collimatorrotationdirection = collimatorrotationdirection;}

    //startssd
    inline float get_startssd() const { return m_startssd;}
    inline void set_startssd(const float& startssd) { m_startssd = startssd;}

    //endssd
    inline float get_endssd() const { return m_endssd;}
    inline void set_endssd(const float& endssd) { m_endssd = endssd;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}
    //////////////////////////////////////////////////////////////////////////
    //if null then create one. Should NOT delete the pointer outside!
    inline RtContour* get_beamoutline() { return m_beam_outline;}
    inline RtContour* get_beamoutline() const { return m_beam_outline;}

    //must have. Should NOT delete the pointer outside!
    inline RtMlcshape* get_startmlcshape() { return m_start_mlcshape;}
    inline RtMlcshape* get_startmlcshape() const { return m_start_mlcshape;}

    //must have. Should NOT delete the pointer outside!
    inline RtMlcshape* get_endmlcshape() { return m_end_mlcshape;}
    inline RtMlcshape* get_endmlcshape() const { return m_end_mlcshape;}

private:    //22 parameters
    std::string						m_uid;
    std::string						m_beamuid;
    int								m_sequencenumber;
    std::string						m_name;
    float							m_mu;
    float							m_weight;
    double							m_startcollimatorangle;
    double							m_startgantryangle;
    double							m_starttablerotationangle;
    double							m_endcollimatorangle;
    double							m_arclength;
    double							m_endtablerotationangle;
    float                           m_t_beam_to_pat[16];
    bool							m_ispre;
    bool							m_ispost;
    bool							m_isduring;
    bool							m_isdouble;
    int								m_gantryrotationdirection;
    int								m_collimatorrotationdirection;
    float							m_startssd;
    float							m_endssd;
    DATETIME_BOOST					m_updatetime;
    //////////////////////////////////////////////////////////////////////////
    RtContour*                      m_beam_outline;
    RtMlcshape*                     m_start_mlcshape;
    RtMlcshape*                     m_end_mlcshape;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif