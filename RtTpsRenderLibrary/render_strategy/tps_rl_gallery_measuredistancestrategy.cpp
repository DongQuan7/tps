//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan mailto:yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measuredistancestrategy.cpp
///  \brief   tps gallery measure distance strategy class
///
///  \version 1.0
///  \date    2015-04-23
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuredistancestrategy.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"

//MCSF
#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"



//OPENGL
#include "gl\glew.h"
#include "gl\GL.h"
#include "gl\GLU.h"
#include <cmath>

TPS_BEGIN_NAMESPACE

GalleryMeasureDistanceStrategy::GalleryMeasureDistanceStrategy()
{

}

GalleryMeasureDistanceStrategy::~GalleryMeasureDistanceStrategy()
{

}
void GalleryMeasureDistanceStrategy::Render_i()
{
    if(mGalleryMeasureDistanceGraphicObject == nullptr) {
        return;
    }
    bool actionStarted = false;
    bool actionDone = false;
    int hitIndex = -1;
    //float cursorX = 0.0f;
    //float cursorY = 0.0f;

    //mGalleryMeasureDistanceGraphicObject->GetMeasureAngleCursorPoint(&cursorX, &cursorY, mWidth, mHeight);
    mGalleryMeasureDistanceGraphicObject->GetMeasureDistanceActionStatus(&actionStarted, &actionDone);
    mGalleryMeasureDistanceGraphicObject->GetMeasureDistanceHitIndex(&hitIndex);

    std::list<Mcsf::MedViewer3D::Point2D> distancePoints;// = new std::list<Mcsf::MedViewer3D::Point2D>();
    distancePoints.clear();
    mGalleryMeasureDistanceGraphicObject->GetMeasureDistancePoints(distancePoints, mWidth, mHeight);

    std::vector<std::list<Mcsf::MedViewer3D::Point2D>> distancePointsList;
    distancePointsList.clear();
    mGalleryMeasureDistanceGraphicObject->GetMeasureDistancePointsList(distancePointsList, mWidth, mHeight);

    std::vector<bool> isOnPlaneResultList;
    isOnPlaneResultList.clear();
    mGalleryMeasureDistanceGraphicObject->GetMeasureDistanceIsOnPlaneResultList(isOnPlaneResultList);

    RenderMeasureDistance(distancePointsList, distancePoints, isOnPlaneResultList, hitIndex/*, actionStarted, actionDone*/);
    return;
}

void GalleryMeasureDistanceStrategy::Initialize()
{

}

void GalleryMeasureDistanceStrategy::Finalize()
{

}

