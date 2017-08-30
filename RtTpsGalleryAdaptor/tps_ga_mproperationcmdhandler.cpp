//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_mproperationcmdhandler.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
///////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_mproperationcmdhandler.h"

// TPS
#include "tps_logger.h"
#include "RtTpsProtoLib/rt_tps_mprpageturn.pb.h"
#include "RtTpsProtoLib/rt_tps_mprpan.pb.h"
#include "RtTpsProtoLib/rt_tps_mprzoom.pb.h"
#include "Rttpsprotolib/rt_tps_mprwindowing.pb.h"
#include "Rttpsprotolib/rt_tps_drrpan.pb.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"    
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"
#include "RtTpsGalleryControllerApp/tps_gca_improperator.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

TpsGaMPRPageTurnCmdHandler::TpsGaMPRPageTurnCmdHandler(IGalleryController *galleryController) 
    : IGalleryCmdHandler(galleryController) {
}

TpsGaMPRPageTurnCmdHandler::~TpsGaMPRPageTurnCmdHandler(){

}

int TpsGaMPRPageTurnCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, 
    std::string *pReplyObject) {
        try {
            //TEST_PERFORMANCE_INIT
            //TEST_PERFORMANCE_BEGIN

            TPS_LOG_DEV_INFO<<"TpsGaMPRPageTurnCmdHandler::HandleCommand";
            if (nullptr == pContext || nullptr == pReplyObject) {
                TPS_LOG_DEV_ERROR<<"TpsGaMPRPageTurnCmdHandler::HandleCommand"
                    << "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            //parse the context
            proto::RT_TPS_MPR_PAGETURN pageturnInfo;
            pageturnInfo.ParseFromString(pContext->sSerializeObject);

            LAYOUT_UNIT unit;
            unit.viewerControlID = pageturnInfo.viewercontrolid();
            unit.cellID = pageturnInfo.cellid();
            int iPageTurn = pageturnInfo.pageturn();
            mIGalleryController->GetMPROperator()->MPRPageturn(unit, iPageTurn);
            //  TEST_PERFORMANCE_END("MPRPageTurn");

            return TPS_SUCCESS;
        }
        catch (std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaMPRPageTurnCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TpsGaMPRPanCmdHandler::TpsGaMPRPanCmdHandler(IGalleryController *galleryController) 
    : IGalleryCmdHandler(galleryController) {
}

TpsGaMPRPanCmdHandler::~TpsGaMPRPanCmdHandler(){

}

int TpsGaMPRPanCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, 
    std::string *pReplyObject) {
        try {
            TEST_PERFORMANCE_INIT
                TEST_PERFORMANCE_BEGIN

                TPS_LOG_DEV_INFO<<"TpsGaMPRPanCmdHandler::HandleCommand";
            if (nullptr == pContext || nullptr == pReplyObject) {
                TPS_LOG_DEV_ERROR<<"TpsGaMPRPanCmdHandler::HandleCommand"
                    << "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            //parse the context
            proto::RT_TPS_MPR_PAN panInfo;
            panInfo.ParseFromString(pContext->sSerializeObject);

            // todo: unit related info will be deleted later
            LAYOUT_UNIT unit;
            unit.viewerControlID = panInfo.viewercontrolid();
            unit.cellID = panInfo.cellid();

            float startx = panInfo.startx();
            float starty = panInfo.starty();
            float stopx = panInfo.stopx();
            float stopy = panInfo.stopy();

            //TODO: delete following protection code later
            float dis = (startx-stopx)*(startx-stopx)+(starty-stopy)*(starty-stopy);
            if (dis > 100 || dis<1e-6) {
                return TPS_SUCCESS;
            }

            mIGalleryController->GetMPROperator()->MPRPan(
                unit, startx,starty,stopx,stopy);

            TEST_PERFORMANCE_END("MPRPan");

            return TPS_SUCCESS;
        }
        catch (std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaMPRPanCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TpsGaMPRZoomCmdHandler::TpsGaMPRZoomCmdHandler(IGalleryController *galleryController) 
    : IGalleryCmdHandler(galleryController) {
}

TpsGaMPRZoomCmdHandler::~TpsGaMPRZoomCmdHandler(){

}

int TpsGaMPRZoomCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, 
    std::string *pReplyObject) {
        try {
            TEST_PERFORMANCE_INIT
                TEST_PERFORMANCE_BEGIN

                TPS_LOG_DEV_INFO<<"TpsGaMPRZoomCmdHandler::HandleCommand";
            if (nullptr == pContext || nullptr == pReplyObject) {
                TPS_LOG_DEV_ERROR<<"TpsGaMPRZoomCmdHandler::HandleCommand"
                    << "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            //parse the context
            proto::RT_TPS_MPR_ZOOM zoomInfo;
            zoomInfo.ParseFromString(pContext->sSerializeObject);

            // todo: unit related info will be deleted later
            LAYOUT_UNIT unit;
            unit.viewerControlID = zoomInfo.viewercontrolid();
            unit.cellID = zoomInfo.cellid();

            float zoomFactor = zoomInfo.zoomfactor();
            printf("TpsGaMPRZoomCmdHandler: zoom factor is %.2f\n", zoomFactor);

            mIGalleryController->GetMPROperator()->MPRZoom(unit, zoomFactor);
            TEST_PERFORMANCE_END("MPRZoom");

            return TPS_SUCCESS;
        }
        catch (std::exception& ex)
        {
            TPS_LOG_DEV_ERROR<<"TpsGaMPRZoomCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TpsGaMPRWindowingCmdHandler::TpsGaMPRWindowingCmdHandler(IGalleryController *galleryController) 
    : IGalleryCmdHandler(galleryController) {
}

TpsGaMPRWindowingCmdHandler::~TpsGaMPRWindowingCmdHandler(){

}

int TpsGaMPRWindowingCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, 
    std::string *pReplyObject) {
        try {
            TEST_PERFORMANCE_INIT
                TEST_PERFORMANCE_BEGIN

                TPS_LOG_DEV_INFO<<"TpsGaMPRWindowingCmdHandler::HandleCommand";
            if (nullptr == pContext || nullptr == pReplyObject) {
                TPS_LOG_DEV_ERROR<<"TpsGaMPRWindowingCmdHandler::HandleCommand"
                    << "nullptr == pContext || nullptr == pReplyObject";
                return TPS_FAILURE;
            }

            //parse the context
            proto::RT_TPS_MPR_WINDOWING windowingInfo;
            windowingInfo.ParseFromString(pContext->sSerializeObject);

            // TODO: unit related info will be deleted later
            LAYOUT_UNIT unit;
            unit.viewerControlID = windowingInfo.viewercontrolid();
            unit.cellID = windowingInfo.cellid();

            float delWinWidth = windowingInfo.deltaww();
            float delWinCenter = windowingInfo.deltawl();
            bool needUpdateAll = windowingInfo.needupdateall();

            //winWidth = 250; winCenter = 50;
            mIGalleryController->GetMPROperator()->MPRWindowing(
                unit, delWinWidth, delWinCenter, needUpdateAll);

            TEST_PERFORMANCE_END("MPRWindowing");

            return TPS_SUCCESS;
        }
        catch (std::exception& ex) {
            TPS_LOG_DEV_ERROR<<"TpsGaMPRWindowingCmdHandler::HandleCommand:"<<ex.what();
            return TPS_FAILURE;
        }
}

TpsGaMPRRotateCmdHandler::TpsGaMPRRotateCmdHandler(IGalleryController *galleryController)
    : IGalleryCmdHandler(galleryController){}

TpsGaMPRRotateCmdHandler::~TpsGaMPRRotateCmdHandler(){}

int TpsGaMPRRotateCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject)
{
    try 
    {
        TEST_PERFORMANCE_INIT
            TEST_PERFORMANCE_BEGIN

            TPS_LOG_DEV_INFO<<"TpsGaMPRRotateCmdHandler::HandleCommand";
        if (nullptr == pContext || nullptr == pReplyObject) 
        {
            TPS_LOG_DEV_ERROR<<"TpsGaMPRRotateCmdHandler::HandleCommand"
                << "nullptr == pContext || nullptr == pReplyObject";
            return TPS_FAILURE;
        }

        int bRet = TPS_FAILURE;
        if(nullptr != mIGalleryController)
        {
            if(pContext->sSerializeObject.empty())
            {
                TPS_LOG_DEV_ERROR<<"Empty sSerializeObject in int TpsGaMPRRotateCmdHandler::HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject)";
                return TPS_FAILURE;
            }

            //parse the context
            proto::RT_TPS_MPR_PAN mprPan;
            mprPan.ParseFromString(pContext->sSerializeObject);

            // todo: unit related info will be deleted later
            tps::LAYOUT_UNIT unit;
            if(mprPan.has_cellid())
            {
                unit.cellID = mprPan.cellid();
            }
            else 
            {
                unit.cellID = 0;
            }
            unit.viewerControlID = mprPan.viewercontrolid();
            bRet = mIGalleryController->GetMPROperator()->MPRRotate(unit,/* drrPan.curimageuid(),*/ mprPan.startx(),
                mprPan.starty(), mprPan.stopx(), mprPan.stopy());
        }
        TEST_PERFORMANCE_END("MPRRotate");

        return bRet;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGaMPRZoomCmdHandler::HandleCommand:"<<ex.what();
        return TPS_FAILURE;
    }
}



TPS_END_NAMESPACE  // end namespace tps
