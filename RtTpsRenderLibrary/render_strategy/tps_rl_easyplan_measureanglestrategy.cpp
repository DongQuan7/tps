//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  wenlong.zhao mailto:wenlong.zhao@united-imaging.com
///
///  \file    tps_rl_easyplan_measureanglestrategy.cpp
///  \brief   tps easyplan measure angle strategy class
///
///  \version 1.0
///  \date    2014-11-11
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measureanglestrategy.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuregraphicobject.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

//MCSF
#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"

//OPENGL
#include "gl\glew.h"
#include "gl\GL.h"
#include "gl\GLU.h"
#include <cmath>

TPS_BEGIN_NAMESPACE

EasyPlanMeasureAngleStrategy::EasyPlanMeasureAngleStrategy(){

}

EasyPlanMeasureAngleStrategy::~EasyPlanMeasureAngleStrategy() {

}
void EasyPlanMeasureAngleStrategy::Render_i() {
    if(mEasyPlanMeasureAngleGraphicObject == nullptr) {
        return;
    }
    bool actionStarted = false;
    bool firstSideDone = false;
    bool secondSideDone = false;
    float cursorX = 0.0f;
    float cursorY = 0.0f;

    mEasyPlanMeasureAngleGraphicObject->GetMeasureAngleCursorPoint(&cursorX, &cursorY);
    mEasyPlanMeasureAngleGraphicObject->GetMeasureAngleActionStatus(&actionStarted, &firstSideDone, &secondSideDone);
    std::list<Mcsf::MedViewer3D::Point2D> anglePoints;// = new std::list<Mcsf::MedViewer3D::Point2D>();
    anglePoints.clear();
    mEasyPlanMeasureAngleGraphicObject->GetMeasureAnglePoints(anglePoints);
    RenderMeasureAngle(anglePoints, cursorX, cursorY, actionStarted, firstSideDone, secondSideDone);
    return;
}

void EasyPlanMeasureAngleStrategy::Initialize() {

}

void EasyPlanMeasureAngleStrategy::Finalize() {

}

