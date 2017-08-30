////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_crosshair_renderstrategy.cpp
/// 
///  \brief class CrosshairRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_crosshair_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"

TPS_BEGIN_NAMESPACE

CrosshairRenderStrategy::CrosshairRenderStrategy()
    : m_spCrosshairGO(nullptr)
{
    mDirty = true;
}

CrosshairRenderStrategy::CrosshairRenderStrategy(std::shared_ptr<CrosshairGraphicObject> spCrosshairGO)
    :m_spCrosshairGO(spCrosshairGO) {

}

void CrosshairRenderStrategy::Initialize() {
    mDirty = true;
}

CrosshairRenderStrategy::~CrosshairRenderStrategy() {

}

void CrosshairRenderStrategy::SetName(std::string& strName) {
    m_strName = strName;
}

void CrosshairRenderStrategy::SetCrosshairGraphicObject(std::shared_ptr<CrosshairGraphicObject> spCrosshairGO) {
    if (m_spCrosshairGO != spCrosshairGO){
        m_spCrosshairGO = spCrosshairGO;
        mDirty = true;
    }
}

static void DrawArrow(float fCenterX, float fCenterY, float fAxisAngle, float fArrowLen, float fArrowEdge) {

    float fAngle = fAxisAngle * PI / 180.0f;
    float fCos = cos(fAngle);
    float fSin = sin(fAngle);

    float fA1x = fCenterX - fArrowLen * fSin;
    float fA1y = fCenterY + fArrowLen * fCos;
    float fA2x = fCenterX + fArrowLen * fSin;
    float fA2y = fCenterY - fArrowLen * fCos;

    glBegin(GL_LINES);
    glVertex2f(fA1x, fA1y);
    glVertex2f(fA2x, fA2y);
    glEnd();

    glBegin(GL_LINES);
    fAngle = (fAxisAngle - 60) * PI / 180;
    glVertex2f(fA1x, fA1y);
    glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

    fAngle = (fAxisAngle - 120) * PI / 180;
    glVertex2f(fA1x, fA1y);
    glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

    fAngle = (fAxisAngle + 60) * PI / 180;
    glVertex2f(fA2x, fA2y);
    glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));

    fAngle = (fAxisAngle + 120) * PI / 180;
    glVertex2f(fA2x, fA2y);
    glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));
    glEnd();

}

