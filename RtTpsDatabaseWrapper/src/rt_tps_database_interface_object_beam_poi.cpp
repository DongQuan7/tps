﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_beam_poi.cpp
///  \brief     This file was generated by CodeGenerater.exe 
///              From database version: 2016/1/18 15:03:26
///
///  \version 1.0
///  \date    1/19/2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam_poi.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

//////////////////////////////////////////////////////////////////////////

RtBeam_poiImp::RtBeam_poiImp():
    m_beamuid(""),
    m_poiuid(""),
    m_dosecontribution(0.f),
    m_updatetime(boost::date_time::not_a_date_time)
{
}

RtBeam_poiImp::RtBeam_poiImp(const RtBeam_poiImp& beam_poi):
    m_beamuid(beam_poi.get_beamuid()),
    m_poiuid(beam_poi.get_poiuid()),
    m_dosecontribution(beam_poi.get_dosecontribution()),
    m_updatetime(beam_poi.get_updatetime())
{
}

RtBeam_poiImp& RtBeam_poiImp::operator=(const RtBeam_poiImp& beam_poi)
{
    if (this != &beam_poi){
        this->m_beamuid = beam_poi.get_beamuid();
        this->m_poiuid = beam_poi.get_poiuid();
        this->m_dosecontribution = beam_poi.get_dosecontribution();
        this->m_updatetime = beam_poi.get_updatetime();
    }
    return *this;
}

RtBeam_poiImp::~RtBeam_poiImp()
{
}


//////////////////////////////////////////////////////////////////////////
RtBeam_poi::RtBeam_poi()
{
    m_pImp = new RtBeam_poiImp();
}

RtBeam_poi::RtBeam_poi(const RtBeam_poi& beam_poi):
   m_pImp(new RtBeam_poiImp(*beam_poi.m_pImp))
{
}

RtBeam_poi& RtBeam_poi::operator = (const RtBeam_poi& beam_poi)
{
    if(this != &beam_poi) *this->m_pImp = *beam_poi.m_pImp;
    return *this;
}

RtBeam_poi::~RtBeam_poi()
{
    DEL_PTR(m_pImp);
}

//beamuid
std::string RtBeam_poi::get_beamuid() const { return m_pImp->get_beamuid();}
void RtBeam_poi::set_beamuid(const std::string& beamuid) { m_pImp->set_beamuid(beamuid);}

//poiuid
std::string RtBeam_poi::get_poiuid() const { return m_pImp->get_poiuid();}
void RtBeam_poi::set_poiuid(const std::string& poiuid) { m_pImp->set_poiuid(poiuid);}

//dosecontribution
float RtBeam_poi::get_dosecontribution() const { return m_pImp->get_dosecontribution();}
void RtBeam_poi::set_dosecontribution(const float& dosecontribution) { m_pImp->set_dosecontribution(dosecontribution);}

//updatetime
DATETIME_BOOST RtBeam_poi::get_updatetime() const { return m_pImp->get_updatetime();}
void RtBeam_poi::set_updatetime(const DATETIME_BOOST& updatetime) { m_pImp->set_updatetime(updatetime);}

RT_TPS_DATABASE_END_NAMESPACE