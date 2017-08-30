////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_crosshair_graphicobject.cpp
/// 
///  \brief class CrosshairGraphicObejct implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"

TPS_BEGIN_NAMESPACE

const float CrosshairGraphicObject::HIT_TEST_MARGIN = 0.015f;
const float CrosshairGraphicObject::OBLIQUE_ROTATION_REGION = 0.1f;

CrosshairGraphicObject::CrosshairGraphicObject() :
    m_pt3dCrosshair(0.0, 0.0, 0.0),
    m_pt2dCrosshair(0.5, 0.5),
    m_nCrosshairMode(CROSSHAIR_ORTHOGONAL),
    m_fAngleX(0.0), m_fAngleY(90.0),
    m_nHitXAxis(0), m_nHitYAxis(0),
    m_nRotate(0),
    m_bVisible(true),
    m_bReady(false){

}

CrosshairGraphicObject::~CrosshairGraphicObject() {

}

bool CrosshairGraphicObject::Initialize() {
    return true;
}

bool CrosshairGraphicObject::Finalize() {
    return true;
}

void CrosshairGraphicObject::GetCrosshairPoint3d(Mcsf::MedViewer3D::Point3D* pCrosshairPt3d) const {
    *pCrosshairPt3d = m_pt3dCrosshair;
}

void CrosshairGraphicObject::GetCrossFourPoints(Mcsf::MedViewer3D::Point3D* point1, Mcsf::MedViewer3D::Point3D* point2, Mcsf::MedViewer3D::Point3D* point3, Mcsf::MedViewer3D::Point3D* point4) const {
    *point1 = m_topPoint;
    *point2 = m_bottomPoint;
    *point3 = m_leftPoint;
    *point4 = m_rightPoint;
}

void CrosshairGraphicObject::GetCrosshairPoint2d(Mcsf::MedViewer3D::Point2D* pCrosshairPt2d) const {
    *pCrosshairPt2d = m_pt2dCrosshair;
}

void CrosshairGraphicObject::GetAngles(float *pAngleX, float *pAngleY) const
{
    if (pAngleX)
        *pAngleX = m_fAngleX;
    if(pAngleY)
        *pAngleY = m_fAngleY;
}

void CrosshairGraphicObject::GetXColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha) const{
    if(pRed)    *pRed = m_vecXColor[0];
    if(pGreen)  *pGreen = m_vecXColor[1];
    if(pBlue)   *pBlue = m_vecXColor[2];
    if(pAlpha)  *pAlpha = m_vecXColor[3];
}

void CrosshairGraphicObject::GetYColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha) const{
    if(pRed)    *pRed = m_vecYColor[0];
    if(pGreen)  *pGreen = m_vecYColor[1];
    if(pBlue)   *pBlue = m_vecYColor[2];
    if(pAlpha)  *pAlpha = m_vecYColor[3];
}

void CrosshairGraphicObject::SetCrosshairPoint3d(Mcsf::MedViewer3D::Point3D& pt3dCrosshair) {
    m_pt3dCrosshair = pt3dCrosshair;
}

void CrosshairGraphicObject::SetCrossFourPoints(Mcsf::MedViewer3D::Point3D& point1, Mcsf::MedViewer3D::Point3D& point2, Mcsf::MedViewer3D::Point3D& point3, Mcsf::MedViewer3D::Point3D& point4){
    m_topPoint = point1;
    m_bottomPoint = point2;
    m_leftPoint = point3;
    m_rightPoint = point4;
}

void CrosshairGraphicObject::SetCrosshairPoint2d(Mcsf::MedViewer3D::Point2D& pt2dCrosshair) {
    m_pt2dCrosshair = pt2dCrosshair;
}

void CrosshairGraphicObject::SetAngles(float fAngleX, float fAngleY) {
    m_fAngleX = fAngleX;
    m_fAngleY = fAngleY;
}

void CrosshairGraphicObject::SetXColor(float fRed, float fGreen, float fBlue, float fAlpha) {
    m_vecXColor[0] = fRed;
    m_vecXColor[1] = fGreen;
    m_vecXColor[2] = fBlue;
    m_vecXColor[3] = fAlpha;
}

