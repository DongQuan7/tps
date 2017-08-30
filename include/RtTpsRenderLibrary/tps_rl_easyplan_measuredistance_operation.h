////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_measuredistance_operation.h
/// 
///  \brief class TpsEasyPlanMeasureDistanceOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMEASUREDISTANCEOPERATION_H_
#define TPS_RL_EASYPLANMEASUREDISTANCEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanMeasureDistanceOperation
    : public OperationBase {
public:
    TpsEasyPlanMeasureDistanceOperation(const std::string &planUID, const std::string &drrImageUID, 
        const float startX, const float startY, const float stopX, const float stopY,
        const bool actionStarted, const bool actionDone);
    ~TpsEasyPlanMeasureDistanceOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                     mPlanUID;
    std::string                     mDrrImageUID;
    float                           mStartX;
    float                           mStartY;
    float                           mStopX;
    float                           mStopY;
    bool                            mActionStarted;
    bool                            mActionDone;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanMeasureDistanceOperation);
};
TPS_END_NAMESPACE

#endif