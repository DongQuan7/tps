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

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_VOI_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_VOI_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <vector>

RT_TPS_DATABASE_BEGIN_NAMESPACE;

enum VOI_FIELD
{
    VOI_UID = 0,
    VOI_STRUCTURESETUID,
    VOI_CONCEPTUALVOLUMEUID,
    VOI_NAME,
    VOI_ISBOLUS,
    VOI_XCOORDINATE3DMAX,
    VOI_YCOORDINATE3DMAX,
    VOI_ZCOORDINATE3DMAX,
    VOI_XCOORDINATE3DMIN,
    VOI_YCOORDINATE3DMIN,
    VOI_ZCOORDINATE3DMIN,
    VOI_ORIENTATION,
    VOI_VOLUME,
    VOI_DENSITY,
    VOI_LOCKUSERUID,
    VOI_LOCKTIME,
    VOI_LOCKMACADDRESS,
    VOI_RED,
    VOI_GREEN,
    VOI_BLUE,
    VOI_ALPHA,
    VOI_AVERAGEDENSITY,
    VOI_MAXDENSITY,
    VOI_MINDENSITY,
    VOI_SURFACEAREA,
    VOI_CENTROIDX,
    VOI_CENTROIDY,
    VOI_CENTROIDZ,
    VOI_VOIROLE,
    VOI_STANDARDDEVIATION,
    VOI_ISVISIBLE,
    VOI_RTROIINTERPRETEDTYPE,
    VOI_ROIINTERPRETER,
    VOI_REFERENCESUBVOLUMEOFFSETX,
    VOI_REFERENCESUBVOLUMEOFFSETY,
    VOI_REFERENCESUBVOLUMEOFFSETZ,
    VOI_REFERENCESUBVOLUMESIZEX,
    VOI_REFERENCESUBVOLUMESIZEY,
    VOI_REFERENCESUBVOLUMESIZEZ,
    VOI_REFERENCESUBVOLUMEDATAFILEPATH,
    VOI_PAT2VOLUMEMATRIX,
    VOI_INTERPOLATE,
    VOI_LOCKTYPE,
    VOI_PLANNINGROLE,
    VOI_SUBROLE,
    VOI_OVERRIDEDENSTIY,
    VOI_TCPNTCP,
    VOI_ISOVERRIDE,
    VOI_DESCRIPTION,
    VOI_ALPHA3D,
    VOI_ROINUMBERINDCM,
    VOI_UPDATETIME,
    VOI_SOFTWAREVERSIONNO,
    VOI_ISSHADED,
    VOI_FIELD_MAX
};

static const char* ENUM2STRING_VOI_FIELD[] =
{
    "UID",
    "STRUCTURESETUID",
    "CONCEPTUALVOLUMEUID",
    "NAME",
    "ISBOLUS",
    "XCOORDINATE3DMAX",
    "YCOORDINATE3DMAX",
    "ZCOORDINATE3DMAX",
    "XCOORDINATE3DMIN",
    "YCOORDINATE3DMIN",
    "ZCOORDINATE3DMIN",
    "ORIENTATION",
    "VOLUME",
    "DENSITY",
    "LOCKUSERUID",
    "LOCKTIME",
    "LOCKMACADDRESS",
    "RED",
    "GREEN",
    "BLUE",
    "ALPHA",
    "AVERAGEDENSITY",
    "MAXDENSITY",
    "MINDENSITY",
    "SURFACEAREA",
    "CENTROIDX",
    "CENTROIDY",
    "CENTROIDZ",
    "VOIROLE",
    "STANDARDDEVIATION",
    "ISVISIBLE",
    "RTROIINTERPRETEDTYPE",
    "ROIINTERPRETER",
    "REFERENCESUBVOLUMEOFFSETX",
    "REFERENCESUBVOLUMEOFFSETY",
    "REFERENCESUBVOLUMEOFFSETZ",
    "REFERENCESUBVOLUMESIZEX",
    "REFERENCESUBVOLUMESIZEY",
    "REFERENCESUBVOLUMESIZEZ",
    "REFERENCESUBVOLUMEDATAFILEPATH",
    "PAT2VOLUMEMATRIX",
    "INTERPOLATE",
    "LOCKTYPE",
    "PLANNINGROLE",
    "SUBROLE",
    "OVERRIDEDENSTIY",
    "TCPNTCP",
    "ISOVERRIDE",
    "DESCRIPTION",
    "ALPHA3D",
    "ROINUMBERINDCM",
    "UPDATETIME",
    "SOFTWAREVERSIONNO",
    "ISSHADED",
    "VOI_FIELD_MAX"
};