void CrosshairGraphicObject::SetYColor(float fRed, float fGreen, float fBlue, float fAlpha) {
    m_vecYColor[0] = fRed;
    m_vecYColor[1] = fGreen;
    m_vecYColor[2] = fBlue;
    m_vecYColor[3] = fAlpha;
}

int CrosshairGraphicObject::HitTest(float x, float y) {
    if (!m_bVisible) {
        m_nHitXAxis = 0;
        m_nHitXAxis = 0;
        m_nRotate = 0;
        return 0;
    }
    float x1, y1;
    float x2, y2;
    float d1, d2;

    x1 = x - m_pt2dCrosshair.x;
    y1 = y - m_pt2dCrosshair.y;
    d1 = x1 * x1 + y1 * y1;

    x2 = cos(m_fAngleX * PI / 180.0);
    y2 = sin(m_fAngleX * PI / 180.0);
    d2 = fabs(x1 * x2 + y1 * y2);
    d2 *= d2;
    if (fabs(d1 - d2) < (HIT_TEST_MARGIN * HIT_TEST_MARGIN)) {
        m_nHitXAxis = 1;
    }
    else {
        m_nHitXAxis = 0;
    }

    x2 = cos(m_fAngleY * PI / 180.0);
    y2 = sin(m_fAngleY * PI / 180.0);
    d2 = fabs(x1 * x2 + y1 * y2);
    d2 *= d2;
    if (fabs(d1 - d2) < (HIT_TEST_MARGIN * HIT_TEST_MARGIN)) {
        m_nHitYAxis = 1;
    }
    else {
        m_nHitYAxis = 0;
    }

    if (m_nCrosshairMode == CROSSHAIR_ORTHOGONAL) {
        m_nRotate = 0;
    }
    else if (m_nCrosshairMode == CROSSHAIR_OBLIQUE || m_nCrosshairMode == CROSSHAIR_SINGLE_LINE) {
        if (m_nHitXAxis && m_nHitYAxis) {
            // Selected both X and Y axis
            if (d1 < (OBLIQUE_ROTATION_REGION * OBLIQUE_ROTATION_REGION)) {
                m_nRotate = 0;
            }
            else {
                // X axis and Y axis almost coincide, just select X axis
                m_nRotate = 1;
                m_nHitYAxis = 0;
            }
        }
        else if(m_nHitXAxis || m_nHitYAxis) {
            m_nRotate = 1;
        }
        else {
            m_nRotate = 0;
        }
    }
    return m_nHitXAxis + m_nHitYAxis * 2 + m_nRotate * 4;
}

void CrosshairGraphicObject::SetHittedMode(HITTED_MODE mode)
{
    switch(mode)
    {
    case HITTED_NOTHING:
        m_nHitXAxis = m_nHitYAxis = 0;
        break;
    case HITTED_CENTER:
        m_nHitYAxis = m_nHitXAxis = 1;
        break;
    case HITTED_AXIS_X:
        m_nHitXAxis = 1;
        m_nHitYAxis = 0;
        break;
    case HITTED_AXIS_Y:
        m_nHitXAxis = 0;
        m_nHitYAxis = 1;
        break;
    default:
        return;
    }
}

int CrosshairGraphicObject::IsHit() {
    if (!m_bVisible) {
        return 0;
    }

    return m_nHitXAxis + m_nHitYAxis * 2 + m_nRotate * 4;
    //// In orthogonal mode, no rotation
    //if (m_nCrosshairMode == CROSSHAIR_ORTHOGONAL) {
    //    return m_nHitXAxis + m_nHitYAxis * 2; 
    //}
    //// In oblique mode, we have move center, rotate around x/y axis
    //if (m_nCrosshairMode == CROSSHAIR_OBLIQUE ||
    //    m_nCrosshairMode == CROSSHAIR_SINGLE_LINE) {
    //    if(m_nRotate) {
    //        return m_nHitXAxis + m_nHitYAxis * 2 + m_nRotate * 4;
    //    }
    //    else {
    //        return (m_nHitXAxis & m_nHitYAxis) * 3;
    //    }
    //}
    //return 0;
}

TPS_END_NAMESPACE