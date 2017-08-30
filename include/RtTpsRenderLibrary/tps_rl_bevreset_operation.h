//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  WANG Zhendong zhendong.wang@united-imaging.com
///
///  \file    tps_rl_bevreset_operation.h
///  \brief   
///
///  \version 1.0
///  \date    November 6, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE

/// \class BEVResetOperation
/// 
/// \brief this operation perform the reset operation on BEV
class TPS_RL_DECLSPEC BEVResetOperation
    : public OperationBase {

public:

    BEVResetOperation(const std::string& imageUID, int actionType);
    BEVResetOperation(const std::string& imageUID, const std::string& planUID, int actionType);
    ~BEVResetOperation();   

    virtual bool ModifyGraphicObject() const;   

private:
    int             mActionType;  
    std::string     mImageUID;
	std::string     mPlanUID;
    
   
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(BEVResetOperation);
};

TPS_END_NAMESPACE
