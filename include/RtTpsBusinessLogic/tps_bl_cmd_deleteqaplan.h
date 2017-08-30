////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  zuo kai  kai.zuo@united-imaging.com
/// 
///  \file tps_bl_cmd_deleteqaplan.h
/// 
///  \brief class TpsPlanQADeleteCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/3/08
//////////////////////////////////////////////////////////////////

#ifndef TPS_BL_DELETEQAPLAN_H_
#define TPS_BL_DELETEQAPLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifieddelete.h"


TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsPlanQADeleteCmd : public TpsCommand {
public:
    TpsPlanQADeleteCmd(std::string planUid, std::string seriesUid, std::string courseUid);

    virtual ~TpsPlanQADeleteCmd();

protected:

    virtual int Execute();

private:
    bool DeletePlan_i(const std::string &planUid);

private:
    std::string             mPlanUid;
    std::string             mSeriesUid;
    std::string             mCourseUid;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPlanQADeleteCmd);
};

TPS_END_NAMESPACE

#endif
