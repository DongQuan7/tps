//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file beam_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_KERNEL_DATA_CONVERTER_H_
#define TPS_DP_KERNEL_DATA_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_kerneldata.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_kerneldata.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC KernelDataConverter
{
public:
    static bool ConvertToALG(
        const RtKerneldata& tpsKernelData,
        std::shared_ptr<RTFWK::RTFWKDLDOKernelData>* algKernelData);

    static bool CreateALGList(
        std::vector<RtKerneldata*>& tpsKernelDataList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelData>>* algKernelDataList);
};
TPS_END_NAMESPACE
#endif