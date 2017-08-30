////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_bev_dose_shaded_strategy.cpp
/// 
///  \brief class TpsBevDoseShadedStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/06/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_bev_dose_shaded_strategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"

#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsAlgorithmProxy/tps_ap_dose_mesh_helper.h"

//test by mcf
//#include<iostream>
//#include<iomanip>
//#include<fstream>

#include "tps_logger.h"

#define  MAX_VERT_NUM   512*512 *80

TPS_BEGIN_NAMESPACE

TpsBevDoseShadedStrategy::TpsBevDoseShadedStrategy()
    : m_spCameraGO(nullptr),
      m_spDoseMeshGOCollection(nullptr),
      m_bRenderInLine(false),
      m_bRenderInMesh(false)
{
    //m_bRenderInMesh = true;
}

TpsBevDoseShadedStrategy::~TpsBevDoseShadedStrategy() {

}

void TpsBevDoseShadedStrategy::Initialize() {

}

static void  CalcViewMatrix(Mcsf::MedViewer3D::Point3D ptEye, Mcsf::MedViewer3D::Point3D ptLookAt, Mcsf::MedViewer3D::Vector3D vecUp,
                            Mcsf::MedViewer3D::Matrix4x4& matViewMatrix)
{
    Mcsf::MedViewer3D::Vector3D zaxis = ptEye - ptLookAt;
    zaxis.Normalize();
    Mcsf::MedViewer3D::Vector3D xaxis = vecUp.CrossProduct(zaxis);
    xaxis.Normalize();
    Mcsf::MedViewer3D::Vector3D yaxis = zaxis.CrossProduct(xaxis);

    matViewMatrix.m[0][0] = xaxis.x;
    matViewMatrix.m[1][0] = xaxis.y;
    matViewMatrix.m[2][0] = xaxis.z;
    matViewMatrix.m[3][0] = -(xaxis.DotProduct(ptEye - Mcsf::MedViewer3D::Point3D(0,0,0)));

    matViewMatrix.m[0][1] = yaxis.x;
    matViewMatrix.m[1][1] = yaxis.y;
    matViewMatrix.m[2][1] = yaxis.z;
    matViewMatrix.m[3][1] = -(yaxis.DotProduct(ptEye - Mcsf::MedViewer3D::Point3D(0,0,0)));


    matViewMatrix.m[0][2] = zaxis.x;
    matViewMatrix.m[1][2] = zaxis.y;
    matViewMatrix.m[2][2] = zaxis.z;
    matViewMatrix.m[3][2] = -(zaxis.DotProduct(ptEye - Mcsf::MedViewer3D::Point3D(0,0,0)));


    matViewMatrix.m[0][3] = 0;
    matViewMatrix.m[1][3] = 0;
    matViewMatrix.m[2][3] = 0;
    matViewMatrix.m[3][3] = 1;
}

int compare_matrix(Mcsf::MedViewer3D::Matrix4x4& matViewMatrix, float matView[16])
{
    for (int i = 0; i < 16; i++)
    {
        if (fabs(matViewMatrix._m[i] - matView[i]) > 0.0001)
            return 1;
    }
    return 0;
}

