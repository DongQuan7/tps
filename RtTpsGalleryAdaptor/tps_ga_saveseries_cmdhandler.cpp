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
///  \date    Apr. 22, 2015
/////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_saveseries_cmdhandler.h"

#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaSaveSeriesCmdHandler::TpsGaSaveSeriesCmdHandler(IGalleryController *commonController) 
    : IGalleryCmdHandler(commonController){
}

TpsGaSaveSeriesCmdHandler::~TpsGaSaveSeriesCmdHandler(){

}

int TpsGaSaveSeriesCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext,
    std::string * pReplyObject){
        try {
            TPSPRINTF("\n SaveSeries Start.\r\n");
            TPS_LOG_DEV_INFO<<"[INFO][TpsGaSaveSeriesCmdHandler][HandleCommand]: "
                <<"TpsGaSaveSeriesCmdHandler Enter.";

            if(nullptr == pContext || nullptr == pReplyObject){
                TPS_LOG_DEV_ERROR<< "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            proto::RT_TPS_Gallery_SaveSeries protoSaveSeries;
            protoSaveSeries.ParseFromString(pContext->sSerializeObject);

            bool bRet = mIGalleryController->GetDataAccessor()->DeleteSlices(protoSaveSeries.seriesuid(), protoSaveSeries.issaveasnewseries());

            if (bRet){
               // 如果是SaveAs，当前的Series需要重新Reload
               mIGalleryController->ClearImageData(protoSaveSeries.seriesuid());
            }

            *pReplyObject = mIGalleryController->GetReplyString(bRet);

            TPSPRINTF("SaveSeries End.\r\n");

            TPS_LOG_DEV_INFO<<"[INFO][TpsGaSaveSeriesCmdHandler][HandleCommand]: "
                <<"TpsGaSaveSeriesCmdHandler Leave";
            return TPS_SUCCESS;
        }
        catch(std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaSaveSeriesCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE  // end namespace tps
