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

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_PRESCRIPTION_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_PRESCRIPTION_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum PRESCRIPTION_FIELD
{
    PRESCRIPTION_UID = 0,
    PRESCRIPTION_SITEUID,
    PRESCRIPTION_NAME,
    PRESCRIPTION_TECHNICAL,
    PRESCRIPTION_NOTES,
    PRESCRIPTION_DOSEPERFRACTION,
    PRESCRIPTION_FRACTIONSNUMBER,
    PRESCRIPTION_CREATEDTIME,
    PRESCRIPTION_RADIATIONTYPE,
    PRESCRIPTION_BEAMMODE,
    PRESCRIPTION_RELATEDPRESCRIPTIONUID,
    PRESCRIPTION_RELATEDPRESCRIPTIONDURATION,
    PRESCRIPTION_DURATIONTYPE,
    PRESCRIPTION_AFTERFRACTIONNUMBER,
    PRESCRIPTION_TREATMENTORDERTYPE,
    PRESCRIPTION_ISMONDAYSELECTED,
    PRESCRIPTION_ISTUESDAYSELECTED,
    PRESCRIPTION_ISWEDNESDAYSELECTED,
    PRESCRIPTION_ISTHURSDAYSELECTED,
    PRESCRIPTION_ISFRIDAYSELECTED,
    PRESCRIPTION_ISSATURDAYSELECTED,
    PRESCRIPTION_ISSUNDAYSELECTED,
    PRESCRIPTION_ISINCLUDEHOLIDAY,
    PRESCRIPTION_TIMESPERDAY,
    PRESCRIPTION_INTERVALHOUR,
    PRESCRIPTION_OTHER,
    PRESCRIPTION_TARGETTYPE,
    PRESCRIPTION_TARGETEFFECTTYPE,
    PRESCRIPTION_TARGETEFFECTPERCENT,
    PRESCRIPTION_ISDEFAULT,
    PRESCRIPTION_SITENAME,
    PRESCRIPTION_ISTXPATTERNOFRXCHANGED,
    PRESCRIPTION_ISTXPATTERNOFPLANGOALCHANGED,
    PRESCRIPTION_TREATMENTTARGET,
    PRESCRIPTION_DEPTH,
    PRESCRIPTION_PLANNINGROLETYPE,
    PRESCRIPTION_CRC,
    PRESCRIPTION_TOTALDOSE,
    PRESCRIPTION_ISGATING,
    PRESCRIPTION_UPDATETIME,
    PRESCRIPTION_BINMODE,
    PRESCRIPTION_BEAMONRANGESTART,
    PRESCRIPTION_BEAMONRANGEEND,
    PRESCRIPTION_IMAGEGUIDEPATTERN,
    PRESCRIPTION_FIELD_MAX
};

static const char* ENUM2STRING_PRESCRIPTION_FIELD[] =
{
    "UID",
    "SITEUID",
    "NAME",
    "TECHNICAL",
    "NOTES",
    "DOSEPERFRACTION",
    "FRACTIONSNUMBER",
    "CREATEDTIME",
    "RADIATIONTYPE",
    "BEAMMODE",
    "RELATEDPRESCRIPTIONUID",
    "RELATEDPRESCRIPTIONDURATION",
    "DURATIONTYPE",
    "AFTERFRACTIONNUMBER",
    "TREATMENTORDERTYPE",
    "ISMONDAYSELECTED",
    "ISTUESDAYSELECTED",
    "ISWEDNESDAYSELECTED",
    "ISTHURSDAYSELECTED",
    "ISFRIDAYSELECTED",
    "ISSATURDAYSELECTED",
    "ISSUNDAYSELECTED",
    "ISINCLUDEHOLIDAY",
    "TIMESPERDAY",
    "INTERVALHOUR",
    "OTHER",
    "TARGETTYPE",
    "TARGETEFFECTTYPE",
    "TARGETEFFECTPERCENT",
    "ISDEFAULT",
    "SITENAME",
    "ISTXPATTERNOFRXCHANGED",
    "ISTXPATTERNOFPLANGOALCHANGED",
    "TREATMENTTARGET",
    "DEPTH",
    "PLANNINGROLETYPE",
    "CRC",
    "TOTALDOSE",
    "ISGATING",
    "UPDATETIME",
    "BINMODE",
    "BEAMONRANGESTART",
    "BEAMONRANGEEND",
    "IMAGEGUIDEPATTERN",
    "PRESCRIPTION_FIELD_MAX"
};