void TpsBevDoseShadedStrategy::RenderBevDoseSurface()
{
    auto doseMeshCollection = m_spDoseMeshGOCollection->GetAllDoseMeshGo();
    for (int index = 0; index < doseMeshCollection.size();++index)
    {
        auto m_spDoseMeshGO = doseMeshCollection[index];
        if(!m_spDoseMeshGO->GetValidFlag()) continue;
        STC_STRIPS* pStrips = m_spDoseMeshGO->GetDoseMesh();
        if (pStrips == nullptr)
        {
            return;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
        glUseProgram(0);

        float   color[4] ={1.0f, 0.0f, 0.0f, 0.8f};
        float   l_amb[4], l_dif[4], l_spec[4];
        //float   position[] = { 0.0f, 0.0f, 20.0f, 0.0f};

        float fRed, fGreen, fBlue, fAlpha;
        m_spDoseMeshGO->GetDoseColor(&fRed, &fGreen, &fBlue, &fAlpha);
        color[0] = fRed;
        color[1] = fGreen;
        color[2] = fBlue;

        l_amb[0] = 0.6*color[0];
        l_amb[1] = 0.6*color[1];
        l_amb[2] = 0.6*color[2];
        l_amb[3] = 1.0;// * 0.5;
        l_dif[0] = 0.4*color[0];
        l_dif[1] = 0.4*color[1];
        l_dif[2] = 0.4*color[2];
        l_dif[3] = 1.0;// * 0.5;
        l_spec[0] = 0.3f;
        l_spec[1] = 0.3f;
        l_spec[2] = 0.3f;
        l_spec[3] = 1.0f;// * 0.5f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, l_amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, l_dif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, l_spec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0F);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_LIGHT0);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        glEnable(GL_LIGHTING);
        //glDisable(GL_LIGHTING);

        m_spCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
        Mcsf::MedViewer3D::Matrix4x4 matViewProjection = this->m_spCameraGO->GetViewProjectionMatrix();
        int startX(0), startY(0);
        size_t sizeX(mWidth), sizeY(mHeight);
        //glViewport(startX, startY, sizeX, sizeY);
        float fScale = 1.0;
        if (sizeX >= sizeY)
        {
            glViewport(startX, startY -(sizeX - sizeY)/2, sizeX, sizeX);
            fScale = (float)sizeY / (float)sizeX;
        }
        else
        {
            glViewport(startX -(sizeY - sizeX)/2, startY, sizeY, sizeY);
            fScale =  (float)sizeX / (float)sizeY;
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float lightPosition[4]={0.0, 0.0, 5.0, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        // parameters are from plunc!
        Mcsf::MedViewer3D::Point3D ptEye = m_spCameraGO->GetEye();
        Mcsf::MedViewer3D::Point3D ptLookAt = m_spCameraGO->GetLookAt();

        Mcsf::MedViewer3D::Vector3D vecView = ptLookAt - ptEye;
        Mcsf::MedViewer3D::Vector3D vecUp = m_spCameraGO->GetUpDirection();
        vecUp.Normalize();
        float fDistance = vecView.Magnitude();
        Mcsf::MedViewer3D::Matrix4x4 matViewMatrix;
        CalcViewMatrix(ptEye, ptLookAt, vecUp, matViewMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Calculate near and far clipping plane
        float cubeVertex[8][3] = {-1, -1, -1,
            1, -1, -1,
            -1,  1, -1,
            1,  1, -1,
            -1, -1,  1,
            1, -1,  1,
            -1,  1,  1,
            1,  1,  1 };
        float zMin = 10000, zMax = 0;
        for (int i = 0; i < 8; i++)
        {
            Mcsf::MedViewer3D::Point3D ptCubeInWorld(cubeVertex[i][0], cubeVertex[i][1], cubeVertex[i][2]);
            Mcsf::MedViewer3D::Point3D ptCubeInView = matViewMatrix.Transform(ptCubeInWorld);
            if (i == 0)
            {
                zMin = -ptCubeInView.z;
                zMax = -ptCubeInView.z;
            }
            if (zMin > -ptCubeInView.z) zMin = -ptCubeInView.z;
            if (zMax < -ptCubeInView.z) zMax = -ptCubeInView.z;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, zMax - zMin);

        //glDisable(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        int cloop = 0;
        int loop = 0;
        STC_STRIP* pStrip = nullptr;
        plunc::PNT3D    p1, n1;

        //glColor4f(1.0, 1.0, 1.0, 1.0);
        glDisable(GL_POLYGON_SMOOTH);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        //glDisable(GL_DEPTH_TEST);

        Mcsf::MedViewer3D::Vector3D vecNormalInWorld;
        Mcsf::MedViewer3D::Vector3D vecNormalInView;
        Mcsf::MedViewer3D::Point3D ptInWorld;
        Mcsf::MedViewer3D::Point3D ptInView;
        float perFactor = 0.0;
        for (cloop = 0; cloop < pStrips->nCount; cloop++) 
        {
            glBegin(GL_QUAD_STRIP); //glBegin(GL_TRIANGLE_STRIP);
            color[2] = cloop % 2;
            pStrip = pStrips->pStrip + cloop;
            for (loop = 0; loop < pStrip->count; loop++) 
            {
                //color[0] = (loop % 2 + 1) / 2.0;
                //color[1] = (loop % 3) /3.0;
                p1 = pStrip->dpt[loop];
                n1 = pStrip->norm[loop];
                vecNormalInWorld.x = n1.x;
                vecNormalInWorld.y = n1.y;
                vecNormalInWorld.z = n1.z;
                vecNormalInView = matViewMatrix.Transform(vecNormalInWorld);
                vecNormalInWorld.z *= -1.0;

                //glColor4fv((float*)color);
                //glNormal3fv((float *)&n1);
                glNormal3f(vecNormalInView.x, vecNormalInView.y, vecNormalInView.z);

                ptInWorld.x = p1.x;
                ptInWorld.y = p1.y;
                ptInWorld.z = p1.z;
                ptInView = matViewMatrix.Transform(ptInWorld);
                perFactor = -fDistance / ptInView.z * fScale;
                ptInView.x *= perFactor;
                ptInView.y *= perFactor;
                ptInView.z += zMin;
                glVertex3f(ptInView.x, ptInView.y, ptInView.z);
                //glVertex3f(p1.x, p1.y, p1.z);
            }
            glEnd();
            glFlush();
        }
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);               // for blend
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}


void TpsBevDoseShadedStrategy::RenderBevDoseMesh()
{
    auto doseMeshCollection = m_spDoseMeshGOCollection->GetAllDoseMeshGo();
    for (int index = 0; index < doseMeshCollection.size();++index)
    {
        auto m_spDoseMeshGO = doseMeshCollection[index];
        if(!m_spDoseMeshGO->GetValidFlag()) continue;
        STC_STRIPS* pStrips = m_spDoseMeshGO->GetDoseMesh();
        if (pStrips == nullptr)
        {
            return;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
        glUseProgram(0);

        float   color[4] ={1.0f, 0.0f, 0.0f, 0.8f};
        float fRed, fGreen, fBlue, fAlpha;
        m_spDoseMeshGO->GetDoseColor(&fRed, &fGreen, &fBlue, &fAlpha);
        color[0] = fRed;
        color[1] = fGreen;
        color[2] = fBlue;
        color[2] = fAlpha;

        glDisable(GL_LIGHTING);

        m_spCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
        Mcsf::MedViewer3D::Matrix4x4 matViewProjection = this->m_spCameraGO->GetViewProjectionMatrix();
        int startX(0), startY(0);
        size_t sizeX(mWidth), sizeY(mHeight);
        //glViewport(startX, startY, sizeX, sizeY);
        float fScale = 1.0;
        if (sizeX >= sizeY)
        {
            glViewport(startX, startY -(sizeX - sizeY)/2, sizeX, sizeX);
            fScale = (float)sizeY / (float)sizeX;
        }
        else
        {
            glViewport(startX -(sizeY - sizeX)/2, startY, sizeY, sizeY);
            fScale =  (float)sizeX / (float)sizeY;
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // parameters are from plunc!
        Mcsf::MedViewer3D::Point3D ptEye = m_spCameraGO->GetEye();
        Mcsf::MedViewer3D::Point3D ptLookAt = m_spCameraGO->GetLookAt();
        Mcsf::MedViewer3D::Vector3D vecView = ptLookAt - ptEye;
        Mcsf::MedViewer3D::Vector3D vecUp = m_spCameraGO->GetUpDirection();
        vecUp.Normalize();
        float fDistance = vecView.Magnitude();
        Mcsf::MedViewer3D::Matrix4x4 matViewMatrix;
        CalcViewMatrix(ptEye, ptLookAt, vecUp, matViewMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Calculate near and far clipping plane
        float cubeVertex[8][3] = {-1, -1, -1,
                                   1, -1, -1,
                                  -1,  1, -1,
                                   1,  1, -1,
                                  -1, -1,  1,
                                   1, -1,  1,
                                  -1,  1,  1,
                                   1,  1,  1 };
        float zMin = 10000, zMax = 0;
        for (int i = 0; i < 8; i++)
        {
            Mcsf::MedViewer3D::Point3D ptCubeInWorld(cubeVertex[i][0], cubeVertex[i][1], cubeVertex[i][2]);
            Mcsf::MedViewer3D::Point3D ptCubeInView = matViewMatrix.Transform(ptCubeInWorld);
            if (i == 0)
            {
                zMin = -ptCubeInView.z;
                zMax = -ptCubeInView.z;
            }
            if (zMin > -ptCubeInView.z) zMin = -ptCubeInView.z;
            if (zMax < -ptCubeInView.z) zMax = -ptCubeInView.z;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, zMax - zMin);

        glDisable(GL_BLEND);

        int cloop = 0;
        int loop = 0;
        STC_STRIP* pStrip = nullptr;

        //glColor4f(1.0, 1.0, 1.0, 1.0);
        glDisable(GL_POLYGON_SMOOTH);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        //glDisable(GL_DEPTH_TEST);

        Mcsf::MedViewer3D::Vector3D vecNormalInWorld;
        Mcsf::MedViewer3D::Vector3D vecNormalInView;
        Mcsf::MedViewer3D::Point3D ptInWorld;
        Mcsf::MedViewer3D::Point3D ptInView;
        float perFactor = 0.0;
        glColor4f(fRed, fGreen, fBlue, 1.0f);
        for (cloop = 0; cloop < pStrips->nCount; cloop++) 
        {
            glBegin(GL_QUAD_STRIP); //glBegin(GL_TRIANGLE_STRIP);
            pStrip = pStrips->pStrip + cloop;
            for (loop = 0; loop < pStrip->count; loop++) 
            {
                plunc::PNT3D p1 = pStrip->dpt[loop];
                ptInWorld.x = p1.x;
                ptInWorld.y = p1.y;
                ptInWorld.z = p1.z;
                ptInView = matViewMatrix.Transform(ptInWorld);
                perFactor = -fDistance / ptInView.z * fScale;
                ptInView.x *= perFactor;
                ptInView.y *= perFactor;
                ptInView.z += zMin;
                glVertex3f(ptInView.x, ptInView.y, ptInView.z);
                //glVertex3f(p1.x, p1.y, p1.z);
            }
            glEnd();
            glFlush();
        }
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);               // for blend
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void TpsBevDoseShadedStrategy::RenderBevDoseLine() 
{
    auto doseMeshCollection = m_spDoseMeshGOCollection->GetAllDoseMeshGo();
    for (int index = 0; index < doseMeshCollection.size();++index)
    {
        auto m_spDoseMeshGO = doseMeshCollection[index];
        if(!m_spDoseMeshGO->GetValidFlag()) continue;
        plunc::ANASTRUCT* pAnastruct = m_spDoseMeshGO->GetDoseAnastruct();
        if (pAnastruct == nullptr)
        {
            return;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
        glUseProgram(0);

        m_spCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
        Mcsf::MedViewer3D::Matrix4x4 matViewProjection = this->m_spCameraGO->GetViewProjectionMatrix();
        int startX(0), startY(0);
        size_t sizeX(mWidth), sizeY(mHeight);
        //glViewport(startX, startY, sizeX, sizeY);
        float fScale = 1.0;
        if (sizeX >= sizeY)
        {
            glViewport(startX, startY -(sizeX - sizeY)/2, sizeX, sizeX);
            fScale = (float)sizeY / (float)sizeX;
        }
        else
        {
            glViewport(startX -(sizeY - sizeX)/2, startY, sizeY, sizeY);
            fScale =  (float)sizeX / (float)sizeY;
        }

        double viewportSize = sizeX > sizeY ? sizeY : sizeX;
        double pixelSize = 1.0 / viewportSize;

        //glViewport(0, 0, m_iWidth, m_iHeight);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_LIGHTING);

        glEnable(GL_DEPTH_TEST);
        //glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);

        glDisable(GL_LINE_STIPPLE);

        glDisable(GL_BLEND);
        //glEnable(GL_LINE_SMOOTH);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glLineWidth(2.0f);
        //glBlendFunc(GL_ONE, GL_ZERO);
        //glBlendEquation(GL_FUNC_ADD);
        //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);//GL_DONT_CARE);

        Mcsf::MedViewer3D::Point3D ptEye = m_spCameraGO->GetEye();
        Mcsf::MedViewer3D::Point3D ptLookAt = m_spCameraGO->GetLookAt();

        Mcsf::MedViewer3D::Vector3D vecView = ptLookAt - ptEye;
        Mcsf::MedViewer3D::Vector3D vecUp = m_spCameraGO->GetUpDirection();
        vecUp.Normalize();
        float fDistance = vecView.Magnitude();
        Mcsf::MedViewer3D::Matrix4x4 matViewMatrix;
        CalcViewMatrix(ptEye, ptLookAt, vecUp, matViewMatrix);
        //double fScale = m_spCameraGO->GetScale();
        //fScale = 1.0;

        // Calculate near and far clipping plane
        float cubeVertex[8][3] = {-1, -1, -1,
                                    1, -1, -1,
                                    -1,  1, -1,
                                    1,  1, -1,
                                    -1, -1,  1,
                                    1, -1,  1,
                                    -1,  1,  1,
                                    1,  1,  1 };
        float zMin = 10000, zMax = 0;
        for (int i = 0; i < 8; i++)
        {
            Mcsf::MedViewer3D::Point3D ptCubeInWorld(cubeVertex[i][0], cubeVertex[i][1], cubeVertex[i][2]);
            Mcsf::MedViewer3D::Point3D ptCubeInView = matViewMatrix.Transform(ptCubeInWorld);
            if (i == 0)
            {
                zMin = -ptCubeInView.z;
                zMax = -ptCubeInView.z;
            }
            if (zMin > -ptCubeInView.z) zMin = -ptCubeInView.z;
            if (zMax < -ptCubeInView.z) zMax = -ptCubeInView.z;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, zMax - zMin);

        glEnable(GL_FOG);
        glFogf(GL_FOG_START, 0.0);
        glFogf(GL_FOG_END, zMax - zMin);
        glFogi(GL_FOG_MODE, GL_LINEAR);
        float fog_color[4] = {0.1f, 0.1f, 0.1f, 0.0f};
        glFogfv(GL_FOG_COLOR, fog_color);

        //Mcsf::MedViewer3D::Point3D ptInWorld;
        Mcsf::MedViewer3D::Point3D ptInView;
        float perFactor = 0.0;

        float color[4];
        float fRed, fGreen, fBlue, fAlpha;
        m_spDoseMeshGO->GetDoseColor(&fRed, &fGreen, &fBlue, &fAlpha);
        color[0] = fRed;
        color[1] = fGreen;
        color[2] = fBlue;
        color[3] = 1.0f;

        glColor4f(color[0], color[1], color[2], 1.0f);//alpha should be 1.0f

        for (int i = 0; i < pAnastruct->contour_count; ++i)
        {
            for(int j = 0; j < pAnastruct->contours[i].vertex_count; ++j) 
            {
                plunc::CONTOUR_STC* pCon = pAnastruct->contours + i;
                if (pCon->vertex_count < 4) continue;

                std::vector<double> vecX;
                std::vector<double> vecY;
                std::vector<double> vecZ;
                for(int loop = 0; loop < pCon->vertex_count; ++loop) 
                {

                    Mcsf::MedViewer3D::Point3D ptInWorld(pCon->x[loop], pCon->y[loop], pCon->z);
                    ptInView = matViewMatrix.Transform(ptInWorld);

                    perFactor = -fDistance / ptInView.z * fScale;
                    ptInView.x *= perFactor;
                    ptInView.y *= perFactor;
                    ptInView.z += zMin;
                    vecX.push_back(ptInView.x);
                    vecY.push_back(ptInView.y);
                    vecZ.push_back(ptInView.z);
                    //glVertex3f(ptInView.x, ptInView.y, ptInView.z);
                }

                bool bIsoPlane = true;
                double dAverageDepth = 0;
                double dAdjust = 0;
                double xLeft = vecX[0];
                double depthLeft = vecZ[0];
                double xRight = vecX[0];
                double depthRight = vecZ[0];
                for (int p = 0; p < vecX.size(); p++)
                {
                    if (fabs(vecY[p]) > pixelSize)
                    {
                        bIsoPlane = false;
                        break;
                    }
                    if (xLeft > vecX[p])
                    {
                        xLeft = vecX[p];
                        depthLeft = vecZ[p];
                    }
                    if (xRight < vecX[p])
                    {
                        xRight = vecX[p];
                        depthRight = vecZ[p];
                    }
                    //dAverageDepth += vecZ[p];
                }
                if (bIsoPlane)
                {
                    //dAverageDepth /= vecZ.size();
                    dAverageDepth = (depthLeft + depthRight) * 0.5;
                    glBegin(GL_LINE_LOOP);                
                    for (int p = 0; p < vecX.size(); p++)
                    {
                        dAdjust = 0.0;
                        if (vecZ[p] > dAverageDepth)
                        {
                            if (vecY[p] > 0) 
                                dAdjust = pixelSize;
                            else
                                dAdjust = -pixelSize;
                        }
                        glVertex3f(vecX[p], vecY[p] + dAdjust, vecZ[p]);
                    }
                    glEnd();
                }
                else
                {
                    glBegin(GL_LINE_LOOP);                
                    for (int p = 0; p < vecX.size(); p++)
                    {
                        glVertex3f(vecX[p], vecY[p], vecZ[p]);
                    }
                    glEnd();
                }
            }
        }   // slice loop

        
        glFlush();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_FOG);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}


void TpsBevDoseShadedStrategy::Render_i() {

    if(m_spCameraGO == nullptr || m_spDoseMeshGOCollection == nullptr) {
        return;
    }

    if (m_bRenderInLine)
    {
        RenderBevDoseLine();
        return;
    }

    if (m_bRenderInMesh)
    {
        RenderBevDoseMesh();
        return;
    }

    RenderBevDoseSurface();
}

void TpsBevDoseShadedStrategy::Resize(int iWidth, int iHeight) {
    mHeight = iHeight;
    mWidth = iWidth;
    mDirty =true;
}

int TpsBevDoseShadedStrategy::CacheChanges() {
    if(m_spDoseMeshGOCollection == nullptr || m_spCameraGO == nullptr) {
        return -1;
    }
    if(m_spDoseMeshGOCollection->IsDirty() || m_spCameraGO->IsDirty()) {
        mDirty = true;
    }
    return -1;
}

void TpsBevDoseShadedStrategy::Finalize() {

}

void TpsBevDoseShadedStrategy::SetDoseMeshGOCollection(std::shared_ptr<DoseMeshGraphicObjectCollection> spDoseMeshGO) {
    if (m_spDoseMeshGOCollection != spDoseMeshGO){
        m_spDoseMeshGOCollection = spDoseMeshGO;
        mDirty = true;
    }
}



void TpsBevDoseShadedStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject> spCameroGO) {
    if (m_spCameraGO != spCameroGO){
        m_spCameraGO = spCameroGO;
        mDirty = true;
    }
}

TPS_END_NAMESPACE
