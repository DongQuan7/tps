////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  xiaowei.li@united-imaging.com
/// 
///  \file tps_bl_cmd_phantom_update.h
/// 
///  \brief class PhantomUpdateCmd
/// 
///  \version 1.0
/// 
///  \date    2017/07/18
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_PHANTOM_UPDATE_H_
#define TPS_BL_CMD_PHANTOM_UPDATE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace Mcsf
{
    class IDataHeaderElementMap;
}

TPS_BEGIN_NAMESPACE;

class TPS_BL_DECLSPEC PhantomUpdateCmd : public TpsCommand
{
public:
    PhantomUpdateCmd(const std::string &srtPatientUid, const std::string &srtPhantomUid, const std::string &srtPhantomName, const std::string &srtPhantomDescription);

    ~PhantomUpdateCmd();

protected:
    virtual int Execute();

private:
    std::string                 m_strPatientUid;
    std::string                 m_strPhantomUid;
    std::string                 m_strPhantomName;
    std::string                 m_strPhantomDescription;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_PHANTOM_UPDATE_H_