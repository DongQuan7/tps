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
#include "accessory_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_jawlimits.h"
#include "Mcsf3DArithmetic\mcsf_3d_matrix4x4.h"

TPS_BEGIN_NAMESPACE

bool AccessoryConverter::ConvertToALG(
    const RtAccessory& tpsAccessory,
    std::shared_ptr<RTFWK::RTFWKDLDOAccessory>* algAccessory)
{
    algAccessory->reset(new RTFWK::RTFWKDLDOAccessory(false));
    (*algAccessory)->SetUID(tpsAccessory.get_uid());
    (*algAccessory)->SetMachineUID(tpsAccessory.get_machineuid());
    (*algAccessory)->SetCode(tpsAccessory.get_code());
    (*algAccessory)->SetAccessoryType((RTFWK::ACCESSORY_TYPE)tpsAccessory.get_accessorytype());
    (*algAccessory)->SetWedgeOrientation((RTFWK::WEDGE_ORIENTATION)tpsAccessory.get_wedgeorientation());
    // set jaw limit
    RTFWK::RTFWKDLDOJawLimits* jaw1 = new RTFWK::RTFWKDLDOJawLimits();
    jaw1->Setmin_1(tpsAccessory.get_min1_jaw1());
    jaw1->Setmax_1(tpsAccessory.get_max1_jaw1());
    jaw1->Setmin_2(tpsAccessory.get_min2_jaw1());
    jaw1->Setmax_2(tpsAccessory.get_max2_jaw1());
    jaw1->Setmin_w(tpsAccessory.get_minw_jaw1());
    jaw1->Setmax_w(tpsAccessory.get_maxw_jaw1());
    jaw1->SetSDD(tpsAccessory.get_sdd_jaw1());
    jaw1->Setname_1(tpsAccessory.get_name1_jaw1());
    jaw1->Setname_2(tpsAccessory.get_name2_jaw1());
    RTFWK::RTFWKDLDOJawLimits* jaw2 = new RTFWK::RTFWKDLDOJawLimits();
    jaw2->Setmin_1(tpsAccessory.get_min1_jaw2());
    jaw2->Setmax_1(tpsAccessory.get_max1_jaw2());
    jaw2->Setmin_2(tpsAccessory.get_min2_jaw2());
    jaw2->Setmax_2(tpsAccessory.get_max2_jaw2());
    jaw2->Setmin_w(tpsAccessory.get_minw_jaw2());
    jaw2->Setmax_w(tpsAccessory.get_maxw_jaw2());
    jaw2->SetSDD(tpsAccessory.get_sdd_jaw2());
    jaw2->Setname_1(tpsAccessory.get_name1_jaw2());
    jaw2->Setname_2(tpsAccessory.get_name2_jaw2());
    RTFWK::RTFWKDLDOJawLimits* m_jaw_limits = new RTFWK::RTFWKDLDOJawLimits[2];
    m_jaw_limits[0] = *jaw1;
    m_jaw_limits[1] = *jaw2;
    (*algAccessory)->SetJaw_limits(m_jaw_limits);
    // set beam 2 filter
    Mcsf::Matrix4f m_fALGT_beam_to_filter;
    std::string strBeamtofilter = tpsAccessory.get_beamtofilter();
    std::vector<std::string> vecValue;
    (void)boost::split(vecValue, strBeamtofilter, boost::is_any_of(","));
    if (vecValue.size() == 16)
    {
        float fx,fy,fz,fw;
        fx = (float)atof(vecValue[0].c_str());
        fy = (float)atof(vecValue[1].c_str());
        fz = (float)atof(vecValue[2].c_str());
        fw = (float)atof(vecValue[3].c_str());
        Mcsf::Vector4f v0(fx,fy,fz,fw);
        m_fALGT_beam_to_filter.SetCol0(v0);

        fx = (float)atof(vecValue[4].c_str());
        fy = (float)atof(vecValue[5].c_str());
        fz = (float)atof(vecValue[6].c_str());
        fw = (float)atof(vecValue[7].c_str());
        Mcsf::Vector4f v1(fx,fy,fz,fw);
        m_fALGT_beam_to_filter.SetCol1(v1);

        fx = (float)atof(vecValue[8].c_str());
        fy = (float)atof(vecValue[9].c_str());
        fz = (float)atof(vecValue[10].c_str());
        fw = (float)atof(vecValue[11].c_str());
        Mcsf::Vector4f v2(fx,fy,fz,fw);
        m_fALGT_beam_to_filter.SetCol2(v2);

        fx = (float)atof(vecValue[12].c_str());
        fy = (float)atof(vecValue[13].c_str());
        fz = (float)atof(vecValue[14].c_str());
        fw = (float)atof(vecValue[15].c_str());
        Mcsf::Vector4f v3(fx,fy,fz,fw);
        m_fALGT_beam_to_filter.SetCol3(v3);
    }
    (*algAccessory)->SetALGT_beam_to_filter(m_fALGT_beam_to_filter);
    // set filter 2 beam
    Mcsf::Matrix4f m_fALGT_filter_to_beam;
    std::string strFiltertobeam = tpsAccessory.get_filtertobeam();
    (void)boost::split(vecValue, strFiltertobeam, boost::is_any_of(","));
    if (vecValue.size() == 16)
    {
        float fx,fy,fz,fw;
        fx = (float)atof(vecValue[0].c_str());
        fy = (float)atof(vecValue[1].c_str());
        fz = (float)atof(vecValue[2].c_str());
        fw = (float)atof(vecValue[3].c_str());
        Mcsf::Vector4f v0(fx,fy,fz,fw);
        m_fALGT_filter_to_beam.SetCol0(v0);

        fx = (float)atof(vecValue[4].c_str());
        fy = (float)atof(vecValue[5].c_str());
        fz = (float)atof(vecValue[6].c_str());
        fw = (float)atof(vecValue[7].c_str());
        Mcsf::Vector4f v1(fx,fy,fz,fw);
        m_fALGT_filter_to_beam.SetCol1(v1);

        fx = (float)atof(vecValue[8].c_str());
        fy = (float)atof(vecValue[9].c_str());
        fz = (float)atof(vecValue[10].c_str());
        fw = (float)atof(vecValue[11].c_str());
        Mcsf::Vector4f v2(fx,fy,fz,fw);
        m_fALGT_filter_to_beam.SetCol2(v2);

        fx = (float)atof(vecValue[12].c_str());
        fy = (float)atof(vecValue[13].c_str());
        fz = (float)atof(vecValue[14].c_str());
        fw = (float)atof(vecValue[15].c_str());
        Mcsf::Vector4f v3(fx,fy,fz,fw);
        m_fALGT_filter_to_beam.SetCol3(v3);
    }
    (*algAccessory)->SetALGT_filter_to_beam(m_fALGT_filter_to_beam);
    (*algAccessory)->SetWedgeAngle(tpsAccessory.get_wedgeangle());

    DEL_ARRAY(m_jaw_limits);

    return true;
}

bool AccessoryConverter::CreateALGList(
    std::vector<RtAccessory*>& tpsAccessoryList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessory>>* algAccessoryList)
{
    std::vector<RtAccessory*>::iterator itTpsAccessory = tpsAccessoryList.begin();
    for (; itTpsAccessory != tpsAccessoryList.end(); ++itTpsAccessory)
    {
        RtAccessory* tpsAccessory = *itTpsAccessory;
        std::shared_ptr<RTFWK::RTFWKDLDOAccessory> algAccessory;
        AccessoryConverter::ConvertToALG(*tpsAccessory, &algAccessory);
        algAccessoryList->push_back(algAccessory);
    }
    return true;
}

TPS_END_NAMESPACE