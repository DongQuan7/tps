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
#include "tps_ga_loadseries_cmdhandler.h"

#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

LoadSeriesCmdHandler::LoadSeriesCmdHandler(IGalleryController *galleryController)
    : IGalleryCmdHandler(galleryController) {
}

LoadSeriesCmdHandler::~LoadSeriesCmdHandler(){

}

int LoadSeriesCmdHandler::HandleCommand(
    const Mcsf::CommandContext *pContext, std::string *pReplyObject)
{
    try
    {
        const std::string sSeriesUID = pContext->sSerializeObject;
        mIGalleryController->LoadImageData(sSeriesUID);

        TPS_LOG_TRACE_INFO<<"LoadSeriesCommandHandler with SeriesUID:"<<sSeriesUID;

        // 0: orthogonal 1: oblique, 2: single line
        int crosshairMode = 0;
        mIGalleryController->GetCrosshairMode(crosshairMode);
        if (0 == crosshairMode)
        {
            *pReplyObject = "0";
        } else if(1== crosshairMode)
        {
           *pReplyObject = "1";
        }
        
        return TPS_SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<ex.what();
        return TPS_FAILURE;
    }
}

TPS_END_NAMESPACE
