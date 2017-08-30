//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_dosecalculation.cpp
/// 
///  \version 1.0
/// 
///  \date    2013/12/14
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "RtTpsDoseProxy/tps_dp_algcommissionproxy.h"
#include "tps_logger.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_kernelweight.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mcphocommissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_goldenstt.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"
#include "machine_converter.h"
#include "alg/alg_commission.h"
#include "commissioned_unit_converter.h"
#include "measureprofile_converter.h"
#include "RtTpsProtoLib/rt_ms_importfromfile.pb.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measuredprofile.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measureddata.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedelectroncontamination.h"
#include "RtTpsDataAccess/tps_da_machinetables.h"
#include "alg/alg_commission_prenotifier.h"
#include "RtTpsFramework/tps_fw_prepare_notifier.h"



TPS_BEGIN_NAMESPACE


MachineSuit::MachineSuit(RtMachine *rtmachine, TpsMachineSettingManager* manager)
{
    machine = rtmachine;
    manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(rtmachine->get_uid()), vRtAccessoryList);
    manager->GetTable<RtTrayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(rtmachine->get_uid()), vRtTrayList);
}

MachineSuit::~MachineSuit()
{
}

CommissionUintSuit::CommissionUintSuit(RtCommissionedunit *rtcomUnit, TpsMachineSettingManager* manager)
{
    comUnit = rtcomUnit;
    manager->GetTable<RtKernelweightTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUnit->get_uid()), kernelWeightList);

    manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUnit->get_machineuid()), accessorys);

    manager->GetTable<RtMcphocommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUnit->get_uid()), mcPhoCommissionedUnit);

    for (auto accessory = accessorys.begin(); accessory != accessorys.end(); accessory++)
    {
        std::vector<std::shared_ptr<RtAccessorychunk>> accessoryChunks;
        manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple((*accessory)->get_uid(), comUnit->get_uid()), accessoryChunks);
        if (accessoryChunks.size() > 0)
        {
            accessoryChunksMap[(*accessory)->get_uid()] = accessoryChunks;
        }

        for (auto accessoryChunk = accessoryChunks.begin(); accessoryChunk != accessoryChunks.end(); accessoryChunk++)
        {
            std::shared_ptr<RtContour> contour;
            if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple((*accessoryChunk)->get_uid()), contour) > 0)
            {
                contours.push_back(contour);
            }
        }

        std::shared_ptr<RtGoldenstt> goldenstts = nullptr;
        if (manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(comUnit->get_uid(), (*accessory)->get_uid()), goldenstts) > 0)
        {
            goldensttsMap[(*accessory)->get_uid()] = goldenstts;
        }
    }
}

CommissionUintSuit::~CommissionUintSuit()
{
}


AlgCommissionProxy::AlgCommissionProxy()
{
    algcommission.reset(new RTALG::AlgCommission());
}

void AlgCommissionProxy::Init(MachineSuit *rtmachine, CommissionUintSuit *rtcomUnit)
{
    InitializeAlgMachine(rtmachine);
    InitializeAlgCommission(rtcomUnit);
}


bool AlgCommissionProxy::InitializeAlgMachine(MachineSuit *rtmachine)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::InitializeAlgMachine!";
    MachineConverter::ConvertToALG(rtmachine, algMachine);
    RTFWK::RTFWKDLDOMachine* machine = algMachine.get();
    algcommission->SetMachine(machine);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::InitializeAlgMachine!";
    return true;
}

bool AlgCommissionProxy::InitializeAlgCommission(CommissionUintSuit *rtcomUnit)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::InitializeAlgCommission!";
    CommissionedUnitConverter::ConvertToALG(rtcomUnit, algCommissionUnit);
    RTFWK::RTFWKDLDOCommissionedunit* unit = algCommissionUnit.get();
    algcommission->SetCommissionUnit(unit);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::InitializeAlgCommission!";
    return true;
}

bool AlgCommissionProxy::SetMeasureProfileList(std::vector<RtMeasuredprofile*> measureprofilList)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::SetMeasureProfileList!";
    std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> algMeaList;
    MeasureProfileConverter::ConvertToALGList(measureprofilList, mealist);
    for (auto meaureprofile = mealist.begin(); meaureprofile != mealist.end(); meaureprofile++)
    {
        algMeaList.push_back(meaureprofile->get());
    }
    algcommission->SetMeasureProfileList(algMeaList);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::SetMeasureProfileList!";
    return true;
}

int AlgCommissionProxy::SetAccessory(RTFWK::RTFWKDLDOAccessory *algAccessory)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::SetAccessory!";
    int result = algcommission->SetAccessory(algAccessory);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::SetAccessory!";
    return result;
}

int AlgCommissionProxy::CalcGsttCorrectionFactor(RTFWK::RTFWKDLDOGoldenSTT *&pGoldenSTT, RTFWK::DOSEALGORITHM_TYPE algType)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::CalcGsttCorrectionFactor!";
    int result = algcommission->CalcGsttCorrectionFactor(pGoldenSTT, algType);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::CalcGsttCorrectionFactor!";
    return result;
}

int AlgCommissionProxy::CalcCommiDose(int algType, bool bCalcAbsOrRel /*= false*/)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::CalcCommiDose!";
    int result = algcommission->CalcCommiDose((RTFWK::DOSEALGORITHM_TYPE)algType, bCalcAbsOrRel);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::CalcCommiDose!";
    return result;
}

