//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file goldenstt_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2016/06/29
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "goldenstt_converter.h"
#include "boost/lexical_cast.hpp"
#include "tps_logger.h"
#include "RtTpsDoseProxy/tps_dp_alg_converter_common.h"
TPS_BEGIN_NAMESPACE



void GoldenSTTConverter::ConvertToALG(
    const RtGoldenstt& tpsGoldenStt,
        std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT>* algGoldenSTT)
{
    algGoldenSTT->reset(new RTFWK::RTFWKDLDOGoldenSTT(false));
    (*algGoldenSTT)->SetUID(tpsGoldenStt.get_uid());
    (*algGoldenSTT)->SetDBUID(tpsGoldenStt.get_uid());
    (*algGoldenSTT)->SetAccessoryUID(tpsGoldenStt.get_accessoryuid());
    (*algGoldenSTT)->SetCommissionedunitUID(tpsGoldenStt.get_commissionedunituid());
    (*algGoldenSTT)->SetWedgeFactor(tpsGoldenStt.get_wedgefactor());
    (*algGoldenSTT)->SetWedgeAngle(tpsGoldenStt.get_wedgeangle());
    (*algGoldenSTT)->SetFixJAWPos(tpsGoldenStt.get_fixjawpos());

    // AccumulateMU
    std::vector<float> vFloat;
    if (!tpsGoldenStt.get_accumulatedmu().empty())
    {
        vFloat = GetVecFromString<float>(tpsGoldenStt.get_accumulatedmu());
        (*algGoldenSTT)->SetAccumulatedMU(vFloat);
    }

    // CorrectionFactorPencilBeam
    vFloat.clear();
    if (!tpsGoldenStt.get_correctionfactorpencilbeam().empty())
    {
        vFloat = GetVecFromString<float>(tpsGoldenStt.get_correctionfactorpencilbeam());
        (*algGoldenSTT)->SetCorrectionFactorPencilBeam(vFloat);
    }

    // CorrectionFactorConvolution
    vFloat.clear();
    if (!tpsGoldenStt.get_correctionfactorconvolution().empty())
    {
        vFloat = GetVecFromString<float>(tpsGoldenStt.get_correctionfactorconvolution());
        (*algGoldenSTT)->SetCorrectionFactorConvolution(vFloat);
    }

    // CorrectionFactorMonteCarlo
    vFloat.clear();
    if (!tpsGoldenStt.get_correctionfactormontecarlo().empty())
    {
        vFloat = GetVecFromString<float>(tpsGoldenStt.get_correctionfactormontecarlo());
        (*algGoldenSTT)->SetCorrectionFactorMonteCarlo(vFloat);
    }

    // MoveJawPos
    vFloat.clear();
    if (!tpsGoldenStt.get_movjawpos().empty())
    {
        vFloat = GetVecFromString<float>(tpsGoldenStt.get_movjawpos());
        (*algGoldenSTT)->SetMovJAWPos(vFloat);
    }

    // exppara
    vFloat.clear();
    float* pExpPara = nullptr;
    if (!tpsGoldenStt.get_exppara().empty())
    {
        vFloat = GetVecFromString<float>(tpsGoldenStt.get_exppara());
        pExpPara = new float[vFloat.size()];
        for (int i = 0; i < vFloat.size(); i++)
        {
            pExpPara[i] = vFloat[i];
        }
        (*algGoldenSTT)->SetExpPara(pExpPara);
        DEL_ARRAY(pExpPara);
    }

    // DeviceID Energy Orientation 数据库中没有对应字段 
    // 算法组同事需要从Accessory，CommissionedUnit等表中获取
}

void GoldenSTTConverter::CreateALGList(
    std::vector<RtGoldenstt*>& tpsGoldenSttList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT>>* algGoldenSTTList)
{
    try 
    {
        for (std::vector<RtGoldenstt*>::iterator itGoldenStt = tpsGoldenSttList.begin();
            itGoldenStt != tpsGoldenSttList.end(); ++itGoldenStt)
        {
            RtGoldenstt* tpsGoldenStt = *itGoldenStt;

            if (tpsGoldenStt == nullptr)
            {
                TPS_LOG_DEV_ERROR << "tpsGoldenStt is null";
                return;
            }
            std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT> algGoldenStt;
            GoldenSTTConverter::ConvertToALG(*tpsGoldenStt, &algGoldenStt);
            algGoldenSTTList->push_back(algGoldenStt);
        }
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "fail to convert to RTFWKDLDOGoldenSTT." << ex.what();
        printf("fail to convert to RTFWKDLDOGoldenSTT.\n");
    }
}

TPS_END_NAMESPACE