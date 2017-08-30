//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_pra_planreportadaptor.cpp
///
/// \brief   TpsPlanReportAdaptor
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_commandids.h"

#include "RtTpsPlanReportControllerApp/tps_prca_iplanreportcontroller.h"

#include "tps_pra_requestplanreport_commandhandler.h"
#include "tps_pra_loadcommonreportdata_commandhandler.h"

#include "tps_pra_planreportadaptor.h"

TPS_BEGIN_NAMESPACE

IMPLEMENT_APP_ADAPTOR(TpsPlanReportAdaptor)

TpsPlanReportAdaptor::TpsPlanReportAdaptor(int id)
    : TpsAdaptorBase(id), mPlanReportController(NULL) {
}

TpsPlanReportAdaptor::~TpsPlanReportAdaptor(void) {
}

int TpsPlanReportAdaptor::Start(const std::vector<std::string> &args){
    mPlanReportController = static_cast<IPlanReportController*> (this->GetController());
    if (nullptr == mPlanReportController) {
        TPS_LOG_DEV_ERROR << "failed to query IPlanController";
        return TPS_FAILURE;
    }

    if (TPS_FAILURE == TpsAdaptorBase::Start(args)){
        TPS_LOG_DEV_ERROR << "failed to start adapter";
        return TPS_FAILURE;
    }

    return TPS_SUCCESS;
}

int TpsPlanReportAdaptor::End(){
    TpsAdaptorBase::End();

    return TPS_SUCCESS;
}

int TpsPlanReportAdaptor::RegisterCommand() {

    Mcsf::ICommunicationProxy* proxy = this->GetCommProxy();
    if (proxy == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get communication proxy";
        return TPS_FAILURE;
    }

    if (TPS_SUCCESS != this->GetCommProxy()->RegisterCommandHandler(LOAD_COMMONREPORTDATA,
        new LoadCommonReportDataCommandHandler(mPlanReportController))) {
            TPS_LOG_DEV_ERROR<<"Failed to register MPR_PAGE_TRUN command handler";
            return TPS_FAILURE;
    }

    if (TPS_SUCCESS != this->GetCommProxy()->RegisterCommandHandler(REQUEST_PLANREPORT,
        new RequestPlanReportCommandHandler(mPlanReportController))) {
            TPS_LOG_DEV_ERROR<<"Failed to register MPR_PAGE_TRUN command handler";
            return TPS_FAILURE;
    }

    return TPS_SUCCESS;
}

TPS_END_NAMESPACE