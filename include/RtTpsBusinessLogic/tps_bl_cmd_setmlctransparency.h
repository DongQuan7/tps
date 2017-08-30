////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_setmlctransparency.h
/// 
///  \brief class SetMlcTransparencyCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    Apr.16,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_SETMLCTRANSPARENCY_H_
#define TPS_BL_CMD_SETMLCTRANSPARENCY_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE;  // begin namespace tps

/// \class SetMlcTransparency
/// 
/// \brief *****
class TPS_BL_DECLSPEC SetMlcTransparencyCmd : public TpsCommand
{
public:
    //the constructor and destructor
    SetMlcTransparencyCmd(float fTransparency);
    virtual ~SetMlcTransparencyCmd();

protected:
    virtual int Execute();

private:
    float       mTransparency;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SetMlcTransparencyCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif