////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refresh_registration_result_operation.h
/// 
///  \brief class RefreshRegistrationResultOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/12/05
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_REFRESH_REGISTRATION_RESULT_OPERATION_H_
#define TPS_RL_REFRESH_REGISTRATION_RESULT_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class RefreshRegistrationResultOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC RefreshRegistrationResultOperation
    : public OperationBase{
public:
    //the constructor and destructor
    RefreshRegistrationResultOperation();
    virtual ~RefreshRegistrationResultOperation();

    virtual bool ModifyGraphicObject() const;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RefreshRegistrationResultOperation);
};

class TPS_RL_DECLSPEC RefreshApprovedRegistrationResultOperation
    : public OperationBase{
public:
    //the constructor and destructor
    RefreshApprovedRegistrationResultOperation(const std::string& fixedUid, 
        const std::string& floatUid);
    virtual ~RefreshApprovedRegistrationResultOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mFixedUid;
    std::string mFloatUid;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RefreshApprovedRegistrationResultOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif