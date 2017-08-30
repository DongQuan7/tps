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
#include "commissioned_unit_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_commissionedunit.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_machineunit.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_sartable.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_timecalpara.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_convdatastruct.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDoseProxy/tps_dp_algcommissionproxy.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_kernelweight.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measureddata.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_measureddata.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_wedgefactor.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_wedgefactor.h"
#include "alg/alg_commission.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmachinesetting.h"
#include "kernel_weight_converter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessorychunk.h"
#include "accessory_chunk_converter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "tps_logger.h"
#include "goldenstt_converter.h"
#include "mcphocommissioned_unit_converter.h"
#include "RtTpsDoseProxy/tps_dp_alg_converter_common.h"

#include "boost/lexical_cast.hpp"

TPS_BEGIN_NAMESPACE

bool CommissionedUnitConverter::ConvertToALG(
    const RtCommissionedunit& tpsCommissionedUnit,
    std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>* algCommissionedUnit)
{
    algCommissionedUnit->reset(new RTFWK::RTFWKDLDOCommissionedunit(false));
    (*algCommissionedUnit)->SetUID(tpsCommissionedUnit.get_uid());
    (*algCommissionedUnit)->SetMachineUID(tpsCommissionedUnit.get_machineuid());
    (*algCommissionedUnit)->SetKernelDataListUID(tpsCommissionedUnit.get_kerneldatalistuid());
    (*algCommissionedUnit)->SetRadiationType((RTFWK::RADIATION_TYPE)tpsCommissionedUnit.get_radiationtype());
    (*algCommissionedUnit)->SetEnergy(tpsCommissionedUnit.get_energy());
    (*algCommissionedUnit)->SetRadiationType((RTFWK::RADIATION_TYPE)tpsCommissionedUnit.get_radiationtype());
    (*algCommissionedUnit)->SetMaximumDoseRate_MUperMinute(tpsCommissionedUnit.get_maximumdoserate_muperminute());
    RTFWK::RTFWKDLDOMachineUnit* machineUnit = new RTFWK::RTFWKDLDOMachineUnit();
    SetMachineUnit_i(tpsCommissionedUnit, machineUnit);
    (*algCommissionedUnit)->SetMachineUnit(machineUnit);
    RTFWK::RTFWKDLDOSartable* sar = new RTFWK::RTFWKDLDOSartable();
    SetSarTable_i(tpsCommissionedUnit, sar);
    (*algCommissionedUnit)->SetSartable(sar);
    RTFWK::RTFWKDLDOTimecalParameters* pTimecal = new RTFWK::RTFWKDLDOTimecalParameters();
    SetTimecal_i(tpsCommissionedUnit, pTimecal);
    (*algCommissionedUnit)->SetTimeCal(pTimecal);
    RTFWK::RTFWKDLDOConvolutionData* pConvolutionData = new RTFWK::RTFWKDLDOConvolutionData();
    SetConvolutionData_i(tpsCommissionedUnit, pConvolutionData);
    (*algCommissionedUnit)->SetConvolutionData(pConvolutionData);
    (*algCommissionedUnit)->SetAllowContinuousDoseRate(tpsCommissionedUnit.get_allowcontinuousdoserate());
    (*algCommissionedUnit)->SetDiscreteDoseRate(tpsCommissionedUnit.get_discretedoserate());
    (*algCommissionedUnit)->SetIsFlattened(tpsCommissionedUnit.get_isflattened());
    (*algCommissionedUnit)->SetIsConvolutionSupport(tpsCommissionedUnit.get_isconvolutionsupport());
    (*algCommissionedUnit)->SetIsPencileBeamSupport(tpsCommissionedUnit.get_ispencilebeamsupport());
    (*algCommissionedUnit)->SetIsMonteCarloSupport(tpsCommissionedUnit.get_ismontecarlosupport());
    // Set AvailablePhotonPlanType
    std::vector<std::string> vecValue;
    std::vector<RTFWK::PLAN_TYPE> vAvailablePhotonPlanType;
    std::string strAvailablePhotonPlanType = tpsCommissionedUnit.get_availablephotonplantype();
    (void)boost::split(vecValue,strAvailablePhotonPlanType, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    vAvailablePhotonPlanType.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            vAvailablePhotonPlanType[i] = static_cast<RTFWK::PLAN_TYPE>(atoi(vecValue[i].c_str()));
        }
    }
    vecValue.clear();
    (*algCommissionedUnit)->SetAvailablePhotonPlanType(vAvailablePhotonPlanType);
    // Set AvailableElectronPlanType 
    std::vector<RTFWK::PLAN_ELECTORN_TYPE> vAvailableElectronPlanType;
    std::string strAvailableElectronPlanType = tpsCommissionedUnit.get_availableelectronplantype();
    (void)boost::split(vecValue,strAvailableElectronPlanType, boost::is_any_of(","));
    iLen = vecValue.size();
    vAvailableElectronPlanType.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            vAvailableElectronPlanType[i] = static_cast<RTFWK::PLAN_ELECTORN_TYPE>(atoi(vecValue[i].c_str()));
        }
    }
    vecValue.clear();
    (*algCommissionedUnit)->SetAvailableElectronPlanType(vAvailableElectronPlanType);
    (*algCommissionedUnit)->SetCalibrationFsX(tpsCommissionedUnit.get_calibrationfsx());
    (*algCommissionedUnit)->SetCalibrationFsY(tpsCommissionedUnit.get_calibrationfsy());
    (*algCommissionedUnit)->SetCommissionedUnit_Dmax(tpsCommissionedUnit.get_commissionedunit_dmax());
    (*algCommissionedUnit)->SetSSD(tpsCommissionedUnit.get_ssd());
    (*algCommissionedUnit)->SetPhantomSizeDepth(tpsCommissionedUnit.get_phantomsizedepth());
    (*algCommissionedUnit)->SetPhantomSizeLateral(tpsCommissionedUnit.get_phantomsizelateral());
    (*algCommissionedUnit)->SetBlockTrayPenumbra(tpsCommissionedUnit.get_blocktraypenumbra());
    //(*algCommissionedUnit)->SetBlockTrayTrayFactor(tpsCommissionedUnit.get_blocktraytrayfactor());
    (*algCommissionedUnit)->SetBlockAttenFactor(tpsCommissionedUnit.get_blockattenfactor());
    // Set TrayFactor
    std::map<std::string, float> trays = tpsCommissionedUnit.get_discrete_trayfactor();
    std::vector<RTFWK::RTFWKDLDOTrayFactor*> vTrayFactor;
    for (std::map<std::string, float>::iterator it = trays.begin(); it != trays.end(); ++it)
    {
        RTFWK::RTFWKDLDOTrayFactor* trayFactor = new RTFWK::RTFWKDLDOTrayFactor();
        trayFactor->m_sTrayUID = it->first;
        trayFactor->m_fTrayFactor = it->second;
        vTrayFactor.push_back(trayFactor);
    }
    (*algCommissionedUnit)->SetTrayFactor(vTrayFactor);
    (*algCommissionedUnit)->SetNormalizedDose(tpsCommissionedUnit.get_normalizeddose());
    (*algCommissionedUnit)->SetXJawFactor(tpsCommissionedUnit.get_xjawfactor());
    (*algCommissionedUnit)->SetYJawFactor(tpsCommissionedUnit.get_yjawfactor());
    (*algCommissionedUnit)->SetMLCFactor(tpsCommissionedUnit.get_mlcfactor());

    (*algCommissionedUnit)->SetCCTongueWidth(tpsCommissionedUnit.get_cctonguewidth());
    (*algCommissionedUnit)->SetCCGrooveWidth(tpsCommissionedUnit.get_ccgroovewidth());
    (*algCommissionedUnit)->SetCCLeafTipWidth(tpsCommissionedUnit.get_ccleaftipwidth());
    (*algCommissionedUnit)->SetCCReserve(tpsCommissionedUnit.get_reservecc());


    (*algCommissionedUnit)->SetPBTongueWidth(tpsCommissionedUnit.get_pbtonguewidth());
    (*algCommissionedUnit)->SetPBGrooveWidth(tpsCommissionedUnit.get_pbgroovewidth());
    (*algCommissionedUnit)->SetPBLeafTipWidth(tpsCommissionedUnit.get_pbleaftipwidth());
    (*algCommissionedUnit)->SetPBReserve(tpsCommissionedUnit.get_reservepb());


    (*algCommissionedUnit)->SetCCTransmissionTG(tpsCommissionedUnit.get_cctransmission());
    (*algCommissionedUnit)->SetPBTransmissionTG(tpsCommissionedUnit.get_pbtransmission());

    DEL_PTR(machineUnit);
    DEL_PTR(sar);
    DEL_PTR(pTimecal);
    DEL_PTR(pConvolutionData);
    return true;
}

