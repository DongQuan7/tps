//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file image_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/19
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_IMAGE_CONVERTER_H_
#define TPS_DP_IMAGE_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_image3d.h"

TPS_BEGIN_NAMESPACE;

class TpsImage3DEntity;

class TPS_DP_DECLSPEC Image3dConverter
{
public:
    static bool ConvertToALG(const TpsImage3DEntity& image3d, std::shared_ptr<RTFWK::RTFWKDLDOImage3D>* algImage3d);
    //static bool ConvertToTPS(const RTFWK::RTFWKDLDOImage3D& algImage3d, TpsImage3DEntity* image3d);
};
TPS_END_NAMESPACE
#endif