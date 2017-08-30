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
#include "machine_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_mlcsetting.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_jawlimits.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_machine.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_tray.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDoseProxy/tps_dp_algcommissionproxy.h"

TPS_BEGIN_NAMESPACE

bool MachineConverter::ConvertToALG(
    const RtMachine& tpsMachine,
    std::vector<RtTray*>& vTray,
    std::shared_ptr<RTFWK::RTFWKDLDOMachine>* algMachine)
{
    algMachine->reset(new RTFWK::RTFWKDLDOMachine(false));
    (*algMachine)->SetUID(tpsMachine.get_uid());
    (*algMachine)->SetGantryMax(tpsMachine.get_gantrymax());
    (*algMachine)->SetGantryMin(tpsMachine.get_gantrymin());
    (*algMachine)->SetTableAngleMax(tpsMachine.get_tableanglemax());
    (*algMachine)->SetTableAngleMin(tpsMachine.get_tableanglemin());
    // SetMLCSetting
    RTFWK::RTFWKDLDOMLCSetting* pMLC = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDOMLCSetting> algMlcSettingCarrier;
    MachineConverter::SetMLCSetting_i(tpsMachine, &algMlcSettingCarrier);
    pMLC = algMlcSettingCarrier.get();
    (*algMachine)->SetMLCSetting(pMLC);
    (*algMachine)->SetMaximumGantrySpeed_degreePerSecond(tpsMachine.get_maximumgantryspeed_degreepersecond());
    (*algMachine)->SetMaximumLeafSpeed_mmPerSecond(tpsMachine.get_maximumleafspeed_mmpersecond());
    (*algMachine)->SetDeviceID(tpsMachine.get_deviceid());
    (*algMachine)->SetCollimatorAngleMax(tpsMachine.get_collimatoranglemax());
    (*algMachine)->SetCollimatorAngleMin(tpsMachine.get_collimatoranglemin());
    (*algMachine)->SetGantryClockWiseRange(tpsMachine.get_gantryclockwiserange());
    (*algMachine)->SetGantryCounterClockWiseRange(tpsMachine.get_gantrycounterclockwiserange());
    // Set AvailablePlanType
    std::vector<RTFWK::PLAN_TYPE> m_vAvailablePlanType;
    std::vector<std::string> vecValue;
    std::string strAvailablePlanType = tpsMachine.get_availableplantype();
    (void)boost::split(vecValue,strAvailablePlanType, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    m_vAvailablePlanType.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            m_vAvailablePlanType[i] = static_cast<RTFWK::PLAN_TYPE>(atoi(vecValue[i].c_str()));
        }
    }
    vecValue.clear();
    (*algMachine)->SetAvailablePlanType(m_vAvailablePlanType);
    (*algMachine)->SetSupportApetureBlock(tpsMachine.get_supportapetureblock());
    (*algMachine)->SetSupportShieldBlock(tpsMachine.get_supportshieldblock());
    (*algMachine)->SetSourceToBlockTrayDistance(tpsMachine.get_sourcetoblocktraydistance());
    (*algMachine)->SetBlockHeight(tpsMachine.get_blockheight());
    // SetTrayObjectList
    std::vector<RTFWK::RTFWKDLDOTray*> vAlgTray;
    for (std::vector<RtTray*>::iterator it = vTray.begin(); it != vTray.end(); ++it)
    {
        if ((*it)->get_machineuid() == tpsMachine.get_uid())
        {
            RTFWK::RTFWKDLDOTray* tray = new RTFWK::RTFWKDLDOTray();
            tray->SetCode((*it)->get_code());
            tray->SetMachineUID((*it)->get_machineuid());
            tray->SetTrayID((*it)->get_trayid());
            tray->SetUID((*it)->get_uid());
            vAlgTray.push_back(tray);
        }
    }
    (*algMachine)->SetTrayObjectList(vAlgTray);
    (*algMachine)->SetSourceToWedgeTopDistance(tpsMachine.get_sourcetowedgetopdistance());
    (*algMachine)->SetSourceToApplicatorDistance(tpsMachine.get_sourcetoapplicatordistance());
    (*algMachine)->SetBlockThickness(tpsMachine.get_blockthickness());
    (*algMachine)->SetSAD(tpsMachine.get_sad());

    return true;
}

