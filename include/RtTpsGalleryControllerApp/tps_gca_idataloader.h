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
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#ifndef TPS_GALLERY_IDATALOADER_H_
#define TPS_GALLERY_IDATALOADER_H_

#include "tps_gca_defs.h"

TPS_BEGIN_NAMESPACE
struct CertifiedUnit;
struct CertifiedPoiInfo;
struct CertifiedDoseListInfo;
class DoseLineTemplate;

class ITpsGcaDataLoader {
public:
    /// \brief  Initialize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Initialize() = 0;

    virtual void ClearGalleryManager() = 0;

    virtual void ClearGalleryPatientManager() = 0;

    /// \brief  Finalize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Finalize() = 0;

    /// \brief Load a patient from TMS
    /// \param[in] sPatientUID
    /// \return bool 
    virtual bool LoadPatient(const std::string&) = 0;

    virtual bool LoadSeries(const std::string&, std::vector<std::string>&) = 0;

    virtual bool ReloadSeries(const std::string&, std::vector<std::string>&) = 0;

    virtual bool LoadCoursesOfPatient(const std::string& patientUID, std::vector<std::string>& vCourseUIDs) = 0;

    /// \brief  Load image 3d from TMS
    /// \param[in] sImage3DUID  the image 3d UID in database
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool LoadImage3D(const std::string& sImage3DUID) = 0;
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IDATALOADER_H_
