//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_crosshairmovement_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 11, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_ga_crosshairmovement_cmdhandler.h"

#include "tps_common.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsProtoLib/rt_tps_manual_fusion.pb.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGaCrosshairMovementCmdHandler::TpsGaCrosshairMovementCmdHandler(IGalleryController *galleryController)
    :IGalleryCmdHandler(galleryController) {

}

TpsGaCrosshairMovementCmdHandler::~TpsGaCrosshairMovementCmdHandler(){

}

int TpsGaCrosshairMovementCmdHandler::HandleCommand(const Mcsf::CommandContext* pContext, std::string* pReplyObject) {
    try{
        proto::RT_TPS_MANUAL_FUSION protoManualFusion;
        protoManualFusion.ParseFromString(pContext->sSerializeObject);
        int iMouseAction= 0;
        if (protoManualFusion.action() == MOVE){
            iMouseAction = 0;
        }
        else if(protoManualFusion.action() == DRAG){
            iMouseAction = 1;
        }

        double xStart = protoManualFusion.xstart();
        double yStart = 1 - protoManualFusion.ystart();
        double xEnd = protoManualFusion.xend();
        double yEnd = 1 - protoManualFusion.yend();
        int nHitTestMode = protoManualFusion.hittestmode();
        LAYOUT_UNIT unit;
        unit.viewerControlID = protoManualFusion.viewercontrolid();
        unit.cellID = protoManualFusion.cellid();

        // when move crosshair, also need to update cornor info
       // bool bRender = true;
        //printf("TpsGaCrosshairMovementCmdHandler: now update cornor info: x = %f y = %f \n", xEnd, yEnd);
        //mIGalleryController->UpdateCornerInfo(unit, xEnd, yEnd, bRender);

        int nHitTestResult = mIGalleryController->UpdateCrosshair(unit, iMouseAction, xStart, yStart, xEnd, yEnd, nHitTestMode);

        if ( nHitTestResult > 0) {
                //*pReplyObject = "1";
                if (nHitTestResult == 1) 
                    *pReplyObject = "1";
                else if(nHitTestResult == 2)
                    *pReplyObject = "2";
                else if(nHitTestResult == 3)
                    *pReplyObject = "3";
                else if(nHitTestResult == 5)
                    *pReplyObject = "5";
                else if(nHitTestResult == 6)
                    *pReplyObject = "6";
        }
    }
    catch(std::exception ex) {
        return -1;
    }
    return 0;
}
TPS_END_NAMESPACE  // end namespace tps