static const char* ENUM2STRING_VOI_FIELD_NULL = "100000000000000000000000000000000000000000000000000100";

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

class RtVoiImp;
class RtContour;

class RT_DB_EXPORT RtVoi : public RtDatabaseObject
{
public:

    //default is false to improve performance
    RtVoi(bool bGeneraterUid = false);

    ~RtVoi();

    RtVoi(const RtVoi& voi);

    RtVoi& operator = (const RtVoi& voi);

    /*uid varchar
      VOI Unique ID*/
    std::string get_uid() const;
    void set_uid(const std::string& uid);

    /*structuresetuid varchar
      Related structureset UID*/
    std::string get_structuresetuid() const;
    void set_structuresetuid(const std::string& structuresetuid);

    /*conceptualvolumeuid varchar
      Related conceptual volume UID*/
    std::string get_conceptualvolumeuid() const;
    void set_conceptualvolumeuid(const std::string& conceptualvolumeuid);

    /*name varchar
      Name of VOI(感兴趣区域）*/
    std::string get_name() const;
    void set_name(const std::string& name);

    /*isbolus tinyint
      用于区别bolus和ROI*/
    bool get_isbolus() const;
    void set_isbolus(const bool& isbolus);

    /*xcoordinate3dmax float
      Max coordinate's x*/
    float get_xcoordinate3dmax() const;
    void set_xcoordinate3dmax(const float& xcoordinate3dmax);

    /*ycoordinate3dmax float
      Max coordinate's y*/
    float get_ycoordinate3dmax() const;
    void set_ycoordinate3dmax(const float& ycoordinate3dmax);

    /*zcoordinate3dmax float
      Max coordinate's z*/
    float get_zcoordinate3dmax() const;
    void set_zcoordinate3dmax(const float& zcoordinate3dmax);

    /*xcoordinate3dmin float
      Min coordinate's x*/
    float get_xcoordinate3dmin() const;
    void set_xcoordinate3dmin(const float& xcoordinate3dmin);

    /*ycoordinate3dmin float
      Min coordinate's y*/
    float get_ycoordinate3dmin() const;
    void set_ycoordinate3dmin(const float& ycoordinate3dmin);

    /*zcoordinate3dmin float
      Min coordinate's z*/
    float get_zcoordinate3dmin() const;
    void set_zcoordinate3dmin(const float& zcoordinate3dmin);

    /*orientation int
      Orientation
            copy from Plunc ANASTRUCT*/
    int get_orientation() const;
    void set_orientation(const int& orientation);

    /*volume float
      体积大小*/
    float get_volume() const;
    void set_volume(const float& volume);

    /*density float
      override reference e-density*/
    float get_density() const;
    void set_density(const float& density);

    /*lockuseruid varchar
      Data locked by who*/
    std::string get_lockuseruid() const;
    void set_lockuseruid(const std::string& lockuseruid);

    /*locktime datetime
      Data locked time*/
    DATETIME_BOOST get_locktime() const;
    void set_locktime(const DATETIME_BOOST& locktime);

    /*lockmacaddress varchar
      Data locked machine mac address.*/
    std::string get_lockmacaddress() const;
    void set_lockmacaddress(const std::string& lockmacaddress);