bool MachineConverter::CreateALGList(
    std::vector<RtMachine*>& tpsMachineList,
    std::vector<RtTray*>& vTray,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMachine>>* algMachineList)
{
    std::vector<RtMachine*>::iterator itTpsMachine = tpsMachineList.begin();
    for (; itTpsMachine != tpsMachineList.end(); ++itTpsMachine)
    {
        RtMachine* tpsMachine = *itTpsMachine;
        std::shared_ptr<RTFWK::RTFWKDLDOMachine> algMachine;
        MachineConverter::ConvertToALG(*tpsMachine, vTray, &algMachine);
        algMachineList->push_back(algMachine);
    }
    return true;
}

bool MachineConverter::ConvertToALG(MachineSuit* sourceSuit, std::shared_ptr<RTFWK::RTFWKDLDOMachine>& target)
{
    RtMachine* source = sourceSuit->GetMachine();
    target.reset(new RTFWK::RTFWKDLDOMachine(false));
    target->SetUID(source->get_uid());
    target->SetGantryMax(source->get_gantrymax());
    target->SetGantryMin(source->get_gantrymin());
    target->SetTableAngleMax(source->get_tableanglemax());
    target->SetTableAngleMin(source->get_tableanglemin());
    target->SetMaximumGantrySpeed_degreePerSecond(source->get_maximumgantryspeed_degreepersecond());
    target->SetMaximumLeafSpeed_mmPerSecond(source->get_maximumleafspeed_mmpersecond());
    target->SetDeviceID(source->get_deviceid());
    target->SetCollimatorAngleMax(source->get_collimatoranglemax());
    target->SetCollimatorAngleMin(source->get_collimatoranglemin());
    target->SetGantryClockWiseRange(source->get_gantryclockwiserange());
    target->SetGantryCounterClockWiseRange(source->get_gantrycounterclockwiserange());
    target->SetSupportApetureBlock(source->get_supportapetureblock());
    target->SetSupportShieldBlock(source->get_supportshieldblock());
    target->SetSourceToBlockTrayDistance(source->get_sourcetoblocktraydistance());
    target->SetBlockHeight(source->get_blockheight());
    target->SetSourceToWedgeTopDistance(source->get_sourcetowedgetopdistance());
    target->SetSourceToApplicatorDistance(source->get_sourcetoapplicatordistance());
    target->SetBlockThickness(source->get_blockthickness());
    target->SetSAD(source->get_sad());
	target->SetMinimumGantrySpeed_degreePerSecond(source->get_mingantryspeed_degreepersecond());
	target->SetMaximumLeafAcc_mmPerSecond2(source->get_maxleafacc_mmpersec2());

    // SetTrayObjectList
    std::vector<shared_ptr<RTFWK::RTFWKDLDOTray>> shareTrayList;
    std::vector<RtTray>& vTrayList = sourceSuit->GetTrayList();
    for (auto it = vTrayList.begin(); it != vTrayList.end(); ++it)
    {
        shared_ptr<RTFWK::RTFWKDLDOTray> tray;
        ConvertToALG(&(*it), tray);
        shareTrayList.push_back(tray);
    }

    std::vector<RTFWK::RTFWKDLDOTray*> vAlgTray;
    for (auto tray = shareTrayList.begin(); tray != shareTrayList.end(); ++tray)
    {
        vAlgTray.push_back(tray->get());
    }
    target->SetTrayObjectList(vAlgTray);

    // SetMLCSetting
    shared_ptr<RTFWK::RTFWKDLDOMLCSetting> algMlcSettingCarrier;
    ConvertToALG(source, algMlcSettingCarrier);
    RTFWK::RTFWKDLDOMLCSetting* mlcSetting = algMlcSettingCarrier.get();
    target->SetMLCSetting(mlcSetting);

    // Set AvailablePlanType
    std::vector<RTFWK::PLAN_TYPE> m_vAvailablePlanType;
    std::vector<std::string> vecValue;
    std::string strAvailablePlanType = source->get_availableplantype();
    (void)boost::split(vecValue, strAvailablePlanType, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    m_vAvailablePlanType.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            m_vAvailablePlanType[i] = static_cast<RTFWK::PLAN_TYPE>(atoi(vecValue[i].c_str()));
        }
    }
    vecValue.clear();
    target->SetAvailablePlanType(m_vAvailablePlanType);

    //Set Accessory
    std::vector<RtAccessory>& accessoryList = sourceSuit->GetAccessoryList();
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessory>> sharedAccessoryList;
    for (auto accessory = accessoryList.begin(); accessory != accessoryList.end(); ++accessory)
    {
        std::shared_ptr<RTFWK::RTFWKDLDOAccessory> accessoryTemp;
        ConvertToALG(&(*accessory), accessoryTemp);
        sharedAccessoryList.push_back(accessoryTemp);
    }

    std::vector<RTFWK::RTFWKDLDOAccessory*> algAccessoryList;
    for (auto accessory = sharedAccessoryList.begin(); accessory != sharedAccessoryList.end(); ++accessory)
    {
        algAccessoryList.push_back(accessory->get());
    }
    target->SetAccessory(algAccessoryList);

    return true;
}

