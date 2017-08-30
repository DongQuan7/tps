//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author linqi.liu  linqi.liu@united-imaging.com
///
///  \file   tps_cmd_approve_voi.h 
///  \brief   
///
///  \version 1.0
///  \date  2014/06/23 
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_CCA_APPROVEVOIDETAIL_CMD_H_
#define TPS_CCA_APPROVEVOIDETAIL_CMD_H_

#include <vector>
#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE


class RtApproverecord;

class TPS_BL_DECLSPEC TpsApproveVoiDetailCmd : public TpsCommand
{
public:
    TpsApproveVoiDetailCmd(std::vector<RtApproverecord*> voiApproveRecordList);
    ~TpsApproveVoiDetailCmd();
protected:
    virtual int Execute();

private:
    std::vector<RtApproverecord*> mVoiApproveRecordList;
};

TPS_END_NAMESPACE

#endif