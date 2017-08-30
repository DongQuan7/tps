////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setzoom_operation.h
/// 
///  \brief class TpsEasyPlanSetZoomOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETZOOMOPERATION_H_
#define TPS_RL_EASYPLANSETZOOMOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetZoomOperation
    : public OperationBase {
public:
    TpsEasyPlanSetZoomOperation(const std::string &drrImageUID, const std::string &planUID,
        const float zoomFactor);
    ~TpsEasyPlanSetZoomOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                     mPlanUID;
    std::string                     mDrrImageUID;
    mutable float                   mZoomFactor;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetZoomOperation);
};
TPS_END_NAMESPACE

#endif