//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_pl_ct2density_helper.cpp 
///  \brief   ct to denstity alg
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsAlgorithmProxy/tps_pl_ct2density_helper.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

float Ct2DensityHelper::GetDensityFromCTValue(const float fCTValue, const RtCt2density& ct2Density)
{
    int ctValueFloor = static_cast<int>(fCTValue);

    const float *ct2densityArray = ct2Density.get_ct2densitybuffer();
    if (nullptr == ct2densityArray) {
        TPS_LOG_DEV_INFO << "CurrentCT2densityArray is null.";
        return -1e20f;
    }
    if (ctValueFloor <= miMinCTValue){
        TPS_LOG_DEV_WARNING << "Failed to get CT2Density: "<< fCTValue;
        return ct2densityArray[0];
    }
    if (ctValueFloor >= miMaxCTValue){
        TPS_LOG_DEV_WARNING<<"Failed to get CT2Density: "<< fCTValue;
        return ct2densityArray[miMaxCTValue - miMinCTValue];
    }

    int ctValueCeil = ctValueFloor + 1;

    return ct2densityArray[ctValueFloor - miMinCTValue] * (ctValueCeil - fCTValue)
        + ct2densityArray[ctValueCeil - miMinCTValue] * (fCTValue - ctValueFloor);
}
TPS_END_NAMESPACE
