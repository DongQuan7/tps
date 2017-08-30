////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setpan_operation.h
/// 
///  \brief class TpsEasyPlanSetPanOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/06
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETPANOPERATION_H_
#define TPS_RL_EASYPLANSETPANOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetPanOperation
    : public OperationBase {
public:
    TpsEasyPlanSetPanOperation(const std::string& planUID, const std::string &drrImageUID, 
        const float startX, const float startY, const float stopX, const float stopY, 
        const float viewerWidth, const float viewerHeight);
    ~TpsEasyPlanSetPanOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                     mPlanUID;
    std::string                     mDrrImageUID;
    float                           mStartX;
    float                           mStartY;
    float                           mStopX;
    float                           mStopY;
    float                           mViewerWidth;
    float                           mViewerHeight;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetPanOperation);
};
TPS_END_NAMESPACE

#endif