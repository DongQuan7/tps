////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_table_box_renderstrategy.cpp
/// 
///  \brief class TableBoxRenderStrategy 
/// 
///  \version 1.0
/// 
///  \date    2014/9/25
////////////////////////////////////////////////////////////////

#include "RtTpsRenderLibrary/tps_rl_table_box_renderstrategy.h"

#include "RtTpsRenderLibrary/tps_rl_manualremovetablegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "GL/glew.h"

TPS_BEGIN_NAMESPACE
TableBoxRenderStrategy::TableBoxRenderStrategy(std::string& strWindowType): 
    m_spTableBoxGraphicObejct(nullptr),
    m_strWindowType(strWindowType) {

}

void TableBoxRenderStrategy::Initialize() {
}

TableBoxRenderStrategy::~TableBoxRenderStrategy() {

}

void TableBoxRenderStrategy::Render_i() {

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);

    if(nullptr == m_spTableBoxGraphicObejct) {
        return;
    }
    if(!m_spTableBoxGraphicObejct->GetVisibilty()) {
        return;
    }
    if(m_strWindowType != "Axial" && m_strWindowType != "Sagittal") {
        return;
    }

    //float height =1.0f -  mGraphicObejct->GetHeight();
    float height = m_spTableBoxGraphicObejct->GetHeight(); // in world coordinate

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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(fScale, fScale, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    Mcsf::MedViewer3D::Point3D pt1InWorld(-1.0, height, 0.0);
    Mcsf::MedViewer3D::Point3D pt2InWorld( 1.0, height, 0.0);
    Mcsf::MedViewer3D::Point2D pt1InPlane = m_spMPRGO->GetPlaneCoordinate(pt1InWorld, false);
    Mcsf::MedViewer3D::Point2D pt2InPlane = m_spMPRGO->GetPlaneCoordinate(pt2InWorld, false);

    glColor4f(0.0f,1.0f,1.0f,1.0f);
    glBegin(GL_LINES);
    //if (m_strWindowType == "Axial") {
    //    glVertex3f(-10.0f, -height, 0.0f);
    //    glVertex3f( 10.0f, -height, 0.0f);
    //}
    //else {
    //    glVertex3f(height, -10.0f, 0.0f);
    //    glVertex3f(height,  10.0f, 0.0f);
    //}
    if (m_strWindowType == "Axial") {
        // the line of table in axial mpr
        Mcsf::MedViewer3D::Point3D pt1InWorld(-1.0, height, 0.0);
        Mcsf::MedViewer3D::Point3D pt2InWorld( 1.0, height, 0.0);
        Mcsf::MedViewer3D::Point2D pt1InPlane = m_spMPRGO->GetPlaneCoordinate(pt1InWorld, false);
        Mcsf::MedViewer3D::Point2D pt2InPlane = m_spMPRGO->GetPlaneCoordinate(pt2InWorld, false);
        //glVertex3f(2.0 * pt1InPlane.x - 1.0, 2.0 * pt1InPlane.y - 1.0, 0.0);
        //glVertex3f(2.0 * pt2InPlane.x - 1.0, 2.0 * pt2InPlane.y - 1.0, 0.0);
        glVertex3f(-1.0, 2.0 * pt1InPlane.y - 1.0, 0.0);
        glVertex3f( 1.0, 2.0 * pt2InPlane.y - 1.0, 0.0);
    }
    else
    {
        // the line of table in sagittal mpr
        Mcsf::MedViewer3D::Point3D pt1InWorld(0.0, height, -1.0);
        Mcsf::MedViewer3D::Point3D pt2InWorld(0.0, height,  1.0);
        Mcsf::MedViewer3D::Point2D pt1InPlane = m_spMPRGO->GetPlaneCoordinate(pt1InWorld, false);
        Mcsf::MedViewer3D::Point2D pt2InPlane = m_spMPRGO->GetPlaneCoordinate(pt2InWorld, false);
        glVertex3f(2.0 * pt1InPlane.x - 1.0, -1.0, 0.0);
        glVertex3f(2.0 * pt2InPlane.x - 1.0,  1.0, 0.0);
    }
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glViewport(0, 0, mWidth, mHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

}

void TableBoxRenderStrategy::Resize(int w, int h) {
    mHeight = h;
    mWidth = w;
}

void TableBoxRenderStrategy::Finalize() {

}

int TableBoxRenderStrategy::CacheChanges() {
    if(nullptr == m_spTableBoxGraphicObejct) {
        return -1;
    }
    if(m_spTableBoxGraphicObejct->IsDirty()) {
        mDirty = true;
    }
    if(m_spMPRGO->IsDirty())
    {
        mDirty = true;
    }
    return 0;
}

void TableBoxRenderStrategy::SetGraphicObject(std::shared_ptr<ManualRemoveTableGraphicObject> go) {
    if (m_spTableBoxGraphicObejct != go){
        m_spTableBoxGraphicObejct = go;
        mDirty = true;
    }
}

void TableBoxRenderStrategy::SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject> mprGO) {
    if (m_spMPRGO != mprGO){
        m_spMPRGO = mprGO;
        mDirty = true;
    }
}

TPS_END_NAMESPACE
