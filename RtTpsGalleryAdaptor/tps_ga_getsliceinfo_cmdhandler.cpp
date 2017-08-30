//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Apr. 18, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_getsliceinfo_cmdhandler.h"

// TPS
#include "tps_logger.h"
#include "RtTpsProtoLib/rt_tps_rotate.pb.h"
#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
  
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"


TPS_BEGIN_NAMESPACE// begin namespace tps


TpsGaGetSliceInfoCmdHandler::TpsGaGetSliceInfoCmdHandler(IGalleryController *galleryController)
    : IGalleryCmdHandler(galleryController){}

TpsGaGetSliceInfoCmdHandler::~TpsGaGetSliceInfoCmdHandler(){}

int TpsGaGetSliceInfoCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject)
{
    try 
    {   
        // Check ?
        pContext;
        int currentSlice = 0;
        int totalSlice = 0;
        if(!mIGalleryController->GetCurrentMPRSlice(GALLERY_AXIAL, currentSlice, totalSlice)) {
            return TPS_FAILURE;
        }
        proto::RT_TPS_Gallery_SliceInfo protoSliceInfo;
        protoSliceInfo.set_currentslice(currentSlice);
        protoSliceInfo.set_totalslice(totalSlice);

        *pReplyObject = protoSliceInfo.SerializeAsString();

        return TPS_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGaMPRZoomCmdHandler::HandleCommand:"<<ex.what();
        return TPS_FAILURE;
    }
}



TPS_END_NAMESPACE  // end namespace tps