bool CommissionedUnitConverter::CreateALGList(
    std::vector<RtCommissionedunit*>& tpsCommissionedUnitList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>>* algCommissionedUnitList)
{
    std::vector<RtCommissionedunit*>::iterator itTpsCommissionedUnit = tpsCommissionedUnitList.begin();
    for (; itTpsCommissionedUnit != tpsCommissionedUnitList.end(); ++itTpsCommissionedUnit)
    {
        RtCommissionedunit* tpsCommissionedUnit = *itTpsCommissionedUnit;
        std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit> algCommissionedUnit;
        CommissionedUnitConverter::ConvertToALG(*tpsCommissionedUnit, &algCommissionedUnit);
        algCommissionedUnitList->push_back(algCommissionedUnit);
    }
    return true;
}

void CommissionedUnitConverter::SetMachineUnit_i(
    const RtCommissionedunit& tpsCommissionedUnit, RTFWK::RTFWKDLDOMachineUnit* machineUnit)
{
    machineUnit->SetHidden(tpsCommissionedUnit.get_hidden());
    machineUnit->SetName(tpsCommissionedUnit.get_name());
    machineUnit->SetDmax(tpsCommissionedUnit.get_dmax());
    machineUnit->SetSAD(tpsCommissionedUnit.get_sad());
    machineUnit->SetPSD(tpsCommissionedUnit.get_psd());
    machineUnit->Setsrc_diam1(tpsCommissionedUnit.get_srcdiam1());
    machineUnit->Setsrc_diam2(tpsCommissionedUnit.get_srcdiam2());
    machineUnit->Setsrc_diam3(tpsCommissionedUnit.get_srcdiam3());
    machineUnit->Setsrc_ratio1(tpsCommissionedUnit.get_srcratio1());
    machineUnit->Setsrc_ratio3(tpsCommissionedUnit.get_srcratio3());
    machineUnit->Sete_eq_diam(tpsCommissionedUnit.get_eeqdiam());
    machineUnit->Setx_jaw_count(tpsCommissionedUnit.get_xjawcount());
    machineUnit->Sety_jaw_count(tpsCommissionedUnit.get_yjawcount());
    machineUnit->Setx_jaw_type(tpsCommissionedUnit.get_xjawtype());
    machineUnit->Sety_jaw_type(tpsCommissionedUnit.get_yjawtype());
    machineUnit->SetXSDD(tpsCommissionedUnit.get_xsdd());
    machineUnit->SetYSDD(tpsCommissionedUnit.get_ysdd());
    machineUnit->Setgantry_sense(tpsCommissionedUnit.get_gantrysense());
    machineUnit->Setgantry_offset(tpsCommissionedUnit.get_gantryoffset());
    machineUnit->Settable_offset(tpsCommissionedUnit.get_tableoffset());
    machineUnit->Settable_sense(tpsCommissionedUnit.get_tablesense());
    machineUnit->Setcollimator_offset(tpsCommissionedUnit.get_collimatoroffset());
    machineUnit->Setcollimator_sense(tpsCommissionedUnit.get_collimatorsense());
    machineUnit->SetReferenceFluence(tpsCommissionedUnit.get_referencefluence());
    machineUnit->SetReferenceDose(tpsCommissionedUnit.get_referencedose());
}

