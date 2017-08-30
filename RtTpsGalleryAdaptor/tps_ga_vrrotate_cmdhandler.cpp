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
///  \date    Apr. 17, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_vrrotate_cmdhandler.h"

// TPS
#include "tps_logger.h"
#include "RtTpsProtoLib/rt_tps_rotate.pb.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
  
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"


TPS_BEGIN_NAMESPACE// begin namespace tps


TpsGaVRRotateCmdHandler::TpsGaVRRotateCmdHandler(IGalleryController *galleryController)
    : IGalleryCmdHandler(galleryController){}

TpsGaVRRotateCmdHandler::~TpsGaVRRotateCmdHandler(){}

int TpsGaVRRotateCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject)
{
    try 
    {
        pReplyObject;
        proto::RT_TPS_ROTATE_SHADEDSURFACE protoRotate;
        protoRotate.ParseFromString(pContext->sSerializeObject);

        Mcsf::Point2f prePt,curPt;
        prePt.SetX(protoRotate.precoordx());
        prePt.SetY(protoRotate.precoordy());
        curPt.SetX(protoRotate.currentcoordx());
        curPt.SetY(protoRotate.currentcoordy());

        if(TPS_SUCCESS != mIGalleryController->RotateShadedSurface(prePt,curPt)) {
            return TPS_FAILURE;
        }
        return TPS_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGaMPRZoomCmdHandler::HandleCommand:"<<ex.what();
        return TPS_FAILURE;
    }
}



TPS_END_NAMESPACE  // end namespace tps
