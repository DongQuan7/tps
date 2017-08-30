//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easy_plan_add_plan.h 
///  \brief   add easy plan
///
///  \version 1.0
///  \date  2015/10/08  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASY_PLAN_ADD_PLAN_H_
#define TPS_BL_CMD_EASY_PLAN_ADD_PLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace tps{
    class RtPlan;
    class RtNormgroup;
    class RtPoi;
    class RtRtimage;
}

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsAddEasyPlanCmd : public TpsCommand {
public:
    TpsAddEasyPlanCmd(int iWndType, std::string windowUid, RtPlan* rtPlan, RtNormgroup* rtNormgroup, std::vector<RtPoi*>& rtPoiList);
    virtual ~TpsAddEasyPlanCmd();

protected:
    virtual int Execute();

private:
    RtRtimage* SetEmpty2DImageForEasyPlan();

private:
    int mWinType;
    std::string mWinUid;
    RtPlan* mRtPlan;
    RtNormgroup* mRtNormgroup;
    std::vector<RtPoi*> mRtPoiList;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(TpsAddEasyPlanCmd);
};

TPS_END_NAMESPACE

#endif