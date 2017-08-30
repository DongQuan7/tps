//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_dvh.cpp
///  \brief     dvh
///
///  \version 1.0
///  \date    Aug 21,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dvh.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

RtDvhImp::RtDvhImp():
    m_binwidth(0.f),
    m_voiuid(""),
    m_volume(0.f),
    m_mindose(0.f),
    m_maxdose(0.f),
    m_meandose(0.f),
    m_deviation(0.f),
    m_ingrid(0.f),
    m_standarddeviation(0.f)
{
    m_differentialdvhdata.clear();
    m_cumulativedvhdata.clear();
}

RtDvhImp::RtDvhImp(const RtDvhImp& dvh): 
    m_binwidth(dvh.get_binwidth()),
    m_voiuid(dvh.get_uid()),
    m_volume(dvh.get_volume()),
    m_mindose(dvh.get_mindose()),
    m_maxdose(dvh.get_maxdose()),
    m_meandose(dvh.get_meandose()),
    m_deviation(dvh.get_deviation()),
    m_ingrid(dvh.get_ingrid()),
    m_standarddeviation(dvh.get_standarddeviation())
{
    m_differentialdvhdata = dvh.get_differential_dvhdata();
    m_cumulativedvhdata = dvh.get_cumulative_dvhdata();
}

RtDvhImp& RtDvhImp::operator=(const RtDvhImp& dvh)
{
    if (this != &dvh){
        this->m_binwidth = dvh.get_binwidth();
        this->m_voiuid = dvh.get_uid();
        this->m_volume = dvh.get_volume();
        this->m_mindose = dvh.get_mindose();
        this->m_maxdose = dvh.get_maxdose();
        this->m_meandose = dvh.get_meandose();
        this->m_deviation = dvh.get_deviation();
        this->m_ingrid = dvh.get_ingrid();
        this->m_standarddeviation = dvh.get_standarddeviation();
        this->m_differentialdvhdata = dvh.get_differential_dvhdata();
        this->m_cumulativedvhdata = dvh.get_cumulative_dvhdata();
    }
    return *this;
}

RtDvhImp::~RtDvhImp()
{

}

//////////////////////////////////////////////////////////////////////////
RtDvh::RtDvh()
{
    m_pImp = new RtDvhImp();
}

RtDvh::RtDvh(const RtDvh& dvh):
m_pImp(new RtDvhImp(*dvh.m_pImp))
{
}

RtDvh& RtDvh::operator = (const RtDvh& dvh)
{
    if(this != &dvh) *this->m_pImp = *dvh.m_pImp;
    return *this;
}

RtDvh::~RtDvh()
{
    DEL_PTR(m_pImp);
}

float RtDvh::get_binwidth() const { return m_pImp->get_binwidth();}
void RtDvh::set_binwidth(const float& binwidth) { m_pImp->set_binwidth(binwidth);}

std::string RtDvh::get_uid() const { return m_pImp->get_uid();}
void RtDvh::set_uid(const std::string& voiuid) { m_pImp->set_uid(voiuid);}

float RtDvh::get_volume() const { return m_pImp->get_volume();}
void RtDvh::set_volume(const float& volume) { m_pImp->set_volume(volume);}

float RtDvh::get_mindose() const { return m_pImp->get_mindose();}
void RtDvh::set_mindose(const float& mindose) { m_pImp->set_mindose(mindose);}

float RtDvh::get_maxdose() const { return m_pImp->get_maxdose();}
void RtDvh::set_maxdose(const float& maxdose) { m_pImp->set_maxdose(maxdose);}

float RtDvh::get_meandose() const { return m_pImp->get_meandose();}
void RtDvh::set_meandose(const float& meandose) { m_pImp->set_meandose(meandose);}

float RtDvh::get_deviation() const { return m_pImp->get_deviation();}
void RtDvh::set_deviation(const float& deviation) { m_pImp->set_deviation(deviation);}

float RtDvh::get_ingrid() const { return m_pImp->get_ingrid();}
void RtDvh::set_ingrid(const float& ingrid) { m_pImp->set_ingrid(ingrid);}

float RtDvh::get_standarddeviation() const { return m_pImp->get_standarddeviation();}
void RtDvh::set_standarddeviation(const float& standarddeviation) { m_pImp->set_standarddeviation(standarddeviation);}

//////////////////////////////////////////////////////////////////////////
std::map<int, float> RtDvh::get_differential_dvhdata() const { return m_pImp->get_differential_dvhdata();}
std::map<int, float> RtDvh::get_differential_dvhdata() { return m_pImp->get_differential_dvhdata();}
void RtDvh::set_differential_dvhdata(const std::map<int, float>& differentialdvhdata) { m_pImp->set_differential_dvhdata(differentialdvhdata);}

std::map<int, float> RtDvh::get_cumulative_dvhdata() const { return m_pImp->get_cumulative_dvhdata();}
std::map<int, float> RtDvh::get_cumulative_dvhdata() { return m_pImp->get_cumulative_dvhdata();}
void RtDvh::set_cumulative_dvhdata(const std::map<int, float>& cumulativedvhdata) { m_pImp->set_cumulative_dvhdata(cumulativedvhdata);}

RT_TPS_DATABASE_END_NAMESPACE
