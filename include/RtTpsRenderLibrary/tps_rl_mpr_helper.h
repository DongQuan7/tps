////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mpr_helper.h
/// 
///  \brief class MPR helper declaration 
/// 
///  \version 1.0
/// 
///  \date    Sep.21,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPR_HELPER_H_
#define TPS_RL_MPR_HELPER_H_

#include "tps_rl_defs.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace TPS

class MPRGraphicObject;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;
typedef std::shared_ptr<MPRGraphicObject> MprGOPtr;

class TPS_RL_DECLSPEC TpsMprHelper
{
public:
    static bool GetMinThickness(
        const VolumeGOPtr& spVolumeGO, 
        const MprGOPtr& spMPRGO, 
        double& dThicknessInWorld);

    enum PlaneTypeInVol
    {
        X = 0,
        Y,
        Z,
        Oblique
    };
    //get current slice number, total slice number & plane type, 
    //which indicates how the plane locates in volume c.s.; 
    //note that the slice number starts from 1.
    static bool GetSliceNumber(const VolumeGOPtr& spVolumeGO, 
                               const MprGOPtr& spMPRGO, 
                               int& iCurrentSliceNumber, 
                               int& iTotalSliceNumber, 
                               PlaneTypeInVol& ePlaneType);
    //the slice number increment direction is view direction
    static bool GetSliceNumberInViewOrder(const VolumeGOPtr& spVolumeGO, 
                                          const MprGOPtr& spMPRGO, 
                                          int& iCurrentSliceNumber, 
                                          int& iTotalSliceNumber);

    static bool CalcSpacingInWorld(const VolumeGOPtr& spVolumeGO, 
                                   const MprGOPtr& spMPRGO,
                                   double& dSpacing);
};

TPS_END_NAMESPACE  // end namespace TPS

#endif  //TPS_RL_MPR_HELPER_H_
