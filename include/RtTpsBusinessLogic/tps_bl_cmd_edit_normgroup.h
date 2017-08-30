////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_edit_normgroup.h
/// 
///  \brief class for edit normgroup
/// 
///  \version 1.0
/// 
///  \date    Nov.6,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_EDIT_NORMGROUP_H_
#define TPS_BL_CMD_EDIT_NORMGROUP_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_common_enums.h"


TPS_BEGIN_NAMESPACE;

class RtNormgroup;
class RtPlan;


class TPS_BL_DECLSPEC EditNormgroupCmd : public TpsCommand
{
public:
    EditNormgroupCmd(RtNormgroup*& pNormgroup);
    virtual ~EditNormgroupCmd(void);



protected:
    virtual int Execute();

private:

    bool GetRescaleDoseFlag_i(RtNormgroup* ng, RtPlan* plan);

    RtNormgroup*                      mNormgroup;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(EditNormgroupCmd);
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_EDIT_PLAN_H_