static const char* ENUM2STRING_PRESCRIPTION_FIELD_NULL = "11000000000000011111111000000100000000010000";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtPrescriptionImp;

class RT_DB_EXPORT RtPrescription : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtPrescription(bool bGeneraterUid = false);

    ~RtPrescription();

    RtPrescription(const RtPrescription& prescription);

    RtPrescription& operator = (const RtPrescription& prescription);

    /*uid varchar
      PK*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*siteuid varchar
      record the related site's uid*/
    std::string get_siteuid() const;
    void set_siteuid(const std::string& siteuid);

    /*name varchar
      UI Element on Therapy plan*/
    std::string get_name() const;
    void set_name(const std::string& name);

    /*technical int
      record the address of technical*/
    int get_technical() const;
    void set_technical(const int& technical);

    /*notes varchar
      (30xx,0970) Notes on this prescription in general. */
    std::string get_notes() const;
    void set_notes(const std::string& notes);

    /*doseperfraction double
      record the rx's dose per fraction*/
    double get_doseperfraction() const;
    void set_doseperfraction(const double& doseperfraction);

    /*fractionsnumber int
      (30xx,0972) Number of Fractions. */
    int get_fractionsnumber() const;
    void set_fractionsnumber(const int& fractionsnumber);

    /*createdtime datetime
      (30xx,0979) Date and time on which the annotation was made. */
    DATETIME_BOOST get_createdtime() const;
    void set_createdtime(const DATETIME_BOOST& createdtime);

    /*radiationtype int
      record the address of radiationtype*/
    int get_radiationtype() const;
    void set_radiationtype(const int& radiationtype);

    /*beammode varchar
      "Technecal" radio button  in therapy plan's prescription*/
    std::string get_beammode() const;
    void set_beammode(const std::string& beammode);

    /*relatedprescriptionuid varchar
      record the related rx's uid*/
    std::string get_relatedprescriptionuid() const;
    void set_relatedprescriptionuid(const std::string& relatedprescriptionuid);

    /*relatedprescriptionduration int
      record the related rx's duration*/
    int get_relatedprescriptionduration() const;
    void set_relatedprescriptionduration(const int& relatedprescriptionduration);

    /*durationtype int
      record the duration type*/
    int get_durationtype() const;
    void set_durationtype(const int& durationtype);

    /*afterfractionnumber int
      record the fraction number of after duration*/
    int get_afterfractionnumber() const;
    void set_afterfractionnumber(const int& afterfractionnumber);

    /*treatmentordertype int
      record the treatment order type*/
    int get_treatmentordertype() const;
    void set_treatmentordertype(const int& treatmentordertype);

    /*ismondayselected tinyint
      record the rx is need to be treated at Monday with every week or not*/
    bool get_ismondayselected() const;
    void set_ismondayselected(const bool& ismondayselected);

    /*istuesdayselected tinyint
      record the rx is need to be treated at Tuesday with every week or not*/
    bool get_istuesdayselected() const;
    void set_istuesdayselected(const bool& istuesdayselected);

    /*iswednesdayselected tinyint
      record the rx is need to be treated at Wednesday with every week or not*/
    bool get_iswednesdayselected() const;
    void set_iswednesdayselected(const bool& iswednesdayselected);

    /*isthursdayselected tinyint
      record the rx is need to be treated at Thursday with every week or not*/
    bool get_isthursdayselected() const;
    void set_isthursdayselected(const bool& isthursdayselected);

    /*isfridayselected tinyint
      record the rx is need to be treated at Friday with every week or not*/
    bool get_isfridayselected() const;
    void set_isfridayselected(const bool& isfridayselected);

    /*issaturdayselected tinyint
      record the rx is need to be treated at Saturday with every week or not*/
    bool get_issaturdayselected() const;
    void set_issaturdayselected(const bool& issaturdayselected);

    /*issundayselected tinyint
      record the rx is need to be treated at Sunday with every week or not*/
    bool get_issundayselected() const;
    void set_issundayselected(const bool& issundayselected);

    /*isincludeholiday tinyint
      record the rx is need to be treated in holiday or not*/
    bool get_isincludeholiday() const;
    void set_isincludeholiday(const bool& isincludeholiday);

    /*timesperday int
      record the rx's time per day in treatment*/
    int get_timesperday() const;
    void set_timesperday(const int& timesperday);

    /*intervalhour float
      record the rx's interval hour in treatment*/
    float get_intervalhour() const;
    void set_intervalhour(const float& intervalhour);

    /*other varchar
      record the other comment with the rx*/
    std::string get_other() const;
    void set_other(const std::string& other);

    /*targettype varchar
      record the target type with the rx*/
    std::string get_targettype() const;
    void set_targettype(const std::string& targettype);

    /*targeteffecttype int
      record the target effect type with the rx*/
    int get_targeteffecttype() const;
    void set_targeteffecttype(const int& targeteffecttype);

    /*targeteffectpercent float
      record the target effect percent with the rx*/
    float get_targeteffectpercent() const;
    void set_targeteffectpercent(const float& targeteffectpercent);

    /*isdefault tinyint
      record the rx is the default or not,one site only has a default rx*/
    bool get_isdefault() const;
    void set_isdefault(const bool& isdefault);

    /*sitename varchar
      record the related site's name*/
    std::string get_sitename() const;
    void set_sitename(const std::string& sitename);

    /*istxpatternofrxchanged tinyint
      表示该处方的治疗模式发生改变  (在PlanOrder界面更改)*/
    bool get_istxpatternofrxchanged() const;
    void set_istxpatternofrxchanged(const bool& istxpatternofrxchanged);

    /*istxpatternofplangoalchanged tinyint
      表示该处方下面的DeliveryBeamGroup的治疗模式发生改变  (在BeamSchedule界面更改)*/
    bool get_istxpatternofplangoalchanged() const;
    void set_istxpatternofplangoalchanged(const bool& istxpatternofplangoalchanged);

    /*treatmenttarget int
      record the rx's treatment target*/
    int get_treatmenttarget() const;
    void set_treatmenttarget(const int& treatmenttarget);

    /*depth float
      record the rx's depth value*/
    float get_depth() const;
    void set_depth(const float& depth);

    /*planningroletype int
      record the rx's planning role type*/
    int get_planningroletype() const;
    void set_planningroletype(const int& planningroletype);

    /*crc varchar
      record the rx's crc code*/
    std::string get_crc() const;
    void set_crc(const std::string& crc);

    /*totaldose double
      record the rx's total dose*/
    double get_totaldose() const;
    void set_totaldose(const double& totaldose);

    /*isgating tinyint
      record the rx's gating status*/
    bool get_isgating() const;
    void set_isgating(const bool& isgating);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);

    /*binmode int
      门控参数*/
    int get_binmode() const;
    void set_binmode(const int& binmode);

    /*beamonrangestart double
      门控参数范围*/
    double get_beamonrangestart() const;
    void set_beamonrangestart(const double& beamonrangestart);

    /*beamonrangeend double
      门控参数范围*/
    double get_beamonrangeend() const;
    void set_beamonrangeend(const double& beamonrangeend);

    /*imageguidepattern varchar
      保存图像引导模式bool 值*/
    std::string get_imageguidepattern() const;
    void set_imageguidepattern(const std::string& imageguidepattern);


