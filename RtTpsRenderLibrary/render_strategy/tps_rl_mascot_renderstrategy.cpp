////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mascot_renderstrategy.cpp
/// 
///  \brief class MascotRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/09/05
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_mascot_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mascot_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mascot_graphicobject.h"

#include "McsfMedViewer3DGraphicUtils/camera.h"

#include "gl/glut.h"
#pragma warning(disable:4505)

TPS_BEGIN_NAMESPACE

MascotRenderStrategy::MascotRenderStrategy()
    :m_spMPRGO(nullptr), m_spMascotGO(nullptr)
{
    mDirty = true;
}

MascotRenderStrategy::MascotRenderStrategy(std::shared_ptr<MPRGraphicObject> spMPRGO)
    : m_spMPRGO(spMPRGO), m_spMascotGO(nullptr) {

}

void MascotRenderStrategy::Initialize() {
    m_spMascotGO.reset(new MascotGraphicObject());
    m_spMascotGO->Initialize();
    mDirty = true;
}

MascotRenderStrategy::~MascotRenderStrategy() {

}

void MascotRenderStrategy::SetName(std::string& strName) {
    m_strName = strName;
}

void MascotRenderStrategy::SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject> spMPRGO) {
    if (m_spMPRGO != spMPRGO){
        m_spMPRGO = spMPRGO;
        mDirty = true;
    }
}

/*!\fn MascotRenderStrategy::Render()
* <key> \n
* PRA: Yes \n
* Name:void MascotRenderStrategy::Render() \n
* Traced from: SSFS_PRA_TPS_OrientationIndicator \n
* Description: render U boy to indicate the direction \n
* Short Description: UBoyIndicator \n
* Component: TPS \n
* </key> \n
*******************************************************************************/
void MascotRenderStrategy::Render_i() {
    //delete by wzd 暂时注释掉 小人前端绘制完成后可删除
    return;
    if (m_spMPRGO == nullptr) {
        return;
    }
    if (!m_spMascotGO->Visible()) {
        return;
    }
    //printf("\n%s\n", m_strName.c_str());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);

    //GLint viewport[4];
    //glGetIntegerv(GL_VIEWPORT, viewport);
    //m_iWidth = viewport[2];
    //m_iHeight = viewport[3];

    //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glFlush();

    int iMaxLen = std::max(mWidth, mHeight);
    int iCubeSize = 96;
    if(iMaxLen > 1300) {
        iCubeSize = 192;
    }
    else if(iMaxLen >= 512) {
        iCubeSize = 108;
    }
    glViewport(0, 0, iCubeSize * 1.3, iCubeSize * 1.3);

    Mcsf::MedViewer3D::Vector3D vecUp, vecView;
    m_spMPRGO->GetViewDirection(&vecUp, &vecView);

    Mcsf::MedViewer3D::ProjectionCamera camera;
    Mcsf::MedViewer3D::Point3D ptLookAt(0.0, 0.0, 0.0);
    double distanceLook = 10.0;
    Mcsf::MedViewer3D::Point3D ptEye = ptLookAt + (-vecView * distanceLook);
    camera.SetLookAt(ptLookAt);
    camera.SetEye(ptEye);
    camera.SetUpDirection(vecUp);
    camera.SetFarClipDistance(11.0);
    camera.SetNearClipDistance(9.0);
    camera.SetProjectionType(Mcsf::MedViewer3D::ProjectionCamera::PT_ORTHOGRAPHIC);
    camera.SetAspectRatio(1.0);
    camera.SetOrthoWindow(3.5, 3.5);
    //camera.SetEye(Point3D(0.5, 0.5,0.5 -4.0));
    Mcsf::MedViewer3D::Matrix4x4 matViewProjection = camera.GetViewProjectionMatrix();

    float lightPosition[4] = {0.0, 0.0, 2.0, 1.0};
    lightPosition[0] = ptEye.x;
    lightPosition[1] = ptEye.y;
    lightPosition[2] = ptEye.z;
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    float matObj2Pat[4][4];
    memset(matObj2Pat, 0, sizeof(float) * 16);
    matObj2Pat[0][0] = 1.0f;
    matObj2Pat[2][1] = -1.0f;
    matObj2Pat[1][2] = 1.0f;
    matObj2Pat[3][3] = 1.0f;
    glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixf((float*)matObj2Pat);
    glLoadMatrixd(matViewProjection._m);
    //glMultMatrixd(matViewProjection._m);
    //glMultMatrixf((float*)matObj2Pat);
    glLoadIdentity();
    gluLookAt(ptEye.x, ptEye.y, ptEye.z, 0.0, 0.0, 0.0, vecUp.x, vecUp.y, vecUp.z);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.8, 1.8, -1.8, 1.8, distanceLook - 1.8, distanceLook + 1.8);
    glDisable(GL_CULL_FACE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    m_spMascotGO->Draw(lightPosition);

    glFlush();
    glViewport(0, 0, mWidth, mHeight);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //printf("render finished!\n");
}

void MascotRenderStrategy::Resize(int w, int h) {
    mHeight = h;
    mWidth = w;
}

void MascotRenderStrategy::Finalize() {

}

int MascotRenderStrategy::CacheChanges() {
    //m_bDirty = true;
    if ( !m_spMPRGO )
    {
        return 0;
    }

    Mcsf::MedViewer3D::Vector3D vecUp, vecView;
    m_spMPRGO->GetViewDirection(&vecUp, &vecView);

    if ( m_vecUp != vecUp || m_vecView != vecView)
    {
        m_vecUp = vecUp;
        m_vecView = vecView;
        mDirty = true;
        return 0;
    }
    return 0;
}

TPS_END_NAMESPACE