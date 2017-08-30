//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_dvh.h
///  \brief     dvh
///
///  \version 1.0
///  \date    Aug 21,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_DVH_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_DVH_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include <map>

RT_TPS_DATABASE_BEGIN_NAMESPACE;

class RtDvhImp;

class RT_DB_EXPORT RtDvh
{
public:

    RtDvh();

    ~RtDvh();

    RtDvh(const RtDvh& dvh);

    RtDvh& operator = (const RtDvh& dvh);

    float get_binwidth() const;
    void set_binwidth(const float& binwidth);

    std::string get_uid() const;
    void set_uid(const std::string& voiuid);

    float get_volume() const;
    void set_volume(const float& volume);

    float get_mindose() const;
    void set_mindose(const float& mindose);

    float get_maxdose() const;
    void set_maxdose(const float& maxdose);

    float get_meandose() const;
    void set_meandose(const float& meandose);

    float get_deviation() const;
    void set_deviation(const float& deviation);

    float get_ingrid() const;
    void set_ingrid(const float& ingrid);

    float get_standarddeviation() const;
    void set_standarddeviation(const float& standarddeviation);

    //////////////////////////////////////////////////////////////////////////
    std::map<int, float> get_differential_dvhdata() const;
    std::map<int, float> get_differential_dvhdata();
    void set_differential_dvhdata(const std::map<int, float>& differentialdvhdata);

    std::map<int, float> get_cumulative_dvhdata() const;
    std::map<int, float> get_cumulative_dvhdata();
    void set_cumulative_dvhdata(const std::map<int, float>& cumulativedvhdata);

private:
    RtDvhImp*       m_pImp;
};


//////////////////////////////////////////////////////////////////////////
//for imp
class RtDvhImp
{
public:

    RtDvhImp();

    ~RtDvhImp();

    RtDvhImp(const RtDvhImp& dvhimp);

    RtDvhImp& operator = (const RtDvhImp& dvhimp);

    inline float get_binwidth() const { return m_binwidth;}
    inline void set_binwidth(const float& binwidth) { m_binwidth = binwidth;}

    inline std::string get_uid() const { return m_voiuid;}
    inline void set_uid(const std::string& voiuid) { m_voiuid = voiuid;}

    inline float get_volume() const { return m_volume;}
    inline void set_volume(const float& volume) { m_volume = volume;}

    inline float get_mindose() const { return m_mindose;}
    inline void set_mindose(const float& mindose) { m_mindose = mindose;}

    inline float get_maxdose() const { return m_maxdose;}
    inline void set_maxdose(const float& maxdose) { m_maxdose = maxdose;}

    inline float get_meandose() const { return m_meandose;}
    inline void set_meandose(const float& meandose) { m_meandose = meandose;}

    inline float get_deviation() const { return m_deviation;}
    inline void set_deviation(const float& deviation) { m_deviation = deviation;}

    inline float get_ingrid() const { return m_ingrid;}
    inline void set_ingrid(const float& ingrid) { m_ingrid = ingrid;}

    inline float get_standarddeviation() const { return m_standarddeviation;}
    inline void set_standarddeviation(const float& standarddeviation) { m_standarddeviation = standarddeviation;}

    //////////////////////////////////////////////////////////////////////////
    inline std::map<int, float> get_differential_dvhdata() const { return m_differentialdvhdata;}
    inline std::map<int, float> get_differential_dvhdata() { return m_differentialdvhdata;}
    inline void set_differential_dvhdata(const std::map<int, float>& differentialdvhdata) { m_differentialdvhdata = differentialdvhdata;}

    inline std::map<int, float> get_cumulative_dvhdata() const { return m_cumulativedvhdata;}
    inline std::map<int, float> get_cumulative_dvhdata() { return m_cumulativedvhdata;}
    inline void set_cumulative_dvhdata(const std::map<int, float>& cumulativedvhdata) { m_cumulativedvhdata = cumulativedvhdata;}

private:    //11 parameters
    float                           m_binwidth;
    std::string                     m_voiuid;
    float                           m_volume;
    float                           m_mindose;
    float                           m_maxdose;
    float                           m_meandose;
    float                           m_deviation;
    float                           m_ingrid;
    float                           m_standarddeviation;

    // start from zero, first value is bin number, second value is volume unit
    std::map<int, float>            m_differentialdvhdata;
    std::map<int, float>            m_cumulativedvhdata;
};
RT_TPS_DATABASE_END_NAMESPACE
#endif