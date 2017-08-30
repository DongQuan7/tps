//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file mcphocommissioned_unit_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2016/03/17
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mcphocommissioned_unit_converter.h"
#include "boost/lexical_cast.hpp"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mcphomonoenergyweight.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_mcphomonoenergyweight.h"
#include "tps_logger.h"
#include "RtTpsDoseProxy/tps_dp_alg_converter_common.h"

TPS_BEGIN_NAMESPACE



void McPhoCommissionedUnitConverter::FillMcPhoLeafMapList_i(
    std::vector<RTFWK::RTFWKDLDOMcPhoLeafMap*> &vMcPhoLeafMap,
    std::string sMcPhoLeafMap)
{
    std::map<int, std::string> mMcPhoLeafMap =
        GetMapFromString<int, std::string>(sMcPhoLeafMap);
    vMcPhoLeafMap.resize(mMcPhoLeafMap.size(), NULL);
    int i = 0;
    for (auto iter = mMcPhoLeafMap.begin(); iter != mMcPhoLeafMap.end(); ++iter, ++i)
    {
        vMcPhoLeafMap[i] = new RTFWK::RTFWKDLDOMcPhoLeafMap;
        vMcPhoLeafMap[i]->m_iLeafMapIdx = iter->first;
        vMcPhoLeafMap[i]->m_sFilePath = iter->second;
    }
}

void McPhoCommissionedUnitConverter::FillEasyOffAxis_i(
    std::vector<RTFWK::RTFWKDLDOEasyOffAxis*> &vEasyOffAxis,
    std::string sEasyOffAxis)
{
    std::map<float, float> mEasyOffAxis = GetMapFromString<float, float>(sEasyOffAxis);
    vEasyOffAxis.resize(mEasyOffAxis.size(), NULL);
    int i = 0;
    for (auto iter = mEasyOffAxis.begin(); iter != mEasyOffAxis.end(); ++iter, ++i)
    {
        vEasyOffAxis[i] = new RTFWK::RTFWKDLDOEasyOffAxis;
        vEasyOffAxis[i]->m_fRadius = iter->first;
        vEasyOffAxis[i]->m_fValue = iter->second;
    }
}

void McPhoCommissionedUnitConverter::FillMcPhoAbsCorrectionFactor_i(
    std::vector<RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor*> &vCorrectFac, 
    std::string sCorrectorFactors)
{
    std::vector<std::string> vecString;
    (void)boost::split(vecString, sCorrectorFactors, boost::is_any_of("|"));
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
            RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor* correctFac = new RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor;
            correctFac->m_fMcPhoAbsCorrectionFieldSizeX = x;
            correctFac->m_fMcPhoAbsCorrectionFieldSizeY = y;
            correctFac->m_fMcPhoAbsCorrectionFactor = f;
            vCorrectFac.push_back(correctFac);
        }
    }
}

