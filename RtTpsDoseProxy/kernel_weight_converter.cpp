//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file beam_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "kernel_weight_converter.h"
#include "RtTpsAlgorithmProxy\tps_ap_utility.h"

TPS_BEGIN_NAMESPACE

bool KernelWeightConverter::ConvertToALG(
    const RtKernelweight& tpsKernelWeight,
    std::shared_ptr<RTFWK::RTFWKDLDOKernelWeight>* algKernelWeight)
{
    algKernelWeight->reset(new RTFWK::RTFWKDLDOKernelWeight(false));
    (*algKernelWeight)->SetUID(tpsKernelWeight.get_uid());
    (*algKernelWeight)->SetFieldSize(tpsKernelWeight.get_fieldsize());

    std::vector<int> kernelWeightidList;
    TpsUtility::StringToVector(tpsKernelWeight.get_kernelidlist(), kernelWeightidList);
    (*algKernelWeight)->SetKernelIDList(kernelWeightidList);

    std::vector<float> kernelWeightList;
    TpsUtility::StringToVector(tpsKernelWeight.get_kernelweightlist(), kernelWeightList);
    (*algKernelWeight)->SetKernelWeightList(kernelWeightList);

    (*algKernelWeight)->SetCommissionedUnitUID(tpsKernelWeight.get_commissionedunituid());

    return true;
}

bool KernelWeightConverter::CreateALGList(
    std::vector<RtKernelweight*>& tpsKernelWeightList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelWeight>>* algKernelWeightList)
{
    std::vector<RtKernelweight*>::iterator itTpsKernelWeight = tpsKernelWeightList.begin();
    for (; itTpsKernelWeight != tpsKernelWeightList.end(); ++itTpsKernelWeight)
    {
        RtKernelweight* tpsKernelWeight = *itTpsKernelWeight;
        std::shared_ptr<RTFWK::RTFWKDLDOKernelWeight> algKernelWeight;
        KernelWeightConverter::ConvertToALG(*tpsKernelWeight, &algKernelWeight);
        algKernelWeightList->push_back(algKernelWeight);
    }
    return true;
}

TPS_END_NAMESPACE