bool AlgCommissionProxy::GetMeasureProfileList(std::vector<shared_ptr<RtMeasuredprofile>>& meaList)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::GetMeasureProfileList!";
    std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> algMeaList = algcommission->GetMeasureProfileList();
    meaList.clear();
    MeasureProfileConverter::ConverBackFromALGList(algMeaList, meaList);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::GetMeasureProfileList!";
    return true;
}

AlgCommissionProxy::~AlgCommissionProxy()
{

}

int AlgCommissionProxy::RescaleCurves(bool bCalcAbsOrRel, RtMeasureddata* measureData, RtWedgefactor* wedgefactor)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::RescaleCurves!";
    shared_ptr<RTFWK::RTFWKDLDOMeasuredData> algmeasureData;
    if (measureData != nullptr)
    {
        CommissionedUnitConverter::ConvertToALG(measureData, algmeasureData);

        if (wedgefactor != nullptr)
        {
            std::shared_ptr<RTFWK::RTFWKDLDOWedgeFactor> wedgefactorShared;
            CommissionedUnitConverter::ConvertToALG(wedgefactor, wedgefactorShared);
            algmeasureData->SetWedgeFactor(wedgefactorShared.get());
        }
    }
    algcommission->RescaleCurves(bCalcAbsOrRel, algmeasureData.get());
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::RescaleCurves!";
    return true;
}

int AlgCommissionProxy::CalcPBScCorrection(RtMeasureddata* measureData, RtCommissionedunit& commissionUnit)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::CalcPBScCorrection!";
    shared_ptr<RTFWK::RTFWKDLDOMeasuredData> algmeasureData;
    if (measureData != nullptr)
    {
        CommissionedUnitConverter::ConvertToALG(measureData, algmeasureData);
    }

    RTFWK::RTFWKDLDOTimecalParameters *timeCalParameter;
    float newReferenceFluence = 0.0f;
    algcommission->CalcPBScCorrection(algmeasureData.get(), timeCalParameter, newReferenceFluence);
    CommissionedUnitConverter::ALGConvertToDB(*timeCalParameter, commissionUnit);
    commissionUnit.set_referencefluence(newReferenceFluence);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::CalcPBScCorrection!";
    return true;
}

int AlgCommissionProxy::CalcOptConvolutionEC(RtDbDef::DOSEALGORITHM_TYPE eAlgType, bool bCalcAbsOrRel, CertifiedElectronContInfo& electronContInfo)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::CalcOptConvolutionEC!";
    RTALG::ElecContamination elecContamination;
    algcommission->CalcOptConvolutionEC((RTFWK::DOSEALGORITHM_TYPE)eAlgType, bCalcAbsOrRel, elecContamination);

    electronContInfo.electronContaminationFlag = (elecContamination.m_iOn == 1);
    electronContInfo.electronMaxDepth = elecContamination.m_fDm;
    electronContInfo.surfaceDose = elecContamination.m_fFfs10;
    electronContInfo.depthCoefficientK = elecContamination.m_fK;
    electronContInfo.offaxisCoefficientA = elecContamination.m_fA;
    electronContInfo.df = elecContamination.m_fDF;
    electronContInfo.sf = elecContamination.m_fSF;
    electronContInfo.cOne = elecContamination.m_fC1;
    electronContInfo.cTwo = elecContamination.m_fC2;
    electronContInfo.cThree = elecContamination.m_fC3;
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::CalcOptConvolutionEC!";
    return TPS_ER_SUCCESS;
}

int AlgCommissionProxy::PrepareMonoPDD(TpsMCPrepareNotifier* pNotifier)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::PrepareMonoPDD!";
    //pNotifier = new TpsMCPrepareNotifier;
    CommiPreEneNotifier * pd2 = dynamic_cast<CommiPreEneNotifier*>(pNotifier);
    algcommission->SetCommiPreEneNotifier(pd2);

    int PreEneCount = -1;
    algcommission->ReadEneSpecStorNumber(PreEneCount);
    int result = algcommission->ThreadFuncForMonteCarloPrepare(PreEneCount);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::PrepareMonoPDD!";
    return result;
}

int AlgCommissionProxy::ClearMCPraPareFileFolder()
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::ClearMCPraPareFileFolder!";
    int result = algcommission->ClearMCPraPareFileFolder();
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::ClearMCPraPareFileFolder!";
    return result;
}

int AlgCommissionProxy::ReadEneSpecStorNumber(int &iFileNum)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::ReadEneSpecStorNumber!";
    int result = algcommission->ReadEneSpecStorNumber(iFileNum);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::ReadEneSpecStorNumber!";
    return result;
}



int AlgCommissionProxy::MonteCarloOptSpectrum(std::vector<RTALG::EnergyAndWeight>& OutPWeight, std::vector<RTALG::EnergyAndWeight>& OutSWeight, std::vector<RTALG::EnergyAndWeight>& OutEWeight)
{
    TPS_LOG_DEV_ERROR << "Begin AlgCommissionProxy::MonteCarloOptSpectrum!";
    int result = algcommission->MonteCarloOptSpectrum(OutPWeight, OutSWeight, OutEWeight);
    TPS_LOG_DEV_ERROR << "End AlgCommissionProxy::MonteCarloOptSpectrum!";
    return result;
}

TPS_END_NAMESPACE
