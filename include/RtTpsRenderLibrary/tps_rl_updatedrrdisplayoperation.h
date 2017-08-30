////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatedrrdisplayoperation.h
/// 
///  \brief UpdateDrrDisplayOperation
/// 
///  \version 1.0
/// 
///  \date    2014/5/20
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_UPDATEDRRDISPLAYOPERATION_H_
#define TPS_RL_UPDATEDRRDISPLAYOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC UpdateDrrDisplayOperation
    : public OperationBase {
public:
    UpdateDrrDisplayOperation(std::string mImageUid,
        int DeltaWindowWidth, int DeltaWindowCenter, float gray, bool isVisible);
    virtual ~UpdateDrrDisplayOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mImageUid;
    int         mDeltaWindowWidth;
    int         mDeltaWindowCenter;

    //current not use
    float       mGray;
    bool        mIsVisible;
    //float    mTransparency;
    //float    mMinValue;
    //float    mMaxValue;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateDrrDisplayOperation);

};


TPS_END_NAMESPACE
#endif