    /*red float
      use vector4d instead of general RGB according xiaoqing's advice on 20140113*/

    /*green float
      use vector4d instead of general RGB according xiaoqing's advice on 20140113*/

    /*blue float
      use vector4d instead of general RGB according xiaoqing's advice on 20140113*/

    /*alpha float
      use vector4d instead of general RGB according xiaoqing's advice on 20140113*/

    /*averagedensity float
      平均密度*/
    float get_averagedensity() const;
    void set_averagedensity(const float& averagedensity);

    /*maxdensity float
      最大密度*/
    float get_maxdensity() const;
    void set_maxdensity(const float& maxdensity);

    /*mindensity float
      最小密度*/
    float get_mindensity() const;
    void set_mindensity(const float& mindensity);

    /*surfacearea float
      表面积*/
    float get_surfacearea() const;
    void set_surfacearea(const float& surfacearea);

    /*centroidx float
      中心点X值*/
    float get_centroidx() const;
    void set_centroidx(const float& centroidx);

    /*centroidy float
      中心点Y值*/
    float get_centroidy() const;
    void set_centroidy(const float& centroidy);

    /*centroidz float
      中心点Z值*/
    float get_centroidz() const;
    void set_centroidz(const float& centroidz);

    /*voirole int
      角色*/
    int get_voirole() const;
    void set_voirole(const int& voirole);

    /*standarddeviation float
      标准差*/
    float get_standarddeviation() const;
    void set_standarddeviation(const float& standarddeviation);

    /*isvisible tinyint
      是否显示*/
    bool get_isvisible() const;
    void set_isvisible(const bool& isvisible);

    /*rtroiinterpretedtype int
      Enum EXTERNA, PTV, CTV, GTV, TREATED_VOLUME, IRRAD_VOLUME, BOLUS, AVOIDANCE, ORGAN, MARKER, REGISTRATION, ISOCENTER, CONTRAST_AGENT*/
    int get_rtroiinterpretedtype() const;
    void set_rtroiinterpretedtype(const int& rtroiinterpretedtype);

    /*roiinterpreter varchar
      DicomTags.RoiInterpreter*/
    std::string get_roiinterpreter() const;
    void set_roiinterpreter(const std::string& roiinterpreter);

    /*referencesubvolumeoffsetx int
      ReferenceSubVolume OffsetX*/
    int get_referencesubvolumeoffsetx() const;
    void set_referencesubvolumeoffsetx(const int& referencesubvolumeoffsetx);

    /*referencesubvolumeoffsety int
      ReferenceSubVolume OffsetY*/
    int get_referencesubvolumeoffsety() const;
    void set_referencesubvolumeoffsety(const int& referencesubvolumeoffsety);

    /*referencesubvolumeoffsetz int
      ReferenceSubVolume OffsetZ*/
    int get_referencesubvolumeoffsetz() const;
    void set_referencesubvolumeoffsetz(const int& referencesubvolumeoffsetz);

    /*referencesubvolumesizex int
      ReferenceSubVolume SizeX*/
    int get_referencesubvolumesizex() const;
    void set_referencesubvolumesizex(const int& referencesubvolumesizex);

    /*referencesubvolumesizey int
      ReferenceSubVolume SizeY*/
    int get_referencesubvolumesizey() const;
    void set_referencesubvolumesizey(const int& referencesubvolumesizey);

    /*referencesubvolumesizez int
      ReferenceSubVolume SizeZ*/
    int get_referencesubvolumesizez() const;
    void set_referencesubvolumesizez(const int& referencesubvolumesizez);

    /*referencesubvolumedatafilepath varchar
      ReferenceSubVolumeData file path*/
    std::string get_referencesubvolumedatafilepath() const;
    void set_referencesubvolumedatafilepath(const std::string& referencesubvolumedatafilepath);

    /*pat2volumematrix varchar
      Pat2Volume matrix*/

    /*interpolate varchar
      是否插值得到的VOI*/

