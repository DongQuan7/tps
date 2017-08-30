//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mproperator.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

//TMS
#include "tms/rt_fwk_dl_access.h"
#include "tms/rt_fwk_dl_object_image3d.h"

//#include "tps_comca_cursorinfodisplaycmd.h"
//#include "tps_comca_moviecmd.h"
#include "RtTpsProtoLib/rt_tps_certifiedmouseposinfo.h"
#include "tps_logger.h"
#include "tps_gca_mproperator.h"  
#include "tps_gca_mprzoompanwindowingcmd.h"
#include "tps_gca_mprrotatecmd.h"
#include "tps_gca_mprpageturncmd.h"
#include "tps_gca_mprhittest_cmd.h"
#include "tps_gca_measureanglecmd.h"
#include "tps_gca_measuredistancecmd.h"
#include "tps_gca_mprupdatecornertextinfo_cmd.h"
#include "tps_gca_mprcornertextinfo_visible_cmd.h"

TPS_BEGIN_NAMESPACE
bool TpsGcaMPROperator::Initialize() { 

    return true;
}

bool TpsGcaMPROperator::Finalize() {

    return true;
}

int TpsGcaMPROperator::MPRPageturn(LAYOUT_UNIT unit, int iPageTurn){
        TpsGcaMPRPageturnCmd cmd(unit, iPageTurn);

        if (!cmd.RunCommand()) {
            TPS_LOG_DEV_ERROR<<"MPRPageTurn RunCommand error.";
            return TPS_FAILURE;
        }

        return TPS_SUCCESS;
}
//
//int TpsGcaMPROperator::MPRPageturn4D(LAYOUT_UNIT unit, int iPageTurn, 
//    const std::vector<std::string>& seriesUidList, FIRST_POSTFIX_COMPONENT sectionType){
//        MPRPageturn4DCmd cmd(unit, iPageTurn, seriesUidList, sectionType);
//
//        if (!cmd.RunCommand()) {
//            TPS_LOG_DEV_ERROR<<"MPRPageTurn RunCommand error.";
//            return TPS_FAILURE;
//        }
//
//        return TPS_SUCCESS;
//}

int TpsGcaMPROperator::MPRPan(LAYOUT_UNIT unit, float startx, float starty, 
    float stopx, float stopy) {
        TpsGcaMPRPanCmd cmd(unit, startx, starty, stopx, stopy);

        if (!cmd.RunCommand()) {
            TPS_LOG_DEV_ERROR<<"MPRPan RunCommand error.";
            return TPS_FAILURE;
        }

        return TPS_SUCCESS;
}

int TpsGcaMPROperator::MPRZoom(LAYOUT_UNIT unit, float zoomFactor) {
        TpsGcaMPRZoomCmd cmd(unit, zoomFactor);

        if (!cmd.RunCommand()) {
            TPS_LOG_DEV_ERROR<<"MPRZoom RunCommand error.";
            return TPS_FAILURE;
        }

        return TPS_SUCCESS;
}

int TpsGcaMPROperator::MPRZoom(LAYOUT_UNIT unit, float startx, float starty, 
    float stopx, float stopy) {
        TpsGcaMPRZoomCmd cmd(unit, startx, starty, stopx, stopy);

        if (!cmd.RunCommand()) {
            TPS_LOG_DEV_ERROR<<"MPRZoom RunCommand error.";
            return TPS_FAILURE;
        }

        return TPS_SUCCESS;
}

int TpsGcaMPROperator::MPRWindowing(LAYOUT_UNIT unit, float deltaWidth, 
    float deltaCenter, bool updateAll) {
        TpsGcaMPRWindowingCmd cmd(unit, deltaWidth, deltaCenter, updateAll);

        if (!cmd.RunCommand()) {
            TPS_LOG_DEV_ERROR<<"MPRWindowing RunCommand error.";
            return TPS_FAILURE;
        }

        return TPS_SUCCESS;
}

int TpsGcaMPROperator::MPRRotate( const tps::LAYOUT_UNIT &unit,
    const float startX, const float startY, const float stopX, const float stopY ){
        TpsGcaMPRRotateCmd cmd(unit, startX, startY, stopX, stopY);
        if(!cmd.RunCommand()){
            TPS_LOG_DEV_ERROR << "MPRRotate RunCommand error.";
            return TPS_FAILURE;
        }
        return TPS_SUCCESS;
}

