////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang mailto:qiangqiang.zhou@united-imaing.com
/// 
///  \file tps_rl_setmlctransparency_operation.h
/// 
///  \brief class SetMlcTransparencyOperation 
/// 
///  \version 1.0
/// 
///  \date    Apr.16, 2015
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SETMLCTRANSPARENCY_OPERATION_H_
#define TPS_RL_SETMLCTRANSPARENCY_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC SetMlcTransparencyOperation : public OperationBase 
{
public:
    SetMlcTransparencyOperation(float mlcTransparency);

    ~SetMlcTransparencyOperation();

    virtual bool ModifyGraphicObject() const;

private:
    float  mMlcTransparency;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(SetMlcTransparencyOperation);
};

TPS_END_NAMESPACE
#endif