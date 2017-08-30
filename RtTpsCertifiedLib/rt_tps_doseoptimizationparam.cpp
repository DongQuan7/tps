////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  xi.chen  xi.chen@united-imaging.com
/// 
///  \file rt_tps_doseoptimizationparam.cpp
/// 
///  \brief struct DoseOptimizationParam implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/08/27
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_doseoptimizationparam.h"
#include "RtTpsProtoLib/rt_tps_optinput.pb.h"

TPS_BEGIN_NAMESPACE

DoseOptimizationParam::DoseOptimizationParam(){

}

DoseOptimizationParam::DoseOptimizationParam(const proto::RT_TPS_Optinput& protoOptinput){
    mIterNum = protoOptinput.endcriterialvalue();
    mCriterionType = protoOptinput.endcriterialtype();
}

int DoseOptimizationParam::GetIterNum() const { 
    return mIterNum; 
}
int DoseOptimizationParam::GetCriterionType() const { 
    return mCriterionType; 
}

TPS_END_NAMESPACE