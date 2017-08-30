//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_optproxy.h
/// 
///  \version 1.0
/// 
///  \date    2014/04/14
//////////////////////////////////////////////////////////////////////////

#ifndef	TPS_DP_OPTPROXY_H_
#define TPS_DP_OPTPROXY_H_

#include "tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_doseoptinputbase.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

namespace RTFWK
{
    class RTFWKDLDODoseOptInputBase;
}

namespace RTALG
{
    class OptimizerBase;
}


TPS_BEGIN_NAMESPACE

class TpsOptNotifier;
class TpsOptObserver;
class ITpsDataRepository;
class RtNormgroup;
class RtDoseoptgoalbase;
class RtPlan;

struct DoseOptimizationParam;

class TPS_DP_DECLSPEC TpsOptProxy
{
public:
    TpsOptProxy();
    ~TpsOptProxy();

    bool Initialize();

    void SetDataRepository(ITpsDataRepository* repos) {mDataRepository = repos;}

    void SetObserver(TpsOptObserver& observer) { mObserver = &observer; }

    bool OptStart(RtDbDef::PLAN_TYPE optType, const std::string& ngUid, 
        const std::vector<std::string>& vecBasedOnNgUids,
        const std::vector<RtDoseoptgoalbase*>& vecGoals,
        const DoseOptimizationParam& param);

    bool OptAbort();

private:
    RTFWK::DOSEOPTALGO GetOptType_i(const RtDbDef::PLAN_TYPE& planType);

    bool PrepareOptNg_i(const RtNormgroup& pNormgroup,
        const std::vector<std::string>& vBasedOnNgUids,
        RTFWK::RTFWKDLDODoseOptInputBase* optInput);

    bool PrepareConceptVolume_i(const RtPlan& plan,
        RTFWK::RTFWKDLDODoseOptInputBase* optInput);

    bool PrepareOptParams_i(
        const std::vector<RtDoseoptgoalbase*>& vGoals, 
        const DoseOptimizationParam& param, 
        RTFWK::RTFWKDLDODoseOptInputBase* optInput,
        const std::string& sImageSeriesUid);

private:
    RTALG::OptimizerBase*       mOptimizer;
    TpsOptNotifier*             mNotifier;
    TpsOptObserver*             mObserver;
    ITpsDataRepository*         mDataRepository;
    map<RTFWK::DOSEOPTALGO, RTALG::OptimizerBase*> optimizerMap;
};

TPS_END_NAMESPACE

#endif
