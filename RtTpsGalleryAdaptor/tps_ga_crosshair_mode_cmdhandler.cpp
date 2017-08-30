//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_crosshair_mode_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 12, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_ga_crosshairmode_cmdhandler.h"

#include "tps_common.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsProtoLib/rt_tps_crosshair_mode.pb.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGaCrosshairModeCmdHandler::TpsGaCrosshairModeCmdHandler(IGalleryController *galleryController)
    :IGalleryCmdHandler(galleryController) {

}

TpsGaCrosshairModeCmdHandler::~TpsGaCrosshairModeCmdHandler(){

}

int TpsGaCrosshairModeCmdHandler::HandleCommand(const Mcsf::CommandContext* pContext, std::string* /*pReplyObject*/) {
    try{
        proto::RT_TPS_CROSSHAIR_MODE protoCrosshairMode;
        protoCrosshairMode.ParseFromString(pContext->sSerializeObject);

        int nCrosshairMode = protoCrosshairMode.crosshairmode();
        int nVisible = protoCrosshairMode.visible();

        LAYOUT_UNIT unit;
        unit.viewerControlID = protoCrosshairMode.viewercontrolid();
        unit.cellID = protoCrosshairMode.cellid();

        mIGalleryController->UpdateCrosshairMode(nCrosshairMode, nVisible);

    }
    catch(std::exception ex) {
        return TPS_FAILURE;
    }
    return TPS_SUCCESS;
}
TPS_END_NAMESPACE  // end namespace tps