bool MachineConverter::ConvertToALG(RtTray* source, std::shared_ptr<RTFWK::RTFWKDLDOTray>& target)
{
    target.reset(new RTFWK::RTFWKDLDOTray(false));
    target->SetCode(source->get_code());
    target->SetMachineUID(source->get_machineuid());
    target->SetTrayID(source->get_trayid());
    target->SetUID(source->get_uid());
    return true;
}

bool MachineConverter::ConvertToALG(RtMachine* source, std::shared_ptr<RTFWK::RTFWKDLDOMLCSetting>& target)
{
    target.reset(new RTFWK::RTFWKDLDOMLCSetting());
    target->SetXJawNum(source->get_xjawnum());
    target->SetYJawNum(source->get_yjawnum());
    target->SetLeaivesNum(source->get_leafnum());
    target->SetLeafBoundaries(source->get_leafboundaries_list());
    target->SetNegativeLeafRangeMax(-source->get_positiveleafrangemin());
    target->SetNegativeLeafRangeMin(-source->get_positiveleafrangemax());
    target->SetPositiveLeafRangeMax(source->get_positiveleafrangemax());
    target->SetPositiveLeafRangeMin(source->get_positiveleafrangemin());
    target->SetMLCLeafDirection((RTFWK::MLC_LEAF_DIRECTION_TYPE)source->get_mlcdirectiontype());
    target->SetJawXIsSymmetry(source->get_jawxissymmetry());
    target->SetJawYIsSymmetry(source->get_jawyissymmetry());
    target->SetInterdigitation(source->get_isinterdigitation());
    target->SetLeafSpan(source->get_leafspan());
    target->SetMinLeafEndGap(source->get_minleafendgap());
    target->SetMaximumJawSpeed_mmPerSecond(source->get_maximumjawspeed_mmpersecond());
    target->SetMinMLCJawCover(source->get_minimumjawgap());
    target->SetDynamicTrackingJawX(source->get_dynamictrackingjawx());
    target->SetDynamicTrackingJawY(source->get_dynamictrackingjawy());
    target->SetSourceToMLCTopDistance(source->get_sourcetomlctopdistance());
    target->SetJawTrackingGap(source->get_jawtrackinggap());

    //set jawLimitx
    std::shared_ptr<RTFWK::RTFWKDLDOJawLimits> jawLimitx;
    jawLimitx.reset(new RTFWK::RTFWKDLDOJawLimits());
    jawLimitx->SetIsXJawLimit(true);
    jawLimitx->Setindependent(!source->get_jawxissymmetry());
    jawLimitx->Setmax_1(source->get_xmax_1());
    jawLimitx->Setmax_2(source->get_xmax_2());
    jawLimitx->Setmax_w(source->get_xmax_w());
    jawLimitx->Setmin_1(source->get_xmin_1());
    jawLimitx->Setmin_2(source->get_xmin_2());
    jawLimitx->Setmin_w(source->get_xmin_w());
    jawLimitx->SetSDD(source->get_xsdd());
    jawLimitx->Setname_1(source->get_xname_1());
    jawLimitx->Setname_2(source->get_xname_2());
    jawLimitx->Setindependent(!source->get_jawyissymmetry());

    //set jawLimity
    std::shared_ptr<RTFWK::RTFWKDLDOJawLimits> jawLimity;
    jawLimity.reset(new RTFWK::RTFWKDLDOJawLimits());
    jawLimity->SetIsXJawLimit(false);
    jawLimity->Setmax_1(source->get_ymax_1());
    jawLimity->Setmax_2(source->get_ymax_2());
    jawLimity->Setmax_w(source->get_ymax_w());
    jawLimity->Setmin_1(source->get_ymin_1());
    jawLimity->Setmin_2(source->get_ymin_2());
    jawLimity->Setmin_w(source->get_ymin_w());
    jawLimity->SetSDD(source->get_ysdd());
    jawLimity->Setname_1(source->get_yname_1());
    jawLimity->Setname_2(source->get_yname_2());

    RTFWK::RTFWKDLDOJawLimits* jawx = jawLimitx.get();
    RTFWK::RTFWKDLDOJawLimits* jawy = jawLimity.get();
    target->SetJawLimitX(jawx);
    target->SetJawLimitY(jawy);

    return true;
}

