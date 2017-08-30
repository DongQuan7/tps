//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  wenlong.zhao mailto:wenlong.zhao@united-imaging.com
///
///  \file    tps_rl_easyplan_measuregraphicobject.cpp
///  \brief   tps easyplan measure graphic object class
///
///  \version 1.0
///  \date    2014-11-08
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuregraphicobject.h"


TPS_BEGIN_NAMESPACE

EasyPlanMeasureGraphicObject::EasyPlanMeasureGraphicObject( void ) {

}

EasyPlanMeasureGraphicObject::~EasyPlanMeasureGraphicObject( void ) {

}

bool EasyPlanMeasureGraphicObject::Initialize() {
    return true;
}

bool EasyPlanMeasureGraphicObject::Finalize() {
    return true;
}

// Masure distance
void EasyPlanMeasureGraphicObject::SetMeasureDistanceStartPoint( float startX, 
    float startY ) {
    mRulerStartX = startX;
    mRulerStartY = startY;
}

void EasyPlanMeasureGraphicObject::GetMeasureDistanceStartPoint( float *startX, 
    float *startY ) {
    *startX = mRulerStartX;
    *startY = mRulerStartY;
}

void EasyPlanMeasureGraphicObject::SetMeasureDistanceStopPoint( float stopX, float stopY ) {
    mRulerStopX = stopX;
    mRulerStopY = stopY;
}

void EasyPlanMeasureGraphicObject::GetMeasureDistanceStopPoint( float *stopX, float *stopY )
{
    *stopX = mRulerStopX;
    *stopY = mRulerStopY;
}

void EasyPlanMeasureGraphicObject::SetMeasureDistanceActionStatus( bool isActionStarted,
    bool isActionDone ) {
    mRulerIsActionStarted = isActionStarted;
    mRulerIsActionDone = isActionDone;
}

void EasyPlanMeasureGraphicObject::GetMeasureDistanceActionStatus( bool *isActionStarted,
    bool *isActionDone ) {
    *isActionStarted = mRulerIsActionStarted;
    *isActionDone = mRulerIsActionDone;
}


// Measure angle
void EasyPlanMeasureGraphicObject::SetMeasureAnglePoints( 
    std::list<Mcsf::MedViewer3D::Point2D> anglePoints ) {
    mProtractorAnglePoints = anglePoints;
}

void EasyPlanMeasureGraphicObject::GetMeasureAnglePoints(
    std::list<Mcsf::MedViewer3D::Point2D>& anglePoints ) const {
    anglePoints = mProtractorAnglePoints;
}

void EasyPlanMeasureGraphicObject::SetMeasureAngleCursorPoint( float cursorX, 
    float cursorY ) {
    mProtractorCursorX = cursorX;
    mProtractorCursorY = cursorY;
}

void EasyPlanMeasureGraphicObject::GetMeasureAngleCursorPoint( float *cursorX, 
    float *cursorY ) {
    *cursorX = mProtractorCursorX;
    *cursorY = mProtractorCursorY;
}

void EasyPlanMeasureGraphicObject::SetMeasureAngleActionStatus( bool isActionStarted, 
    bool isFirstSideDone, bool isSecondSideDone ) {
    mProtractorActionStarted = isActionStarted;
    mProtractorFirstSideDone = isFirstSideDone;
    mProtractorSecondSideDone = isSecondSideDone;
}

void EasyPlanMeasureGraphicObject::GetMeasureAngleActionStatus( bool *isActionStarted, 
    bool *isFirstSideDone, bool *isSecondSideDone ) {
    *isActionStarted = mProtractorActionStarted;
    *isFirstSideDone = mProtractorFirstSideDone;
    *isSecondSideDone = mProtractorSecondSideDone;
}

TPS_END_NAMESPACE