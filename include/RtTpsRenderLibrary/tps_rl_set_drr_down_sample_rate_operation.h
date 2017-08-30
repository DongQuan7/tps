////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_set_drr_down_sample_rate_operation.h
/// 
///  \brief class SetDrrDownSampleRate declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/01/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SET_DRR_DOWN_SAMPLE_RATE_OPERATION_H_
#define TPS_RL_SET_DRR_DOWN_SAMPLE_RATE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class SetDrrDownSampleRateOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC SetDrrDownSampleRateOperation
 : public OperationBase{
public:
    //the constructor and destructor
    SetDrrDownSampleRateOperation(float downSampleRate);
    virtual ~SetDrrDownSampleRateOperation();

    virtual bool ModifyGraphicObject() const;

private:
    float mDownSampleRate;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SetDrrDownSampleRateOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif