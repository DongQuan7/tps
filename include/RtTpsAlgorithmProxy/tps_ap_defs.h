//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_object.h
///  \brief   top base class for backend
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_AP_DEFS_H_
#define TPS_AP_DEFS_H_

#include "tps_defs.h"
//
#include <vector>
TPS_BEGIN_NAMESPACE

#ifdef TPS_AP_EXPORT
#define TPS_AP_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_AP_DECLSPEC    TPS_IMPORT_CLS
#endif


TPS_END_NAMESPACE

#endif //TPS_AP_DEFS_H_
