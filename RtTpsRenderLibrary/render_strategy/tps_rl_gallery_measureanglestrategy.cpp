//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan mailto:yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measureanglestrategy.cpp
///  \brief   tps gallery measure angle strategy class
///
///  \version 1.0
///  \date    2015-03-09
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measureanglestrategy.h"
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

GalleryMeasureAngleStrategy::GalleryMeasureAngleStrategy()
{

}

GalleryMeasureAngleStrategy::~GalleryMeasureAngleStrategy()
{

}
void GalleryMeasureAngleStrategy::Render_i()
{
    if(mGalleryMeasureAngleGraphicObject == nullptr) {
        return;
    }
    bool actionStarted = false;
    bool firstSideDone = false;
    bool secondSideDone = false;
    float cursorX = 0.0f;
    float cursorY = 0.0f;

    mGalleryMeasureAngleGraphicObject->GetMeasureAngleCursorPoint(&cursorX, &cursorY, mWidth, mHeight);
    mGalleryMeasureAngleGraphicObject->GetMeasureAngleActionStatus(&actionStarted, &firstSideDone, &secondSideDone);

    std::list<Mcsf::MedViewer3D::Point2D> anglePoints;// = new std::list<Mcsf::MedViewer3D::Point2D>();
    anglePoints.clear();
    mGalleryMeasureAngleGraphicObject->GetMeasureAnglePoints(anglePoints, mWidth, mHeight);

    std::vector<std::list<Mcsf::MedViewer3D::Point2D>> anglePointsList;
    anglePointsList.clear();
    mGalleryMeasureAngleGraphicObject->GetMeasureAnglePointsList(anglePointsList, mWidth, mHeight);

    std::vector<bool> isOnPlaneResultList;
    isOnPlaneResultList.clear();
    mGalleryMeasureAngleGraphicObject->GetMeasureAngleIsOnPlaneResultList(isOnPlaneResultList);

    //// image coordinate to cell coordinate
    //std::list<Mcsf::MedViewer3D::Point2D> anglePointsInScreenCoordinate;
    //for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = anglePoints.begin(); iter != anglePoints.end(); iter++)
    //{
    //    Mcsf::MedViewer3D::Point2D anglePointNew = 
    //        ImageCoordinateToScreenCoordinate(*iter, mWidth, mHeight);
    //    anglePointsInScreenCoordinate.push_back(anglePointNew);
    //}

    //Mcsf::MedViewer3D::Point2D cursor(cursorX, cursorY);
    //Mcsf::MedViewer3D::Point2D cursorInScreenCoordinate = 
    //    ImageCoordinateToScreenCoordinate(cursor, mWidth, mHeight);

    RenderMeasureAngle(anglePointsList, anglePoints, isOnPlaneResultList, cursorX, cursorY, actionStarted, firstSideDone, secondSideDone);
    //RenderMeasureAngle(anglePointsInScreenCoordinate, cursorInScreenCoordinate.x, cursorInScreenCoordinate.y, actionStarted, firstSideDone, secondSideDone);
    return;
}

void GalleryMeasureAngleStrategy::Initialize()
{

}

void GalleryMeasureAngleStrategy::Finalize()
{

}

