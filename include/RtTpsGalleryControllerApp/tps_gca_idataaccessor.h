////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_idataaccessor.h
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_IDATAACCESSOR_H_
#define TPS_GCA_IDATAACCESSOR_H_

#include "tps_gca_defs.h"

namespace RTFWK{
    class RTFWKDLDOCourse;
    class RTFWKDLDOImage3DHeader;
    class RTFWKDLDOSeries;
}

TPS_BEGIN_NAMESPACE

namespace proto {
    class RT_TPS_Gallery_Courses;
    class RT_TPS_Gallery_EditCourse;
    class RT_TPS_Gallery_ExpandSeries;
}
struct CertifiedImage3D;

class ITpsGcaDataAccessor {
public:
    /// \brief  Initialize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Initialize() = 0;

    /// \brief  Finalize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Finalize() = 0;

    /// \brief  Get course
    ///  
    /// \param[in]  courseUID          uid string of Course
    /// \param[out] pCourse            RTFWKDLDOCourse
    virtual void GetCourse(const std::string& courseUID, 
        RTFWK::RTFWKDLDOCourse ** pCourse) const = 0;

    /// \brief  Get Image3DHeader
    ///
    /// \param[in]  seriesUID       uid string of series
    /// \param[out] pImage3DHeader  RTFWKDLDOImage3DHeader
    /// \return bool  true when succeeded, false otherwise
    virtual bool GetImage3DHeader(const std::string& sSeriesUID,
        RTFWK::RTFWKDLDOImage3DHeader** pImage3DHeader) const = 0;

    /// \brief  Get CertifiedImage3Dentity
    ///
    /// \param[in]  image3DUID       uid of image3D
    /// \param[out] pointer to ceritifiedImage3DEntity
    /// \return bool  [true] for success, [false] otherwise
    virtual bool GetCertifiedImage3DEntity(const std::string& image3DUID, 
        CertifiedImage3D& ceritifiedImage3DEntity) = 0;

    /// \brief Get all series uid 
    ///
    /// \param[in] sPatient             UID of patient
    /// \param[out] pSerUIDList         uid list of all series
    /// \return bool  true when succeeded, false otherwise
    virtual bool GetSeriesUIDList(const std::string& sPatient, 
        std::vector<std::string>* pSerUIDList) const = 0;

    /// \brief Get current active series
    ///
    /// \param[out] sCurSerUID         uid of current active series
    /// \return void
    virtual void GetCurrentSeriesUID(std::string& sCurSerUID) const = 0;

    /// \brief Get DLDO series by series UID
    ///
    /// \param[in]  serUID          uid of series
    /// \param[out] pDLDOSeris      pointer to DLDO object
    /// \return void
    virtual bool GetSeriesByUID(const std::string& serUID, 
        RTFWK::RTFWKDLDOSeries** pDLDOSeris) const = 0;

    /// \brief Get all courses 
    ///
    /// \param[in] patientUid             UID of patient
    /// \param[out] courseList         course list of the series
    /// \return void
    virtual void GetCourseListByPatientUID(const std::string& sPatientUID, proto::RT_TPS_Gallery_Courses &courses) = 0;

    virtual bool EditCourse(proto::RT_TPS_Gallery_EditCourse protoEditCourse) = 0;

    virtual bool DeleteSeries(const std::string &sSeriesUID) = 0;

    virtual bool ExpandSeries(proto::RT_TPS_Gallery_ExpandSeries protoExpandSeries) = 0;

    virtual bool DeleteSlices(const std::string &seriesUID, const bool& isSaveAsNewSeries) = 0;
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IMPROPERATOR_H_
