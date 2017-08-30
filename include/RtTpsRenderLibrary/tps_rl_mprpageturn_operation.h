//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_mprpageturn_operation.h
///  \brief   MPR page turn declaration
///
///  \version 1.0
///  \date    Dec. 20, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPRPAGETURN_OPERATION_H_
#define TPS_RL_MPRPAGETURN_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC MPRPageTurnOperation : public OperationBase
{
public:
    //the constructor and destructor
    MPRPageTurnOperation(
        int turnStep, const std::string& strImageUID, 
        FIRST_POSTFIX_COMPONENT ucSectionType, 
        SECOND_POSTFIX_COMPONENT ucSectionExtendType = EMPTY_COMPONENT, 
        bool bIsCyclic = false);

    ~MPRPageTurnOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string              mImageUID;
    FIRST_POSTFIX_COMPONENT  mSectionType;
    SECOND_POSTFIX_COMPONENT mSectionExtendType;
    int                      mTurnStep;
    bool                     mIsCyclic;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MPRPageTurnOperation);
};

TPS_END_NAMESPACE

#endif
