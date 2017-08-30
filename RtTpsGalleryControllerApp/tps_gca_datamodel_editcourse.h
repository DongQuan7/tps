////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_idataloader.h
///  \brief   
///
///  \version 1.0
///
///  \date    Apr. 03, 2015
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_DM_EDITCOURSE_H_
#define TPS_GCA_DM_EDITCOURSE_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "tms/rt_fwk_dl_object_base.h"
#include <cpprest/json.h>

namespace RTFWK{
    class RTFWKDLDOStorageObjectBase;
}

TPS_BEGIN_NAMESPACE


class TpsGcaDmEditCourse: public RTFWK::RTFWKDLDOStorageObjectBase
{  
public:
    struct Course{
    public:
       std::vector<std::string> m_vAddedSeriesUIDs;
       std::vector<std::string> m_vRemovedSeriesUIDs;
       std::string m_sUID;
       std::string m_sName;
    }; 

public:  
    TpsGcaDmEditCourse();
    ~TpsGcaDmEditCourse();

    bool SerializeToJson(web::json::value& jsonValue);

    /////////////////////////////////////////////////////////////////
    // Save To DB by appserver
    /////////////////////////////////////////////////////////////////
    virtual bool SaveEx();

    std::string m_sPatientUID;

    std::vector<Course> m_vAddedCourses;
    std::vector<Course> m_vDeletedCourses;
    std::vector<Course> m_vUpdatedCourses;

private:
    bool SerializeCourseToJson(Course& course, web::json::value& jsonValue);

};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IDATALOADER_H_
