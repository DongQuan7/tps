//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_defs.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_GCA_DEFS_H
#define TPS_GCA_DEFS_H

#include "tps_common.h"

//protobuf defines
//#include "RtTpsProtoLib/rt_tps_proto_cmn.h"

TPS_BEGIN_NAMESPACE

#ifdef TPS_GCA_EXPORT
#define TPS_GCA_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_GCA_DECLSPEC    TPS_IMPORT_CLS
#endif


TPS_END_NAMESPACE

#endif
