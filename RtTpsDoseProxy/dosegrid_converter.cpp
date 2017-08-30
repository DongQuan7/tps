//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file dosegrid_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dosegrid_converter.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

TPS_BEGIN_NAMESPACE

bool DosegridConverter::ConvertToALG(
    RtDosegrid& tpsDosegrid, std::shared_ptr<RTFWK::RTFWKDLDODoseGrid>* algDosegrid)
{
    algDosegrid->reset(new RTFWK::RTFWKDLDODoseGrid(false));
    (*algDosegrid)->SetUID(tpsDosegrid.get_uid());
    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(tpsDosegrid.get_grid_to_pat_t());
    (*algDosegrid)->SetGrid_to_pat_T(matG2P);
    Mcsf::Matrix4f matP2G = matG2P.Inverse();
    (*algDosegrid)->SetPat_to_grid_T(matP2G);
    (*algDosegrid)->SetX_count(tpsDosegrid.get_xcount());
    (*algDosegrid)->SetY_count(tpsDosegrid.get_ycount());
    (*algDosegrid)->SetZ_count(tpsDosegrid.get_zcount());
    (*algDosegrid)->SetIsDoseValid(true);
    (*algDosegrid)->SetMax(tpsDosegrid.get_max());
    (*algDosegrid)->SetMin(tpsDosegrid.get_min());

    float* tpsDoseGridBuffer = tpsDosegrid.get_dosegrid_buffer();
    int tpsDoseGridBufferSize = tpsDosegrid.get_xcount() * tpsDosegrid.get_ycount() * tpsDosegrid.get_zcount();
    (*algDosegrid)->SetDoseGridBuffer(tpsDoseGridBuffer, tpsDoseGridBufferSize);
    return true;
}

bool DosegridConverter::ConvertToTPS(RTFWK::RTFWKDLDODoseGrid& algDosegrid, RtDosegrid* tpsDosegrid)
{
    tpsDosegrid->set_uid(algDosegrid.GetUID());
    tpsDosegrid->set_max(algDosegrid.GetMax());
    tpsDosegrid->set_min(algDosegrid.GetMin());
    tpsDosegrid->set_isdosevalid(algDosegrid.GetIsDoseValid());
    float* algDoseGridBuffer = algDosegrid.GetDoseGridBuffer();
    int sizeX = algDosegrid.m_iX_count;
    int sizeY = algDosegrid.m_iY_count;
    int sizeZ = algDosegrid.m_iZ_count;
    int bufferSize = sizeX * sizeY * sizeZ;
    memcpy(tpsDosegrid->get_dosegrid_buffer(), algDoseGridBuffer, sizeof(float)*bufferSize);
    return true;
}

bool DosegridConverter::CreateALGDosegridList(
    std::vector<RtDosegrid*>& tpsDosegridList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDODoseGrid>>* algDosegridList)
{
    std::vector<RtDosegrid*>::iterator itTpsDosegrid = tpsDosegridList.begin();
    for (; itTpsDosegrid != tpsDosegridList.end(); ++itTpsDosegrid)
    {
        RtDosegrid* tpsDosegrid = *itTpsDosegrid;
        std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> algDosegrid;
        DosegridConverter::ConvertToALG(*tpsDosegrid, &algDosegrid);
        algDosegridList->push_back(algDosegrid);
    }
    return true;
}
TPS_END_NAMESPACE