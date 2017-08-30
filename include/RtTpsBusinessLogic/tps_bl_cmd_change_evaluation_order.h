//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_change_evaluation_order.h
///  \brief   
///
///  \version 1.0
///  \date  2015/10/26  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_CHANGE_EVALUATION_ORDER_H_
#define TPS_BL_CMD_CHANGE_EVALUATION_ORDER_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

class ITpsDataRepository;
class TPS_BL_DECLSPEC ChangeEvaluationOrderCmd : public TpsCommand
{
public:
	ChangeEvaluationOrderCmd(int order, CertifiedPlan &plan);
    ChangeEvaluationOrderCmd(int order);
	~ChangeEvaluationOrderCmd();
protected:
	virtual int Execute();

private:
	CertifiedPlan		mCertPlan;
	int					mEvaluationOrder;
private:
	TPS_DISALLOW_COPY_AND_ASSIGN(ChangeEvaluationOrderCmd);
    
};
TPS_END_NAMESPACE

#endif