bool MachineConverter::ConvertToALG(RtAccessory* source, std::shared_ptr<RTFWK::RTFWKDLDOAccessory>& target)
{
    target.reset(new RTFWK::RTFWKDLDOAccessory(false));
    target->SetUID(source->get_uid());
    target->SetMachineUID(source->get_machineuid());
    target->SetCode(source->get_code());
    target->SetAccessoryType((RTFWK::ACCESSORY_TYPE)source->get_accessorytype());
    target->SetWedgeOrientation((RTFWK::WEDGE_ORIENTATION)source->get_wedgeorientation());
    target->SetWedgeAngle(source->get_wedgeangle());

    // set jaw limit
    shared_ptr<RTFWK::RTFWKDLDOJawLimits> jaw1(new RTFWK::RTFWKDLDOJawLimits());
    jaw1->Setmin_1(source->get_min1_jaw1());
    jaw1->Setmax_1(source->get_max1_jaw1());
    jaw1->Setmin_2(source->get_min2_jaw1());
    jaw1->Setmax_2(source->get_max2_jaw1());
    jaw1->Setmin_w(source->get_minw_jaw1());
    jaw1->Setmax_w(source->get_maxw_jaw1());
    jaw1->SetSDD(source->get_sdd_jaw1());
    jaw1->Setname_1(source->get_name1_jaw1());
    jaw1->Setname_2(source->get_name2_jaw1());
    shared_ptr<RTFWK::RTFWKDLDOJawLimits> jaw2(new RTFWK::RTFWKDLDOJawLimits());
    jaw2->Setmin_1(source->get_min1_jaw2());
    jaw2->Setmax_1(source->get_max1_jaw2());
    jaw2->Setmin_2(source->get_min2_jaw2());
    jaw2->Setmax_2(source->get_max2_jaw2());
    jaw2->Setmin_w(source->get_minw_jaw2());
    jaw2->Setmax_w(source->get_maxw_jaw2());
    jaw2->SetSDD(source->get_sdd_jaw2());
    jaw2->Setname_1(source->get_name1_jaw2());
    jaw2->Setname_2(source->get_name2_jaw2());
    RTFWK::RTFWKDLDOJawLimits* m_jaw_limits = new RTFWK::RTFWKDLDOJawLimits[2];
    m_jaw_limits[0] = *jaw1;
    m_jaw_limits[1] = *jaw2;
    target->SetJaw_limits(m_jaw_limits);

    // set beam 2 filter
    Mcsf::Matrix4f m_fALGT_beam_to_filter;
    std::string strBeamtofilter = source->get_beamtofilter();
    std::vector<std::string> vecValue;
    (void)boost::split(vecValue, strBeamtofilter, boost::is_any_of(","));
    if (vecValue.size() == 16)
    {
        float fx, fy, fz, fw;
        fx = (float)atof(vecValue[0].c_str());
        fy = (float)atof(vecValue[1].c_str());
        fz = (float)atof(vecValue[2].c_str());
        fw = (float)atof(vecValue[3].c_str());
        Mcsf::Vector4f v0(fx, fy, fz, fw);
        m_fALGT_beam_to_filter.SetCol0(v0);

        fx = (float)atof(vecValue[4].c_str());
        fy = (float)atof(vecValue[5].c_str());
        fz = (float)atof(vecValue[6].c_str());
        fw = (float)atof(vecValue[7].c_str());
        Mcsf::Vector4f v1(fx, fy, fz, fw);
        m_fALGT_beam_to_filter.SetCol1(v1);

        fx = (float)atof(vecValue[8].c_str());
        fy = (float)atof(vecValue[9].c_str());
        fz = (float)atof(vecValue[10].c_str());
        fw = (float)atof(vecValue[11].c_str());
        Mcsf::Vector4f v2(fx, fy, fz, fw);
        m_fALGT_beam_to_filter.SetCol2(v2);

        fx = (float)atof(vecValue[12].c_str());
        fy = (float)atof(vecValue[13].c_str());
        fz = (float)atof(vecValue[14].c_str());
        fw = (float)atof(vecValue[15].c_str());
        Mcsf::Vector4f v3(fx, fy, fz, fw);
        m_fALGT_beam_to_filter.SetCol3(v3);
    }
    target->SetALGT_beam_to_filter(m_fALGT_beam_to_filter);

    // set filter 2 beam
    Mcsf::Matrix4f m_fALGT_filter_to_beam;
    std::string strFiltertobeam = source->get_filtertobeam();
    (void)boost::split(vecValue, strFiltertobeam, boost::is_any_of(","));
    if (vecValue.size() == 16)
    {
        float fx, fy, fz, fw;
        fx = (float)atof(vecValue[0].c_str());
        fy = (float)atof(vecValue[1].c_str());
        fz = (float)atof(vecValue[2].c_str());
        fw = (float)atof(vecValue[3].c_str());
        Mcsf::Vector4f v0(fx, fy, fz, fw);
        m_fALGT_filter_to_beam.SetCol0(v0);

        fx = (float)atof(vecValue[4].c_str());
        fy = (float)atof(vecValue[5].c_str());
        fz = (float)atof(vecValue[6].c_str());
        fw = (float)atof(vecValue[7].c_str());
        Mcsf::Vector4f v1(fx, fy, fz, fw);
        m_fALGT_filter_to_beam.SetCol1(v1);

        fx = (float)atof(vecValue[8].c_str());
        fy = (float)atof(vecValue[9].c_str());
        fz = (float)atof(vecValue[10].c_str());
        fw = (float)atof(vecValue[11].c_str());
        Mcsf::Vector4f v2(fx, fy, fz, fw);
        m_fALGT_filter_to_beam.SetCol2(v2);

        fx = (float)atof(vecValue[12].c_str());
        fy = (float)atof(vecValue[13].c_str());
        fz = (float)atof(vecValue[14].c_str());
        fw = (float)atof(vecValue[15].c_str());
        Mcsf::Vector4f v3(fx, fy, fz, fw);
        m_fALGT_filter_to_beam.SetCol3(v3);
    }
    target->SetALGT_filter_to_beam(m_fALGT_filter_to_beam);
    DEL_ARRAY(m_jaw_limits);
    return true;
}

