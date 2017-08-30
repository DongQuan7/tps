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
#include "accessory_chunk_converter.h"
#include "contour_converter.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "boost/lexical_cast.hpp"
#include "RtTpsDoseProxy/tps_dp_alg_converter_common.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE



void AccessoryChunkConverter::FillMcPhoOffAxis_i(
    std::vector<RTFWK::RTFWKDLDOMcPhoOffAxis*> &vMcPhoOffAxis,
    std::string sMcPhoOffAxis)
{
    std::map<float, float> mMcPhoOffAxis = GetMapFromString<float, float>(sMcPhoOffAxis);
    vMcPhoOffAxis.resize(mMcPhoOffAxis.size(), NULL);
    int i = 0;
    for (auto iter = mMcPhoOffAxis.begin(); iter != mMcPhoOffAxis.end(); ++iter, ++i)
    {
        vMcPhoOffAxis[i] = new RTFWK::RTFWKDLDOMcPhoOffAxis;
        vMcPhoOffAxis[i]->m_fradius = iter->first;
        vMcPhoOffAxis[i]->m_fvalue = iter->second;
    }
}

bool AccessoryChunkConverter::ConvertToALG(
    const RtAccessorychunk& tpsAccessorychunk,
    const RtContour* profile,
    std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk>* algAccessorychunk)
{
    algAccessorychunk->reset(new RTFWK::RTFWKDLDOAccessorychunk(false));
    (*algAccessorychunk)->SetUID(tpsAccessorychunk.get_uid());
    (*algAccessorychunk)->SetAccessoryUID(tpsAccessorychunk.get_accessoryuid());
    (*algAccessorychunk)->SetCommissionedUnitUID(tpsAccessorychunk.get_commissionedunituid());
    (*algAccessorychunk)->SetMu(tpsAccessorychunk.get_mu());
    (*algAccessorychunk)->SetMu_dx(tpsAccessorychunk.get_mu_dx());
    (*algAccessorychunk)->SetMu_dr(tpsAccessorychunk.get_mu_dr());
    (*algAccessorychunk)->SetMu_da(tpsAccessorychunk.get_mu_da());
    (*algAccessorychunk)->SetMu_dv(tpsAccessorychunk.get_mu_dv());
    (*algAccessorychunk)->SetHvl_slope(tpsAccessorychunk.get_hvl_slope());
    (*algAccessorychunk)->SetDensity(tpsAccessorychunk.get_density());
    (*algAccessorychunk)->SetDataType(tpsAccessorychunk.get_datatype());
    (*algAccessorychunk)->setAlgorithmType(tpsAccessorychunk.get_algorithmtype());
    (*algAccessorychunk)->SetMcPhoParam1(tpsAccessorychunk.get_mcphoparam1());
    (*algAccessorychunk)->SetMcPhoParam2(tpsAccessorychunk.get_mcphoparam2());
    (*algAccessorychunk)->SetMcPhoParam3(tpsAccessorychunk.get_mcphoparam3());
    (*algAccessorychunk)->SetMcPhoParam4(tpsAccessorychunk.get_mcphoparam4());
    (*algAccessorychunk)->SetMcPhoParam5(tpsAccessorychunk.get_mcphoparam5());
    (*algAccessorychunk)->SetMcPhoParam6(tpsAccessorychunk.get_mcphoparam6());

    // mcphooffaxisvec
    if (!tpsAccessorychunk.get_mcphooffaxisvec().empty())
    {
        std::vector<RTFWK::RTFWKDLDOMcPhoOffAxis*> vMcPhoOffAxis;
        FillMcPhoOffAxis_i(vMcPhoOffAxis, tpsAccessorychunk.get_mcphooffaxisvec());
        (*algAccessorychunk)->SetMcPhoOffAxisVec(vMcPhoOffAxis);
        std::vector<RTFWK::RTFWKDLDOMcPhoOffAxis*>::iterator itAlgMcphoAxis = vMcPhoOffAxis.begin();
        for (; itAlgMcphoAxis != vMcPhoOffAxis.end(); ++itAlgMcphoAxis)
        {
            DEL_PTR(*itAlgMcphoAxis);
        }
    }

    RTFWK::RTFWKDLDOContour* algProfiler = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDOContour> algProfileCarrier;
    if (profile != nullptr)
    {
        ContourConverter::ConvertToALG(*profile, &algProfileCarrier);
        algProfiler = algProfileCarrier.get();
    }

    (*algAccessorychunk)->SetProfiles(algProfiler);
    return true;
}

bool AccessoryChunkConverter::CreateALGList(
    std::vector<RtAccessorychunk*>& tpsAccessorychunkList,
    std::vector<RtContour*>& vProfile,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk>>* algAccessorychunkList)
{
    std::vector<RtAccessorychunk*>::iterator itTpsAccessorychunk = tpsAccessorychunkList.begin();
    for(; itTpsAccessorychunk != tpsAccessorychunkList.end(); ++itTpsAccessorychunk)
    {
        RtAccessorychunk* tpsAccessorychunk = *itTpsAccessorychunk;
        RtContour* profile = nullptr;
        if (tpsAccessorychunk->get_uid() != "ideal_comp_chunk")
        {
            std::vector<RtContour*>::iterator it = vProfile.begin();
            for (; it != vProfile.end(); ++it)
            {
                std::string profileUid = (*it)->get_uid();
                if (profileUid == tpsAccessorychunk->get_contouruid())
                {
                    profile = *it;
                    break;
                }
            }
        }
        std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk> algAccessorychunk;
        AccessoryChunkConverter::ConvertToALG(*tpsAccessorychunk, profile, &algAccessorychunk);
        algAccessorychunkList->push_back(algAccessorychunk);
    }
    return true;
}

TPS_END_NAMESPACE