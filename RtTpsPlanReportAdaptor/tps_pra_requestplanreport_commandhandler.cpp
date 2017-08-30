////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xi.Chen  Xi.Chen@united-imaging.com
/// 
///  \file tps_pra_requestplanreport_commandhandler.cpp
/// 
///  \brief RequestPlanReportCommandHandler
/// 
///  \version 1.0
/// 
///  \date    2015/05/04
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_commproxy.h"
#include "RtTpsFramework/tps_fw_commandids.h"

#include "RtTpsCommonControllerApp/tps_comca_idataloader.h"
#include "RtTpsCommonControllerApp/tps_comca_icommonoperator.h"
#include "RtTpsCommonControllerApp/tps_comca_commoncontrollerapp.h"

#include "RtTpsPlanReportControllerApp/tps_prca_iplanreportcontroller.h"

#include "RtTpsProtoLib/rt_tps_proto_reportinput.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_reportimage.pb.h"

#include "tps_pra_requestplanreport_commandhandler.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

RequestPlanReportCommandHandler::RequestPlanReportCommandHandler(
    IPlanReportController* planReportController)
    : IPlanReportCommandHandler(planReportController) {

}

RequestPlanReportCommandHandler::~RequestPlanReportCommandHandler(){

}

int RequestPlanReportCommandHandler::HandleCommand(
    const Mcsf::CommandContext* pContext, std::string* pReplyObject) {

    if(nullptr == pContext || nullptr == pReplyObject) {
        TPS_LOG_DEV_ERROR<< "nullptr == pContext || nullptr == pReplyObject";
        return TPS_HANDLE_FAILED;
    }

    proto::RtTpsProtoReportInput protoReportInput;
    protoReportInput.ParseFromString(pContext->sSerializeObject);

    std::string planUID = protoReportInput.planuid();
    if (planUID.empty()) {
        TPS_LOG_DEV_ERROR<<"Failed to request plan report. PlanUID is empty";
        return TPS_HANDLE_FAILED;
    }

    std::vector<int> requestSliceNum;
    for (int nIndex = 0; nIndex < protoReportInput.slicenumber_size(); nIndex++) {
        requestSliceNum.push_back(protoReportInput.slicenumber(nIndex));
    }

    if (requestSliceNum.empty()) {
        TPS_LOG_DEV_ERROR<<"Failed to request plan report. Slice Number is empty";
        return TPS_HANDLE_FAILED;
    }

    int resolutionX = protoReportInput.has_resolutionx() ? protoReportInput.resolutionx() : 512;
    int resolutionY = protoReportInput.has_resolutiony() ? protoReportInput.resolutiony() : 512;

    ICommonOperator* commonOperator = TpsCommonControllerApp::GetInstance()->GetCommonOperator();
    if (commonOperator == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get common operator interface.";
        return TPS_HANDLE_FAILED;
    }

    if(!commonOperator->ClearPatientTreeData()){
        TPS_LOG_DEV_ERROR<<"Failed to clear patient";
        return TPS_HANDLE_FAILED;
    }

    IDataLoader* dataLoader = TpsCommonControllerApp::GetInstance()->GetDataLoader();
    if (dataLoader == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get data loader interface.";
        return TPS_HANDLE_FAILED;
    }

    if (!dataLoader->LoadMachine()) {
        TPS_LOG_DEV_ERROR<<"Failed to load machine";
        return TPS_HANDLE_FAILED;
    }

    if (!dataLoader->LoadPlanReportData(planUID)) {
        TPS_LOG_DEV_ERROR<<"Failed to load plan report data. PlanUID:"<<planUID;
        return TPS_HANDLE_FAILED;
    }

    if (TPS_SUCCESS != mPlanReportController->InitializeMPRPrinter(resolutionX, resolutionY,
        PrinterResultCallback)) {
            TPS_LOG_DEV_ERROR<<"Failed to init mpr printer";
            return TPS_HANDLE_FAILED;
    }

    if (TPS_SUCCESS != mPlanReportController->PrintMPR(planUID, requestSliceNum)) {
        TPS_LOG_DEV_ERROR<<"Failed to print mpr";
        return TPS_HANDLE_FAILED;
    }

    return TPS_HANDLE_OK;
}

void PrinterResultCallback(unsigned char* data, int width, int height) {

    if (data == nullptr || width == 0 || height == 0) {
        TPS_LOG_DEV_ERROR<<"Image is empty.";
        return;
    }

    auto communicationProxy = TpsCommProxy::GetInstance()->GetMcsfProxy();
    if (communicationProxy == nullptr) {
        TPS_LOG_DEV_ERROR<<"The communication proxy is empty.";
        return;
    }

    proto::RtTpsProtoReportImage protoReportImage;
    protoReportImage.set_buffer(data, width * height * 4);

    Mcsf::CommandContext context;
    context.iCommandId = 100000;
    context.sReceiver = TpsCommProxy::GetInstance()->GetTpsFEName();
    context.sSerializeObject = protoReportImage.SerializeAsString();

    if (0 != communicationProxy->AsyncSendCommand(&context)) {
        TPS_LOG_DEV_ERROR<<"Failed to send command to FE."<<context.iCommandId;
    }
}


TPS_END_NAMESPACE  // end namespace tps
