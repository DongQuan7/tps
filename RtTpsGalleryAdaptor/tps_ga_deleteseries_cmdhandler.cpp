////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_editcourse_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Apr. 03, 2015
/////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_deleteseries_cmdhandler.h"
#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaDeleteSeriesCmdHandler::TpsGaDeleteSeriesCmdHandler(IGalleryController *commonController) 
    : IGalleryCmdHandler(commonController){
}

TpsGaDeleteSeriesCmdHandler::~TpsGaDeleteSeriesCmdHandler(){

}

int TpsGaDeleteSeriesCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext,
    std::string * pReplyObject){
        try {
            TPSPRINTF("\r\n DeleteSeries Start.\r\n");
            TPS_LOG_DEV_INFO<<"[INFO][TpsGaDeleteSeriesCmdHandler][HandleCommand]: "
                <<"TpsGaDeleteSeriesCmdHandler Enter.";

            if(nullptr == pContext || nullptr == pReplyObject){
                TPS_LOG_DEV_ERROR<< "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }
            std::string sSeriesUID = pContext->sSerializeObject;

            bool bRet = mIGalleryController->GetDataAccessor()->DeleteSeries(sSeriesUID);
            if (bRet) {
                mIGalleryController->ClearImageData(sSeriesUID);
            }

            *pReplyObject = mIGalleryController->GetReplyString(bRet);

            TPSPRINTF("DeleteSeries End.\r\n");

            TPS_LOG_DEV_INFO<<"[INFO][TpsGaDeleteSeriesCmdHandler][HandleCommand]: "
                <<"TpsGaDeleteSeriesCmdHandler Leave";
            return TPS_SUCCESS;
        }
        catch(std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaDeleteSeriesCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE  // end namespace tps
