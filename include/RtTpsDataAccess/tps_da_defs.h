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

#ifndef TPS_DATAACCESS_DEFS_H
#define TPS_DATAACCESS_DEFS_H

#include "tps_defs.h"

//protobuf defines
//#include "RtTpsProtoLib/rt_tps_proto_cmn.h"
#include <map>
#include <vector>

TPS_BEGIN_NAMESPACE

#ifdef TPS_DA_EXPORT
#define TPS_DA_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_DA_DECLSPEC    TPS_IMPORT_CLS
#endif


TPS_END_NAMESPACE

#endif //TPS_DATAACCESS_DEFS_H