int TpsGcaMPROperator::MeasureAngle( const tps::LAYOUT_UNIT &unit, const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, 
    const float cursorX, const float cursorY, const bool actionStarted, const bool firstSideDone, const bool secondSideDone ){
    TpsGcaMeasureAngleCmd cmd(unit, anglePoints, cursorX, cursorY,
        actionStarted, firstSideDone, secondSideDone);
    if(!cmd.RunCommand()){
        TPS_LOG_DEV_ERROR << "Measure angle RunCommand error.";
        return TPS_FAILURE;
    }
    return TPS_SUCCESS;
}

int TpsGcaMPROperator::MeasureDistance(const tps::LAYOUT_UNIT &unit, const float startX, const float startY,
    const float stopX, const float stopY, const bool actionStarted, const bool actionDone){
    TpsGcaMeasureDistanceCmd cmd(unit, startX, startY, stopX, stopY,
        actionStarted, actionDone);
    if(!cmd.RunCommand()){
        TPS_LOG_DEV_ERROR << "Measure distance RunCommand error.";
        return TPS_FAILURE;
    }
    return TPS_SUCCESS;
}

//TODO
HIT_TEST_RESULT TpsGcaMPROperator::HitTest(const CertifiedMousePosInfo& certifedMousePosInfo){
    TpsGcaMPRHitTestCmd cmd(certifedMousePosInfo);

    if (!cmd.RunCommand()) {
        TPS_LOG_DEV_ERROR<<"MPRHitTest RunCommand error.";
        return 0;
    }
    return cmd.GetHitTestResult();
    //return TPS_SUCCESS;
}
//
// TODO
//int TpsGcaMPROperator::UpdateCrosshair(LAYOUT_UNIT unit, int iMouseAction, 
//    double xStart, double yStart, double xEnd, double yEnd, 
//    int nHitTestMode) {
//        if (iMouseAction == 0 &&                          // 0: move,  1: drag
//            nHitTestMode == 1) {
//                //MprHitTestCmd cmd(certifedMousePosInfo);
//
//                //if (!cmd.RunCommand()) {
//                //    TPS_LOG_DEV_ERROR<<"MPRHitTest RunCommand error.";
//                //    return 0;
//                //}
//                //return cmd.GetHitTestResult();
//        }
//        unit, xStart, yStart, xEnd, yEnd;
//        return TPS_SUCCESS;
//}
 // TODO
int TpsGcaMPROperator::MPRUpdateCornerTextInfo(const CertifiedMousePosInfo& certifedMousePosInfo){
    TpsGcaMPRUpdateCornerTextInfoCmd cmd(certifedMousePosInfo);

    if (!cmd.RunCommand()) {
        TPS_LOG_DEV_ERROR<<"TpsGcaMPRUpdateCornerTextInfoCmd RunCommand error.";
        return TPS_FAILURE;
    }

    return TPS_SUCCESS;
}

int TpsGcaMPROperator::MPRCornerImageTextVisible(const std::string &sSeriesUID, const bool& bIsVisible){
    TpsGcaMPRCornerTextInfoVisibleCmd cmd(sSeriesUID, bIsVisible);

    if (!cmd.RunCommand()) {
        TPS_LOG_DEV_ERROR<<"TpsGcaMPRCornerTextInfoVisibleCmd RunCommand error.";
        return TPS_FAILURE;
    }

    return TPS_SUCCESS;
}

//int TpsGcaMPROperator::MPRCursorPointerAction(float normPlanX, 
//    float normPlanY, const std::string& seriesUid, 
//    const std::string& planUid, FIRST_POSTFIX_COMPONENT sectionType) {
//        MprCursorInfoDisplayCmd cmd(normPlanX, normPlanY, seriesUid, planUid, sectionType);
//
//        if (!cmd.RunCommand()) {
//            TPS_LOG_DEV_ERROR<<"MPRWindowing RunCommand error.";
//            return TPS_FAILURE;
//        }
//
//        return TPS_SUCCESS;
//}
//
//int TpsGcaMPROperator::Movie(const CertifiedMovieAction &certifiedMovie) {
//    MovieCmd cmd(certifiedMovie);
//
//    if(!cmd.RunCommand()) {
//        TPS_LOG_DEV_ERROR<<"movie runCommand error!";
//        return TPS_FAILURE;
//    }
//    return TPS_SUCCESS;
//}

TPS_END_NAMESPACE

