////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_rendercontrollerbase.cpp
/// 
///  \brief class TpsRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include"StdAfx.h"
#include "RtTpsRenderController/tps_rc_rendercontroller.h"

#include "McsfMedViewer3D/McsfMedViewer3DScene/mcsf_viewer3d_view.h"

#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsRenderController::TpsRenderController(WINDOW_TYPE type) 
    : TpsRenderControllerBase(type) {

}

TpsRenderController::~TpsRenderController(){

}

TpsGOConverterBase* TpsRenderController::GetConverter_i(GO_TYPE goType) {
    return mGOConverterFactory->GetConverter(goType);
}

TPS_END_NAMESPACE  // end namespace tps