void CrosshairRenderStrategy::Render_i() {
    float fAngleX, fAngleY;
    float fAngle, fCos, fSin;

    if (m_spCrosshairGO == nullptr) {
        return;
    }
    if (!m_spCrosshairGO->Visible() || !m_spCrosshairGO->IsReady()) {
        return;
    }
    //printf("\n%s\n", m_strName.c_str());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    mWidth = viewport[2];
    mHeight = viewport[3];

    float fScale = 1.0f;
    if (mWidth >= mHeight)
    {
        glViewport(0, -(mWidth - mHeight)/2, mWidth, mWidth);
        fScale = (float)mHeight / (float)mWidth;
    }
    else
    {
        glViewport(-(mHeight - mWidth)/2, 0, mHeight, mHeight);
        fScale =  (float)mWidth / (float)mHeight;
    }
    //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glFlush();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 2.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(fScale, fScale, 1.0f);
    //gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    m_spCrosshairGO->GetAngles(&fAngleX, &fAngleY);
    Mcsf::MedViewer3D::Point2D pt2dCrosshairInPlane;
    m_spCrosshairGO->GetCrosshairPoint2d(&pt2dCrosshairInPlane);
    pt2dCrosshairInPlane.x = (pt2dCrosshairInPlane.x - 0.5f) * 2;
    pt2dCrosshairInPlane.y = (pt2dCrosshairInPlane.y - 0.5f) * 2;
    int nHitTest = m_spCrosshairGO->IsHit();
    bool bRotate = (nHitTest & 0x4) != 0;
    bool bXSelected = (nHitTest & 0x1) != 0;
    bool bYSelected = (nHitTest & 0x2) != 0;

    float fRed, fGreen, fBlue, fAlpha;

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // x axis
    fAngle = fAngleX * PI / 180;
    fCos = cos(fAngle);
    fSin = sin(fAngle);
    m_spCrosshairGO->GetXColor(&fRed, &fGreen, &fBlue, &fAlpha);
    glLineWidth(1.0);

    glBegin(GL_LINES);
    glColor4f(fRed, fGreen, fBlue, fAlpha);
    glVertex2f(pt2dCrosshairInPlane.x - 4.0f * fCos, pt2dCrosshairInPlane.y - 4.0f * fSin);
    glVertex2f(pt2dCrosshairInPlane.x + 4.0f * fCos, pt2dCrosshairInPlane.y + 4.0f * fSin);
    glEnd();

    float fArrowLen = 0.04f;
    float fArrowEdge = 0.02f;
    if (bXSelected && !bRotate){
        DrawArrow(pt2dCrosshairInPlane.x - 0.5 * fCos, pt2dCrosshairInPlane.y - 0.5 * fSin, fAngleX, fArrowLen, fArrowEdge);
        DrawArrow(pt2dCrosshairInPlane.x + 0.5 * fCos, pt2dCrosshairInPlane.y + 0.5 * fSin, fAngleX, fArrowLen, fArrowEdge);
        ////Left arrow
        //float fA1x = pt2dCrosshairInPlane.x - 0.5 * fCos - fArrowLen * fSin;
        //float fA1y = pt2dCrosshairInPlane.y - 0.5 * fSin + fArrowLen * fCos;
        //float fA2x = pt2dCrosshairInPlane.x - 0.5 * fCos + fArrowLen * fSin;
        //float fA2y = pt2dCrosshairInPlane.y - 0.5 * fSin - fArrowLen * fCos;

        //glBegin(GL_LINES);
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA2x, fA2y);
        //glEnd();

        ////glBegin(GL_TRIANGLES);
        //glBegin(GL_LINES);
        //fAngle = (fAngleX - 60) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleX - 120) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));


        //fAngle = (fAngleX + 60) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleX + 120) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));
        //glEnd();

        ////Right arrow
        //fA1x = pt2dCrosshairInPlane.x + 0.5 * fCos - fArrowLen * fSin;
        //fA1y = pt2dCrosshairInPlane.y + 0.5 * fSin + fArrowLen * fCos;
        //fA2x = pt2dCrosshairInPlane.x + 0.5 * fCos + fArrowLen * fSin;
        //fA2y = pt2dCrosshairInPlane.y + 0.5 * fSin - fArrowLen * fCos;

        //glBegin(GL_LINES);
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA2x, fA2y);

        //fAngle = (fAngleX - 60) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleX - 120) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));


        //fAngle = (fAngleX + 60) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleX + 120) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));
        //glEnd();
    }

    // y axis
    fAngle = fAngleY * PI / 180;
    fCos = cos(fAngle);
    fSin = sin(fAngle);
    m_spCrosshairGO->GetYColor(&fRed, &fGreen, &fBlue, &fAlpha);

    glBegin(GL_LINES);
    glColor4f(fRed, fGreen, fBlue, fAlpha);
    glVertex2f(pt2dCrosshairInPlane.x - 4.0f * fCos, pt2dCrosshairInPlane.y - 4.0f * fSin);
    glVertex2f(pt2dCrosshairInPlane.x + 4.0f * fCos, pt2dCrosshairInPlane.y + 4.0f * fSin);
    glEnd();

    if (bYSelected && !bRotate)
    {
        DrawArrow(pt2dCrosshairInPlane.x - 0.5 * fCos, pt2dCrosshairInPlane.y - 0.5 * fSin, fAngleY, fArrowLen, fArrowEdge);
        DrawArrow(pt2dCrosshairInPlane.x + 0.5 * fCos, pt2dCrosshairInPlane.y + 0.5 * fSin, fAngleY, fArrowLen, fArrowEdge);
        //// Bottom arrow
        //float fA1x = pt2dCrosshairInPlane.x - 0.5 * fCos - fArrowLen * fSin;
        //float fA1y = pt2dCrosshairInPlane.y - 0.5 * fSin + fArrowLen * fCos;
        //float fA2x = pt2dCrosshairInPlane.x - 0.5 * fCos + fArrowLen * fSin;
        //float fA2y = pt2dCrosshairInPlane.y - 0.5 * fSin - fArrowLen * fCos;

        //glBegin(GL_LINES);
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA2x, fA2y);

        //fAngle = (fAngleY - 60) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleY - 120) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleY + 60) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleY + 120) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));

        //// Top arraw
        //fA1x = pt2dCrosshairInPlane.x + 0.5 * fCos - fArrowLen * fSin;
        //fA1y = pt2dCrosshairInPlane.y + 0.5 * fSin + fArrowLen * fCos;
        //fA2x = pt2dCrosshairInPlane.x + 0.5 * fCos + fArrowLen * fSin;
        //fA2y = pt2dCrosshairInPlane.y + 0.5 * fSin - fArrowLen * fCos;

        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA2x, fA2y);

        //fAngle = (fAngleY - 60) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleY - 120) * PI / 180;
        //glVertex2f(fA1x, fA1y);
        //glVertex2f(fA1x + fArrowEdge * cos(fAngle), fA1y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleY + 60) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));

        //fAngle = (fAngleY + 120) * PI / 180;
        //glVertex2f(fA2x, fA2y);
        //glVertex2f(fA2x + fArrowEdge * cos(fAngle), fA2y + fArrowEdge * sin(fAngle));
        //glEnd();
    }

    glFlush();
    glViewport(0, 0, mWidth, mHeight);

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //printf("render finished!\n");
}

void CrosshairRenderStrategy::Resize(int w, int h) {
    mHeight = h;
    mWidth = w;
    mDirty = true;
}

void CrosshairRenderStrategy::Finalize() {

}

int CrosshairRenderStrategy::CacheChanges() {
    //m_bDirty = true;
    if (m_spCrosshairGO == nullptr) {
        return -1;
    }

    if (m_spCrosshairGO->IsDirty()){
        mDirty = true;
    }
    return 0;
}
TPS_END_NAMESPACE
