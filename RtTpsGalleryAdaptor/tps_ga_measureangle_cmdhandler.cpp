//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yuxuan.duan (mailto:yuxuan.duan@united-imaging.com)
///
///  \file   tps_ga_measureanglecmdhandler.cpp 
///  \brief  gallery measure angle command handler
///
///  \version 1.0
///  \date  2015/03/06
///  \
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tps_ga_measureangle_cmdhandler.h"

#include "McsfMedViewer3DArithmetic/point2d.h"
#include "RtTpsGalleryControllerApp/tps_gca_improperator.h"
#include "RtTpsProtoLib/rt_tps_measureangle.pb.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE //begin namespace tps

TpsGaMeasureAngleCmdHandler::TpsGaMeasureAngleCmdHandler( IGalleryController *controller ) 
    : IGalleryCmdHandler(controller){
}

TpsGaMeasureAngleCmdHandler::~TpsGaMeasureAngleCmdHandler() {
}

int TpsGaMeasureAngleCmdHandler::HandleCommand( const Mcsf::CommandContext *pContext, std::string *pReplyObject ) 
{
    //pContext;
    //pReplyObject;
    try 
    {
        TEST_PERFORMANCE_INIT
            TEST_PERFORMANCE_BEGIN

            TPS_LOG_DEV_INFO<<"TpsGaMeasureAngleCmdHandler::HandleCommand";
        if (nullptr == pContext || nullptr == pReplyObject) 
        {
            TPS_LOG_DEV_ERROR<<"TpsGaMeasureAngleCmdHandler::HandleCommand"
                << "nullptr == pContext || nullptr == pReplyObject";
            return TPS_FAILURE;
        }
//            TPS_LOG_DEV_INFO<<"TpsGaMeasureAngleCmdHandler::HandleCommand";
//#ifdef _DEBUG
//            printf("\n[TPS][BE][Gallery]TpsGaMeasureAngleCmdHandler::HandleCommand");
//#endif
        int bRet = TPS_FAILURE;

        if(nullptr != mIGalleryController)
        {
            if(pContext->sSerializeObject.empty())
            {
                TPS_LOG_DEV_ERROR<<"Empty sSerializeObject in int TpsGaMeasureAngleCmdHandler::HandleCommand";
                return TPS_FAILURE;
            }

            proto::RT_TPS_MEASURE_ANGLE measureAngle;
            measureAngle.ParseFromString(pContext->sSerializeObject);

            tps::LAYOUT_UNIT unit;
            if(measureAngle.has_cellid())
            {
                unit.cellID = measureAngle.cellid();
            }
            else 
            {
                unit.cellID = 0;
            }
            unit.viewerControlID = measureAngle.viewercontrolid();

            int anglePointsSize = measureAngle.m_anglepoints().manglepoints_size();
            std::list<Mcsf::MedViewer3D::Point2D> anglePoints;
            for(int i=0; i<anglePointsSize; i++)
            {
                Mcsf::MedViewer3D::Point2D newAnglePoint;
                newAnglePoint.x = measureAngle.m_anglepoints().manglepoints().Get(i).m_x();
                newAnglePoint.y = measureAngle.m_anglepoints().manglepoints().Get(i).m_y();
                anglePoints.push_back(newAnglePoint);
            }
            // need to use easy plan controller to measure angle, write measure angle method in EasyPlanController
            bRet = mIGalleryController->GetMPROperator()->MeasureAngle(unit, anglePoints, measureAngle.m_fcursorx(), measureAngle.m_fcursory(),
                measureAngle.actionstarted(), measureAngle.firstsidedone(), measureAngle.secondsidedone());
        }
        return bRet;
    }
    catch (std::exception &e) {
        TPS_LOG_DEV_ERROR<<e.what();
        return TPS_FAILURE;
    }

}

TPS_END_NAMESPACE