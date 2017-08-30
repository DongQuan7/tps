//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_planreportcontrollerapp.h
///
/// \brief   TpsPlanReportControllerApp
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_PRCA_PLANREPORTCONTROLLERAPP_H_
#define TPS_PRCA_PLANREPORTCONTROLLERAPP_H_

#include "RtTpsFramework/tps_fw_controllerbase.h"
#include "RtTpsPlanReportControllerApp/tps_prca_iplanreportcontroller.h"

TPS_BEGIN_NAMESPACE

class MPRPrinter;

class PlanReportControllerApp : public IPlanReportController{

public:
	PlanReportControllerApp(int id);
	virtual ~PlanReportControllerApp(void);

	virtual int Start(const std::vector<std::string> &args);
	virtual int End();

    virtual int InitializeMPRPrinter(int width, int height, boost::function<void (unsigned char*, int, int)> printerListener);

    virtual int PrintMPR(std::string planUID, const std::vector<int> sliceNums);

private:
    std::shared_ptr<MPRPrinter> mMPRPrinter;
};

DECLARE_APP_CONTROLLER(PlanReportControllerApp);

TPS_END_NAMESPACE

#endif // TPS_PRCA_PLANREPORTCONTROLLERAPP_H_