void CommissionedUnitConverter::SetSarTable_i(
    const RtCommissionedunit& tpsCommissionedUnit, RTFWK::RTFWKDLDOSartable* sar)
{
    std::vector<std::string> vecValue;
    sar->SetType(tpsCommissionedUnit.get_type());
    sar->Setversion(tpsCommissionedUnit.get_version());
    sar->Settran(tpsCommissionedUnit.get_tran());
    sar->Setflatness_depth_count(tpsCommissionedUnit.get_flatnessdepthcount());
    sar->Setflatness_radius_count(tpsCommissionedUnit.get_flatnessradiuscount());
    float m_fflatness_depth_vec[MAX_FLATNESS_DEPTHS];
    std::string strflatness_depth_vec = tpsCommissionedUnit.get_flatnessdepthvec();
    (void)boost::split(vecValue,strflatness_depth_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_FLATNESS_DEPTHS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fflatness_depth_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    sar->Setflatness_depth_vec(m_fflatness_depth_vec);
    float m_fflatness_radius_vec[MAX_FLATNESS_RADII];
    std::string strflatness_radius_vec = tpsCommissionedUnit.get_flatnessradiusvec();
    (void)boost::split(vecValue,strflatness_radius_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_FLATNESS_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fflatness_radius_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    sar->Setflatness_radius_vec(m_fflatness_radius_vec);
    sar->Setdepth_count(tpsCommissionedUnit.get_depthcount());
    sar->Setradius_count(tpsCommissionedUnit.get_radiuscount());
    float m_fdepth_vec[MAX_SAR_DEPTHS];
    std::string strdepth_vec = tpsCommissionedUnit.get_depthvec();
    (void)boost::split(vecValue, strdepth_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_DEPTHS && i < vecValue.size(); i++)
    {
         if (!vecValue[i].empty())
         {
            m_fdepth_vec[i] = (float)atof(vecValue[i].c_str());
         }
    }
    vecValue.clear();
    sar->Setdepth_vec(m_fdepth_vec);
    float m_fradius_vec[MAX_SAR_RADII];
    std::string strradius_vec = tpsCommissionedUnit.get_radiusvec();
    (void)boost::split(vecValue, strradius_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fradius_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    sar->Setradius_vec(m_fradius_vec);
    float m_fSAR[MAX_SAR_DEPTHS][MAX_SAR_RADII];
    std::string strSAR = tpsCommissionedUnit.get_sar();
    (void)boost::split(vecValue, strSAR, boost::is_any_of(","));
    int iCount = 0;
    for (int i = 0; i < MAX_SAR_DEPTHS && i < tpsCommissionedUnit.get_depthcount(); i++)
    {
        for (int j = 0; j < MAX_SAR_RADII && j < tpsCommissionedUnit.get_radiuscount(); j++)
        {
            if (!vecValue[iCount].empty())
            {
                m_fSAR[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    sar->SetSAR(m_fSAR);
    sar->SetTAR0_depth_count(tpsCommissionedUnit.get_tar0depthcount());
    float m_fTAR0_depth_vec[MAX_SAR_DEPTHS];
    std::string strTAR0_depth_vec = tpsCommissionedUnit.get_tar0_depth_vec();
    (void)boost::split(vecValue, strTAR0_depth_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_DEPTHS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fTAR0_depth_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    sar->SetTAR0_depth_vec(m_fTAR0_depth_vec);
    float m_fTAR0[MAX_SAR_DEPTHS];
    std::string strTAR0 = tpsCommissionedUnit.get_tar0();
    (void)boost::split(vecValue, strTAR0, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_DEPTHS && i < vecValue.size(); i++)
    {
         if (!vecValue[i].empty())
         {
            m_fTAR0[i] = (float)atof(vecValue[i].c_str());
         }
    }
    vecValue.clear();
    sar->SetTAR0(m_fTAR0);
    float m_fflatness[MAX_FLATNESS_RADII][MAX_FLATNESS_DEPTHS];
    std::string strflatness = tpsCommissionedUnit.get_flatness();
    (void)boost::split(vecValue,strflatness, boost::is_any_of(","));
    iCount = 0;
    for (int i = 0; i < MAX_FLATNESS_RADII && i < tpsCommissionedUnit.get_flatnessradiuscount();i++)
    {
        for (int j = 0; j < MAX_FLATNESS_DEPTHS && j < tpsCommissionedUnit.get_flatnessdepthcount();j++)
        {
            if (!vecValue[iCount].empty())
            {
                m_fflatness[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    sar->Setflatness(m_fflatness);
    float m_fSp[MAX_SAR_RADII];
    std::string strSp = tpsCommissionedUnit.get_sp();
    (void)boost::split(vecValue,strSp, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSp[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    sar->SetSp(m_fSp);
    sar->SetSp0(tpsCommissionedUnit.get_sp0());
    float m_fSp_prime[MAX_SAR_RADII];
    std::string strSp_prime = tpsCommissionedUnit.get_sp_prime();
    (void)boost::split(vecValue, strSp_prime, boost::is_any_of(","));
    for (int i = 0 ; i < MAX_SAR_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSp_prime[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    sar->SetSp_prime(m_fSp_prime);
}

void CommissionedUnitConverter::SetTimecal_i(
    const RtCommissionedunit& tpsCommissionedUnit, RTFWK::RTFWKDLDOTimecalParameters* timecal)
{
    std::vector<std::string> vecValue;
    timecal->Setdose_rate(tpsCommissionedUnit.get_doserate());
    timecal->Setdecay_constant(tpsCommissionedUnit.get_decayconstant());
    timecal->Setcal_dist(tpsCommissionedUnit.get_cal_dist());
    timecal->Setcal_depth(tpsCommissionedUnit.get_cal_depth());
    timecal->Setend_effect(tpsCommissionedUnit.get_endeffect());
    timecal->Settime_units(tpsCommissionedUnit.get_timeunits());
    timecal->SetSc_x_count(tpsCommissionedUnit.get_scxcount());
    timecal->SetSc_y_count(tpsCommissionedUnit.get_scycount());
    float m_fSc_x_positions[MAX_Sc_POSITIONS];
    std::string strSc_x_positions = tpsCommissionedUnit.get_scxpositions();
    (void)boost::split(vecValue, strSc_x_positions, boost::is_any_of(","));
    for (int i = 0; i < MAX_Sc_POSITIONS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSc_x_positions[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    timecal->SetSc_x_positions(m_fSc_x_positions);
    float m_fSc_y_positions[MAX_Sc_POSITIONS];
    std::string strSc_y_positions = tpsCommissionedUnit.get_scypositions();
    (void)boost::split(vecValue, strSc_y_positions, boost::is_any_of(","));
    for (int i = 0; i < MAX_Sc_POSITIONS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSc_y_positions[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    timecal->SetSc_y_positions(m_fSc_y_positions);
    float m_fSc[MAX_Sc_POSITIONS][MAX_Sc_POSITIONS];
    std::string strSc = tpsCommissionedUnit.get_sc();
    (void)boost::split(vecValue, strSc, boost::is_any_of(","));
    int iCount = 0;
    for (int i = 0; i < MAX_Sc_POSITIONS && i < tpsCommissionedUnit.get_scycount(); i++)
    {
        for (int j = 0; j < MAX_Sc_POSITIONS && j < tpsCommissionedUnit.get_scxcount(); j++)
        {
            if (!vecValue[iCount].empty())
            {
                m_fSc[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    timecal->SetSc(m_fSc);
}

void CommissionedUnitConverter::SetConvolutionData_i(
    const RtCommissionedunit& tpsCommissionedUnit, RTFWK::RTFWKDLDOConvolutionData* convolutionData)
{
    //std::vector<std::string> vecValue;
    RTFWK::RTFWKDLDOConvDataStruct convDataStruct;
    convDataStruct.SetDiam1(tpsCommissionedUnit.get_diam1());
    convDataStruct.SetDiam2(tpsCommissionedUnit.get_diam2());
    convDataStruct.SetRatio1(tpsCommissionedUnit.get_ratio1());
    convDataStruct.SetTransmission(tpsCommissionedUnit.get_transmission());
    convDataStruct.SetMu(tpsCommissionedUnit.get_mu());
    convDataStruct.SetDmu_dx(tpsCommissionedUnit.get_dmu_dx());
    convDataStruct.SetDmu_dr(tpsCommissionedUnit.get_dmu_dr());

    // offaxisdatax
    std::vector<float> vFloat;
    vFloat = GetVecFromString<float>(tpsCommissionedUnit.get_offaxisdatax());
    convDataStruct.SetOffAxisDataX(vFloat);

    // offaxixsdatay
    vFloat.clear();
    vFloat = GetVecFromString<float>(tpsCommissionedUnit.get_offaxisdatay());
    convDataStruct.SetOffAxisDataY(vFloat);

    // mudatax
    vFloat.clear();
    vFloat = GetVecFromString<float>(tpsCommissionedUnit.get_mudatax());
    convDataStruct.SetMuDataX(vFloat);

    // mudatay
    vFloat.clear();
    vFloat = GetVecFromString<float>(tpsCommissionedUnit.get_mudatay());
    convDataStruct.SetMuDataY(vFloat);

    // dmudatay
    vFloat.clear();
    vFloat = GetVecFromString<float>(tpsCommissionedUnit.get_dmudatay());
    convDataStruct.SetDMuDataY(vFloat);

    // outputmuy
    std::string strOutputMuY = tpsCommissionedUnit.get_outputmuy();
    RemoveSpace(strOutputMuY);
    std::vector<std::string> vecString;
    (void)boost::split(vecString, strOutputMuY, boost::is_any_of("|"));
    std::vector<RTFWK::OutputItem> vOutputList;
    vOutputList.resize(vecString.size());
    int i = 0;
    for (auto iter = vecString.begin(); iter != vecString.end(); ++iter, ++i)
    {
        std::vector<std::string> vString;
        (void)boost::split(vString, vecString[i], boost::is_any_of(","));
        if (vString.size() == 3)
        {
            float x = boost::lexical_cast<float>(vString[0]);
            float y = boost::lexical_cast<float>(vString[1]);
            float f = boost::lexical_cast<float>(vString[2]);
            vOutputList[i].x = x;
            vOutputList[i].y = y;
            vOutputList[i].output = f;
        }
    }
    convolutionData->SetOutputList(vOutputList);

    // ddmudr
    vFloat.clear();
    vFloat = GetVecFromString<float>(tpsCommissionedUnit.get_ddmudr());
    convDataStruct.SetDDMuDR(vFloat);

    convDataStruct.SetSrc_d(tpsCommissionedUnit.get_src_d());
    convDataStruct.SetSrc_atten(tpsCommissionedUnit.get_src_atten());
    convDataStruct.SetNorm_10x10(tpsCommissionedUnit.get_norm_10x10());
    //convDataStruct.SetOn(tpsCommissionedUnit.get_on);
    convDataStruct.SetDm(tpsCommissionedUnit.get_dm());
    convDataStruct.SetFfs10(tpsCommissionedUnit.get_ffs10());
    convDataStruct.SetK(tpsCommissionedUnit.get_k());
    convDataStruct.SetA(tpsCommissionedUnit.get_a());
    convDataStruct.SetDF(tpsCommissionedUnit.get_df());
    convDataStruct.SetSF(tpsCommissionedUnit.get_sf());
    convDataStruct.SetC1(tpsCommissionedUnit.get_c1());
    convDataStruct.SetC2(tpsCommissionedUnit.get_c2());
    convDataStruct.SetC3(tpsCommissionedUnit.get_c3());
    convDataStruct.SetConvDataStruct_PSD(tpsCommissionedUnit.get_convdatastruct_psd());
    convDataStruct.SetConvDataStruct_SDD1(tpsCommissionedUnit.get_convdatastruct_sdd1());
    convDataStruct.SetConvDataStruct_SDD2(tpsCommissionedUnit.get_convdatastruct_sdd2());
    convolutionData->SetConvDataStruct(convDataStruct);
}

bool CommissionedUnitConverter::ConvertToALG(CommissionUintSuit* sourceSuit, std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>& target)
{
    RtCommissionedunit *source = sourceSuit->comUnit;
    target.reset(new RTFWK::RTFWKDLDOCommissionedunit(false));
    target->SetUID(source->get_uid());
    target->SetMachineUID(source->get_machineuid());
    target->SetKernelDataListUID(source->get_kerneldatalistuid());
    target->SetRadiationType((RTFWK::RADIATION_TYPE)source->get_radiationtype());
    target->SetEnergy(source->get_energy());
    target->SetRadiationType((RTFWK::RADIATION_TYPE)source->get_radiationtype());
    target->SetMaximumDoseRate_MUperMinute(source->get_maximumdoserate_muperminute());
    target->SetAllowContinuousDoseRate(source->get_allowcontinuousdoserate());
    target->SetDiscreteDoseRate(source->get_discretedoserate());
    target->SetIsFlattened(source->get_isflattened());
    target->SetIsConvolutionSupport(source->get_isconvolutionsupport());
    target->SetIsPencileBeamSupport(source->get_ispencilebeamsupport());
    target->SetIsMonteCarloSupport(source->get_ismontecarlosupport());
    target->SetCalibrationFsX(source->get_calibrationfsx());
    target->SetCalibrationFsY(source->get_calibrationfsy());
    target->SetCommissionedUnit_Dmax(source->get_commissionedunit_dmax());
    target->SetSSD(source->get_ssd());
    target->SetPhantomSizeDepth(source->get_phantomsizedepth());
    target->SetPhantomSizeLateral(source->get_phantomsizelateral());
    target->SetBlockTrayPenumbra(source->get_blocktraypenumbra());
    //target->SetBlockTrayTrayFactor(source->get_blocktraytrayfactor());
    target->SetBlockAttenFactor(source->get_blockattenfactor());
    target->SetNormalizedDose(source->get_normalizeddose());
    target->SetXJawFactor(source->get_xjawfactor());
    target->SetYJawFactor(source->get_yjawfactor());
    target->SetMLCFactor(source->get_mlcfactor());
	target->SetMinimumDoseRate_MUperMinute(source->get_mindoserate_mupermin());

    //set machineUnit
    shared_ptr<RTFWK::RTFWKDLDOMachineUnit> machineUnit;
    ConvertToALG(source, machineUnit);
    RTFWK::RTFWKDLDOMachineUnit *macUnit = machineUnit.get();
    target->SetMachineUnit(macUnit);

    //set sartable
    shared_ptr<RTFWK::RTFWKDLDOSartable> sar;
    ConvertToALG(source, sar);
    RTFWK::RTFWKDLDOSartable* sarTemp = sar.get();
    target->SetSartable(sarTemp);

    //set TimeCal
    shared_ptr<RTFWK::RTFWKDLDOTimecalParameters> pTimecal;
    ConvertToALG(source, pTimecal);
    RTFWK::RTFWKDLDOTimecalParameters * timCalTemp = pTimecal.get();
    target->SetTimeCal(timCalTemp);

   //set kernelweightList and ConvolutionData
    shared_ptr<RTFWK::RTFWKDLDOConvolutionData> pConvolutionData;
    ConvertToALG(source, pConvolutionData);
    RTFWK::RTFWKDLDOConvolutionData * convolutionTemp = pConvolutionData.get();

    std::vector<RTFWK::RTFWKDLDOKernelWeight*> algkernelweightList;
    vector<shared_ptr<RTFWK::RTFWKDLDOKernelWeight>> kwsharedlist;
    std::vector<RtKernelweight>& kernelweightList = sourceSuit->kernelWeightList;
    for (auto kernelweight = kernelweightList.begin(); kernelweight != kernelweightList.end(); kernelweight++)
    {
        shared_ptr<RTFWK::RTFWKDLDOKernelWeight> kernelweightShared;
        ConvertToALG(&(*kernelweight), kernelweightShared);
        algkernelweightList.push_back(kernelweightShared.get());
        kwsharedlist.push_back(kernelweightShared);
    }
    convolutionTemp->SetKernelWeight(algkernelweightList);
    target->SetConvolutionData(convolutionTemp);

    //Set AccessoryChunk
    std::map<std::string, std::vector<RTFWK::RTFWKDLDOAccessorychunk*>> accessoryChunkMap;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk>> tempaccessoryChunks;
    for (auto accessoryChunks = sourceSuit->accessoryChunksMap.begin(); accessoryChunks != sourceSuit->accessoryChunksMap.end(); accessoryChunks++)
    {
        std::vector<RTFWK::RTFWKDLDOAccessorychunk*> algAccessoryChunks;
        algAccessoryChunks.clear();
        for (auto accessoryChunk = accessoryChunks->second.begin(); accessoryChunk != accessoryChunks->second.end(); accessoryChunk++)
        {
            std::shared_ptr<RtContour> contourPtr = nullptr;
            for (auto contour = sourceSuit->contours.begin(); contour != sourceSuit->contours.end(); contour++)
            {
                if ((*contour)->get_accessorychunkuid() == (*accessoryChunk)->get_uid())
                {
                    contourPtr = *contour;
                    break;
                }
            }
            std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk> accessoryChunkT;
            AccessoryChunkConverter::ConvertToALG(*(*accessoryChunk).get(), contourPtr.get(), &accessoryChunkT);
            tempaccessoryChunks.push_back(accessoryChunkT);
            algAccessoryChunks.push_back(accessoryChunkT.get());
        }
        accessoryChunkMap[accessoryChunks->first] = algAccessoryChunks;
    }
    target->SetAccessoryChunk(accessoryChunkMap);

    //set goldenstt
    std::map<std::string, RTFWK::RTFWKDLDOGoldenSTT*> goldensttsMap;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT>> tempgoldenstts;
    for (auto goldenstt = sourceSuit->goldensttsMap.begin(); goldenstt != sourceSuit->goldensttsMap.end(); goldenstt++)
    {
        std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT> goldensttT;
        GoldenSTTConverter::ConvertToALG(*(goldenstt->second), &goldensttT);
        tempgoldenstts.push_back(goldensttT);
        goldensttsMap[goldenstt->first] = goldensttT.get();
    }
    target->SetAccessoryGoldenSTT(goldensttsMap);


    // Set AvailablePhotonPlanType
    std::vector<std::string> vecValue;
    std::vector<RTFWK::PLAN_TYPE> vAvailablePhotonPlanType;
    std::string strAvailablePhotonPlanType = source->get_availablephotonplantype();
    (void)boost::split(vecValue, strAvailablePhotonPlanType, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    vAvailablePhotonPlanType.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            vAvailablePhotonPlanType[i] = static_cast<RTFWK::PLAN_TYPE>(atoi(vecValue[i].c_str()));
        }
    }
    vecValue.clear();
    target->SetAvailablePhotonPlanType(vAvailablePhotonPlanType);

    // Set AvailableElectronPlanType 
    std::vector<RTFWK::PLAN_ELECTORN_TYPE> vAvailableElectronPlanType;
    std::string strAvailableElectronPlanType = source->get_availableelectronplantype();
    (void)boost::split(vecValue, strAvailableElectronPlanType, boost::is_any_of(","));
    iLen = vecValue.size();
    vAvailableElectronPlanType.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            vAvailableElectronPlanType[i] = static_cast<RTFWK::PLAN_ELECTORN_TYPE>(atoi(vecValue[i].c_str()));
        }
    }
    vecValue.clear();
    target->SetAvailableElectronPlanType(vAvailableElectronPlanType);

    // Set TrayFactor
    std::map<std::string, float> trays = source->get_discrete_trayfactor();
    std::vector<RTFWK::RTFWKDLDOTrayFactor*> vTrayFactor;
    for (std::map<std::string, float>::iterator it = trays.begin(); it != trays.end(); ++it)
    {
        RTFWK::RTFWKDLDOTrayFactor* trayFactor = new RTFWK::RTFWKDLDOTrayFactor();
        trayFactor->m_sTrayUID = it->first;
        trayFactor->m_fTrayFactor = it->second;
        vTrayFactor.push_back(trayFactor);
    }
    target->SetTrayFactor(vTrayFactor);
   
    //Set McPhoCommissionedUnit
    std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit> mcpcommissionedUnit;
    McPhoCommissionedUnitConverter::ConvertToALG(*(sourceSuit->mcPhoCommissionedUnit.get()), &mcpcommissionedUnit);
    target->SetMcPhoCommissionedUnit(mcpcommissionedUnit.get());

    return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOMachineUnit>& target)
{
    target.reset(new RTFWK::RTFWKDLDOMachineUnit());
    target->SetHidden(source->get_hidden());
    target->SetName(source->get_name());
    target->SetDmax(source->get_dmax());
    target->SetSAD(source->get_sad());
    target->SetPSD(source->get_psd());
    target->Setsrc_diam1(source->get_srcdiam1());
    target->Setsrc_diam2(source->get_srcdiam2());
    target->Setsrc_diam3(source->get_srcdiam3());
    target->Setsrc_ratio1(source->get_srcratio1());
    target->Setsrc_ratio3(source->get_srcratio3());
    target->Sete_eq_diam(source->get_eeqdiam());
    target->Setx_jaw_count(source->get_xjawcount());
    target->Sety_jaw_count(source->get_yjawcount());
    target->Setx_jaw_type(source->get_xjawtype());
    target->Sety_jaw_type(source->get_yjawtype());
    target->SetXSDD(source->get_xsdd());
    target->SetYSDD(source->get_ysdd());
    target->Setgantry_sense(source->get_gantrysense());
    target->Setgantry_offset(source->get_gantryoffset());
    target->Settable_offset(source->get_tableoffset());
    target->Settable_sense(source->get_tablesense());
    target->Setcollimator_offset(source->get_collimatoroffset());
    target->Setcollimator_sense(source->get_collimatorsense());
    target->SetReferenceFluence(source->get_referencefluence());
    target->SetReferenceDose(source->get_referencedose());
    return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOSartable>& target)
{
    target.reset(new RTFWK::RTFWKDLDOSartable());
    std::vector<std::string> vecValue;
    target->SetType(source->get_type());
    target->Setversion(source->get_version());
    target->Settran(source->get_tran());
    target->Setflatness_depth_count(source->get_flatnessdepthcount());
    target->Setflatness_radius_count(source->get_flatnessradiuscount());
    target->Setdepth_count(source->get_depthcount());
    target->Setradius_count(source->get_radiuscount());
    target->SetTAR0_depth_count(source->get_tar0depthcount());
    target->SetSp0(source->get_sp0());

    float m_fflatness_depth_vec[MAX_FLATNESS_DEPTHS];
    std::string strflatness_depth_vec = source->get_flatnessdepthvec();
    (void)boost::split(vecValue, strflatness_depth_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_FLATNESS_DEPTHS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fflatness_depth_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->Setflatness_depth_vec(m_fflatness_depth_vec);

    float m_fflatness_radius_vec[MAX_FLATNESS_RADII];
    std::string strflatness_radius_vec = source->get_flatnessradiusvec();
    (void)boost::split(vecValue, strflatness_radius_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_FLATNESS_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fflatness_radius_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->Setflatness_radius_vec(m_fflatness_radius_vec);

    float m_fdepth_vec[MAX_SAR_DEPTHS];
    std::string strdepth_vec = source->get_depthvec();
    (void)boost::split(vecValue, strdepth_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_DEPTHS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fdepth_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->Setdepth_vec(m_fdepth_vec);

    float m_fradius_vec[MAX_SAR_RADII];
    std::string strradius_vec = source->get_radiusvec();
    (void)boost::split(vecValue, strradius_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fradius_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->Setradius_vec(m_fradius_vec);

    float m_fSAR[MAX_SAR_DEPTHS][MAX_SAR_RADII];
    std::string strSAR = source->get_sar();
    (void)boost::split(vecValue, strSAR, boost::is_any_of(","));
    int iCount = 0;
    for (int i = 0; i < MAX_SAR_DEPTHS && i < source->get_depthcount(); i++)
    {
        for (int j = 0; j < MAX_SAR_RADII && j < source->get_radiuscount(); j++)
        {
            if (!vecValue[iCount].empty())
            {
                m_fSAR[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    target->SetSAR(m_fSAR);

    float m_fTAR0_depth_vec[MAX_SAR_DEPTHS];
    std::string strTAR0_depth_vec = source->get_tar0_depth_vec();
    (void)boost::split(vecValue, strTAR0_depth_vec, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_DEPTHS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fTAR0_depth_vec[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->SetTAR0_depth_vec(m_fTAR0_depth_vec);

    float m_fTAR0[MAX_SAR_DEPTHS];
    std::string strTAR0 = source->get_tar0();
    (void)boost::split(vecValue, strTAR0, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_DEPTHS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fTAR0[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->SetTAR0(m_fTAR0);

    float m_fflatness[MAX_FLATNESS_RADII][MAX_FLATNESS_DEPTHS];
    std::string strflatness = source->get_flatness();
    (void)boost::split(vecValue, strflatness, boost::is_any_of(","));
    iCount = 0;
    for (int i = 0; i < MAX_FLATNESS_RADII && i < source->get_flatnessradiuscount(); i++)
    {
        for (int j = 0; j < MAX_FLATNESS_DEPTHS && j < source->get_flatnessdepthcount(); j++)
        {
            if (!vecValue[iCount].empty())
            {
                m_fflatness[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    target->Setflatness(m_fflatness);

    float m_fSp[MAX_SAR_RADII];
    std::string strSp = source->get_sp();
    (void)boost::split(vecValue, strSp, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSp[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->SetSp(m_fSp);

    float m_fSp_prime[MAX_SAR_RADII];
    std::string strSp_prime = source->get_sp_prime();
    (void)boost::split(vecValue, strSp_prime, boost::is_any_of(","));
    for (int i = 0; i < MAX_SAR_RADII && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSp_prime[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->SetSp_prime(m_fSp_prime);
    return true;
}

bool CommissionedUnitConverter::ALGConvertToDB(const RTFWK::RTFWKDLDOTimecalParameters& source, RtCommissionedunit& target)
{
    target.set_doserate(source.Getdose_rate());
    target.set_decayconstant(source.Getdecay_constant());
    target.set_cal_dist(source.Getcal_dist());
    target.set_cal_depth(source.Getcal_depth());
    target.set_endeffect(source.Getend_effect());
    target.set_timeunits(source.Gettime_units());
    target.set_scxcount(source.GetSc_x_count());
    target.set_scycount(source.GetSc_y_count());

    //set sc x positions
    float fSc_x_positions[MAX_Sc_POSITIONS];
    source.GetSc_x_positions(fSc_x_positions);
    int xcount = std::min(MAX_Sc_POSITIONS, source.GetSc_x_count());
    std::vector<std::string> xpositionStr;
    for (int i = 0; i < xcount; i++)
    {
        std::string xpos = std::to_string((long double)fSc_x_positions[i]);
        xpositionStr.push_back(xpos);
    }
    target.set_scxpositions(boost::join(xpositionStr, ","));

    //set sc y positions
    float fSc_y_positions[MAX_Sc_POSITIONS];
    source.GetSc_y_positions(fSc_y_positions);
    int ycount = std::min(MAX_Sc_POSITIONS, source.GetSc_y_count());
    std::vector<std::string> ypositionStr;
    for (int i = 0; i < ycount; i++)
    {
        std::string ypos = std::to_string((long double)fSc_y_positions[i]);
        ypositionStr.push_back(ypos);
    }
    target.set_scxpositions(boost::join(ypositionStr, ","));

    float m_fSc[MAX_Sc_POSITIONS][MAX_Sc_POSITIONS];
    source.GetSc(m_fSc);
    std::vector<std::string> scStr;
    for (int i = 0; i < ycount; i++)
    {
        for (int j = 0; j < xcount; j++)
        {
            std::string sc = std::to_string((long double)m_fSc[i][j]);
            scStr.push_back(sc);
        }
    }
    target.set_sc(boost::join(scStr, ","));

    return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOTimecalParameters>& target)
{
    target.reset(new RTFWK::RTFWKDLDOTimecalParameters());
    std::vector<std::string> vecValue;
    target->Setdose_rate(source->get_doserate());
    target->Setdecay_constant(source->get_decayconstant());
    target->Setcal_dist(source->get_cal_dist());
    target->Setcal_depth(source->get_cal_depth());
    target->Setend_effect(source->get_endeffect());
    target->Settime_units(source->get_timeunits());
    target->SetSc_x_count(source->get_scxcount());
    target->SetSc_y_count(source->get_scycount());

    float m_fSc_x_positions[MAX_Sc_POSITIONS];
    std::string strSc_x_positions = source->get_scxpositions();
    (void)boost::split(vecValue, strSc_x_positions, boost::is_any_of(","));
    for (int i = 0; i < MAX_Sc_POSITIONS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSc_x_positions[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->SetSc_x_positions(m_fSc_x_positions);

    float m_fSc_y_positions[MAX_Sc_POSITIONS];
    std::string strSc_y_positions = source->get_scypositions();
    (void)boost::split(vecValue, strSc_y_positions, boost::is_any_of(","));
    for (int i = 0; i < MAX_Sc_POSITIONS && i < vecValue.size(); i++)
    {
        if (!vecValue[i].empty())
        {
            m_fSc_y_positions[i] = (float)atof(vecValue[i].c_str());
        }
    }
    vecValue.clear();
    target->SetSc_y_positions(m_fSc_y_positions);

    float m_fSc[MAX_Sc_POSITIONS][MAX_Sc_POSITIONS];
    std::string strSc = source->get_sc();
    (void)boost::split(vecValue, strSc, boost::is_any_of(","));
    int iCount = 0;
    for (int i = 0; i < MAX_Sc_POSITIONS && i < source->get_scycount(); i++)
    {
        for (int j = 0; j < MAX_Sc_POSITIONS && j < source->get_scxcount(); j++)
        {
            if (!vecValue[iCount].empty())
            {
                m_fSc[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    target->SetSc(m_fSc);
    return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOConvolutionData>& target)
{
    target.reset(new RTFWK::RTFWKDLDOConvolutionData());
    shared_ptr<RTFWK::RTFWKDLDOConvDataStruct> convdatastruct;
    ConvertToALG(source, convdatastruct);
    target->SetConvDataStruct(*convdatastruct);

    // outputmuy
    std::string strOutputMuY = source->get_outputmuy();
    RemoveSpace(strOutputMuY);
    std::vector<std::string> vecString;
    (void)boost::split(vecString, strOutputMuY, boost::is_any_of("|"));
    std::vector<RTFWK::OutputItem> vOutputList;
    vOutputList.resize(vecString.size());
    int i = 0;
    for (auto iter = vecString.begin(); iter != vecString.end(); ++iter, ++i)
    {
        std::vector<std::string> vString;
        (void)boost::split(vString, vecString[i], boost::is_any_of(","));
        if (vString.size() == 3)
        {
            float x = boost::lexical_cast<float>(vString[0]);
            float y = boost::lexical_cast<float>(vString[1]);
            float f = boost::lexical_cast<float>(vString[2]);
            vOutputList[i].x = x;
            vOutputList[i].y = y;
            vOutputList[i].output = f;
        }
    }
    target->SetOutputList(vOutputList);
    return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtKernelweight* source, std::shared_ptr<RTFWK::RTFWKDLDOKernelWeight>& target)
{
    return KernelWeightConverter::ConvertToALG(*source, &target);
}

bool CommissionedUnitConverter::ConvertToALG(RtCommissionedunit* source, std::shared_ptr<RTFWK::RTFWKDLDOConvDataStruct>& target)
{
    std::vector<std::string> vecValue;
    target.reset(new RTFWK::RTFWKDLDOConvDataStruct());
    target->SetDiam1(source->get_diam1());
    target->SetDiam2(source->get_diam2());
    target->SetRatio1(source->get_ratio1());
    target->SetTransmission(source->get_transmission());
    target->SetMu(source->get_mu());
    target->SetDmu_dx(source->get_dmu_dx());
    target->SetDmu_dr(source->get_dmu_dr());
    target->SetSrc_d(source->get_src_d());
    target->SetSrc_atten(source->get_src_atten());
    target->SetNorm_10x10(source->get_norm_10x10());
    //target->SetOn(source->get_on);
    target->SetDm(source->get_dm());
    target->SetFfs10(source->get_ffs10());
    target->SetK(source->get_k());
    target->SetA(source->get_a());
    target->SetDF(source->get_df());
    target->SetSF(source->get_sf());
    target->SetC1(source->get_c1());
    target->SetC2(source->get_c2());
    target->SetC3(source->get_c3());
    target->SetConvDataStruct_PSD(source->get_convdatastruct_psd());
    target->SetConvDataStruct_SDD1(source->get_convdatastruct_sdd1());
    target->SetConvDataStruct_SDD2(source->get_convdatastruct_sdd2());

    // offaxisdatax
    std::vector<float> vFloat;
    vFloat = GetVecFromString<float>(source->get_offaxisdatax());
    target->SetOffAxisDataX(vFloat);

    // offaxixsdatay
    vFloat.clear();
    vFloat = GetVecFromString<float>(source->get_offaxisdatay());
    target->SetOffAxisDataY(vFloat);

    // mudatax
    vFloat.clear();
    vFloat = GetVecFromString<float>(source->get_mudatax());
    target->SetMuDataX(vFloat);

    // mudatay
    vFloat.clear();
    vFloat = GetVecFromString<float>(source->get_mudatay());
    target->SetMuDataY(vFloat);

    // dmudatay
    vFloat.clear();
    vFloat = GetVecFromString<float>(source->get_dmudatay());
    target->SetDMuDataY(vFloat);

    // ddmudr
    vFloat.clear();
    vFloat = GetVecFromString<float>(source->get_ddmudr());
    target->SetDDMuDR(vFloat);

    return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtWedgefactor* source, std::shared_ptr<RTFWK::RTFWKDLDOWedgeFactor>& target)
{
    target.reset(new RTFWK::RTFWKDLDOWedgeFactor());
    target->SetCommissionedUnitUID(source->get_commissionedunituid());
    target->SetUID(source->get_uid());
    std::vector<RTFWK::RTFWKDLDOWedgeInfo*> algWedgeFactorList;

    std::string strWedgeFactorList = source->get_wedgefactorlist();
    //std::string strWedgeFactorList;
    // ReadFile_i(sFilePath,strWedgeFactorList);
    if ("" != strWedgeFactorList)
    {
        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, strWedgeFactorList, boost::is_any_of("|"));
        int ilen = static_cast<int>(vecValue.size());
        for (int i = 0; i < ilen; i++)
        {
            std::vector<std::string> strValue;
            (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

            RTFWK::RTFWKDLDOWedgeInfo* wedgeInfo = new RTFWK::RTFWKDLDOWedgeInfo();
            wedgeInfo->m_sAccessoryUID = strValue[0];
            wedgeInfo->m_fFieldSizeX = static_cast<float>(atof(strValue[1].c_str()));
            wedgeInfo->m_fFieldSizeY = static_cast<float>(atof(strValue[2].c_str()));
            wedgeInfo->m_fValue = static_cast<float>(atof(strValue[3].c_str()));
            algWedgeFactorList.push_back(wedgeInfo);
        }
        target->SetWedgeFactorList(algWedgeFactorList);
    }

    for (auto wedgefactor = algWedgeFactorList.begin(); wedgefactor != algWedgeFactorList.end(); wedgefactor++)
    {
        DEL_PTR(*wedgefactor);
    }
	return true;
}

bool CommissionedUnitConverter::ConvertToALG(RtMeasureddata* source, std::shared_ptr<RTFWK::RTFWKDLDOMeasuredData>& target)
{
    target.reset(new RTFWK::RTFWKDLDOMeasuredData());
    target->SetCommissionedunitUID(source->get_commissionedunituid());
    target->setPhantomAngle(source->get_phantomagle());
    target->setScpDepth(source->get_scpdepth());
    target->setWedgeFactorDepth(source->get_scpdepth());
    target->SetUID(source->get_uid());

    //set OutputInWaterList
    std::map<RTFWK::FieldSize, float> vOutputInWater;

    std::string strOutputWater;
    strOutputWater = source->get_outputinwaterlist();
    if (strOutputWater != ""){

        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, strOutputWater, boost::is_any_of("|"));
        int ilen = static_cast<int>(vecValue.size());
        for (int i = 0; i < ilen; i++){
            float scpFactor = 0;
            std::vector<std::string> strValue;
            RTFWK::FieldSize fieldSize;
            (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

            fieldSize.m_fFieldSizeX = static_cast<float>(atof(strValue[0].c_str()));
            fieldSize.m_fFieldSizeY = static_cast<float>(atof(strValue[1].c_str()));
            scpFactor = static_cast<float>(atof(strValue[2].c_str()));
            vOutputInWater[fieldSize] = scpFactor;
        }
    }

    target->SetOutputInWaterList(vOutputInWater);

    //set OutputInAirList
    std::map<RTFWK::FieldSize, float> tempScp;
    std::string strOutputInAir;
    strOutputInAir = source->get_outputinairlist();
    if (strOutputInAir != ""){

        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, strOutputInAir, boost::is_any_of("|"));
        int ilen = static_cast<int>(vecValue.size());
        for (int i = 0; i < ilen; i++){
            float scpFactor = 0;
            std::vector<std::string> strValue;
            RTFWK::FieldSize fieldSize;
            (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

            fieldSize.m_fFieldSizeX = static_cast<float>(atof(strValue[0].c_str()));
            fieldSize.m_fFieldSizeY = static_cast<float>(atof(strValue[1].c_str()));
            scpFactor = static_cast<float>(atof(strValue[2].c_str()));
            tempScp[fieldSize] = scpFactor;
        }
    }
    target->SetOutputInAirList(tempScp);
    return true;
}

bool CommissionedUnitConverter::ConvertToALG(std::vector<RTALG::EnergyAndWeight>energyWeight, std::vector<EnergyAndWeight>&SpectrumCollection)
{
    for (int i = 0; i < energyWeight.size() ; ++i)
    {
        EnergyAndWeight item;
        item.energy = energyWeight[i].energy;
        item.weight = (energyWeight[i].weight);
        SpectrumCollection.push_back(item);
    }
    return true;
}

TPS_END_NAMESPACE