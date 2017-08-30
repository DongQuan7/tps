//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file voi_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "voi_converter.h"

#include "contour_converter.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

TPS_BEGIN_NAMESPACE

bool VoiConverter::ConvertToALG(const RtVoi& tpsVoi, std::shared_ptr<RTFWK::RTFWKDLDOVOI>* algVoi)
{
    algVoi->reset(new RTFWK::RTFWKDLDOVOI(false));
    (*algVoi)->SetVOIUID(tpsVoi.get_uid());
    (*algVoi)->SetVOIRole((RTFWK::VOIROLE)tpsVoi.get_voirole());
    //(*algVoi)->SetSubRole((RTFWK::SUBROLE_TYPE)tpsVoi.get_subrole());//wj-20170824-TPS的subRole改BiologyRole，ZJJ说算法组不需要该字段
    (*algVoi)->SetPlanningRole((RTFWK::PLANNING_ROLE_TYPE)tpsVoi.get_planningrole());
    (*algVoi)->SetName(tpsVoi.get_name());
    (*algVoi)->SetAverageDensity(tpsVoi.get_averagedensity());
    (*algVoi)->SetMinDensity(tpsVoi.get_mindensity());
    (*algVoi)->SetMaxDensity(tpsVoi.get_maxdensity());
    (*algVoi)->SetIsOverride(tpsVoi.get_isoverride());
    //(*algVoi)->SetOverrideDenstiy(tpsVoi.get_overridedenstiy());
    (*algVoi)->SetOverrideDenstiy(tpsVoi.get_density());// wzd-2017-04-01 物理密度改电子密度

    Mcsf::Matrix4f matP2V = TpsArithmeticConverter::ConvertToMatrix4f(tpsVoi.get_pat2volumematrix());
    (*algVoi)->SetPat2VolumeMatrix(&matP2V);

    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOContour>> algContourCarrier;
    std::vector<RtContour*> vTpsContours = tpsVoi.get_contours();
    ContourConverter::CreateALGContourList(vTpsContours, &algContourCarrier);
    std::vector<RTFWK::RTFWKDLDOContour*> algContourList;
    for (int i = 0; i < algContourCarrier.size(); ++i)
    {
        algContourList.push_back(algContourCarrier[i].get());
    }
    (*algVoi)->SetVectorContour(algContourList); 

    //todo:
    //algVoi->SetReferenceSubVolume();
    return true;
}

bool VoiConverter::ConvertToTPS(RTFWK::RTFWKDLDOVOI& algVoi, RtVoi* tpsVoi)
{
    tpsVoi->set_uid(algVoi.GetVOIUID());
    //tpsVoi->set_tcpntcp(algVoi.GetTcpNtcp());
    // use beam, ng uid get dose
    // 
    return true;
}

bool VoiConverter::CreateALGVoiList(
    std::vector<RtVoi*>& tpsVoiList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>>* algVoiList)
{
    std::vector<RtVoi*>::iterator itTpsVoi = tpsVoiList.begin();
    for (; itTpsVoi != tpsVoiList.end(); ++itTpsVoi)
    {
        RtVoi* tpsVoi = *itTpsVoi;
        std::shared_ptr<RTFWK::RTFWKDLDOVOI> algVoi;
        VoiConverter::ConvertToALG(*tpsVoi, &algVoi);
        algVoiList->push_back(algVoi);
    }
    return true;
}

bool VoiConverter::ConvertToTPSVoiList(
    std::vector<RTFWK::RTFWKDLDOVOI*>& algVoiList,
    std::vector<RtVoi*>* tpsVoiList)
{
    std::vector<RtVoi*>::iterator itVoi = tpsVoiList->begin();
    for (; itVoi != tpsVoiList->end(); ++itVoi)
    {
        RtVoi* tpsVoi = *itVoi;
        std::vector<RTFWK::RTFWKDLDOVOI*>::iterator itAlgVoi = algVoiList.begin();
        for (; itAlgVoi != algVoiList.end(); ++itAlgVoi)
        {
            RTFWK::RTFWKDLDOVOI* algVoi = *itAlgVoi;
            if (tpsVoi->get_uid() == algVoi->GetVOIUID())
            {
                VoiConverter::ConvertToTPS(*algVoi, tpsVoi);
                break;
            }
        }
    }
    return true;
}
TPS_END_NAMESPACE