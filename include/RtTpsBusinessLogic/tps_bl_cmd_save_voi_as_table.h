////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_save_voi_as_table.h
/// 
///  \brief class SaveVOIAsTableCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/11/24
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_SAVE_VOI_AS_TABLE_H_
#define TPS_BL_CMD_SAVE_VOI_AS_TABLE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC SaveVOIAsTableCmd : public TpsCommand
{
public:
    SaveVOIAsTableCmd(const std::string& voiUID);
    ~SaveVOIAsTableCmd(void);

protected:
    virtual int Execute();

private:
    std::string mVoiUid;
};

TPS_END_NAMESPACE

#endif
