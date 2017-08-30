////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_dataaccessorimpl.h
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_DATAACCESSORIMPL_H_
#define TPS_GCA_DATAACCESSORIMPL_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "RtTpsGalleryControllerApp/tps_gca_idataaccessor.h"  

TPS_BEGIN_NAMESPACE

struct CertifiedImage3D;

class TpsGcaDataAccessorImpl : public ITpsGcaDataAccessor {

public:
    TpsGcaDataAccessorImpl(){}
    ~TpsGcaDataAccessorImpl(){}

    /// \brief  Initialize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Initialize();

    /// \brief  Finalize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Finalize();

    /// \brief  Get course
    ///  
    /// \param[in]  courseUID          uid string of Course
    /// \param[out] pCourse            RTFWKDLDOCourse
    void GetCourse(const std::string& courseUID, 
        RTFWK::RTFWKDLDOCourse ** pCourse) const;

    /// \brief  Get Image3DHeader
    ///
    /// \param[in]  seriesUID       uid string of series
    /// \param[out] pImage3DHeader  RTFWKDLDOImage3DHeader
    /// \return bool  true when succeeded, false otherwise
    bool GetImage3DHeader(const std::string& sSeriesUID,
        RTFWK::RTFWKDLDOImage3DHeader** pImage3DHeader) const;

    /// \brief Get one certifiedImage3D Entity
    ///
    /// \param[in] image3DUID          unit list of the system
    /// \param[out] ceritifiedImage3DEntity     list of the system
    /// \return bool [true] if succeed, [false] if failure
    bool GetCertifiedImage3DEntity(const std::string& image3DUID, 
        CertifiedImage3D& ceritifiedImage3DEntity);

    /// \brief Get all series uid 
    ///
    /// \param[in] sPatient             UID of patient
    /// \param[out] pSerUIDList         uid list of all series
    /// \return bool  true when succeeded, false otherwise
    bool GetSeriesUIDList(const std::string& sPatient,
        std::vector<std::string>* pSerUIDList) const;

    /// \brief Get DLDO series by series UID
    ///
    /// \param[in]  serUID          uid of series
    /// \param[out] pDLDOSeris      pointer to DLDO object
    /// \return void
    bool GetSeriesByUID(const std::string& serUID, 
        RTFWK::RTFWKDLDOSeries** pDLDOSeris) const;

    /// \brief Get current active series
    ///
    /// \param[out] sCurSerUID         uid of current active series
    /// \return void
    void GetCurrentSeriesUID(std::string& sCurSerUID) const;

    /// \brief Get all courses 
    ///
    /// \param[in] patientUid             UID of patient
    /// \param[out] courseList         course list of the series
    /// \return void
    void GetCourseListByPatientUID(const std::string& sPatientUID, proto::RT_TPS_Gallery_Courses &courses);

    bool EditCourse(proto::RT_TPS_Gallery_EditCourse protoEditCourse);

    bool DeleteSeries(const std::string &sSeriesUID);

    bool ExpandSeries(proto::RT_TPS_Gallery_ExpandSeries protoExpandSeries);

    bool DeleteSlices(const std::string &seriesUID, const bool& isSaveAsNewSeries);

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGcaDataAccessorImpl);
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_DATAACCESSORIMPL_H_