void MachineConverter::SetMLCSetting_i(const RtMachine& tpsMachine, std::shared_ptr<RTFWK::RTFWKDLDOMLCSetting>* algMlcSetting)
{
    algMlcSetting->reset(new RTFWK::RTFWKDLDOMLCSetting());
    (*algMlcSetting)->SetXJawNum(tpsMachine.get_xjawnum());
    (*algMlcSetting)->SetYJawNum(tpsMachine.get_yjawnum());
    (*algMlcSetting)->SetLeaivesNum(tpsMachine.get_leafnum());
    (*algMlcSetting)->SetLeafBoundaries(tpsMachine.get_leafboundaries_list());
    (*algMlcSetting)->SetNegativeLeafRangeMax(-tpsMachine.get_positiveleafrangemin());
    (*algMlcSetting)->SetNegativeLeafRangeMin(-tpsMachine.get_positiveleafrangemax());
    (*algMlcSetting)->SetPositiveLeafRangeMax(tpsMachine.get_positiveleafrangemax());
    (*algMlcSetting)->SetPositiveLeafRangeMin(tpsMachine.get_positiveleafrangemin());
    (*algMlcSetting)->SetMLCLeafDirection((RTFWK::MLC_LEAF_DIRECTION_TYPE)tpsMachine.get_mlcdirectiontype());
    (*algMlcSetting)->SetJawXIsSymmetry(tpsMachine.get_jawxissymmetry());
    (*algMlcSetting)->SetJawYIsSymmetry(tpsMachine.get_jawyissymmetry());
    (*algMlcSetting)->SetInterdigitation(tpsMachine.get_isinterdigitation());
    (*algMlcSetting)->SetLeafSpan(tpsMachine.get_leafspan());
    (*algMlcSetting)->SetMinLeafEndGap(tpsMachine.get_minleafendgap());
    (*algMlcSetting)->SetMaximumJawSpeed_mmPerSecond(tpsMachine.get_maximumjawspeed_mmpersecond());
    (*algMlcSetting)->SetMinMLCJawCover(tpsMachine.get_minimumjawgap());
    (*algMlcSetting)->SetDynamicTrackingJawX(tpsMachine.get_dynamictrackingjawx());
    (*algMlcSetting)->SetDynamicTrackingJawY(tpsMachine.get_dynamictrackingjawy());
    (*algMlcSetting)->SetSourceToMLCTopDistance(tpsMachine.get_sourcetomlctopdistance());
    (*algMlcSetting)->SetJawTrackingGap(tpsMachine.get_jawtrackinggap());
    RTFWK::RTFWKDLDOJawLimits* jawLimitx = new RTFWK::RTFWKDLDOJawLimits();
    jawLimitx->SetIsXJawLimit(true);
    jawLimitx->Setindependent(!tpsMachine.get_jawxissymmetry());
    jawLimitx->Setmax_1(tpsMachine.get_xmax_1());
    jawLimitx->Setmax_2(tpsMachine.get_xmax_2());
    jawLimitx->Setmax_w(tpsMachine.get_xmax_w());
    jawLimitx->Setmin_1(tpsMachine.get_xmin_1());
    jawLimitx->Setmin_2(tpsMachine.get_xmin_2());
    jawLimitx->Setmin_w(tpsMachine.get_xmin_w());
    jawLimitx->SetSDD(tpsMachine.get_xsdd());
    jawLimitx->Setname_1(tpsMachine.get_xname_1());
    jawLimitx->Setname_2(tpsMachine.get_xname_2());
    jawLimitx->Setindependent(!tpsMachine.get_jawyissymmetry());
    RTFWK::RTFWKDLDOJawLimits* jawLimity = new RTFWK::RTFWKDLDOJawLimits();
    jawLimity->SetIsXJawLimit(false);
    jawLimity->Setmax_1(tpsMachine.get_ymax_1());
    jawLimity->Setmax_2(tpsMachine.get_ymax_2());
    jawLimity->Setmax_w(tpsMachine.get_ymax_w());
    jawLimity->Setmin_1(tpsMachine.get_ymin_1());
    jawLimity->Setmin_2(tpsMachine.get_ymin_2());
    jawLimity->Setmin_w(tpsMachine.get_ymin_w());
    jawLimity->SetSDD(tpsMachine.get_ysdd());
    jawLimity->Setname_1(tpsMachine.get_yname_1());
    jawLimity->Setname_2(tpsMachine.get_yname_2());
    (*algMlcSetting)->SetJawLimitX(jawLimitx);
    (*algMlcSetting)->SetJawLimitY(jawLimity);
    DEL_PTR(jawLimitx);
    DEL_PTR(jawLimity);

}

TPS_END_NAMESPACE