void EasyPlanMeasureAngleStrategy::RenderMeasureAngle(const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints,
    const float cursorX, const float cursorY, 
    const bool actionStarted, const bool firstSideDone, const bool secondSideDone) {
    if(anglePoints.size() < 1) {
        TPS_LOG_DEV_WARNING<<"No angle points received when rendering MeasureAngle function";
        return;
    }
    if(actionStarted && !firstSideDone && !secondSideDone) {
        //Mcsf::MedViewer3D::Point2D* origin = &(anglePoints.front());
        Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
        //if(nullptr == origin) return;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
        glViewport(0, 0, mWidth, mHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glEnable(GL_STENCIL_TEST);
        glStencilMask(0x01);
        glStencilFunc(GL_ALWAYS, 0x01, 0x01);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINES);
        glVertex3f((float)origin.x * 2 - 1.0f, (float)origin.y * 2 - 1.0f, 0.0f);
        glVertex3f(cursorX * 2 - 1.0f, cursorY * 2 - 1.0f, 0.0f);

        glEnd();

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    else if(actionStarted && firstSideDone && !secondSideDone) {
        //Mcsf::MedViewer3D::Point2D* origin = &(anglePoints.front());
        Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
        if(anglePoints.size() != 2) return;
        //anglePoints.pop_front();
        //Mcsf::MedViewer3D::Point2D* firstVertex = &(anglePoints.front());
        Mcsf::MedViewer3D::Point2D firstVertex = anglePoints.back();
        //if(nullptr == firstVertex) return;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
        glViewport(0, 0, mWidth, mHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glEnable(GL_STENCIL_TEST);
        glStencilMask(0x01);
        glStencilFunc(GL_ALWAYS, 0x01, 0x01);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        // use vector to calculate angle between two sides
        float originX = (float)origin.x * 2 - 1.0f;
        float originY = (float)origin.y * 2 - 1.0f;
        float vertex1X = (float)firstVertex.x * 2 - 1.0f;
        float vertex1Y = (float)firstVertex.y * 2 - 1.0f;
        float vertex2X = cursorX * 2 - 1.0f;
        float vertex2Y = cursorY * 2 - 1.0f;
        glBegin(GL_LINES);
        glVertex3f(originX, originY, 0.0f);
        glVertex3f(vertex1X, vertex1Y, 0.0f);
        glVertex3f(originX, originY, 0.0f);
        glVertex3f(vertex2X, vertex2Y, 0.0f);
        glEnd();

        double side1Length = sqrt((vertex1X-originX)*(vertex1X-originX) + (vertex1Y - originY)*(vertex1Y - originY));
        double side2Length = sqrt((vertex2X-originX)*(vertex2X-originX) + (vertex2Y - originY)*(vertex2Y - originY));
        double arcRadius;
        if(side1Length < side2Length) {
            arcRadius = side1Length * 0.33;
        }
        else {
            arcRadius = side2Length * 0.33;
        }

        Mcsf::MedViewer3D::Point2D ptOrigin(originX,originY);
        Mcsf::MedViewer3D::Point2D ptFirst(vertex1X,vertex1Y);
        Mcsf::MedViewer3D::Point2D ptSecond(vertex2X,vertex2Y);

        Mcsf::MedViewer3D::Vector2D vecFirst = ptFirst - ptOrigin;
        Mcsf::MedViewer3D::Vector2D vecSecond = ptSecond - ptOrigin;

        double angleFirst = vecFirst.AngleBetween(Mcsf::MedViewer3D::Vector2D(1,0));
        double angleSecond = vecSecond.AngleBetween(Mcsf::MedViewer3D::Vector2D(1,0));

        if(vecFirst.y < 0) {
            angleFirst = 2* PI - angleFirst;
        }
        if(vecSecond.y < 0) {
            angleSecond = 2*PI - angleSecond;
        }

        const double minValue = 0.01;


        double deltaAngle = angleSecond - angleFirst;
        if(deltaAngle >= 0 && deltaAngle <= PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleFirst; angleTemp < angleSecond ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadius * cos(angleTemp),
                ptOrigin.y + arcRadius * sin(angleTemp),
                0.0);

            glEnd();

        }
        else if(deltaAngle > PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleSecond; angleTemp < angleFirst + 2*PI ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadius * cos(angleTemp),
                ptOrigin.y + arcRadius * sin(angleTemp),
                0.0);
            glEnd();
        }
        else if(deltaAngle < 0 && deltaAngle >= -PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleSecond; angleTemp < angleFirst ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadius * cos(angleTemp),
                ptOrigin.y + arcRadius * sin(angleTemp),
                0.0);

            glEnd();

        }
        else {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleFirst; angleTemp < angleSecond + 2*PI ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadius * cos(angleTemp),
                ptOrigin.y + arcRadius * sin(angleTemp),
                0.0);
            glEnd();
        }

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void EasyPlanMeasureAngleStrategy::SetEasyPlanMeasureAngleGraphicObject( 
    std::shared_ptr<EasyPlanMeasureGraphicObject> easyPlanMeasureAngleGo ) {

    if (mEasyPlanMeasureAngleGraphicObject != easyPlanMeasureAngleGo){
        mEasyPlanMeasureAngleGraphicObject = easyPlanMeasureAngleGo;
        mDirty = true;
    }
}

void EasyPlanMeasureAngleStrategy::Resize( int iWidth, int iHeight ){
    mHeight = iHeight;
    mWidth = iWidth;
}

int EasyPlanMeasureAngleStrategy::CacheChanges(){
    if(mEasyPlanMeasureAngleGraphicObject == nullptr) {
        return -1;
    }

    mDirty = mEasyPlanMeasureAngleGraphicObject->IsDirty();
    //mDirty |= m_spMPR->IsDirty();
    return 0;
}



TPS_END_NAMESPACE