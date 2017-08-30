//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \file   tps_cmd_deleteeasyplan.h 
///  \brief   delete easy plan
///
///  \version 1.0
///  \date  2015/10/28
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_CMD_DELETEEASYPLAN_H_
#define TPS_CMD_DELETEEASYPLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

//struct CertifiedPlan;

class TPS_BL_DECLSPEC TpsDeleteEasyPlanCmd : public TpsCommand {
public:
    TpsDeleteEasyPlanCmd(const std::string& planUid);
    virtual ~TpsDeleteEasyPlanCmd();

protected:
    virtual int Execute();

private:
	std::string  mPlanUid;

};

TPS_END_NAMESPACE

#endif