private:
    RtPrescriptionImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtPrescriptionImp
{

public:
    //default is false to improve performance
    RtPrescriptionImp(bool bGeneraterUid = false);

    ~RtPrescriptionImp();

    RtPrescriptionImp(const RtPrescriptionImp& prescription);

    RtPrescriptionImp& operator = (const RtPrescriptionImp& prescription);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //siteuid
    inline std::string get_siteuid() const { return m_siteuid;}
    inline void set_siteuid(const std::string& siteuid) { m_siteuid = siteuid;}

    //name
    inline std::string get_name() const { return m_name;}
    inline void set_name(const std::string& name) { m_name = name;}

    //technical
    inline int get_technical() const { return m_technical;}
    inline void set_technical(const int& technical) { m_technical = technical;}

    //notes
    inline std::string get_notes() const { return m_notes;}
    inline void set_notes(const std::string& notes) { m_notes = notes;}

    //doseperfraction
    inline double get_doseperfraction() const { return m_doseperfraction;}
    inline void set_doseperfraction(const double& doseperfraction) { m_doseperfraction = doseperfraction;}

    //fractionsnumber
    inline int get_fractionsnumber() const { return m_fractionsnumber;}
    inline void set_fractionsnumber(const int& fractionsnumber) { m_fractionsnumber = fractionsnumber;}

    //createdtime
    inline DATETIME_BOOST get_createdtime() const { return m_createdtime;}
    inline void set_createdtime(const DATETIME_BOOST& createdtime) { m_createdtime = createdtime;}

    //radiationtype
    inline int get_radiationtype() const { return m_radiationtype;}
    inline void set_radiationtype(const int& radiationtype) { m_radiationtype = radiationtype;}

    //beammode
    inline std::string get_beammode() const { return m_beammode;}
    inline void set_beammode(const std::string& beammode) { m_beammode = beammode;}

    //relatedprescriptionuid
    inline std::string get_relatedprescriptionuid() const { return m_relatedprescriptionuid;}
    inline void set_relatedprescriptionuid(const std::string& relatedprescriptionuid) { m_relatedprescriptionuid = relatedprescriptionuid;}

    //relatedprescriptionduration
    inline int get_relatedprescriptionduration() const { return m_relatedprescriptionduration;}
    inline void set_relatedprescriptionduration(const int& relatedprescriptionduration) { m_relatedprescriptionduration = relatedprescriptionduration;}

    //durationtype
    inline int get_durationtype() const { return m_durationtype;}
    inline void set_durationtype(const int& durationtype) { m_durationtype = durationtype;}

    //afterfractionnumber
    inline int get_afterfractionnumber() const { return m_afterfractionnumber;}
    inline void set_afterfractionnumber(const int& afterfractionnumber) { m_afterfractionnumber = afterfractionnumber;}

    //treatmentordertype
    inline int get_treatmentordertype() const { return m_treatmentordertype;}
    inline void set_treatmentordertype(const int& treatmentordertype) { m_treatmentordertype = treatmentordertype;}

    //ismondayselected
    inline bool get_ismondayselected() const { return m_ismondayselected;}
    inline void set_ismondayselected(const bool& ismondayselected) { m_ismondayselected = ismondayselected;}

    //istuesdayselected
    inline bool get_istuesdayselected() const { return m_istuesdayselected;}
    inline void set_istuesdayselected(const bool& istuesdayselected) { m_istuesdayselected = istuesdayselected;}

    //iswednesdayselected
    inline bool get_iswednesdayselected() const { return m_iswednesdayselected;}
    inline void set_iswednesdayselected(const bool& iswednesdayselected) { m_iswednesdayselected = iswednesdayselected;}

    //isthursdayselected
    inline bool get_isthursdayselected() const { return m_isthursdayselected;}
    inline void set_isthursdayselected(const bool& isthursdayselected) { m_isthursdayselected = isthursdayselected;}

    //isfridayselected
    inline bool get_isfridayselected() const { return m_isfridayselected;}
    inline void set_isfridayselected(const bool& isfridayselected) { m_isfridayselected = isfridayselected;}

    //issaturdayselected
    inline bool get_issaturdayselected() const { return m_issaturdayselected;}
    inline void set_issaturdayselected(const bool& issaturdayselected) { m_issaturdayselected = issaturdayselected;}

    //issundayselected
    inline bool get_issundayselected() const { return m_issundayselected;}
    inline void set_issundayselected(const bool& issundayselected) { m_issundayselected = issundayselected;}

    //isincludeholiday
    inline bool get_isincludeholiday() const { return m_isincludeholiday;}
    inline void set_isincludeholiday(const bool& isincludeholiday) { m_isincludeholiday = isincludeholiday;}

    //timesperday
    inline int get_timesperday() const { return m_timesperday;}
    inline void set_timesperday(const int& timesperday) { m_timesperday = timesperday;}

    //intervalhour
    inline float get_intervalhour() const { return m_intervalhour;}
    inline void set_intervalhour(const float& intervalhour) { m_intervalhour = intervalhour;}

    //other
    inline std::string get_other() const { return m_other;}
    inline void set_other(const std::string& other) { m_other = other;}

    //targettype
    inline std::string get_targettype() const { return m_targettype;}
    inline void set_targettype(const std::string& targettype) { m_targettype = targettype;}

    //targeteffecttype
    inline int get_targeteffecttype() const { return m_targeteffecttype;}
    inline void set_targeteffecttype(const int& targeteffecttype) { m_targeteffecttype = targeteffecttype;}

    //targeteffectpercent
    inline float get_targeteffectpercent() const { return m_targeteffectpercent;}
    inline void set_targeteffectpercent(const float& targeteffectpercent) { m_targeteffectpercent = targeteffectpercent;}

    //isdefault
    inline bool get_isdefault() const { return m_isdefault;}
    inline void set_isdefault(const bool& isdefault) { m_isdefault = isdefault;}

    //sitename
    inline std::string get_sitename() const { return m_sitename;}
    inline void set_sitename(const std::string& sitename) { m_sitename = sitename;}

    //istxpatternofrxchanged
    inline bool get_istxpatternofrxchanged() const { return m_istxpatternofrxchanged;}
    inline void set_istxpatternofrxchanged(const bool& istxpatternofrxchanged) { m_istxpatternofrxchanged = istxpatternofrxchanged;}

    //istxpatternofplangoalchanged
    inline bool get_istxpatternofplangoalchanged() const { return m_istxpatternofplangoalchanged;}
    inline void set_istxpatternofplangoalchanged(const bool& istxpatternofplangoalchanged) { m_istxpatternofplangoalchanged = istxpatternofplangoalchanged;}

    //treatmenttarget
    inline int get_treatmenttarget() const { return m_treatmenttarget;}
    inline void set_treatmenttarget(const int& treatmenttarget) { m_treatmenttarget = treatmenttarget;}

    //depth
    inline float get_depth() const { return m_depth;}
    inline void set_depth(const float& depth) { m_depth = depth;}

    //planningroletype
    inline int get_planningroletype() const { return m_planningroletype;}
    inline void set_planningroletype(const int& planningroletype) { m_planningroletype = planningroletype;}

    //crc
    inline std::string get_crc() const { return m_crc;}
    inline void set_crc(const std::string& crc) { m_crc = crc;}

    //totaldose
    inline double get_totaldose() const { return m_totaldose;}
    inline void set_totaldose(const double& totaldose) { m_totaldose = totaldose;}

    //isgating
    inline bool get_isgating() const { return m_isgating;}
    inline void set_isgating(const bool& isgating) { m_isgating = isgating;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

    //binmode
    inline int get_binmode() const { return m_binmode;}
    inline void set_binmode(const int& binmode) { m_binmode = binmode;}

    //beamonrangestart
    inline double get_beamonrangestart() const { return m_beamonrangestart;}
    inline void set_beamonrangestart(const double& beamonrangestart) { m_beamonrangestart = beamonrangestart;}

    //beamonrangeend
    inline double get_beamonrangeend() const { return m_beamonrangeend;}
    inline void set_beamonrangeend(const double& beamonrangeend) { m_beamonrangeend = beamonrangeend;}

    //imageguidepattern
    inline std::string get_imageguidepattern() const { return m_imageguidepattern;}
    inline void set_imageguidepattern(const std::string& imageguidepattern) { m_imageguidepattern = imageguidepattern;}

private:    //44 parameters
    std::string						m_uid;
    std::string						m_siteuid;
    std::string						m_name;
    int								m_technical;
    std::string						m_notes;
    double							m_doseperfraction;
    int								m_fractionsnumber;
    DATETIME_BOOST					m_createdtime;
    int								m_radiationtype;
    std::string						m_beammode;
    std::string						m_relatedprescriptionuid;
    int								m_relatedprescriptionduration;
    int								m_durationtype;
    int								m_afterfractionnumber;
    int								m_treatmentordertype;
    bool							m_ismondayselected;
    bool							m_istuesdayselected;
    bool							m_iswednesdayselected;
    bool							m_isthursdayselected;
    bool							m_isfridayselected;
    bool							m_issaturdayselected;
    bool							m_issundayselected;
    bool							m_isincludeholiday;
    int								m_timesperday;
    float							m_intervalhour;
    std::string						m_other;
    std::string						m_targettype;
    int								m_targeteffecttype;
    float							m_targeteffectpercent;
    bool							m_isdefault;
    std::string						m_sitename;
    bool							m_istxpatternofrxchanged;
    bool							m_istxpatternofplangoalchanged;
    int								m_treatmenttarget;
    float							m_depth;
    int								m_planningroletype;
    std::string						m_crc;
    double							m_totaldose;
    bool							m_isgating;
    DATETIME_BOOST					m_updatetime;
    int								m_binmode;
    double							m_beamonrangestart;
    double							m_beamonrangeend;
    std::string						m_imageguidepattern;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif