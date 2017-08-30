//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_loadseries_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"     
#include "tps_ga_imagetextvisible_cmdhandler.h"

#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h" 
#include "RtTpsGalleryControllerApp/tps_gca_improperator.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsGaImageTextVisibleCmdHandler::TpsGaImageTextVisibleCmdHandler(IGalleryController *galleryController)
    : IGalleryCmdHandler(galleryController) {
}

TpsGaImageTextVisibleCmdHandler::~TpsGaImageTextVisibleCmdHandler(){

}

int TpsGaImageTextVisibleCmdHandler::HandleCommand(
    const Mcsf::CommandContext *pContext, std::string *pReplyObject)
{
    try
    {
        *pReplyObject = "0";
        TPS_LOG_DEV_INFO<<"InitializeCommandHandler::HandleCommand";
        if (nullptr == pContext || nullptr == pReplyObject){
            TPS_LOG_DEV_ERROR<<"InitializeCommandHandler::HandleCommand"
                << "nullptr == pContext || nullptr == pReplyObject";
            return TPS_FAILURE;
        }

        proto::RT_TPS_Gallery_CornerInfoVisible cornerInfoVisible;
        cornerInfoVisible.ParseFromString(pContext->sSerializeObject);

        mIGalleryController->GetMPROperator()->MPRCornerImageTextVisible(cornerInfoVisible.seriesuid(), cornerInfoVisible.isvisible());

        return TPS_SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<ex.what();
        return TPS_FAILURE;
    }
}

TPS_END_NAMESPACE
