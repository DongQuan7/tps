////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Song LI  song.lig@united-imaging.com
/// 
///  \file tps_cmd_voi_jump_to.cpp
/// 
///  \brief class VoiJumpToCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/16
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_VOI_JUMP_TO_H_
#define TPS_BL_CMD_VOI_JUMP_TO_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC VoiJumpToCmd : public TpsCommand 
{
public:
    VoiJumpToCmd(std::string strVoiUid);
    virtual ~VoiJumpToCmd();
protected:
    virtual int Execute();

private:
    std::string     m_strVoiUid;
    std::string     m_strCommandName;
};

TPS_END_NAMESPACE
#endif //TPS_BL_CMD_VOI_JUMP_TO_H_