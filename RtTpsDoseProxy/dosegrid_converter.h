//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file dosegrid_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_DOSEGRID_CONVERTER_H_
#define TPS_DP_DOSEGRID_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC DosegridConverter
{
public:
    static bool ConvertToALG(RtDosegrid& tpsDosegrid, std::shared_ptr<RTFWK::RTFWKDLDODoseGrid>* algDosegrid);
    static bool ConvertToTPS(RTFWK::RTFWKDLDODoseGrid& algDosegrid, RtDosegrid* tpsDosegrid);
	
	    static bool CreateALGDosegridList(
        std::vector<RtDosegrid*>& tpsDosegridList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDODoseGrid>>* algDosegridList);
};
TPS_END_NAMESPACE
#endif
