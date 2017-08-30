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

#ifndef TPS_RC_DEFS_H
#define TPS_RC_DEFS_H

#include "RtTpsFramework/tps_fw_renderdefines.h"

TPS_BEGIN_NAMESPACE

#ifdef TPS_RC_EXPORT
#define TPS_RC_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_RC_DECLSPEC    TPS_IMPORT_CLS
#endif

TPS_END_NAMESPACE

#endif //TPS_RC_DEFS_H
