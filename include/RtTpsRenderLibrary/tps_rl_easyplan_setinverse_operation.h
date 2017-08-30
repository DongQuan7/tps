////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setinverse_operation.h
/// 
///  \brief class TpsEasyPlanSetInverseOperation
/// 
///  \version 1.0
/// 
///  \date    2014/12/19
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETINVERSEOPERATION_H_
#define TPS_RL_EASYPLANSETINVERSEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetInverseOperation
    : public OperationBase {
public:
    TpsEasyPlanSetInverseOperation(const std::string& imageUID, const std::string& planUID);
    ~TpsEasyPlanSetInverseOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                       mPlanUID;
    std::string                       mImageUID;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetInverseOperation);
};
TPS_END_NAMESPACE

#endif