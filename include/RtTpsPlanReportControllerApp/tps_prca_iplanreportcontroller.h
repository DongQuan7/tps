//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_iplanreportcontroller.h
///
/// \brief   ITpsPlanReportController
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_PRCA_IPLANREPORTCONTROLLER_H_
#define TPS_PRCA_IPLANREPORTCONTROLLER_H_

#include "tps_prca_defs.h"

//TPS
#include "RtTpsFramework/tps_fw_controllerbase.h"

#include "McsfMedViewer3D/McsfMedViewer3DCommon/mcsf_viewer3d_subject.h"

#include "boost/function.hpp"

TPS_BEGIN_NAMESPACE

class IPlanReportController : public TpsControllerBase{
public:
	IPlanReportController(int id) : TpsControllerBase(id) {};
	virtual ~IPlanReportController(void) {};

    virtual int InitializeMPRPrinter(int width, int height, boost::function<void (unsigned char*, int, int)> printerListener) = 0;

    virtual int PrintMPR(std::string planUID, const std::vector<int> sliceNums) = 0;
};


TPS_END_NAMESPACE

#endif //TPS_PRCA_IPLANREPORTCONTROLLER_H_