    /*locktype int
      VOI锁类型*/
    int get_locktype() const;
    void set_locktype(const int& locktype);

    /*planningrole int
      PLANNING_ROLE_TYPE emun*/
    int get_planningrole() const;
    void set_planningrole(const int& planningrole);

    /*subrole int
      SUBROLE_TYPE enum*/
    int get_subrole() const;
    void set_subrole(const int& subrole);

    /*overridedenstiy float
      是否密度被修改*/
    float get_overridedenstiy() const;
    void set_overridedenstiy(const float& overridedenstiy);

    /*tcpntcp float
      剂量优化需要*/
    float get_tcpntcp() const;
    void set_tcpntcp(const float& tcpntcp);

    /*isoverride tinyint
      is override user liwu report*/
    bool get_isoverride() const;
    void set_isoverride(const bool& isoverride);

    /*description varchar
      (用于保存用户在添加VOI时输入的description) add jie.wang*/
    std::string get_description() const;
    void set_description(const std::string& description);

    /*alpha3d float
      用于3D窗口下VOI的透明度控制。*/
    float get_alpha3d() const;
    void set_alpha3d(const float& alpha3d);

    /*roinumberindcm int
      仅用于导入导出，对应tag 3006:0022*/
    int get_roinumberindcm() const;
    void set_roinumberindcm(const int& roinumberindcm);

    /*updatetime timestamp
      时间戳*/
    DATETIME_BOOST get_updatetime() const;
    void set_updatetime(const DATETIME_BOOST& updatetime);

    /*softwareversionno varchar
      */
    std::string get_softwareversionno() const;
    void set_softwareversionno(const std::string& softwareversionno);

    /*isshaded tinyint
      (用于标识该voi是否显示表面网格)*/
    bool get_isshaded() const;
    void set_isshaded(const bool& isshaded);

    //////////////////////////////////////////////////////////////////////////
    const float* get_color() const;
    void set_color(const float* color);

    const float* get_pat2volumematrix() const;
    void set_pat2volumematrix(const float* value);

    std::vector<bool> get_interpolate() const;
    void set_interpolate(const std::vector<bool>& value);

    std::vector<RtContour*> get_contours() const;
    std::vector<RtContour*> get_contours();
    void set_contours(const std::vector<RtContour*>& vtContour);

    std::string get_seriesuid() const;
    void set_seriesuid(const std::string& seriesuid);

private:
    RtVoiImp*                   m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp

class RtVoiImp
{

public:
    //default is false to improve performance
    RtVoiImp(bool bGeneraterUid = false);

    ~RtVoiImp();

    RtVoiImp(const RtVoiImp& voi);

    RtVoiImp& operator = (const RtVoiImp& voi);

    //uid
    inline std::string get_uid() const { return m_uid;}
    inline void set_uid(const std::string& uid) { m_uid = uid;}

    //structuresetuid
    inline std::string get_structuresetuid() const { return m_structuresetuid;}
    inline void set_structuresetuid(const std::string& structuresetuid) { m_structuresetuid = structuresetuid;}

    //conceptualvolumeuid
    inline std::string get_conceptualvolumeuid() const { return m_conceptualvolumeuid;}
    inline void set_conceptualvolumeuid(const std::string& conceptualvolumeuid) { m_conceptualvolumeuid = conceptualvolumeuid;}

    //name
    inline std::string get_name() const { return m_name;}
    inline void set_name(const std::string& name) { m_name = name;}

    //isbolus
    inline bool get_isbolus() const { return m_isbolus;}
    inline void set_isbolus(const bool& isbolus) { m_isbolus = isbolus;}

    //xcoordinate3dmax
    inline float get_xcoordinate3dmax() const { return m_xcoordinate3dmax;}
    inline void set_xcoordinate3dmax(const float& xcoordinate3dmax) { m_xcoordinate3dmax = xcoordinate3dmax;}

