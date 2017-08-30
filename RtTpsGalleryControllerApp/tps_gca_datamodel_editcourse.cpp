                                                     //////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_datamodel_editcourse.cpp
///  \brief   
///
///  \version 1.0
///  \date    Apr. 03, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tps_gca_datamodel_editcourse.h"   
#include <boost/serialization/list.hpp>

//TPS
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsGcaDmEditCourse::TpsGcaDmEditCourse(void){
}

TpsGcaDmEditCourse::~TpsGcaDmEditCourse(void) {
}


bool TpsGcaDmEditCourse::SerializeToJson(web::json::value& jsonValue) {
    bool bRet = true;
    try
    {
        utility::string_t strPatientUID(m_sPatientUID.begin(), m_sPatientUID.end());
        jsonValue[L"PatientUID"] = web::json::value::string(strPatientUID);
        int iAddedCoursesCount = static_cast<int>(m_vAddedCourses.size());
        web::json::value jsonAddedCourses = web::json::value::array(iAddedCoursesCount);
        for (int i=0; i<iAddedCoursesCount;i++)
        {
            web::json::value jsonAddedCourse;
            bRet &= SerializeCourseToJson(m_vAddedCourses[i], jsonAddedCourse);
            if (!bRet)
            {
                TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeToJson"<<"Failed to SerializeToJson for AddedCourse";
                return false;
            }
            jsonAddedCourses[i] = jsonAddedCourse;
        }
        jsonValue[L"AddedCourses"] = jsonAddedCourses;

        int iDeletedCoursesCount = static_cast<int>(m_vDeletedCourses.size());
        web::json::value jsonDeletedCoursesCourses = web::json::value::array(iDeletedCoursesCount);
        for (int i=0; i<iDeletedCoursesCount;i++)
        {
            web::json::value jsonDeletedCourse;
            bRet &= SerializeCourseToJson(m_vDeletedCourses[i], jsonDeletedCourse);
            if (!bRet)
            {
                TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeToJson"<<"Failed to SerializeToJson for DeletedeCourse";
                return false;
            }
            jsonDeletedCoursesCourses[i] = jsonDeletedCourse;
        }
        jsonValue[L"DeletedCourses"] = jsonDeletedCoursesCourses;

        int iUpdatedCoursesCount = static_cast<int>(m_vUpdatedCourses.size());
        web::json::value jsonUpdatedCourses = web::json::value::array(iUpdatedCoursesCount);
        for (int i=0; i<iUpdatedCoursesCount;i++)
        {
            web::json::value jsonUpdatedCourse;
            bRet &= SerializeCourseToJson(m_vUpdatedCourses[i], jsonUpdatedCourse);
            if (!bRet)
            {
                TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeToJson"<<"Failed to SerializeToJson for UpdatedCourse";
                return false;
            }
            jsonUpdatedCourses[i] = jsonUpdatedCourse;
        }
        jsonValue[L"UpdatedCourses"] = jsonUpdatedCourses;

    }
    catch(web::json::json_exception ex)
    {
        TPS_LOG_DEV_ERROR<< "TpsGcaDmEditCourse::SerializeToJson throw json_exception " << std::string(ex.what());
        bRet = false;
    }
    catch(std::bad_cast ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeToJson throw std::bad_cast" << std::string(ex.what());
        bRet = false;
    }
    catch(std::exception ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeToJson throw std::exception "  << std::string(ex.what());
        bRet = false;
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeToJson throw exception ";
        bRet = false;
    }
    return bRet;
}

bool TpsGcaDmEditCourse::SerializeCourseToJson(Course& course, web::json::value& jsonValue) {

    bool bRet = true;
    try
    {
        utility::string_t strUID(course.m_sUID.begin(), course.m_sUID.end());
        jsonValue[L"UID"] = web::json::value::string(strUID);

        utility::string_t strName(course.m_sName.begin(), course.m_sName.end());
        jsonValue[L"Name"] = web::json::value::string(strName);
        int iAddedSeriesCount = static_cast<int>(course.m_vAddedSeriesUIDs.size());
        web::json::value jsonAddedSeriesUIDs = web::json::value::array(iAddedSeriesCount);
        for (int i=0; i<iAddedSeriesCount; i++)
        {
            utility::string_t strSeriesUID(course.m_vAddedSeriesUIDs[i].begin(), course.m_vAddedSeriesUIDs[i].end());
            jsonAddedSeriesUIDs[i] = web::json::value::string(strSeriesUID);
        }
        jsonValue[L"AddedSeriesUIDs"] = jsonAddedSeriesUIDs;

        int iRemovedSeriesCount = static_cast<int>(course.m_vRemovedSeriesUIDs.size());
        web::json::value jsonRemovedSeriesUIDs = web::json::value::array(iRemovedSeriesCount);
        for (int i=0; i<iRemovedSeriesCount; i++)
        {
            utility::string_t strSeriesUID(course.m_vRemovedSeriesUIDs[i].begin(), course.m_vRemovedSeriesUIDs[i].end());
            jsonRemovedSeriesUIDs[i] = web::json::value::string(strSeriesUID);
        }
        jsonValue[L"RemovedSeriesUIDs"] = jsonRemovedSeriesUIDs;
    }
    catch (...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SerializeCourseToJson throw exception ";
        bRet = false;
    }
    return bRet;
}

/////////////////////////////////////////////////////////////////
/// Save To DB by appserver
/////////////////////////////////////////////////////////////////
bool TpsGcaDmEditCourse::SaveEx() {  
    bool bRet = true;
    try
    {
        bRet = SaveByRest("GalleryService","EditCourse");
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmEditCourse::SaveEx throw exception ";
        bRet = false;
    }
    return bRet;
}

TPS_END_NAMESPACE
