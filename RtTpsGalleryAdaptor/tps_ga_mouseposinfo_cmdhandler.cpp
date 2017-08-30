//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_mouseposinfo_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 11, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

//TPS
#include "RtTpsGalleryControllerApp/tps_gca_igallery_controller.h"
#include "RtTpsGalleryControllerApp/tps_gca_improperator.h"

#include "RtTpsProtoLib/rt_tps_mouseposinfo.pb.h"
#include "RtTpsProtoLib/rt_tps_certifiedmouseposinfo.h"

#include "tps_ga_mouseposinfo_cmdhandler.h"
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsGaMousePosInfoCmdHandler::TpsGaMousePosInfoCmdHandler(IGalleryController *galleryController) 
    : IGalleryCmdHandler(galleryController) {
}

TpsGaMousePosInfoCmdHandler::~TpsGaMousePosInfoCmdHandler(){

}

int TpsGaMousePosInfoCmdHandler::HandleCommand(
    const Mcsf::CommandContext *pContext, std::string *pReplyObject){
        try {
            *pReplyObject = "0";
            TPS_LOG_DEV_INFO<<"InitializeCommandHandler::HandleCommand";
            if (nullptr == pContext || nullptr == pReplyObject){
                TPS_LOG_DEV_ERROR<<"InitializeCommandHandler::HandleCommand"
                    << "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }
            // TODO Check SeriesUID in FE
            if (mIGalleryController->GetCurrentSeriesUID().empty()) {
                return TPS_FAILURE;
            }

            proto::RT_TPS_MOUSE_POS_INFO protoMousePosInfo;
            protoMousePosInfo.ParseFromString(pContext->sSerializeObject);

            CertifiedMousePosInfo mousePosInfo;
            mousePosInfo.Convert(protoMousePosInfo);

            mousePosInfo.mMouseX = mousePosInfo.mMouseX / mousePosInfo.mWidth;
            mousePosInfo.mMouseY = 1 - (mousePosInfo.mMouseY / mousePosInfo.mHeight);

            int nHitTestResult = mIGalleryController->GetMPROperator()->HitTest(mousePosInfo);

            *pReplyObject = boost::lexical_cast<std::string>(nHitTestResult);

            mIGalleryController->GetMPROperator()->MPRUpdateCornerTextInfo(mousePosInfo);

            return TPS_SUCCESS;
        }
        catch (std::exception& ex){
            TPS_LOG_DEV_ERROR<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE
