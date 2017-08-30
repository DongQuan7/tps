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
#include "tps_ga_deletecurrentslice_cmdhandler.h"

#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h" 
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaDeleteCurrentSliceCmdHandler::TpsGaDeleteCurrentSliceCmdHandler(IGalleryController *commonController) 
    : IGalleryCmdHandler(commonController){
}

TpsGaDeleteCurrentSliceCmdHandler::~TpsGaDeleteCurrentSliceCmdHandler(){

}

int TpsGaDeleteCurrentSliceCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext,
    std::string * pReplyObject){
        try {
            TPSPRINTF("\r\n DeleteSeries Start.\r\n");
            TPS_LOG_DEV_INFO<<"[INFO][TpsGaDeleteCurrentSliceCmdHandler][HandleCommand]: "
                <<"TpsGaDeleteCurrentSliceCmdHandler Enter.";

            if(nullptr == pContext || nullptr == pReplyObject){
                TPS_LOG_DEV_ERROR<< "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }
            std::string sSeriesUID = pContext->sSerializeObject;
            std::string sCurrentSeriesUID = TpsGalleryImageManager::GetInstance()->GetCurrentImage3DUID();
            if (sSeriesUID != sCurrentSeriesUID) {
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGaDeleteCurrentSliceCmdHandler][HandleCommand]" 
                    <<"SeriesUID not equal to CurrentSeriesUID";
                return TPS_FAILURE;
            }

            int iSlice;
            int iTotalSlice;
            if (!mIGalleryController->GetCurrentMPRSlice(GALLERY_AXIAL, iSlice, iTotalSlice)){
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGaDeleteCurrentSliceCmdHandler][HandleCommand]" 
                    <<"Failed to GetCurrentMPRSlice";
                return TPS_FAILURE;
            }

            bool bRet = mIGalleryController->DeleteSliceInLocal(sSeriesUID, iSlice);

            *pReplyObject = mIGalleryController->GetReplyString(bRet);

            TPSPRINTF("DeleteSeries End.\r\n");

            TPS_LOG_DEV_INFO<<"[INFO][TpsGaDeleteCurrentSliceCmdHandler][HandleCommand]: "
                <<"TpsGaDeleteCurrentSliceCmdHandler Leave";
            return TPS_SUCCESS;
        }
        catch(std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaDeleteCurrentSliceCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE  // end namespace tps
