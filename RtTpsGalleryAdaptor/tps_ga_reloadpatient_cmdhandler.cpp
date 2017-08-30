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
#include "tps_ga_reloadpatient_cmdhandler.h"

#include "RtTpsProtoLib/rt_tps_certifiedimage3d.h" 
#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaReloadPatientCmdHandler::TpsGaReloadPatientCmdHandler(IGalleryController *commonController) 
    : IGalleryCmdHandler(commonController){
}

TpsGaReloadPatientCmdHandler::~TpsGaReloadPatientCmdHandler(){

}

// Temporary
int TpsGaReloadPatientCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext,
    std::string * pReplyObject){
        try {
            TPSPRINTF("\r\n ReLoadPatient Start.\r\n");
            TPS_LOG_DEV_INFO<<"[INFO][TpsGaReloadPatientCmdHandler][HandleCommand]: "
                <<"TpsGaReloadPatientCmdHandler Enter.";

            if(nullptr == pContext || nullptr == pReplyObject){
                TPS_LOG_DEV_ERROR<< "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            std::string patientUID = pContext->sSerializeObject;

            TPSPRINTF("ClearGalleryPatientManager Start.\r\n");
            mIGalleryController->GetDataLoader()->ClearGalleryPatientManager();
            TPSPRINTF("ClearGalleryPatientManager End.\r\n");

            if(!mIGalleryController->GetDataLoader()->LoadPatient(patientUID)){
                TPS_LOG_DEV_ERROR<<"Failed to load patient with uid["<<patientUID<<"].";
                return TPS_FAILURE;
            }

            std::vector<std::string> loadedSeriesUIDList;
            if(!mIGalleryController->GetDataLoader()->ReloadSeries(patientUID, loadedSeriesUIDList)){
                TPS_LOG_DEV_ERROR<<"Failed to load series with patient uid["<<patientUID<<"].";
                return TPS_FAILURE;
            }

            std::vector<std::string> courseUIDList;
            if (!mIGalleryController->GetDataLoader()->LoadCoursesOfPatient(patientUID, courseUIDList)){
                TPS_LOG_DEV_ERROR<<"Failed to load courses with patient uid["<<patientUID<<"].";
                return TPS_FAILURE;
            }

            proto::RT_TPS_Gallery_Courses protoCourses;
            mIGalleryController->GetDataAccessor()->GetCourseListByPatientUID(patientUID, protoCourses);

            *pReplyObject = protoCourses.SerializeAsString();

            TPSPRINTF("ReloadPatient End.\r\n");

            TPS_LOG_DEV_INFO<<"[INFO][TpsGaReloadPatientCmdHandler][HandleCommand]: "
                <<"TpsGaReloadPatientCmdHandler Leave";
            return TPS_SUCCESS;
        }
        catch(std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaReloadPatientCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TPS_END_NAMESPACE  // end namespace tps
