////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatevrdisplayoperation.h
/// 
///  \brief UpdateVRDisplayOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/30
////////////////////////////////////////////////////////////////



#ifndef TPS_RL_UPDATEVRDISPLAYOPRATION_H_
#define TPS_RL_UPDATEVRDISPLAYOPRATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC UpdateVRDisplayOperation
    : public OperationBase {
public:
    UpdateVRDisplayOperation(
        float transparency, float maxCtValue, float minCtValue);
    virtual ~UpdateVRDisplayOperation();

    virtual bool ModifyGraphicObject() const;

private:
    float    mTransparency;
    float    mMinValue;
    float    mMaxValue;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateVRDisplayOperation);

};


TPS_END_NAMESPACE



#endif