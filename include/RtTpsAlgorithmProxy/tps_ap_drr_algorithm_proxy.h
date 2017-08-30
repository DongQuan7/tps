////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_ap_drr_algorithm_proxy.h
/// 
///  \brief class DrrAlgorithmProxy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/28
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_DRR_ALGORITHM_PROXY_H_
#define TPS_AP_DRR_ALGORITHM_PROXY_H_

#include "tps_ap_defs.h"

namespace Mcsf{
    class Matrix4f;
}

namespace RTALG{
    struct DRRGeneratorParam;
}

//namespace RTFWK{
//    class RTFWKDLDOMachine;
//    class RTFWKDLDORTImage;
//}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class RtRtimage;
class RtMachine;
class TpsImage3DEntity;

/// \class DrrAlgorithmProxy
/// 
/// \brief *****
class TPS_AP_DECLSPEC DrrAlgorithmProxy{
public:
    //////////////////////////////////////////////////////////////////////////
    /// \brief  generate drr image
    /// \param[in] drrParam  drr parameters
    /// \param[out] image  the output drr image buffer
    /// \param[out] windowLevel  output window level
    /// \return bool  true if succeeded, false otherwise
    static bool GenerateDrrImage(RTALG::DRRGeneratorParam& drrParam, 
        unsigned short *image, unsigned int* windowLevel);

    //////////////////////////////////////////////////////////////////////////
    /// \brief  generate drr RT image for specified planning beam or treatment beam
    /// \param[in] image  image 3d entity 
    /// \param[in] machine  machine the beam based on
    /// \param[in] beamUID  beam uid in database
    /// \param[in] isoCenterUID  iso-center point uid in database
    /// \param[in] ct2Density  CT to density map, CT value range is [-1025, 5000]
    /// \param[in] matBeam2Volume  transform matrix from beam view to CT volume
    /// \param[in] energy  beam energy, unit MU
    /// \param[in] gantryAngle  gantry angle(degree)
    /// \param[out] rtImage  output RT image
    /// \param[in] isForTreatmenBeam  flag indicates for planning beam or treatment beam
    static bool GenerateDrrRtImage(
        const TpsImage3DEntity* image, 
        const RtMachine* machine, 
        const std::string& beamUID, 
        const std::string& isoCenterUID, 
        float* ct2Density, 
        const Mcsf::Matrix4f& matBeam2Volume,
        float energy, float gantryAngle, 
        RtRtimage*& rtImage, 
        unsigned int width, unsigned int height,
        bool isForTreatmentBeam = false);
};

TPS_END_NAMESPACE  // end namespace tps

#endif