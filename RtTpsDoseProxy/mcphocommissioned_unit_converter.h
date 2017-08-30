//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file mcphocommissioned_unit_converter.h
/// 
///  \version 1.0
/// 
///  \date    2016/03/17
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_MCPHOCOMMISSIONED_UNIT_CONVERTER_H_
#define TPS_DP_MCPHOCOMMISSIONED_UNIT_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mcphocommissionedunit.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_mcphocommissionedunit.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"


TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC McPhoCommissionedUnitConverter
{
public:
    static void ConvertToALG(
        const RtMcphocommissionedunit& tpsMcphocommissionedunit,
        std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit>* algMcPhoCommissionedUnit);

    static void CreateALGList(
        std::vector<RtMcphocommissionedunit*>& tpsMcphocommissionedunitList,
        ITpsDataRepository *mDataRepository,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit>>* algMcPhoCommissionedUnitList);

private:

    static void FillMcPhoAbsCorrectionFactor_i(
        std::vector<RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor*> &vCorrectFac, 
        std::string sCorrectorFactors);

    static void FillEasyOffAxis_i(
        std::vector<RTFWK::RTFWKDLDOEasyOffAxis*> &vEasyOffAxis,
        std::string sEasyOffAxis);

    static void FillMcPhoLeafMapList_i(
        std::vector<RTFWK::RTFWKDLDOMcPhoLeafMap*> &vMcPhoLeafMap,
        std::string sMcPhoLeafMap);
   
};
TPS_END_NAMESPACE
#endif