void GalleryMeasureAngleStrategy::RenderMeasureAngle(
    const std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& anglePointsList,
    const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints,
    const std::vector<bool>& isOnPlaneResultList,
    const float cursorX, const float cursorY, 
    const bool actionStarted, const bool firstSideDone, const bool secondSideDone)
{
    //draw angle points list
    if(anglePointsList.size() > 0 &&
       isOnPlaneResultList.size() > 0 &&
       anglePointsList.size() == isOnPlaneResultList.size())
    {

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

        for (int i = 0; i < anglePointsList.size(); i++)
        {
            if(isOnPlaneResultList[i])
            {
                std::list<Mcsf::MedViewer3D::Point2D> tempAnglePoints = anglePointsList[i];
                if(tempAnglePoints.size() != 3) return;
                Mcsf::MedViewer3D::Point2D origin = tempAnglePoints.front();
                std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = tempAnglePoints.begin();
                Mcsf::MedViewer3D::Point2D firstVertex = *(++iter);
                Mcsf::MedViewer3D::Point2D secondVertex = tempAnglePoints.back();

                // use vector to calculate angle between two sides
                float originX = (float)origin.x * 2 - 1.0f;
                float originY = (float)origin.y * 2 - 1.0f;
                float vertex1X = (float)firstVertex.x * 2 - 1.0f;
                float vertex1Y = (float)firstVertex.y * 2 - 1.0f;
                float vertex2X = (float)secondVertex.x * 2 - 1.0f;
                float vertex2Y = (float)secondVertex.y * 2 - 1.0f;
                glBegin(GL_LINES);
                glVertex3f(originX, originY, 0.0f);
                glVertex3f(vertex1X, vertex1Y, 0.0f);
                glVertex3f(originX, originY, 0.0f);
                glVertex3f(vertex2X, vertex2Y, 0.0f);
                glEnd();

                double side1Length = sqrt((vertex1X-originX)*(vertex1X-originX) + (vertex1Y - originY)*(vertex1Y - originY));
                double side2Length = sqrt((vertex2X-originX)*(vertex2X-originX) + (vertex2Y - originY)*(vertex2Y - originY));
                double arcRadiusA, arcRadiusB;
                //double ratio = mWidth / mHeight;
                if(side1Length < side2Length)
                {
                    arcRadiusA = side1Length * 0.33;
                    arcRadiusB = side1Length * 0.33 /** ratio*/;
                }
                else
                {
                    arcRadiusA = side2Length * 0.33;
                    arcRadiusB = side2Length * 0.33 /** ratio*/;
                }

                //originY *= ratio;
                //vertex1Y *= ratio;
                //vertex2Y *= ratio;

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
                        glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                        ptOrigin.y + arcRadiusB * sin(angleTemp),
                        0.0);

                    glEnd();

                }
                else if(deltaAngle > PI) {
                    glBegin(GL_LINE_STRIP);
                    for(double angleTemp = angleSecond; angleTemp < angleFirst + 2*PI ; angleTemp += minValue)
                        glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                        ptOrigin.y + arcRadiusB * sin(angleTemp),
                        0.0);
                    glEnd();
                }
                else if(deltaAngle < 0 && deltaAngle >= -PI) {
                    glBegin(GL_LINE_STRIP);
                    for(double angleTemp = angleSecond; angleTemp < angleFirst ; angleTemp += minValue)
                        glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                        ptOrigin.y + arcRadiusB * sin(angleTemp),
                        0.0);

                    glEnd();

                }
                else {
                    glBegin(GL_LINE_STRIP);
                    for(double angleTemp = angleFirst; angleTemp < angleSecond + 2*PI ; angleTemp += minValue)
                        glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                        ptOrigin.y + arcRadiusB * sin(angleTemp),
                        0.0);
                    glEnd();
                }
            }
        }

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    //draw angle points
    if(anglePoints.size() < 1)
    {
        TPS_LOG_DEV_WARNING<<"No angle points received when rendering MeasureAngle function";
        return;
    }
    if(actionStarted && !firstSideDone && !secondSideDone)
    {
        Mcsf::MedViewer3D::Point2D origin = anglePoints.front();

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
    else if(actionStarted && firstSideDone && !secondSideDone)
    {
        if(anglePoints.size() != 2) return;
        Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
        Mcsf::MedViewer3D::Point2D firstVertex = anglePoints.back();

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
        double arcRadiusA, arcRadiusB;
        //double ratio = (double)mWidth / (double)mHeight;
        if(side1Length < side2Length)
        {
            arcRadiusA = side1Length * 0.33;
            arcRadiusB = side1Length * 0.33/* * ratio*/;
        }
        else
        {
            arcRadiusA = side2Length * 0.33;
            arcRadiusB = side2Length * 0.33/* * ratio*/;
        }

        //originY *= ratio;
        //vertex1Y *= ratio;
        //vertex2Y *= ratio;

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
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);

            glEnd();

        }
        else if(deltaAngle > PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleSecond; angleTemp < angleFirst + 2*PI ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);
            glEnd();
        }
        else if(deltaAngle < 0 && deltaAngle >= -PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleSecond; angleTemp < angleFirst ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);

            glEnd();

        }
        else {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleFirst; angleTemp < angleSecond + 2*PI ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);
            glEnd();
        }

        glDisable(GL_STENCIL_TEST);
        glFlush();        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    else if(actionStarted && firstSideDone && secondSideDone)
    {
        if(anglePoints.size() != 3) return;
        Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
        std::list<Mcsf::MedViewer3D::Point2D>::const_iterator i = anglePoints.begin();
        Mcsf::MedViewer3D::Point2D firstVertex = *(++i);
        Mcsf::MedViewer3D::Point2D secondVertex = anglePoints.back();
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
        float vertex2X = (float)secondVertex.x * 2 - 1.0f;
        float vertex2Y = (float)secondVertex.y * 2 - 1.0f;
        glBegin(GL_LINES);
        glVertex3f(originX, originY, 0.0f);
        glVertex3f(vertex1X, vertex1Y, 0.0f);
        glVertex3f(originX, originY, 0.0f);
        glVertex3f(vertex2X, vertex2Y, 0.0f);
        glEnd();

        double side1Length = sqrt((vertex1X-originX)*(vertex1X-originX) + (vertex1Y - originY)*(vertex1Y - originY));
        double side2Length = sqrt((vertex2X-originX)*(vertex2X-originX) + (vertex2Y - originY)*(vertex2Y - originY));
        double arcRadiusA, arcRadiusB;
        //double ratio = mWidth / mHeight;
        if(side1Length < side2Length)
        {
            arcRadiusA = side1Length * 0.33;
            arcRadiusB = side1Length * 0.33 /** ratio*/;
        }
        else
        {
            arcRadiusA = side2Length * 0.33;
            arcRadiusB = side2Length * 0.33 /** ratio*/;
        }

        //originY *= ratio;
        //vertex1Y *= ratio;
        //vertex2Y *= ratio;

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
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);

            glEnd();

        }
        else if(deltaAngle > PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleSecond; angleTemp < angleFirst + 2*PI ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);
            glEnd();
        }
        else if(deltaAngle < 0 && deltaAngle >= -PI) {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleSecond; angleTemp < angleFirst ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);

            glEnd();

        }
        else {
            glBegin(GL_LINE_STRIP);
            for(double angleTemp = angleFirst; angleTemp < angleSecond + 2*PI ; angleTemp += minValue)
                glVertex3d(ptOrigin.x + arcRadiusA * cos(angleTemp),
                ptOrigin.y + arcRadiusB * sin(angleTemp),
                0.0);
            glEnd();
        }

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void GalleryMeasureAngleStrategy::SetGalleryMeasureAngleGraphicObject( 
    std::shared_ptr<GalleryMeasureGraphicObject> galleryMeasureAngleGo )
{
    if (mGalleryMeasureAngleGraphicObject != galleryMeasureAngleGo){
        mGalleryMeasureAngleGraphicObject = galleryMeasureAngleGo;
        mDirty = true;
    }
}

//Mcsf::MedViewer3D::Point2D GalleryMeasureAngleStrategy::ImageCoordinateToScreenCoordinate
//    (Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight)
//{
//    int iMinL = std::min(iWidth, iHeight);
//
//    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
//    anglePointOut.x = (anglePoint.x / 2) * iMinL / iWidth + 0.5;
//    anglePointOut.y = (anglePoint.y / 2) * iMinL / iHeight + 0.5;
//
//    return anglePointOut;
//
//}

void GalleryMeasureAngleStrategy::Resize( int iWidth, int iHeight ){
    mHeight = iHeight;
    mWidth = iWidth;
}

int GalleryMeasureAngleStrategy::CacheChanges(){
    if(mGalleryMeasureAngleGraphicObject == nullptr) {
        return -1;
    }

    mDirty = mGalleryMeasureAngleGraphicObject->IsDirty();
    //mDirty |= m_spMPR->IsDirty();
    return 0;
}



TPS_END_NAMESPACE