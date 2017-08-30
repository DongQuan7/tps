////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xi.Chen  Xi.Chen@united-imaging.com
/// 
///  \file tps_pra_loadcommonreportdata_commandhandler.cpp
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2015/05/04
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_pra_loadcommonreportdata_commandhandler.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

LoadCommonReportDataCommandHandler::LoadCommonReportDataCommandHandler(
    IPlanReportController* planReportController)
    : IPlanReportCommandHandler(planReportController) {

}

LoadCommonReportDataCommandHandler::~LoadCommonReportDataCommandHandler(){

}

int LoadCommonReportDataCommandHandler::HandleCommand(
    const Mcsf::CommandContext* pContext, std::string* pReplyObject) 
{
    pContext;
    pReplyObject;
    return TPS_HANDLE_OK;
}

TPS_END_NAMESPACE  // end namespace tps
