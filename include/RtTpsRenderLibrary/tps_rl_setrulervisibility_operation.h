//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  SHI CHUNHUI mailto:chunhui.shi@united-imaging.com
///
///  \file    tps_rl_setrulervisibility_operation.h
///  \brief   class declaration
///
///  \version 1.0
///  \date    2015/4/10
///  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC SetRulerVisibilityOperation : public OperationBase{

public:
    //the constructor and destructor
    SetRulerVisibilityOperation(const std::string& strImageUID, bool isVisible);
    virtual ~SetRulerVisibilityOperation();

    virtual bool ModifyGraphicObject() const;

    //void SetGraphicObjectKeys(const std::string &cornerKey);
private:
    std::string     mImageUID;
    bool             mIsVisible;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(SetRulerVisibilityOperation);
};

TPS_END_NAMESPACE
