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
#include "kernel_data_list_converter.h"

TPS_BEGIN_NAMESPACE

bool KernelDataListConverter::ConvertToALG(
    const RtKerneldatalist& tpsKernelDataList,
        std::shared_ptr<RTFWK::RTFWKDLDOKernelDataList>* algKernelDataList)
{
    algKernelDataList->reset(new RTFWK::RTFWKDLDOKernelDataList(false));
    int m_iNumOfPhi  = tpsKernelDataList.get_numofphi();
    int m_iNumOfR = tpsKernelDataList.get_numofr();
    (*algKernelDataList)->SetUID(tpsKernelDataList.get_uid());
    //(*algKernelDataList)->SetKernelData(tpsKernelDataList.get)
    (*algKernelDataList)->SetNumOfPhi(m_iNumOfPhi);
    (*algKernelDataList)->SetNumOfR(m_iNumOfR);
    std::vector<std::string> vecValue;
    std::string strKernelData = tpsKernelDataList.get_phidata();
    (void)boost::split(vecValue,strKernelData, boost::is_any_of(","));
    float* m_pPhiData = new float[m_iNumOfPhi];
    for (int i = 0; i < m_iNumOfPhi && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_pPhiData[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    (*algKernelDataList)->SetPhiData(m_pPhiData);
    std::string strRData = tpsKernelDataList.get_rdata();
    (void)boost::split(vecValue, strRData, boost::is_any_of(","));
    float* m_pRData = new float[m_iNumOfR];
    for (int i = 0; i < m_iNumOfR && i < vecValue.size();i++)
    {
        if (!vecValue[i].empty())
        {
            m_pRData[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    (*algKernelDataList)->SetRData(m_pRData);

    delete [] m_pPhiData;
    delete [] m_pRData;

    return true;
}

bool KernelDataListConverter::CreateALGList(
    std::vector<RtKerneldatalist*>& tpsKernelDataListList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelDataList>>* algKernelDataListList)
{
    std::vector<RtKerneldatalist*>::iterator itTpsKernelDataList = tpsKernelDataListList.begin();
    for (; itTpsKernelDataList != tpsKernelDataListList.end(); ++itTpsKernelDataList)
    {
        RtKerneldatalist* tpsKernelDataList = *itTpsKernelDataList;
        std::shared_ptr<RTFWK::RTFWKDLDOKernelDataList> algKernelDataList;
        KernelDataListConverter::ConvertToALG(*tpsKernelDataList, &algKernelDataList);
        algKernelDataListList->push_back(algKernelDataList);
    }
    return true;
}

TPS_END_NAMESPACE