//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_defs.h
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_PRCA_DEFS_H_
#define TPS_PRCA_DEFS_H_

#include "tps_common.h"

TPS_BEGIN_NAMESPACE

#ifdef TPS_PRCA_EXPORT
#define TPS_PRCA_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_PRCA_DECLSPEC    TPS_IMPORT_CLS
#endif


TPS_END_NAMESPACE

#endif //TPS_PRCA_DEFS_H_
