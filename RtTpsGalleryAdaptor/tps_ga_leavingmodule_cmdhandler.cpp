////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_loadpatient_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_leavingmodule_cmdhandler.h"

#include "RtTpsFramework/tps_fw_commandids.h"
#include "RtTpsFramework/tps_fw_commproxy.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaLeavingModuleCmdHandler::TpsGaLeavingModuleCmdHandler(IGalleryController *commonController) 
    : IGalleryCmdHandler(commonController){
}

TpsGaLeavingModuleCmdHandler::~TpsGaLeavingModuleCmdHandler(){

}

// Temporary
int TpsGaLeavingModuleCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext,
    std::string * pReplyObject){
        try {
            pContext;
            pReplyObject;
            TPS_LOG_DEV_INFO<<"[INFO][TpsGaLeavingModuleCmdHandler][HandleCommand]: "
                <<"TpsGaLeavingModuleCmdHandler Enter.";

            mIGalleryController->ClearImageDatas();         

            TPS_LOG_DEV_INFO<<"[INFO][TpsGaLeavingModuleCmdHandler][HandleCommand]: "
                <<"TpsGaLeavingModuleCmdHandler Leave";
            return TPS_SUCCESS;
        }
        catch(std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaLeavingModuleCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE  // end namespace tps
