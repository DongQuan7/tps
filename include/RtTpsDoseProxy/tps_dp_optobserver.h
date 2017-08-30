//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file    tps_dp_optobserver.h
/// 
///  \version 1.0
/// 
///  \date    2014/04/18
//////////////////////////////////////////////////////////////////////////

#ifndef	TPS_DP_OPTOBSERVER_H_
#define TPS_DP_OPTOBSERVER_H_

#include "tps_dp_defs.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedoptoutput.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC TpsOptObserver{
public:
    TpsOptObserver() {}
    ~TpsOptObserver() {}
    virtual void optUpdated(CertifiedOptOutput optOutput) = 0;
};

TPS_END_NAMESPACE

#endif