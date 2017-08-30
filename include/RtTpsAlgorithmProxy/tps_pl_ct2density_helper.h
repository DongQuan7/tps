//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_pl_ct2density_helper.h 
///  \brief   ct to denstity alg
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_PL_CT2DENSITY_HELPER_H_
#define TPS_PL_CT2DENSITY_HELPER_H_

#include "tps_ap_defs.h"

TPS_BEGIN_NAMESPACE

class RtCt2density;

class TPS_AP_DECLSPEC Ct2DensityHelper
{
public:
    static float GetDensityFromCTValue(const float fCTValue, const RtCt2density& ct2Density);
private:
    static int const miMaxCTValue = 5000;
    static int const miMinCTValue = -1024;
};
TPS_END_NAMESPACE

#endif