    //ycoordinate3dmax
    inline float get_ycoordinate3dmax() const { return m_ycoordinate3dmax;}
    inline void set_ycoordinate3dmax(const float& ycoordinate3dmax) { m_ycoordinate3dmax = ycoordinate3dmax;}

    //zcoordinate3dmax
    inline float get_zcoordinate3dmax() const { return m_zcoordinate3dmax;}
    inline void set_zcoordinate3dmax(const float& zcoordinate3dmax) { m_zcoordinate3dmax = zcoordinate3dmax;}

    //xcoordinate3dmin
    inline float get_xcoordinate3dmin() const { return m_xcoordinate3dmin;}
    inline void set_xcoordinate3dmin(const float& xcoordinate3dmin) { m_xcoordinate3dmin = xcoordinate3dmin;}

    //ycoordinate3dmin
    inline float get_ycoordinate3dmin() const { return m_ycoordinate3dmin;}
    inline void set_ycoordinate3dmin(const float& ycoordinate3dmin) { m_ycoordinate3dmin = ycoordinate3dmin;}

    //zcoordinate3dmin
    inline float get_zcoordinate3dmin() const { return m_zcoordinate3dmin;}
    inline void set_zcoordinate3dmin(const float& zcoordinate3dmin) { m_zcoordinate3dmin = zcoordinate3dmin;}

    //orientation
    inline int get_orientation() const { return m_orientation;}
    inline void set_orientation(const int& orientation) { m_orientation = orientation;}

    //volume
    inline float get_volume() const { return m_volume;}
    inline void set_volume(const float& volume) { m_volume = volume;}

    //override reference e-density wzd-2017-03-31
    inline float get_density() const { return m_density;}
    inline void set_density(const float& density) { m_density = density;}

    //lockuseruid
    inline std::string get_lockuseruid() const { return m_lockuseruid;}
    inline void set_lockuseruid(const std::string& lockuseruid) { m_lockuseruid = lockuseruid;}

    //locktime
    inline DATETIME_BOOST get_locktime() const { return m_locktime;}
    inline void set_locktime(const DATETIME_BOOST& locktime) { m_locktime = locktime;}

    //lockmacaddress
    inline std::string get_lockmacaddress() const { return m_lockmacaddress;}
    inline void set_lockmacaddress(const std::string& lockmacaddress) { m_lockmacaddress = lockmacaddress;}

    //averagedensity
    inline float get_averagedensity() const { return m_averagedensity;}
    inline void set_averagedensity(const float& averagedensity) { m_averagedensity = averagedensity;}

    //maxdensity
    inline float get_maxdensity() const { return m_maxdensity;}
    inline void set_maxdensity(const float& maxdensity) { m_maxdensity = maxdensity;}

    //mindensity
    inline float get_mindensity() const { return m_mindensity;}
    inline void set_mindensity(const float& mindensity) { m_mindensity = mindensity;}

    //surfacearea
    inline float get_surfacearea() const { return m_surfacearea;}
    inline void set_surfacearea(const float& surfacearea) { m_surfacearea = surfacearea;}

    //centroidx
    inline float get_centroidx() const { return m_centroidx;}
    inline void set_centroidx(const float& centroidx) { m_centroidx = centroidx;}

    //centroidy
    inline float get_centroidy() const { return m_centroidy;}
    inline void set_centroidy(const float& centroidy) { m_centroidy = centroidy;}

    //centroidz
    inline float get_centroidz() const { return m_centroidz;}
    inline void set_centroidz(const float& centroidz) { m_centroidz = centroidz;}

    //voirole
    inline int get_voirole() const { return m_voirole;}
    inline void set_voirole(const int& voirole) { m_voirole = voirole;}

    //standarddeviation
    inline float get_standarddeviation() const { return m_standarddeviation;}
    inline void set_standarddeviation(const float& standarddeviation) { m_standarddeviation = standarddeviation;}

    //isvisible
    inline bool get_isvisible() const { return m_isvisible;}
    inline void set_isvisible(const bool& isvisible) { m_isvisible = isvisible;}