void McPhoCommissionedUnitConverter::ConvertToALG(
    const RtMcphocommissionedunit& tpsMcphocommissionedunit,
    std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit>* algMcPhoCommissionedUnit)
{
    algMcPhoCommissionedUnit->reset(new RTFWK::RTFWKDLDOMcPhoCommissionedUnit(false));
    (*algMcPhoCommissionedUnit)->SetUID(tpsMcphocommissionedunit.get_uid());
    (*algMcPhoCommissionedUnit)->SetDBUID(tpsMcphocommissionedunit.get_uid());
    (*algMcPhoCommissionedUnit)->SetCommissionedUnitUID(tpsMcphocommissionedunit.get_commissionedunituid());
    (*algMcPhoCommissionedUnit)->SetFringe(tpsMcphocommissionedunit.get_fringe());
    (*algMcPhoCommissionedUnit)->SetMcPhoA(tpsMcphocommissionedunit.get_mcphoa());
    (*algMcPhoCommissionedUnit)->SetGaussianBlurSwitch(tpsMcphocommissionedunit.get_mcgaussianblurswitch());
    (*algMcPhoCommissionedUnit)->SetGaussianSigmaX(tpsMcphocommissionedunit.get_mcgaussiansigmax());
    (*algMcPhoCommissionedUnit)->SetGaussianSigmaY(tpsMcphocommissionedunit.get_mcgaussiansigmay());
    (*algMcPhoCommissionedUnit)->SetMlcLeakage(tpsMcphocommissionedunit.get_mcmlcleakage());

    //easyoffaxisvec
    std::vector<float> vFloat;
    std::string sFile;
    if (!tpsMcphocommissionedunit.get_phoflulist().empty())
    {
        std::vector<RTFWK::RTFWKDLDOEasyOffAxis*> vEasyOffAxis;
        FillEasyOffAxis_i(vEasyOffAxis, tpsMcphocommissionedunit.get_phoflulist());
        (*algMcPhoCommissionedUnit)->SetEasyOffAxisVec(vEasyOffAxis);
        std::vector<RTFWK::RTFWKDLDOEasyOffAxis*>::iterator itEasyOffAxis = vEasyOffAxis.begin();
        for (; itEasyOffAxis != vEasyOffAxis.end(); ++itEasyOffAxis)
        {
            DEL_PTR(*itEasyOffAxis);
        }

    }
    //eneoffaxisenergy
    if (!tpsMcphocommissionedunit.get_eneoffaxisenergyvec().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_eneoffaxisenergyvec());
        (*algMcPhoCommissionedUnit)->SetEneOffAxisEnergyVec(vFloat);
    }
    //eneoffaxisradiousvce
    if (!tpsMcphocommissionedunit.get_eneoffaxisradiusveclist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_eneoffaxisradiusveclist());
        (*algMcPhoCommissionedUnit)->SetEneOffAxisRadiusVec(vFloat);
    }
    //eneoffaxisvalue
    if (!tpsMcphocommissionedunit.get_eneoffaxisvaluelist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_eneoffaxisvaluelist());
        (*algMcPhoCommissionedUnit)->SetEneOffAxisValue(vFloat);
    }
    //McPhoabsCorrectionFactor
    if (!tpsMcphocommissionedunit.get_mcphoabscorrectionfactor().empty())
    {
        std::vector<RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor*> vCorrectFactor;
        FillMcPhoAbsCorrectionFactor_i(vCorrectFactor, tpsMcphocommissionedunit.get_mcphoabscorrectionfactor());
        if (vCorrectFactor.size() > 0)
        {
            (*algMcPhoCommissionedUnit)->SetMcPhoAbsCorrectionFactor(vCorrectFactor);
            std::vector<RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor*>::iterator itCorrectFactor = vCorrectFactor.begin();
            DEL_PTRVECTOR(vCorrectFactor);
        }
    }

    (*algMcPhoCommissionedUnit)->SetMcPhoC1(tpsMcphocommissionedunit.get_mcphoc1());
    (*algMcPhoCommissionedUnit)->SetMcPhoC2(tpsMcphocommissionedunit.get_mcphoc2());
    (*algMcPhoCommissionedUnit)->SetMcPhoC3(tpsMcphocommissionedunit.get_mcphoc3());
    (*algMcPhoCommissionedUnit)->SetMcPhoDF(tpsMcphocommissionedunit.get_mcphodf());
    (*algMcPhoCommissionedUnit)->SetMcPhoDm(tpsMcphocommissionedunit.get_mcphodm());

    //McPhoEnergy
    if (!tpsMcphocommissionedunit.get_mcphoenergy().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcphoenergy());
        (*algMcPhoCommissionedUnit)->SetMcPhoEnergy(vFloat);
    }

    (*algMcPhoCommissionedUnit)->SetMcPhoFfs10(tpsMcphocommissionedunit.get_mcphoffs10());
    (*algMcPhoCommissionedUnit)->SetMcPhoFilePath(tpsMcphocommissionedunit.get_mcpholist());
    (*algMcPhoCommissionedUnit)->SetMcPhoK(tpsMcphocommissionedunit.get_mcphok());

    (*algMcPhoCommissionedUnit)->SetMcPhoMLCAirGapWidth(tpsMcphocommissionedunit.get_mcphomlcairgapwidth());
    (*algMcPhoCommissionedUnit)->SetMcPhoMLCCenterPosition(tpsMcphocommissionedunit.get_mcphomlccenterposition());
    (*algMcPhoCommissionedUnit)->SetMcPhoMLCTGWidth(tpsMcphocommissionedunit.get_mcphomlctgwidth());
    (*algMcPhoCommissionedUnit)->SetMcPhoMLCThickness(tpsMcphocommissionedunit.get_mcphomlcthickness());
    (*algMcPhoCommissionedUnit)->SetMcPhoMLLeafEndRadius(tpsMcphocommissionedunit.get_mcphomlleafendradius());

    (*algMcPhoCommissionedUnit)->SetMcPhoMuFactor(tpsMcphocommissionedunit.get_mcphomufactor());
    (*algMcPhoCommissionedUnit)->SetMcPhoNorm10x10(tpsMcphocommissionedunit.get_mcphonorm10x10());
    (*algMcPhoCommissionedUnit)->SetMcPhoOn(tpsMcphocommissionedunit.get_mcphoon());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam1(tpsMcphocommissionedunit.get_mcphoparam1()); 
    (*algMcPhoCommissionedUnit)->SetMcPhoParam2(tpsMcphocommissionedunit.get_mcphoparam2());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam3(tpsMcphocommissionedunit.get_mcphoparam3());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam4(tpsMcphocommissionedunit.get_mcphoparam4());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam5(tpsMcphocommissionedunit.get_mcphoparam5());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam6(tpsMcphocommissionedunit.get_mcphoparam6());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam7(tpsMcphocommissionedunit.get_mcphoparam7());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam8(tpsMcphocommissionedunit.get_mcphoparam8());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam9(tpsMcphocommissionedunit.get_mcphoparam9());
    (*algMcPhoCommissionedUnit)->SetMcPhoParam10(tpsMcphocommissionedunit.get_mcphoparam10());
    (*algMcPhoCommissionedUnit)->SetMcPhoSF(tpsMcphocommissionedunit.get_mcphosf());
    (*algMcPhoCommissionedUnit)->SetMcPhoXJawThickness(tpsMcphocommissionedunit.get_mcphoxjawthickness());
    (*algMcPhoCommissionedUnit)->SetMcPhoXJawUpPosition(tpsMcphocommissionedunit.get_mcphoxjawupposition());
    (*algMcPhoCommissionedUnit)->SetMcPhoYJawThickness(tpsMcphocommissionedunit.get_mcphoyjawthickness());
    (*algMcPhoCommissionedUnit)->SetMcPhoYJawUpPosition(tpsMcphocommissionedunit.get_mcphoyjawupposition());
    (*algMcPhoCommissionedUnit)->SetMeanLeafOriShift(tpsMcphocommissionedunit.get_meanleaforishift());
    (*algMcPhoCommissionedUnit)->SetMlcLeafShapeNum(tpsMcphocommissionedunit.get_mlcleafshapenum());


    (*algMcPhoCommissionedUnit)->SetPhspDistanceFromTarget(tpsMcphocommissionedunit.get_phspdistancefromtarget());

    (*algMcPhoCommissionedUnit)->SetPrimaryEnergystart(tpsMcphocommissionedunit.get_mcprimaryenergystart());
    (*algMcPhoCommissionedUnit)->SetPrimaryEnergyend(tpsMcphocommissionedunit.get_mcprimaryenergyend());
    (*algMcPhoCommissionedUnit)->SetPrimaryEnergybin(tpsMcphocommissionedunit.get_mcprimaryenergybin());
    (*algMcPhoCommissionedUnit)->SetPrimaryEnergyrestart(tpsMcphocommissionedunit.get_mcprimaryenergyrestart());
    (*algMcPhoCommissionedUnit)->SetSecondaryEnergystart(tpsMcphocommissionedunit.get_mcsecondaryenergystart());
    (*algMcPhoCommissionedUnit)->SetSecondaryEnergyend(tpsMcphocommissionedunit.get_mcsecondaryenergyend());
    (*algMcPhoCommissionedUnit)->SetSecondaryEnergybin(tpsMcphocommissionedunit.get_mcsecondaryenergybin());
    (*algMcPhoCommissionedUnit)->SetSecondaryEnergyrestart(tpsMcphocommissionedunit.get_mcsecondaryenergyrestart());
    (*algMcPhoCommissionedUnit)->SetElectronEngery(tpsMcphocommissionedunit.get_mcelectronenergy());
    (*algMcPhoCommissionedUnit)->SetCommiParticleNumber(tpsMcphocommissionedunit.get_mccommiparticlenumber());

    if (!tpsMcphocommissionedunit.get_mcprimaryenergylist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcprimaryenergylist());
        (*algMcPhoCommissionedUnit)->SetPrimaryEnergyList(vFloat);
    }

    if (!tpsMcphocommissionedunit.get_mcprimaryweightlist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcprimaryweightlist());
        (*algMcPhoCommissionedUnit)->SetPrimaryWeightList(vFloat);
    }

    if (!tpsMcphocommissionedunit.get_mcsecondaryenergylist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcsecondaryenergylist());
        (*algMcPhoCommissionedUnit)->SetSecondaryEnergyList(vFloat);
    }

    if (!tpsMcphocommissionedunit.get_mcsecondaryweightlist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcsecondaryweightlist());
        (*algMcPhoCommissionedUnit)->SetSecondaryWeightList(vFloat);
    }

    if (!tpsMcphocommissionedunit.get_mcelecenergylist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcelecenergylist());
        (*algMcPhoCommissionedUnit)->SetElecEnergyList(vFloat);
    }

    if (!tpsMcphocommissionedunit.get_mcelecweightlist().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcelecweightlist());
        (*algMcPhoCommissionedUnit)->SetElecWeightList(vFloat);
    }


    (*algMcPhoCommissionedUnit)->SetFlatFilterDist(tpsMcphocommissionedunit.get_mcprimaryflatfilterdist());
    (*algMcPhoCommissionedUnit)->SetJawDistance(tpsMcphocommissionedunit.get_mcprimaryjawdistance());
    (*algMcPhoCommissionedUnit)->SetSigmaP(tpsMcphocommissionedunit.get_mcsourcesigmaprimary());
    (*algMcPhoCommissionedUnit)->SetSigmaS(tpsMcphocommissionedunit.get_mcsourcesigmasecondary());
    (*algMcPhoCommissionedUnit)->SetPrimarySoftening(tpsMcphocommissionedunit.get_mcprimarysoftening());
    (*algMcPhoCommissionedUnit)->SetSofteningTunningF(tpsMcphocommissionedunit.get_mcsofteningtunningf());
    (*algMcPhoCommissionedUnit)->SetScatteringSoftening(tpsMcphocommissionedunit.get_mcscatteringcompton());
    (*algMcPhoCommissionedUnit)->SetSecondarySourceType(tpsMcphocommissionedunit.get_mcscatteringcompton());

    if (!tpsMcphocommissionedunit.get_mcsecondarysourcepara().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcsecondarysourcepara());
        std::vector<float> vFloatA;
        std::vector<float> vFloatB;
        int size = vFloat.size();
        for (int i = 0; i < size; i += 2)
        {
            vFloatA.push_back(vFloat[i]);
            if (i + 1 < size)
            {
                vFloatB.push_back(vFloat[i + 1]);
            }
        }
        (*algMcPhoCommissionedUnit)->SetFlatFilterA(vFloatA);
        (*algMcPhoCommissionedUnit)->SetFlatFilterB(vFloatB);
    }

    if (!tpsMcphocommissionedunit.get_mceleccorfieldsize().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mceleccorfieldsize());
        (*algMcPhoCommissionedUnit)->SetElecCorFieldSize(vFloat);
    }

    if (!tpsMcphocommissionedunit.get_mcphotonfluence().empty())
    {
        vFloat = GetVecFromString<float>(tpsMcphocommissionedunit.get_mcphotonfluence());
        (*algMcPhoCommissionedUnit)->SetElecCorFactor(vFloat);
    }

    (*algMcPhoCommissionedUnit)->SetMCJawLeakage(tpsMcphocommissionedunit.get_mcjawleakage());
    (*algMcPhoCommissionedUnit)->SetMCTongueWidth(tpsMcphocommissionedunit.get_mctonguewidth());
    (*algMcPhoCommissionedUnit)->SetMCGrooveWidth(tpsMcphocommissionedunit.get_mcgroovewidth());
    (*algMcPhoCommissionedUnit)->SetMCLeafTipWidth(tpsMcphocommissionedunit.get_mcleaftipwidth());
    (*algMcPhoCommissionedUnit)->SetMCTransmissionTG(tpsMcphocommissionedunit.get_mctransmission());
    (*algMcPhoCommissionedUnit)->SetMCReserve(tpsMcphocommissionedunit.get_reserve());
}

