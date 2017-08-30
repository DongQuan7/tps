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
#include "tps_ga_editcourse_cmdhandler.h"
#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaEditCourseCmdHandler::TpsGaEditCourseCmdHandler(IGalleryController *commonController) 
    : IGalleryCmdHandler(commonController){
}

TpsGaEditCourseCmdHandler::~TpsGaEditCourseCmdHandler(){

}

int TpsGaEditCourseCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext,
    std::string * pReplyObject){
        try {
            TPSPRINTF("\r\n EditCourse Start.\r\n");
            TPS_LOG_DEV_INFO<<"[INFO][TpsGaEditCourseCmdHandler][HandleCommand]: "
                <<"TpsGaEditCourseCmdHandler Enter.";

            if(nullptr == pContext || nullptr == pReplyObject){
                TPS_LOG_DEV_ERROR<< "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            proto::RT_TPS_Gallery_EditCourse protoEditCourse;
            protoEditCourse.ParseFromString(pContext->sSerializeObject);

            bool bRet = mIGalleryController->GetDataAccessor()->EditCourse(protoEditCourse);

            *pReplyObject = mIGalleryController->GetReplyString(bRet);

            TPSPRINTF("EditCourse End.\r\n");

            TPS_LOG_DEV_INFO<<"[INFO][TpsGaEditCourseCmdHandler][HandleCommand]: "
                <<"TpsGaEditCourseCmdHandler Leave";
            return TPS_SUCCESS;
        }
        catch(std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaEditCourseCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE  // end namespace tps
