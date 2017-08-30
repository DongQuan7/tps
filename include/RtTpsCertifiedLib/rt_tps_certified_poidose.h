////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan  xiaoqing.shangguan@united-imaging.com
/// 
///  \file rt_tps_certified_poidose.h
/// 
///  \brief struct CertifiedPoi implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/05
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIED_POIDOSE_H_
#define RT_TPS_CERTIFIED_POIDOSE_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

struct CertifiedPoiDose {
    std::string     m_beamUid;
    float           m_beamMU;
    int             m_normgroupFraction;
};
TPS_END_NAMESPACE


#endif