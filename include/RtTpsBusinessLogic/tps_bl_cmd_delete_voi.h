////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_delete_voi.h
/// 
///  \brief class  delete voi declaration 
/// 
///  \version 1.0
/// 
///  \date    Oct.21,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_DELETE_VOI_H_
#define TPS_BL_CMD_DELETE_VOI_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE;
class TPS_BL_DECLSPEC DeleteVoiCmd : public TpsCommand 
{
public:
    DeleteVoiCmd(const std::string& sVoiUid);

    virtual ~DeleteVoiCmd();

protected:
    virtual int Execute();
    virtual int PostExecute();

private:
    std::string             mVoiUid;
};

TPS_END_NAMESPACE

#endif