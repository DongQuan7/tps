//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_planreportcontrollerapp.cpp
///
/// \brief   TpsPlanReportControllerApp
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#include"stdafx.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "tms/rt_fwk_dl_object_plan.h"

#include "tps_prca_mprprinter.h"
#include "tps_prca_planreportcontrollerapp.h"

TPS_BEGIN_NAMESPACE

IMPLEMENT_APP_CONTROLLER(PlanReportControllerApp)

PlanReportControllerApp::PlanReportControllerApp(int id)
: IPlanReportController(id),
    mMPRPrinter(nullptr) {

}

PlanReportControllerApp::~PlanReportControllerApp(void) {

}

int PlanReportControllerApp::Start(const std::vector<std::string> &args) {
    args;

    return TPS_SUCCESS;
}

int PlanReportControllerApp::End() {

    return TPS_SUCCESS;
}

int PlanReportControllerApp::InitializeMPRPrinter(int width, int height, boost::function<void (unsigned char*, int, int)> printerListener) {

    if (mMPRPrinter == nullptr) {
        mMPRPrinter.reset(new MPRPrinter(width, height));
        mMPRPrinter->AttachListener(printerListener);
        return TPS_SUCCESS;
    }

    return TPS_SUCCESS;
}

int PlanReportControllerApp::PrintMPR(std::string planUID, const std::vector<int> sliceNums) {

    if (mMPRPrinter == nullptr) {
        return TPS_FAILURE;
    }

    if (planUID.empty() || sliceNums.empty()) {
        return TPS_FAILURE;
    }

    mMPRPrinter->SetPlanUID(planUID);

    RTFWK::RTFWKDLDOPlan* plan = nullptr;
    if (!TpsPatientManager::GetInstance()->GetPlan(planUID, &plan)) {
        TPS_LOG_DEV_ERROR<<"Failed to get plan of : "<<planUID;
        return TPS_SUCCESS;
    }

    mMPRPrinter->SetSeriesUID(plan->GetSeriesUID());

    std::queue<int> seriesSlices;
    for (auto it = sliceNums.cbegin(); it < sliceNums.cend(); ++it) {
        seriesSlices.push(*it);
    }

    mMPRPrinter->SetSeriesSlices(seriesSlices);

    mMPRPrinter->Print();

    return TPS_SUCCESS;
}


TPS_END_NAMESPACE
