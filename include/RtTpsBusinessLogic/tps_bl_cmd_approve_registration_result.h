////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_approve_registration_result.h
/// 
///  \brief class ApproveRegistrationResultCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/10/20
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_APPROVE_REGISTRATION_RESULT_H_
#define TPS_BL_CMD_APPROVE_REGISTRATION_RESULT_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct ApproveResult
{
    std::string _fixedUid;
    std::string _floatUid;
    std::string _description;
    std::string _uid;
    int _registrationMethod;
    bool        _isSucceeded;
};

class RtApproverecord;

/// \class ApproveRegistrationResultCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC ApproveRegistrationResultCmd : public TpsCommand{
public:
    //the constructor and destructor
    ApproveRegistrationResultCmd(RtApproverecord* approveRecord, std::string description, int registrationMethod);
    ~ApproveRegistrationResultCmd();

    ApproveResult GetApproveResult() const { return mApproveResult; }
protected:
    virtual int Execute();

private:
    ApproveResult mApproveResult;
    RtApproverecord* mApproveRecord;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ApproveRegistrationResultCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif