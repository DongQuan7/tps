//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_pra_planreportadaptor.h
///
/// \brief   TpsPlanReportAdaptor
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_PRA_PLANREPORTADAPTOR_H_
#define TPS_PRA_PLANREPORTADAPTOR_H_

//#include "RtTpsFramework/tps_fw_commproxy.h"

#include "RtTpsFramework/tps_fw_adaptorbase.h"

TPS_BEGIN_NAMESPACE

class ITpsPlanReportController;

class TpsPlanReportAdaptor : public TpsAdaptorBase {
public:
	TpsPlanReportAdaptor(int id);
	virtual ~TpsPlanReportAdaptor(void);
	virtual int Start(const std::vector<std::string> &args);
	virtual int End();
	virtual int RegisterCommand();
private:
	IPlanReportController* mPlanReportController;
};

DECLARE_APP_ADAPTOR(TpsPlanReportAdaptor);

TPS_END_NAMESPACE

#endif // TPS_PRA_PLANREPORTADAPTOR_H_
