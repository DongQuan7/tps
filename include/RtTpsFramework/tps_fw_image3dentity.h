////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_da_image3dentity.h
/// 
///  \brief class TpsImage3DEntity declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/24
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_IMAGE3DENTITY_H_
#define TPS_DA_IMAGE3DENTITY_H_

#include "tps_fw_defs.h"
//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#include <vector>
#include <map>
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_imagetransformation.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"

namespace Mcsf{
    class McsfAlgoAutoContour;
    class Point2f;
    class Point3f;
    class Matrix3f;
    class Matrix4f;
    class Vector2f;
    class Vector3f;
    class Vector4f;
    class Quat;

    namespace MedViewer3D{
        class Point2D;
        class Point3D;
        class Matrix4x4;
        class Vector2D;
        class Vector3D;
        class Quaternion;
    }
}

namespace McsfGeometry
{
    class SubVolume;
}

TPS_BEGIN_NAMESPACE;  // begin namespace tps

class RtSeries;
class RtImage3D;
class RtImage3DHeader;
struct TpsSeriesGroup4d;

/// \class TpsImage3DEntity
/// 
/// \brief an encapsulation to RTFWK::RTFWKDLDOImage3D
class TPS_FW_DECLSPEC TpsImage3DEntity{
public:
    //the constructor and destructor
    TpsImage3DEntity();
    TpsImage3DEntity(const TpsImage3DEntity* pSrcImage3DEntity, bool bDeepCopy);
    ~TpsImage3DEntity();

    //////////////////////////////////////////////////////////////////////////
    // \brief setup TMS image 3d data
    // \param[in] series  the series pointer of database
    // \return bool true if succeeded, false otherwise
    bool SetupImage3D(RtSeries* const series);

    //////////////////////////////////////////////////////////////////////////
    // \brief setup TMS image 3d data
    // \param[in] image3D  the image 3d pointer of database
    // \return bool true if succeeded, false otherwise
    //bool SetupImage3D(RtImage3D* image3D);

    //////////////////////////////////////////////////////////////////////////
    RtSeries* GetRtSeries() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image 3d header
    // \param[out] header  the header pointer output
    // \return bool true if succeeded, false otherwise
    bool GetImage3DHeader(RtImage3DHeader*& header) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image data buffer
    // \param[out] buffer  the buffer pointer output
    // \return bool true if succeeded, false otherwise
    bool GetImage3DBuffer(char*& buffer) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief          Get ImageUID By Index
    ///                  Index from 0 to m_iSliceCount-1
    ///                  if not exist, return ""
    ///
    ///  \param[in]      const int& iIndex
    ///
    ///  \param[out]     None
    ///
    ///  \return         std::string
    /////////////////////////////////////////////////////////////////
    std::string GetImageUidByIndex(const int& iIndex) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image 3d UID
    // \return std::string if succeeded, false empty string
    std::string GetUID() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set the VOI UID list of image 3d data
    // \param[in] vecVOI  the VOI UID list input
    // \return bool true if succeeded, false otherwise
    //bool SetVOIList (/*const */std::vector<std::string>& vecVOI);

    bool AddPOI(const std::string& poiUid);

    bool GetPOIList(std::vector<std::string>& poiList) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set setup point
    // \param[in] point  the setup point to be set
    // \return bool true if succeeded, false otherwise
    // \note setup point is a 3d reference point in DICOM patient c.s., which
    //       specifies the location of the machine isocenter in the patient-based
    //       coordinate system with the frame of reference.
    bool SetSetupPoint(const Mcsf::Point3f& point);

    //////////////////////////////////////////////////////////////////////////
    // \brief get setup point
    // \param[out] point  the setup point output
    // \return bool true if succeeded, false otherwise
    // \note setup point is a 3d reference point in DICOM patient c.s., which
    //       specifies the location of the machine isocenter in the patient-based
    //       coordinate system with the frame of reference.
    Mcsf::Point3f* GetSetupPoint() const;


    //////////////////////////////////////////////////////////////////////////
    // \brief set registration matrix through specified image 3d UID & matrix
    // \param[in] vTransformation  the registration transfromation.
    // \return bool true if succeeded, false otherwise
    bool SetImagetransformation(RtImagetransformation* & vTransformation);
    //////////////////////////////////////////////////////////////////////////
    // \brief set registration matrix through specified image 3d UID & matrix
    // \param[in] sUID  the float image data UID
    // \param[in] matrix  the registration matrix, float to fixed in DICOM patient c.s.
    // \return bool true if succeeded, false otherwise
    bool SetRegistrationMatrix(const std::string& sUID, const Mcsf::Matrix4f& matrix);

    //////////////////////////////////////////////////////////////////////////
    // \brief get registration matrix via specified image 3d UID
    // \param[in] sUID  the float image data UID
    // \param[out] matrix  the registration matrix output, float to fixed in DICOM patient c.s.
    // \return bool true if succeeded, false otherwise
    bool GetRegistrationMatrix(const std::string& sUID, Mcsf::Matrix4f& matrix) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get registration uid via specified image 3d UID
    // \param[in] sUID  the float image data UID
    // \param[out] matrix  the registration matrix output, float to fixed in DICOM patient c.s.
    // \return bool true if succeeded, false otherwise
    bool GetImageTransformationUid(const std::string& sFloatUID, std::string& sUID) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief remove registration matrix via specified image 3d UID
    // \param[in] sUID  the float image data UID
    // \return bool true if succeeded, false otherwise
    bool RemoveImagetransformation(const std::string& sUID);

    //////////////////////////////////////////////////////////////////////////
    // \brief get all registration matrix of this RtImagetransformation(fixed)
    // \return all matrix pair <floatUID, RtImagetransformation*>
    const std::map<std::string, RtImagetransformation*>& GetAllImagetransformation() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief generate empty deformable registration result volume buffer
    //        all element will set to 0, the size of the result buffer is 
    //        same to the original volume buffer
    // \return void* reuslt volume buffer
    void* GenerateEmptyDeformableRegistrationResultBuffer();

    //////////////////////////////////////////////////////////////////////////
    // \brief set deformable registration result
    // \param[in] sUID  the float image data UID
    // \param[in] buffer  the deformable registration result volume buffer
    void SetDeformableRegistrationResult(const std::string& sUID, void* buffer);

    //////////////////////////////////////////////////////////////////////////
    // \brief get deformable registration result via specified image 3d UID
    // \param[in] sUID  the float image data UID
    // \param[out] buffer  the deformable registration result volume buffer
    // \return bool true if succeeded, false otherwise
    bool GetDeformableRegistrationResult(const std::string& sUID, void*& buffer);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove deformable registration result via specified image 3d UID
    // \param[in] sUID  the float image data UID
    // \return bool true if succeeded, false otherwise
    void RemoveDeformableRegistrationResult(const std::string& sUID);

    //////////////////////////////////////////////////////////////////////////
    // \brief get transformation matrix from DIOM patient c.s. to IEC patient c.s.
    // \param[out] matrix  the transformation matrix output
    // \return bool true if succeeded, false otherwise
    //bool GetDICOMPatientToIECPatientMatrix(Mcsf::Matrix4f& matrix) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get transformation matrix from volume c.s. to DICOM patient c.s.
    // \param[out] matrix  the transformation matrix output
    // \return bool true if succeeded, false otherwise
    bool GetVolumeToPatientMatrix(Mcsf::Matrix4f& matrix) const;

    inline MAT4 GetVolumeToPatient() const { return mMatV2P; }

    // \brief get a slice buffer from Image3DEntity
    // \return std::string  the sliceBuffer
    unsigned char* GetSliceBuffer(){ return mSliceBuffer;}


    // \brief Initialize a slice buffer for Image3DEntity
    // \in pageturn page turn direction, default = 0
    // \return bool true for [success], false for [failure]
    bool InitSliceBuffer(int pageturn = 0);

    bool GetIsQACopied();

    //////////////////////////////////////////////////////////////////////////
    /// \brief get a pointer of AutoContour
    /// \return Mcsf::McsfAlgoAutoContour*
    Mcsf::McsfAlgoAutoContour* GetAutoContour() const;

    void CreateAutoContour();
    bool InitializationAutoContour(McsfGeometry::SubVolume* pSkin = nullptr);

    bool SetRtSeries(RtSeries*  pSeries)
    {
        mSeries = pSeries;
        return true;
    }
    bool InImage(float xInPat, float yInPat, float zInPat) const
    {
        if (xInPat < m_fXminInPat || xInPat > m_fXmaxInPat)
            return false;
        if (yInPat < m_fYminInPat || yInPat > m_fYmaxInPat)
            return false;
        if (zInPat < m_fZminInPat || zInPat > m_fZmaxInPat)
            return false;
        return true;
    }

    void GetBoundaryInDosegrid(Mcsf::Matrix4f matP2G, int& lbx, int& lby, int& lbz,
        int& rtx, int& rty, int& rtz) const;

    std::string GetPatientName() const;

    // CT 4D group
    void SetSeriesGroup4d(std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d) 
    {
        m_wpSeriesGroup4d = spSeriesGroup4d;
    }

    std::shared_ptr<TpsSeriesGroup4d> GetSeriesGroup4d()
    {
        return m_wpSeriesGroup4d.lock();
    }

    std::string GetGroupUid();

    //bool BelongsToGroup4d()
    //{
    //    std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = m_wpSeriesGroup4d.lock();
    //    if (spSeriesGroup4d == nullptr)
    //        return false;
    //    return spSeriesGroup4d->Contains();
    //}

    inline int GetStartSliceNumber() const { return mStartSliceNumber; }

    void GetDimension(size_t* dim) const;
    void GetSpacing(double* spacing) const;
private:

    void SetDefaultSetupPoint_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief setup volume to patient transform matrix
    void SetupVolumeToPatient_i();

    void CorrectWindowLevel_i();
    //////////////////////////////////////////////////////////////////////////
    // \brief setup DICOM patient to IEC patient transform matrix
    //void SetupDICOMPatientToIECPatient_i();
private:
    RtSeries*                       mSeries;
    Mcsf::McsfAlgoAutoContour*      mAutoContour;//for auto segmentation
    bool                            mIsCopied;
    std::string                     mCourseUID;

    Mcsf::Matrix4f*                        mVolumeToDICOMPatient;
    MAT4 mMatV2P;
    std::map<std::string, RtImagetransformation*>  mImagetransformationMap;
    //TODO move to DB structure
    std::map<std::string, void*>    mDeformableRegistrationResultMap;

    unsigned char*                  mSliceBuffer;
    int                             mTurnStepSum;
    std::string                     mPatientName;
    Mcsf::Point3f*                  mOriginalPoint;
    int                             mStartSliceNumber;

    double                          mSpacing[3];
    // bounding box of image in DICOM patient coordinate
    // so far, used in POI by Lisong
    float m_fXminInPat;
    float m_fXmaxInPat;
    float m_fYminInPat;
    float m_fYmaxInPat;
    float m_fZminInPat;
    float m_fZmaxInPat;

    // CT 4D group
    std::weak_ptr<TpsSeriesGroup4d> m_wpSeriesGroup4d;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsImage3DEntity);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
