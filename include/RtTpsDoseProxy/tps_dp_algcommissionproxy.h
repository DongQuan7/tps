//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_dosecalculation.h
/// 
///  \version 1.0
/// 
///  \date    2013/12/14
//////////////////////////////////////////////////////////////////////////

#ifndef	TPS_DP_ALGCOMMISSIONPROXY_H_
#define TPS_DP_ALGCOMMISSIONPROXY_H_

#include "tps_dp_defs.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_tray.h"
#include "alg\alg_commission.h"

namespace RTALG
{
    class AlgCommission;
}
namespace RTFWK
{
    class RTFWKDLDOMLCSetting;
    class RTFWKDLDOJawLimits;
    class RTFWKDLDOTrayFactor;
    class RTFWKDLDOMLCShape;
    class RTFWKDLDOMeasuredData;
    class RTFWKDLDOWedgeFactor;
    class RTFWKDLDOMeasuredProfile;
    class RTFWKDLDOKernelWeight;
    class RTFWKDLDOConvDataStruct;
    class RTFWKDLDOConvolutionData;
    class RTFWKDLDOSartable;
    class RTFWKDLDOMachineUnit;
    class RTFWKDLDOMcPhoMonoEnergyWeight;
    class RTFWKDLDOContour;
    class RTFWKDLDOMachine;
    class RTFWKDLDOCommissionedunit;
    class RTFWKDLDOAccessory;
    class RTFWKDLDOAccessorychunk;
    class RTFWKDLDOTray;
    class RTFWKDLDOApplicator;
    class RTFWKDLDOMcPhoCommissionedUnit;
    class RTFWKDLDOGoldenSTT;
    class RTFWKDLDOTimecalParameters;
}

namespace ms{
    namespace proto{
        class RT_MS_MachineConfigInfo;
        class RT_MS_Proto;
        class RT_MS_PencilBeam;
        class RT_MS_Convolution;
        class RT_MS_ImportFromFileList;
        class RT_MS_SCPInfoList;
        class RT_MS_MonteCarlo;
        class RT_MS_ImportGSTTFromFile;
        class RT_MS_WedgeFactorSettingInfoList;
        class RT_MS_PhysicalWedgeField;
        class RT_MS_VirtualWedgeInfo;
        class RT_MS_ImportFromFileInfo;
        class RT_MS_MachineTray;
    }
}



TPS_BEGIN_NAMESPACE
class ITpsDataRepository;
class RtJawLimits;
class RtMLCShape;
class RtConvDataStruct;
class RtConvolutionData;
class RtSartable;
class RtMcPhoMonoEnergyWeight;

class RtMeasureddata;
class RtMeasuredprofile;
class RtKernelweight;
class RtContour;
class RtTrayFactor;
class RtMachine;
class RtCommissionedunit;
class RtAccessory;
class RtAccessorychunk;
class RtTray;
class RtApplicator;
class RtMcphocommissionedunit;
class RtGoldenstt;
class TpsMachineSettingManager;
class RtWedgefactor;
struct CertifiedElectronContInfo;
class TpsMCPrepareNotifier;

class TPS_DP_DECLSPEC MachineSuit
{
public:
    MachineSuit(RtMachine *rtmachine, TpsMachineSettingManager* manager);
    std::vector<RtAccessory>& GetAccessoryList(){ return vRtAccessoryList; }
    std::vector<RtTray>& GetTrayList(){ return vRtTrayList; }
    RtMachine* GetMachine(){ return machine; }
    ~MachineSuit();

private:
    ITpsDataRepository *mDataRepository;
    std::vector<RtAccessory> vRtAccessoryList;
    std::vector<RtTray> vRtTrayList;
    RtMachine *machine;
};

class TPS_DP_DECLSPEC CommissionUintSuit
{
public:
    CommissionUintSuit(RtCommissionedunit *rtcomUnit, TpsMachineSettingManager* manager);
    RtCommissionedunit* GetCommissionUnit(){ return comUnit; }
    std::vector<RtKernelweight>& GetKernelweightList(){ return kernelWeightList; }
    ~CommissionUintSuit();

private:
    ITpsDataRepository *mDataRepository;
public:
    RtCommissionedunit* comUnit;
    std::vector<RtKernelweight> kernelWeightList;
    std::map<std::string, std::vector<std::shared_ptr<RtAccessorychunk>>> accessoryChunksMap;
    std::vector<std::shared_ptr<RtContour>> contours;
    std::map<std::string, std::shared_ptr<RtGoldenstt>> goldensttsMap;
    std::vector<std::shared_ptr<RtAccessory>> accessorys;
    std::shared_ptr<RtMcphocommissionedunit> mcPhoCommissionedUnit;
};

class TPS_DP_DECLSPEC AlgCommissionProxy
{
public:
    AlgCommissionProxy();
    ~AlgCommissionProxy();
    bool InitializeAlgMachine(MachineSuit *rtmachine);
    bool InitializeAlgCommission(CommissionUintSuit *rtcomUnit);
    void Init(MachineSuit *rtmachine, CommissionUintSuit *rtcomUnit);
    bool SetMeasureProfileList(std::vector<RtMeasuredprofile*> measureprofilList);
    int CalcCommiDose(int algType, bool bCalcAbsOrRel = false);
    int RescaleCurves(bool bCalcAbsOrRel, RtMeasureddata* measureData, RtWedgefactor* wedgefactor);
    int CalcPBScCorrection(RtMeasureddata* measureData, RtCommissionedunit &commissionUnit);
    bool GetMeasureProfileList(std::vector<std::shared_ptr<RtMeasuredprofile>>& meaList);
    int CalcOptConvolutionEC(RtDbDef::DOSEALGORITHM_TYPE eAlgType, bool bCalcAbsOrRel, CertifiedElectronContInfo& electronContInfo);
    TPS_ERROR_CODES GetLastError() const;
    int PrepareMonoPDD(TpsMCPrepareNotifier* pNotifier);
    int MonteCarloOptSpectrum(std::vector<RTALG::EnergyAndWeight>& OutPWeight, std::vector<RTALG::EnergyAndWeight>& OutSWeight, std::vector<RTALG::EnergyAndWeight>& OutEWeight);
    int ClearMCPraPareFileFolder();
    int ReadEneSpecStorNumber(int &iFileNum);
    int SetAccessory(RTFWK::RTFWKDLDOAccessory *algAccessory);
    int CalcGsttCorrectionFactor(RTFWK::RTFWKDLDOGoldenSTT *&pGoldenSTT, RTFWK::DOSEALGORITHM_TYPE algType);
private:
    TPS_ERROR_CODES                    mLastError;
    std::shared_ptr<RTALG::AlgCommission> algcommission;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>> mealist;
    std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit> algCommissionUnit;
    std::shared_ptr<RTFWK::RTFWKDLDOMachine> algMachine;
};

TPS_END_NAMESPACE

#endif
