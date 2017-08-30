////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_edit_plan.h
/// 
///  \brief class for edit plan
/// 
///  \version 1.0
/// 
///  \date    Nov.6,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_EDIT_PLAN_H_
#define TPS_BL_CMD_EDIT_PLAN_H_

#include "tps_bl_defs.h"

#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE;

class TPS_BL_DECLSPEC EditPlanCmd : public TpsCommand{
public:
    EditPlanCmd(const std::string& sPlanUid, const std::string& sPlanName, const std::string& sPlanDescription, const std::string& sSetupPoiUid);
    virtual ~EditPlanCmd(void);
protected:
    virtual int Execute();

    //it's better to be another cmd
    int UpdateImageText(const std::string& sPlanName);
    int UpdateImageText(std::string strImageUID, 
        std::string sPlanUid, 
        WINDOW_TYPE nWindowType,
        std::string sPlanName);
private:
    std::string         mPlanUid;
    std::string         mPlanName;
    std::string         mPlanDescription;
    std::string         mSetupPoiUid;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(EditPlanCmd);
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_EDIT_PLAN_H_