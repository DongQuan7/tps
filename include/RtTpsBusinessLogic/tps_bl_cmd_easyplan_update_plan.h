//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tp_bl_cmd_easyplan_update_plan.h 
///  \brief  update easy plan in db
///
///  \version 1.0
///  \date  2015/11/02  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_UPDATE_PLAN_H_
#define TPS_BL_CMD_EASYPLAN_UPDATE_PLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace tps{
    class RtPlan;
    class RtNormgroup;
    class RtPoi;
}

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC UpdateEasyPlanCmd : public TpsCommand
{
public:
	UpdateEasyPlanCmd( tps::RtPlan* rtPlan, tps::RtNormgroup* rtNormgroup, std::vector<tps::RtPoi*>& rtPoiList );
	~UpdateEasyPlanCmd();

protected:
	virtual int Execute();

private:
    bool UpdateBeamInfoAfterMachineChanged(const std::string &normgroupUID, std::string beamUID);

private:
    RtPlan* mRtPlan;
    RtNormgroup* mRtNormgroup;
    std::vector<RtPoi*> mRtPoiList;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(UpdateEasyPlanCmd);
};

TPS_END_NAMESPACE

#endif