    //rtroiinterpretedtype
    inline int get_rtroiinterpretedtype() const { return m_rtroiinterpretedtype;}
    inline void set_rtroiinterpretedtype(const int& rtroiinterpretedtype) { m_rtroiinterpretedtype = rtroiinterpretedtype;}

    //roiinterpreter
    inline std::string get_roiinterpreter() const { return m_roiinterpreter;}
    inline void set_roiinterpreter(const std::string& roiinterpreter) { m_roiinterpreter = roiinterpreter;}

    //referencesubvolumeoffsetx
    inline int get_referencesubvolumeoffsetx() const { return m_referencesubvolumeoffsetx;}
    inline void set_referencesubvolumeoffsetx(const int& referencesubvolumeoffsetx) { m_referencesubvolumeoffsetx = referencesubvolumeoffsetx;}

    //referencesubvolumeoffsety
    inline int get_referencesubvolumeoffsety() const { return m_referencesubvolumeoffsety;}
    inline void set_referencesubvolumeoffsety(const int& referencesubvolumeoffsety) { m_referencesubvolumeoffsety = referencesubvolumeoffsety;}

    //referencesubvolumeoffsetz
    inline int get_referencesubvolumeoffsetz() const { return m_referencesubvolumeoffsetz;}
    inline void set_referencesubvolumeoffsetz(const int& referencesubvolumeoffsetz) { m_referencesubvolumeoffsetz = referencesubvolumeoffsetz;}

    //referencesubvolumesizex
    inline int get_referencesubvolumesizex() const { return m_referencesubvolumesizex;}
    inline void set_referencesubvolumesizex(const int& referencesubvolumesizex) { m_referencesubvolumesizex = referencesubvolumesizex;}

    //referencesubvolumesizey
    inline int get_referencesubvolumesizey() const { return m_referencesubvolumesizey;}
    inline void set_referencesubvolumesizey(const int& referencesubvolumesizey) { m_referencesubvolumesizey = referencesubvolumesizey;}

    //referencesubvolumesizez
    inline int get_referencesubvolumesizez() const { return m_referencesubvolumesizez;}
    inline void set_referencesubvolumesizez(const int& referencesubvolumesizez) { m_referencesubvolumesizez = referencesubvolumesizez;}

    //referencesubvolumedatafilepath
    inline std::string get_referencesubvolumedatafilepath() const { return m_referencesubvolumedatafilepath;}
    inline void set_referencesubvolumedatafilepath(const std::string& referencesubvolumedatafilepath) { m_referencesubvolumedatafilepath = referencesubvolumedatafilepath;}

    //locktype
    inline int get_locktype() const { return m_locktype;}
    inline void set_locktype(const int& locktype) { m_locktype = locktype;}

    //planningrole
    inline int get_planningrole() const { return m_planningrole;}
    inline void set_planningrole(const int& planningrole) { m_planningrole = planningrole;}

    //subrole
    inline int get_subrole() const { return m_subrole;}
    inline void set_subrole(const int& subrole) { m_subrole = subrole;}

    //overridedenstiy
    inline float get_overridedenstiy() const { return m_overridedenstiy;}
    inline void set_overridedenstiy(const float& overridedenstiy) { m_overridedenstiy = overridedenstiy;}

    //tcpntcp
    inline float get_tcpntcp() const { return m_tcpntcp;}
    inline void set_tcpntcp(const float& tcpntcp) { m_tcpntcp = tcpntcp;}

    //isoverride
    inline bool get_isoverride() const { return m_isoverride;}
    inline void set_isoverride(const bool& isoverride) { m_isoverride = isoverride;}

    //description
    inline std::string get_description() const { return m_description;}
    inline void set_description(const std::string& description) { m_description = description;}

    //alpha3d
    inline float get_alpha3d() const { return m_alpha3d;}
    inline void set_alpha3d(const float& alpha3d) { m_alpha3d = alpha3d;}

