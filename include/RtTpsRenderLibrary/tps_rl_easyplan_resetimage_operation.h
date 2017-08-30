////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_resetimage_operation.h
/// 
///  \brief class TpsEasyPlanResetImageOperation
/// 
///  \version 1.0
/// 
///  \date    2014/12/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANRESETIMAGEOPERATION_H_
#define TPS_RL_EASYPLANRESETIMAGEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanResetImageOperation
    : public OperationBase {
public:
    TpsEasyPlanResetImageOperation(const std::string &drrImageUID, const std::string &planUID,
        const int& viewerWidth, const int& viewerHeight);
    ~TpsEasyPlanResetImageOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                     mPlanUID;
    std::string                     mDrrImageUID;
    int                             mViewerWidth;
    int                             mViewerHeight;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanResetImageOperation);
};
TPS_END_NAMESPACE

#endif