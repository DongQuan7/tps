////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatesetupcrosshairoperation.h
/// 
///  \brief class TpsUpdateSetUpCrosshairOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATESETUPCROSSHAIROPERATION_H_ 
#define TPS_RL_UPDATESETUPCROSSHAIROPERATION_H_

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"



TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsUpdateSetUpCrosshairOperation: public OperationBase {
public:
    TpsUpdateSetUpCrosshairOperation(const std::string &imagUid, 
        const Mcsf::Point2f &mousePosition, bool isVisible, FIRST_POSTFIX_COMPONENT section);
    ~TpsUpdateSetUpCrosshairOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string    mImageuid;
    FIRST_POSTFIX_COMPONENT    mSectionType;
    Mcsf::Point2f  mMousePosition;
    bool           mIsVisible;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateSetUpCrosshairOperation);
};

TPS_END_NAMESPACE
#endif