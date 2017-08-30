////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xi.Chen  Xi.Chen@united-imaging.com
/// 
///  \file tps_pra_requestplanreport_commandhandler.h
/// 
///  \brief RequestPlanReportCommandHandler
/// 
///  \version 1.0
/// 
///  \date    2015/05/04
////////////////////////////////////////////////////////////////
#ifndef TPS_PRA_REQUESTPLANREPORT_COMMANDHANDLER_H_
#define TPS_PRA_REQUESTPLANREPORT_COMMANDHANDLER_H_

#include "tps_defs.h"
#include "tps_pra_iplanreport_commandhandler.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class RequestPlanReportCommandHandler
/// 
/// \brief *****
class RequestPlanReportCommandHandler : public IPlanReportCommandHandler {
public:
    //the constructor and destructor
    RequestPlanReportCommandHandler(IPlanReportController* planReportController);
    virtual ~RequestPlanReportCommandHandler();

    virtual int HandleCommand(const Mcsf::CommandContext* pContext, 
        std::string* pReplyObject);
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RequestPlanReportCommandHandler);

};

void PrinterResultCallback(unsigned char* data, int width, int height);

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_PRA_REQUESTPLANREPORT_COMMANDHANDLER_H_
