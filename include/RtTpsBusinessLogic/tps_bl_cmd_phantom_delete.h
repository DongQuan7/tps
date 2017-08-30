//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_aca_phantom_delete_cmd.h
///  \brief   Phantom delete command
///
///  \version 1.0
///  \date    Mar. 23, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_PHANTOM_DELETE_H_
#define TPS_BL_CMD_PHANTOM_DELETE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"


TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC PhantomDeleteCmd : public TpsCommand {
public:
    PhantomDeleteCmd(const std::string& strPatientUid, const std::string& strPhantomSeriesUid);

    virtual ~PhantomDeleteCmd();

protected:
    virtual int Execute();

private:
    std::string             m_strPatientUid;
    std::string             m_strPhantomSeriesUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(PhantomDeleteCmd);
};

TPS_END_NAMESPACE

#endif //TPS_BL_PHANTOM_DELETE_CMD_H_