void GalleryMeasureDistanceStrategy::RenderMeasureDistance(
    const std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& distancePointsList,
    const std::list<Mcsf::MedViewer3D::Point2D>& distancePoints,
    const std::vector<bool>& isOnPlaneResultList,
    const int hitIndex/*,
    const bool actionStarted, const bool actionDone*/)
{
    //draw distance points list
    if(distancePointsList.size() > 0 &&
       isOnPlaneResultList.size() > 0 &&
       distancePointsList.size() == isOnPlaneResultList.size())
    {
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
        //glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

        for (int i = 0; i < distancePointsList.size(); i++)
        {
            if(isOnPlaneResultList[i] && i != hitIndex)
            {
                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            }
            else if(isOnPlaneResultList[i] && i == hitIndex)
            {
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            }
            else
            {
                continue;
            }
            std::list<Mcsf::MedViewer3D::Point2D> tempAnglePoints = distancePointsList[i];
            if(tempAnglePoints.size() != 2) return;
            Mcsf::MedViewer3D::Point2D start = tempAnglePoints.front();
            Mcsf::MedViewer3D::Point2D stop = tempAnglePoints.back();

            // use vector to calculate angle between two sides
            float startX = (float)start.x * 2 - 1.0f;
            float startY = (float)start.y * 2 - 1.0f;
            float stopX = (float)stop.x * 2 - 1.0f;
            float stopY = (float)stop.y * 2 - 1.0f;
            glBegin(GL_LINES);
            glVertex3f(startX, startY, 0.0f);
            glVertex3f(stopX, stopY, 0.0f);
            glEnd();
            
        }

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    //draw distance points
    if(distancePoints.size() != 2)
    {
        TPS_LOG_DEV_WARNING<<"No distance points received when rendering MeasureDistance function";
        return;
    }
    else
    {
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
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

        Mcsf::MedViewer3D::Point2D start = distancePoints.front();
        Mcsf::MedViewer3D::Point2D stop = distancePoints.back();

        // use vector to calculate angle between two sides
        float startX = (float)start.x * 2 - 1.0f;
        float startY = (float)start.y * 2 - 1.0f;
        float stopX = (float)stop.x * 2 - 1.0f;
        float stopY = (float)stop.y * 2 - 1.0f;
        glBegin(GL_LINES);
        glVertex3f(startX, startY, 0.0f);
        glVertex3f(stopX, stopY, 0.0f);
        glEnd();

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    //if(actionStarted && !firstSideDone && !secondSideDone)
    //{
    //    Mcsf::MedViewer3D::Point2D origin = anglePoints.front();

    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    //    glViewport(0, 0, mWidth, mHeight);

    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();

    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();


    //    glEnable(GL_STENCIL_TEST);
    //    glStencilMask(0x01);
    //    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    //    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    //    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    //    glBegin(GL_LINES);
    //    glVertex3f((float)origin.x * 2 - 1.0f, (float)origin.y * 2 - 1.0f, 0.0f);
    //    glVertex3f(cursorX * 2 - 1.0f, cursorY * 2 - 1.0f, 0.0f);

    //    glEnd();

    //    glDisable(GL_STENCIL_TEST);
    //    glFlush();
    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //}
    //else if(actionStarted && firstSideDone && !secondSideDone)
    //{
    //    if(anglePoints.size() != 2) return;
    //    Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
    //    Mcsf::MedViewer3D::Point2D firstVertex = anglePoints.back();

    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    //    glViewport(0, 0, mWidth, mHeight);

    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();

    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();


    //    glEnable(GL_STENCIL_TEST);
    //    glStencilMask(0x01);
    //    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    //    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    //    // use vector to calculate angle between two sides
    //    float originX = (float)origin.x * 2 - 1.0f;
    //    float originY = (float)origin.y * 2 - 1.0f;
    //    float vertex1X = (float)firstVertex.x * 2 - 1.0f;
    //    float vertex1Y = (float)firstVertex.y * 2 - 1.0f;
    //    float vertex2X = cursorX * 2 - 1.0f;
    //    float vertex2Y = cursorY * 2 - 1.0f;
    //    glBegin(GL_LINES);
    //    glVertex3f(originX, originY, 0.0f);
    //    glVertex3f(vertex1X, vertex1Y, 0.0f);
    //    glVertex3f(originX, originY, 0.0f);
    //    glVertex3f(vertex2X, vertex2Y, 0.0f);
    //    glEnd();

    //    double side1Length = sqrt((vertex1X-originX)*(vertex1X-originX) + (vertex1Y - originY)*(vertex1Y - originY));
    //    double side2Length = sqrt((vertex2X-originX)*(vertex2X-originX) + (vertex2Y - originY)*(vertex2Y - originY));
    //    double arcRadiusA, arcRadiusB;
    //    //double ratio = (double)mWidth / (double)mHeight;
    //    if(side1Length < side2Length)
    //    {
    //        arcRadiusA = side1Length * 0.33;
    //        arcRadiusB = side1Length * 0.33/* * ratio*/;
    //    }
    //    else
    //    {
    //        arcRadiusA = side2Length * 0.33;
    //        arcRadiusB = side2Length * 0.33/* * ratio*/;
    //    }

    //    //originY *= ratio;
    //    //vertex1Y *= ratio;
    //    //vertex2Y *= ratio;

    //    Mcsf::MedViewer3D::Point2D ptOrigin(originX,originY);
    //    Mcsf::MedViewer3D::Point2D ptFirst(vertex1X,vertex1Y);
    //    Mcsf::MedViewer3D::Point2D ptSecond(vertex2X,vertex2Y);

    //    Mcsf::MedViewer3D::Vector2D vecFirst = ptFirst - ptOrigin;
    //    Mcsf::MedViewer3D::Vector2D vecSecond = ptSecond - ptOrigin;

    //    double angleFirst = vecFirst.AngleBetween(Mcsf::MedViewer3D::Vector2D(1,0));
    //    double angleSecond = vecSecond.AngleBetween(Mcsf::MedViewer3D::Vector2D(1,0));

    //    if(vecFirst.y < 0) {
    //        angleFirst = 2* PI - angleFirst;
    //    }
    //    if(vecSecond.y < 0) {
    //        angleSecond = 2*PI - angleSecond;
    //    }

    //    const double minValue = 0.01;


    //    double deltaAngle = angleSecond - angleFirst;
    //    if(deltaAngle >= 0 && deltaAngle <= PI) {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleFirst; angleTemp < angleSecond ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);

    //        glEnd();

    //    }
    //    else if(deltaAngle > PI) {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleSecond; angleTemp < angleFirst + 2*PI ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);
    //        glEnd();
    //    }
    //    else if(deltaAngle < 0 && deltaAngle >= -PI) {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleSecond; angleTemp < angleFirst ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);

    //        glEnd();

    //    }
    //    else {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleFirst; angleTemp < angleSecond + 2*PI ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);
    //        glEnd();
    //    }

    //    glDisable(GL_STENCIL_TEST);
    //    glFlush();        
    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //}
    //else if(actionStarted && firstSideDone && secondSideDone)
    //{
    //    if(anglePoints.size() != 3) return;
    //    Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
    //    std::list<Mcsf::MedViewer3D::Point2D>::const_iterator i = anglePoints.begin();
    //    Mcsf::MedViewer3D::Point2D firstVertex = *(++i);
    //    Mcsf::MedViewer3D::Point2D secondVertex = anglePoints.back();
    //    //if(nullptr == firstVertex) return;
    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    //    glViewport(0, 0, mWidth, mHeight);

    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();

    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();


    //    glEnable(GL_STENCIL_TEST);
    //    glStencilMask(0x01);
    //    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    //    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    //    // use vector to calculate angle between two sides
    //    float originX = (float)origin.x * 2 - 1.0f;
    //    float originY = (float)origin.y * 2 - 1.0f;
    //    float vertex1X = (float)firstVertex.x * 2 - 1.0f;
    //    float vertex1Y = (float)firstVertex.y * 2 - 1.0f;
    //    float vertex2X = (float)secondVertex.x * 2 - 1.0f;
    //    float vertex2Y = (float)secondVertex.y * 2 - 1.0f;
    //    glBegin(GL_LINES);
    //    glVertex3f(originX, originY, 0.0f);
    //    glVertex3f(vertex1X, vertex1Y, 0.0f);
    //    glVertex3f(originX, originY, 0.0f);
    //    glVertex3f(vertex2X, vertex2Y, 0.0f);
    //    glEnd();

    //    double side1Length = sqrt((vertex1X-originX)*(vertex1X-originX) + (vertex1Y - originY)*(vertex1Y - originY));
    //    double side2Length = sqrt((vertex2X-originX)*(vertex2X-originX) + (vertex2Y - originY)*(vertex2Y - originY));
    //    double arcRadiusA, arcRadiusB;
    //    //double ratio = mWidth / mHeight;
    //    if(side1Length < side2Length)
    //    {
    //        arcRadiusA = side1Length * 0.33;
    //        arcRadiusB = side1Length * 0.33 /** ratio*/;
    //    }
    //    else
    //    {
    //        arcRadiusA = side2Length * 0.33;
    //        arcRadiusB = side2Length * 0.33 /** ratio*/;
    //    }

    //    //originY *= ratio;
    //    //vertex1Y *= ratio;
    //    //vertex2Y *= ratio;

    //    Mcsf::MedViewer3D::Point2D ptOrigin(originX,originY);
    //    Mcsf::MedViewer3D::Point2D ptFirst(vertex1X,vertex1Y);
    //    Mcsf::MedViewer3D::Point2D ptSecond(vertex2X,vertex2Y);

    //    Mcsf::MedViewer3D::Vector2D vecFirst = ptFirst - ptOrigin;
    //    Mcsf::MedViewer3D::Vector2D vecSecond = ptSecond - ptOrigin;

    //    double angleFirst = vecFirst.AngleBetween(Mcsf::MedViewer3D::Vector2D(1,0));
    //    double angleSecond = vecSecond.AngleBetween(Mcsf::MedViewer3D::Vector2D(1,0));

    //    if(vecFirst.y < 0) {
    //        angleFirst = 2* PI - angleFirst;
    //    }
    //    if(vecSecond.y < 0) {
    //        angleSecond = 2*PI - angleSecond;
    //    }

    //    const double minValue = 0.01;


    //    double deltaAngle = angleSecond - angleFirst;
    //    if(deltaAngle >= 0 && deltaAngle <= PI) {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleFirst; angleTemp < angleSecond ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);

    //        glEnd();

    //    }
    //    else if(deltaAngle > PI) {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleSecond; angleTemp < angleFirst + 2*PI ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);
    //        glEnd();
    //    }
    //    else if(deltaAngle < 0 && deltaAngle >= -PI) {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleSecond; angleTemp < angleFirst ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);

    //        glEnd();

    //    }
    //    else {
    //        glBegin(GL_LINE_STRIP);
    //        for(double angleTemp = angleFirst; angleTemp < angleSecond + 2*PI ; angleTemp += minValue)
    //            glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
    //            ptOrigin.y + arcRadiusB * sin(angleTemp),
    //            0.0);
    //        glEnd();
    //    }

    //    glDisable(GL_STENCIL_TEST);
    //    glFlush();
    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //}
}

void GalleryMeasureDistanceStrategy::SetGalleryMeasureDistanceGraphicObject( 
    std::shared_ptr<GalleryMeasureGraphicObject> galleryMeasureDistanceGo )
{
    if (mGalleryMeasureDistanceGraphicObject != galleryMeasureDistanceGo){
        mGalleryMeasureDistanceGraphicObject = galleryMeasureDistanceGo;
        mDirty = true;
    }
}

void GalleryMeasureDistanceStrategy::Resize( int iWidth, int iHeight ){
    mHeight = iHeight;
    mWidth = iWidth;
}

int GalleryMeasureDistanceStrategy::CacheChanges(){
    if(mGalleryMeasureDistanceGraphicObject == nullptr) {
        return -1;
    }

    mDirty = mGalleryMeasureDistanceGraphicObject->IsDirty();
    //mDirty |= m_spMPR->IsDirty();
    return 0;
}

TPS_END_NAMESPACE