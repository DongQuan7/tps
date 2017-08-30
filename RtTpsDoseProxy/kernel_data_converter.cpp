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
#include "kernel_data_converter.h"

TPS_BEGIN_NAMESPACE

bool KernelDataConverter::ConvertToALG(
     const RtKerneldata& tpsKernelData,
        std::shared_ptr<RTFWK::RTFWKDLDOKernelData>* algKernelData)
{
    algKernelData->reset(new RTFWK::RTFWKDLDOKernelData(false));
    (*algKernelData)->SetUID(tpsKernelData.get_uid());
    (*algKernelData)->SetEnergyOfBeam(tpsKernelData.get_energyofbeam());
    std::vector<float> kerneldata = tpsKernelData.get_kerneldata_list();
    float* algdata = new float[kerneldata.size()];
    int i = 0;
    for (std::vector<float>::iterator it = kerneldata.begin(); it != kerneldata.end(); ++it)
    {
        algdata[i++] = *it;
    }
    (*algKernelData)->SetKernelDataLength(tpsKernelData.get_kerneldatalength());
    (*algKernelData)->SetKernelData(algdata);
    (*algKernelData)->SetKernelDataID(tpsKernelData.get_kerneldataid());

    delete [] algdata;
    return true;
}

bool KernelDataConverter::CreateALGList(
    std::vector<RtKerneldata*>& tpsKernelDataList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelData>>* algKernelDataList)
{
    std::vector<RtKerneldata*>::iterator itTpsKernelData = tpsKernelDataList.begin();
    for (; itTpsKernelData != tpsKernelDataList.end(); ++itTpsKernelData)
    {
        RtKerneldata* tpsKernelData = *itTpsKernelData;
        std::shared_ptr<RTFWK::RTFWKDLDOKernelData> algKernelData;
        KernelDataConverter::ConvertToALG(*tpsKernelData, &algKernelData);
        algKernelDataList->push_back(algKernelData);
    }
    return true;
}

TPS_END_NAMESPACE