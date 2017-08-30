////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_pl_make_bolus.h
/// 
///  \brief header file for generating bolus 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////

#ifndef TPS_PL_MAKE_BOLUS_H
#define TPS_PL_MAKE_BOLUS_H

#include "tps_defs.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
class ROIEntity;
class TpsImage3DEntity;

TPS_EXPORT_API
void make_bolus(const TpsImage3DEntity& image3D,
                ROIEntity* pVOISkin,
                plunc::CONTOUR_STC* pBeamOutline,
                float matPat2Beam[4][4],
                float matBeam2Pat[4][4],
                float fBolusThickness, 
                float fBorder[4],
                ROIEntity* pVOIBolus);

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_MAKE_BOLUS_H
