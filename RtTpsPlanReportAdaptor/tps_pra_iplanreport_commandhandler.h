//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_pra_iplanreport_commandhandler.h
///
/// \brief   TpsICommonCommandHandler
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_PRA_IPLANREPORT_COMMANDHANDLER_H_
#define TPS_PRA_IPLANREPORT_COMMANDHANDLER_H_

// ZhengHe
#include "McsfContainee/mcsf_containee_base.h"


TPS_BEGIN_NAMESPACE

class IPlanReportController;

class IPlanReportCommandHandler : public Mcsf::ICommandHandler
{
public:
    IPlanReportCommandHandler(IPlanReportController* planReportController)
        : mPlanReportController(planReportController) {};
    virtual ~IPlanReportCommandHandler(){};

    virtual int HandleCommand(const Mcsf::CommandContext* pContext, 
        std::string* pReplyObject) = 0;

protected:
    IPlanReportController* mPlanReportController;
};

TPS_END_NAMESPACE

#endif // TPS_PRA_IPLANREPORT_COMMANDHANDLER_H_
