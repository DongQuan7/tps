//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Yuxuan.Duan yuxuan.duan@united-imaging.com
//
/// \file    tps_bl_cmd_gallery_load_patient.cpp
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/11/16
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_gallery_load_patient.h"
#include "RtTpsBusinessLogic/tps_bl_load_objects.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

LoadPatientForGalleryCmd::LoadPatientForGalleryCmd(std::string patientUid) 
    : mPatientUid(patientUid)
{

}

LoadPatientForGalleryCmd::~LoadPatientForGalleryCmd()
{

}

int LoadPatientForGalleryCmd::Execute()
{
    int bRet = TPS_ER_FAILURE;

    try {
        bRet = mLoadObjects->LoadPatientForGallery(mPatientUid)
            ? TPS_ER_SUCCESS : TPS_ER_FAILURE;
    }
    catch (const std::exception& exp) {
        printf("exception: s%", exp.what());
    }
    catch (...) {
        printf("exception");
    }

    return bRet;
}

TPS_END_NAMESPACE