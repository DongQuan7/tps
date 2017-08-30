////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_cmd_create_plan.h
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_CREATE_PLAN_H_
#define TPS_BL_CMD_CREATE_PLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"

#include <vector>

TPS_BEGIN_NAMESPACE

struct CertifiedPlan;

class TPS_BL_DECLSPEC TpsAddPlanCMD : public TpsCommand{
public:
    TpsAddPlanCMD(CertifiedPlan *certPlan);
    virtual ~TpsAddPlanCMD(void);

    CertifiedPlan GetCertfiedPlan() const;
protected:
    virtual int Execute();

private:
    CertifiedPlan               mCertPlan;
};

TPS_END_NAMESPACE

#endif //TPS_PCA_ADDTRYCMD_H
