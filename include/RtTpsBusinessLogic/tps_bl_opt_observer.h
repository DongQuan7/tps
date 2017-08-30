//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  xiaoqing.shangguan xiaoqing.shangguan@united-imaging.com
///
///  \file    tps_pca_planningctrloptobserver.h
///  \brief
///
///  \version 1.0
///  \date    Apr.22, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_PCA_PLANNINGOPTOBSERVER_H_
#define TPS_PCA_PLANNINGOPTOBSERVER_H_

#include "RtTpsDoseProxy/tps_dp_optobserver.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedoptoutput.h"

TPS_BEGIN_NAMESPACE

class ITpsCommonController;
class ITpsDataRepository;
class DoseCalculationImpl;
class TpsSaveObjects;
class OptActiveResponser;
typedef std::shared_ptr<OptActiveResponser> OptResponserPtr;

namespace proto{
    class RT_TPS_OptOutput;
}

class TpsPlanningCtrlOptObserver : public TpsOptObserver
{
public:
    TpsPlanningCtrlOptObserver();
    ~TpsPlanningCtrlOptObserver();
    inline void SetDataRepository(ITpsDataRepository* repos) {mDataRepository = repos;}
    inline void SetSaveCmd(TpsSaveObjects* saveCmd) {mSaveObjects = saveCmd;}
    inline void SetDoseCalcImpl(DoseCalculationImpl* doseCalcImpl) {mDoseCalcImpl = doseCalcImpl;}
    virtual void optUpdated(CertifiedOptOutput optOutput);
    void AttachResponser(const OptResponserPtr& responser) { mResponser = responser;}
private:
    ITpsDataRepository          *mDataRepository;
    DoseCalculationImpl         *mDoseCalcImpl;
    TpsSaveObjects              *mSaveObjects;
    OptResponserPtr              mResponser;
};

TPS_END_NAMESPACE

#endif