void McPhoCommissionedUnitConverter::CreateALGList(
    std::vector<RtMcphocommissionedunit*>& tpsMcphocommissionedunitList,
    ITpsDataRepository *mDataRepository,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit>>* algMcPhoCommissionedUnitList)
{
    try
    {
        for (std::vector<RtMcphocommissionedunit*>::iterator itMcphocommissionedunit = tpsMcphocommissionedunitList.begin();
            itMcphocommissionedunit != tpsMcphocommissionedunitList.end(); ++itMcphocommissionedunit)
        {
            RtMcphocommissionedunit* tpsMcphocommissionedunit = *itMcphocommissionedunit;
            std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit> algMcPhoCommissionedUnit;
            McPhoCommissionedUnitConverter::ConvertToALG(*tpsMcphocommissionedunit, &algMcPhoCommissionedUnit);

            //McPhoMonoEnergyWeight
            std::vector<RTFWK::RTFWKDLDOMcPhoMonoEnergyWeight*> algphoMonoEnergyWeightList;
            std::vector<RtMcphomonoenergyweight*> phoMonoEnergyWeightList;
            mDataRepository->GetMachineManager()->GetMcphomonoenergyweightListByMcphocommissionedunituid(
                tpsMcphocommissionedunit->get_uid(), phoMonoEnergyWeightList);
            for (int i = 0; i < phoMonoEnergyWeightList.size(); i++)
            {
                algphoMonoEnergyWeightList.push_back(new RTFWK::RTFWKDLDOMcPhoMonoEnergyWeight());
                algphoMonoEnergyWeightList[i]->SetInjectElectronEnergy(phoMonoEnergyWeightList[i]->get_injectelectronenergy());
                algphoMonoEnergyWeightList[i]->SetInjectElectronWeight(phoMonoEnergyWeightList[i]->get_injectelectronweight());
            }
            algMcPhoCommissionedUnit->SetMcPhoMonoEnergyWeight(algphoMonoEnergyWeightList);
            DEL_PTRVECTOR(algphoMonoEnergyWeightList);

            algMcPhoCommissionedUnitList->push_back(algMcPhoCommissionedUnit);
        }
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "fail to convert to RTFWKDLDOMcPhoCommissionedUnit." << ex.what();
        printf("fail to convert to RTFWKDLDOMcPhoCommissionedUnit.\n");
    }
}


TPS_END_NAMESPACE