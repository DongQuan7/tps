//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_dosecalculation.cpp
/// 
///  \version 1.0
/// 
///  \date    2013/12/14
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_DEFS_H
#define TPS_DP_DEFS_H

#include "tps_defs.h"

#define DBEXCHANGEALG (10)

TPS_BEGIN_NAMESPACE

#ifdef TPS_DP_EXPORT
#define TPS_DP_DECLSPEC TPS_EXPORT_CLS
#else
#define TPS_DP_DECLSPEC TPS_IMPORT_CLS
#endif

TPS_END_NAMESPACE

#endif