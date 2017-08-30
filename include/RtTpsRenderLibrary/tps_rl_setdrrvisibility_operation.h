////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang mailto:qiangqiang.zhou@united-imaing.com
/// 
///  \file tps_rl_setdrrvisibility_operation.h
/// 
///  \brief class SetDrrVisibilityOperation 
/// 
///  \version 1.0
/// 
///  \date    Apr.16, 2015
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SETDRRVISIBILITY_OPERATION_H_
#define TPS_RL_SETDRRVISIBILITY_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC SetDrrVisibilityOperation : public OperationBase 
{
public:
    SetDrrVisibilityOperation(bool bVisibility);

    ~SetDrrVisibilityOperation();

    virtual bool ModifyGraphicObject() const;

private:
    bool  mVisibility;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(SetDrrVisibilityOperation);
};

TPS_END_NAMESPACE
#endif