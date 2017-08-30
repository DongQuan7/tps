////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_manual_registration.h
/// 
///  \brief class ManualRegistrationCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/02
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_MANUAL_REGISTRATION_H_
#define TPS_BL_CMD_MANUAL_REGISTRATION_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

enum ManualRegistrationType
{
    Manual_Reg_Rotate,
    Manual_Reg_Translate,
};

/// \class ManualRegistration
/// 
/// \brief *****
class TPS_BL_DECLSPEC ManualRegistrationCmd : public TpsCommand{
public:
    //the constructor and destructor
    ManualRegistrationCmd(WINDOW_TYPE wndType, const std::string& wndUid, 
        float preX, float preY, float curX, float curY, 
        ManualRegistrationType actionType);
    ~ManualRegistrationCmd();

protected:
    virtual int Execute();

private:
    WINDOW_TYPE             mWndType;
    std::string             mWndUid;
    float                   mPreX;
    float                   mPreY;
    float                   mCurX;
    float                   mCurY;
    ManualRegistrationType  mActionType;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ManualRegistrationCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif