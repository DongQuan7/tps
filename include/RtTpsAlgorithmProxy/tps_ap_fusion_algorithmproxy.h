////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_ap_fusion_algorithmproxy.h
/// 
///  \brief class FusionAlgorithmProxy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_FUSION_ALGORITHMPROXY_H_
#define TPS_AP_FUSION_ALGORITHMPROXY_H_

#include "tps_ap_defs.h"

namespace Mcsf{
    class Point3f;
    class Matrix4f;
    class PARAMETER_t;
    class CVolumeDataInfo;
    class IProgress;
};


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsImage3DEntity;

enum REG_CARED_TISSUE
{
    BONE_TISSUE = 1,
    SOFT_TISSUE,
};

/// \class FusionAlgorithmProxy
/// 
/// \brief *****
class TPS_AP_DECLSPEC FusionAlgorithmProxy{
public:
    static bool Register(const TpsImage3DEntity& fixedImage, 
                         const TpsImage3DEntity& floatImage, 
                         Mcsf::Matrix4f& registrationMatrix);

    //////////////////////////////////////////////////////////////////////////
    /// \brief  get a default registration matrix
    /// \param[in] fixedImage  the fixed image entity
    /// \param[in] floatImage  the float image entity
    /// \param[out] matrix  the output registration matrix, fixed to float in DICOM patient c.s.
    /// \return bool  true if succeeded, false otherwise
    static bool Register_InitSetup(const TpsImage3DEntity& fixedImage, 
                                   const TpsImage3DEntity& floatImage, 
                                   Mcsf::Matrix4f& registrationMatrix);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief  points registration
    /// \param[in] fixedPoints  the points selected from fixed image in DICOM patient c.s.
    /// \param[in] floatPoints  the points selected from float image in DICOM patient c.s.
    /// \param[in] pairCount  the point pair count
    /// \param[out] matrix  the output registration matrix, fixed to float in DICOM patient c.s.
    /// \return bool  true if succeeded, false otherwise
    static bool Register_Landmark(const Mcsf::Point3f* fixedPoints, 
                                  const Mcsf::Point3f* floatPoints, 
                                  unsigned int pairCount, 
                                  Mcsf::Matrix4f& matrix);

    //////////////////////////////////////////////////////////////////////////
    /// \brief  rigid registration
    /// \param[in] fixedImage  the fixed image entity
    /// \param[in] floatImage  the float image entity
    /// \param[in] fixedRoiMask  the fixed image's registration region of interested
    /// \param[in] floatRoiMask  the float image's registration region of interested
    /// \param[in] caredTissue  the tissue type that registration cared, BONE_TISSUE or SOFT_TISSUE
    /// \param[out] progress  the registration progress value
    /// \param[in & out] matrix  the registration matrix, fixed to float in DICOM patient c.s.
    /// \param[in] iterativeReg  the flag indicates if do registration based on existing result
    /// \return bool  true if succeeded, false otherwise
    static bool Register_Alignment(/*const */TpsImage3DEntity& fixedImage, 
                                   /*const */TpsImage3DEntity& floatImage, 
                                   unsigned char* fixedRoiMask,
                                   unsigned char* floatRoiMask,
                                   REG_CARED_TISSUE caredTissue,
                                   //Mcsf::PARAMETER_t* parameter,
                                   Mcsf::IProgress* progress,
                                   Mcsf::Matrix4f& matrix, 
                                   bool iterativeReg = false);

    //////////////////////////////////////////////////////////////////////////
    /// \brief  non_rigid registration
    /// \param[in] fixedImage  the fixed image entity
    /// \param[in] floatImage  the float image entity
    /// \param[in] fixedRoiBoundingBox  the fixed image's registration region of interested, 
    ///            6 values{minX, maxX, minY, maxY} 
    /// \param[in] floatRoiBoundingBox  the float image's registration region of interested, 
    ///            6 values{minX, maxX, minY, maxY} 
    /// \param[out] progress  the registration progress value
    /// \param[out] resultFloatBuffer  the result float volume buffer, same size with the fixed volume
    /// \return bool  true if succeeded, false otherwise
    static bool Register_NonRigid(/*const */TpsImage3DEntity& fixedImage, 
                                  /*const */TpsImage3DEntity& floatImage, 
                                  unsigned int* fixedRoiBoundingBox,
                                  unsigned int* floatRoiBoundingBox,
                                  Mcsf::IProgress* progress,
                                  void* resultMovBuffer);
};

TPS_END_NAMESPACE  // end namespace tps

#endif