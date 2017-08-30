////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_dataloaderimpl.h
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_TpsGcaDataLoaderImpl_H_
#define TPS_GCA_TpsGcaDataLoaderImpl_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "RtTpsGalleryControllerApp/tps_gca_idataloader.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

namespace RTFWK{
    class RTFWKDLDOCourse;
    class RTFWKDLDOSeries;
}

namespace Mcsf{
    class Point3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE


struct CertifiedUnit;
class TpsImage3DEntity;
class FTStringBufferGenerator;
union Pixel32;

class TpsGcaDataLoaderImpl : public ITpsGcaDataLoader {

public:
    TpsGcaDataLoaderImpl(void);
    virtual ~TpsGcaDataLoaderImpl(void);

    void ClearGalleryManager();

    void ClearGalleryPatientManager();

    virtual bool Initialize();
    virtual bool Finalize();

    /// \brief Load a patient from TMS
    /// \param[in] sPatientUID
    /// \return bool 
    bool LoadPatient(const std::string&);

    bool LoadSeries(const std::string&, std::vector<std::string>&);

    bool ReloadSeries(const std::string&, std::vector<std::string>&);

    bool LoadCoursesOfPatient(const std::string& patientUID, std::vector<std::string>& vCourseUIDs);

    /// \brief  Load image 3d from TMS
    /// \param[in] sImage3DUID  the image 3d UID in database
    /// \return bool  [true] if succeeded, [false] otherwise
    bool LoadImage3D(const std::string& sImage3DUID);

private:
    /// \brief  set default image 3d uid when load patient
    /// define default image3d SeriesDate & SeriesTime
    /// \param[in]  vecSeries the RTFWKDLDOSeries pointer list
    void SetDefaultImage_i(const std::vector<RTFWK::RTFWKDLDOSeries>& vecSeries);

    /// \brief  Load image 3d via a RTFWKDLDOSeries object
    /// \param[in] pSeries  the RTFWKDLDOSeries pointer
    /// \return bool  [true] if succeeded, [false] otherwise
    bool LoadImage3D_i(RTFWK::RTFWKDLDOSeries* const pSeries);

    /// \brief Prepare the image text from dicom information
    ///        image text data will be stored into TpsImageDataManager
    /// \param[in] pSeries  pointer to the series object
    /// \return bool
    bool PrepareImageTextData_i(RTFWK::RTFWKDLDOSeries* const pSeries);

    /// \brief Prepare one image text string for display
    /// \param[in]  bufGen          buffer generator
    /// \param[in]  strText         text string for display
    /// \param[in]  strKey          key for storing this text in data manager
    bool PrepareOneImageTextString_i(FTStringBufferGenerator& bufGen,
        const std::string& strText, const std::string& strKey);

    bool LoadImage2DData(const std::string &imageUID);

    bool LoadBeamGroupUIDs(const RTFWK::RTFWKDLDOCourse* pCourse, std::vector<std::string> &beamGroupUIDs);

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGcaDataLoaderImpl);
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_TpsGcaDataLoaderImpl_H_