    //roinumberindcm
    inline int get_roinumberindcm() const { return m_roinumberindcm;}
    inline void set_roinumberindcm(const int& roinumberindcm) { m_roinumberindcm = roinumberindcm;}

    //updatetime
    inline DATETIME_BOOST get_updatetime() const { return m_updatetime;}
    inline void set_updatetime(const DATETIME_BOOST& updatetime) { m_updatetime = updatetime;}

    //softwareversionno
    inline std::string get_softwareversionno() const { return m_softwareversionno;}
    inline void set_softwareversionno(const std::string& softwareversionno) { m_softwareversionno = softwareversionno;}

    //isshaded
    inline bool get_isshaded() const { return m_isshaded;}
    inline void set_isshaded(const bool& isshaded) { m_isshaded = isshaded;}
    //////////////////////////////////////////////////////////////////////////
    inline const float* get_color() const {return m_color;}
    inline void set_color(const float* color) {memcpy(m_color, color, 4 * sizeof(float));}

    inline const float* get_pat2volumematrix() const {return m_pat2volumematrix;}
    inline void set_pat2volumematrix(const float* value) {memcpy(m_pat2volumematrix, value, sizeof(float)*16);}

    inline std::vector<bool> get_interpolate() const {return m_vInterpolate;}
    inline void set_interpolate(const std::vector<bool>& value) {m_vInterpolate = value;}

    inline std::vector<RtContour*> get_contours() {return m_vtContour;}
    inline std::vector<RtContour*> get_contours() const {return m_vtContour;}
    inline void set_contours(const std::vector<RtContour*>& contours) { m_vtContour = contours;}

    //seriesuid
    inline std::string get_seriesuid() const { return m_seriesuid;}
    inline void set_seriesuid(const std::string& seriesuid) { m_seriesuid = seriesuid;}

private:    //54 parameters
    std::string						m_uid;
    std::string						m_structuresetuid;
    std::string						m_conceptualvolumeuid;
    std::string						m_name;
    bool							m_isbolus;
    float							m_xcoordinate3dmax;
    float							m_ycoordinate3dmax;
    float							m_zcoordinate3dmax;
    float							m_xcoordinate3dmin;
    float							m_ycoordinate3dmin;
    float							m_zcoordinate3dmin;
    int								m_orientation;
    float							m_volume;
    float							m_density;
    std::string						m_lockuseruid;
    DATETIME_BOOST					m_locktime;
    std::string						m_lockmacaddress;
    float							m_averagedensity;
    float							m_maxdensity;
    float							m_mindensity;
    float							m_surfacearea;
    float							m_centroidx;
    float							m_centroidy;
    float							m_centroidz;
    int								m_voirole;
    float							m_standarddeviation;
    bool							m_isvisible;
    int								m_rtroiinterpretedtype;
    std::string						m_roiinterpreter;
    int								m_referencesubvolumeoffsetx;
    int								m_referencesubvolumeoffsety;
    int								m_referencesubvolumeoffsetz;
    int								m_referencesubvolumesizex;
    int								m_referencesubvolumesizey;
    int								m_referencesubvolumesizez;
    std::string						m_referencesubvolumedatafilepath;
    int								m_locktype;
    int								m_planningrole;
    int								m_subrole;
    float							m_overridedenstiy;
    float							m_tcpntcp;
    bool							m_isoverride;
    std::string						m_description;
    float							m_alpha3d;
    int								m_roinumberindcm;
    DATETIME_BOOST					m_updatetime;
    std::string						m_softwareversionno;
    bool							m_isshaded;
    //////////////////////////////////////////////////////////////////////////
    float                           m_color[4];
    float                           m_pat2volumematrix[16]; // 16 element of float
    std::vector<bool>               m_vInterpolate;
    std::vector<RtContour*>         m_vtContour;
    std::string                     m_seriesuid;
};

RT_TPS_DATABASE_END_NAMESPACE
#endif