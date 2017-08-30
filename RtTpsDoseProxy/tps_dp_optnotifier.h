//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_optnotifier.h
/// 
///  \version 1.0
/// 
///  \date    2014/04/14
//////////////////////////////////////////////////////////////////////////

#ifndef	TPS_DP_OPTNOTIFIER_H_
#define TPS_DP_OPTNOTIFIER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"

#include "alg/alg_ioptnotifier.h"

namespace RTFWK{
    class RTFWKDLDODoseOptOutputBase;
}

namespace RTALG{
    class IDoseOptNotifier;
}

TPS_BEGIN_NAMESPACE

class TpsOptObserver;
class ITpsDataRepository;

namespace proto{
    class RT_TPS_OptOutput;
}

class TPS_DP_DECLSPEC TpsOptNotifier : public RTALG::IDoseOptNotifier{
public:
    TpsOptNotifier(void);
    ~TpsOptNotifier(void);
    virtual void NotifyOutput(RTFWK::RTFWKDLDODoseOptOutputBase* pOptOutput);
    virtual void NotifyProgress();

    void SetDataRepository(ITpsDataRepository* repos) {mDataRepository = repos;}
    void SetObservers(TpsOptObserver& observers);
private:
    ITpsDataRepository          *mDataRepository;
    TpsOptObserver*             mObserver;
};

TPS_END_NAMESPACE

#endif