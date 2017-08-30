////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  xi.chen  xi.chen@united-imaging.com
/// 
///  \file rt_tps_doseoptimizationparam.h
/// 
///  \brief struct DoseOptimizationParam declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/27
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DOSEOPTIMIZATIONPARAM_H_
#define RT_TPS_DOSEOPTIMIZATIONPARAM_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_Optinput;
}

struct DoseOptimizationParam{
public:
    DoseOptimizationParam();
    DoseOptimizationParam(const proto::RT_TPS_Optinput& protoOptinput);

    int GetIterNum() const;
    int GetCriterionType() const;
private:
    int mIterNum;
    int mCriterionType;
};

TPS_END_NAMESPACE

#endif