////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_series_table_render_strategy.cpp
/// 
///  \brief class SeriesTableRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_series_table_render_strategy.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"

#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE  // begin namespace tps

SeriesTableRenderStrategy::SeriesTableRenderStrategy() : 
    mMPRGO(nullptr), 
    mVolumeGO(nullptr), 
    mNeedRecalcViewProj(true)
{

}

SeriesTableRenderStrategy::~SeriesTableRenderStrategy()
{

}

void SeriesTableRenderStrategy::Render_i()
{
    if (mVolumeGO == nullptr || !mVolumeGO->IsValid() || 
        mMPRGO == nullptr || mSeriesTableCacheGO == nullptr)
    {
        return;
    }

    if (mSeriesTableCacheGO->IsTableEmpty()) return;

    if (mNeedRecalcViewProj) this->CalcViewProjectionMatrix_i();

    double color[4];
    mSeriesTableCacheGO->GetColor(color);
    bool needDrawFill = color[3] > 1e-6;

    unsigned char* buffer;
    int width, height;
    Matrix4x4 worldToNomalPlaneTM, projection;
    mSeriesTableCacheGO->GetCache(buffer, width, height, worldToNomalPlaneTM);
    projection = mViewProjectionMat;
    projection.Append(worldToNomalPlaneTM.Inverse());

    std::vector<std::vector<Mcsf::Point2f>> vvContour;
    if (TPS_ER_SUCCESS != SubVolumeHelperAPI::GetContoursFromMask(
        buffer, width, height, vvContour))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get contours from mask.";
        return;
    }

    /*for (int i = 0; i < vvContour.size(); ++i)
    {
        TpsUtility::RDPPolygonReduce(vvContour[i]);
    }*/
    for (int i = 0; i < vvContour.size(); ++i)
    {
        for (int j = 0; j < vvContour[i].size(); ++j)
        {
            vvContour[i][j].SetX((vvContour[i][j].GetX() + 0.5) / width);
            vvContour[i][j].SetY((vvContour[i][j].GetY() + 0.5) / height);
        }
    }
    TpsUtility::SmoothCurve(vvContour);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(2.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projection._m);

    glDisable(GL_LINE_STIPPLE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4d(color[0], color[1], color[2], 1.0);
    for (int i = 0; i < vvContour.size(); ++i)
    {
        float* points = new float[vvContour[i].size() * 2];
        for (int j = 0; j < vvContour[i].size(); ++j)
        {
            points[2 * j] = vvContour[i][j].GetX();
            points[2 * j + 1] = vvContour[i][j].GetY();
        }
        glVertexPointer(2, GL_FLOAT, 0, points);
        glDrawArrays(GL_LINE_LOOP, 0, vvContour[i].size());
        TPS_DEL_ARRAY(points);
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    if (needDrawFill)
    {
        glColor4d(color[0], color[1], color[2], color[3]);
        DrawFill(vvContour);
    }

    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

int SeriesTableRenderStrategy::CacheChanges()
{
    if (mVolumeGO == nullptr || !mVolumeGO->IsValid() || 
        mMPRGO == nullptr || mSeriesTableCacheGO == nullptr)
    {
        return -1;
    }
    if (mMPRGO->IsDirty() || mVolumeGO->IsDirty())
    {
        mNeedRecalcViewProj = true;
        mDirty = true;
    }
    if (mSeriesTableCacheGO->IsDirty())
    {
        mDirty = true;
    }
    return 0;
}

void SeriesTableRenderStrategy::Initialize()
{

}

void SeriesTableRenderStrategy::Finalize()
{

}

void SeriesTableRenderStrategy::SetVolumeGO(VolumeGOPtr volume)
{
    if (mVolumeGO != volume){
        mVolumeGO = volume;
        mDirty = true;
    }
}

void SeriesTableRenderStrategy::SetMPRGO(MPRGOPtr mpr)
{
    if (mMPRGO != mpr){
        mMPRGO = mpr;
        mDirty = true;
        if (mMPRGO){
            mNeedRecalcViewProj = true;
        }
    }
}

void SeriesTableRenderStrategy::SetSeriesTableCacheGO(SeriesTableCacheGOPtr tableCache)
{
    if (mSeriesTableCacheGO != tableCache)
    {
        mSeriesTableCacheGO = tableCache;
        mDirty = true;
    }
}

void SeriesTableRenderStrategy::DrawFill(
    const std::vector<std::vector<Mcsf::Point2f>>& contours)
{
    std::vector<float*> vecBuffers;
    std::vector<int> vecSize;
    vecBuffers.reserve(contours.size());
    vecSize.reserve(contours.size());
    float xSum, ySum;
    int count;
    for (int i = 0; i < contours.size(); ++i)
    {
        if (contours[i].size() < 3) continue;
        int size = contours[i].size() + 2;
        float* buffer = new float[size * 2];
        xSum = ySum = 0.f;
        count = 0;
        for (int j = 0; j < contours[i].size(); ++j)
        {
            buffer[2 * (j + 1)] = contours[i][j].GetX();
            buffer[2 * (j + 1) + 1] = contours[i][j].GetY();
            xSum += buffer[2 * (j + 1)];
            ySum += buffer[2 * (j + 1) + 1];
            count++;
        }
        buffer[0] = xSum / count;
        buffer[1] = ySum / count;
        buffer[2 * (size - 1)] = buffer[2];
        buffer[2 * (size - 1) + 1] = buffer[3];
        vecBuffers.push_back(buffer);
        vecSize.push_back(size);
    }

    glClear(GL_STENCIL_BUFFER_BIT); 
    glClearStencil(0x0);
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);    // 禁用颜色缓存写入

    // 设置模板缓存操作函数为GL_INVERT
    glStencilFunc(GL_ALWAYS, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); 

    glDisable(GL_DEPTH_TEST);    // 禁用深度缓存

    // 绘制多边形
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    for (int i = 0; i < vecBuffers.size(); ++i)
    {
        glVertexPointer(2, GL_FLOAT, 0, vecBuffers[i]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vecSize[i]);
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    ////glEnable(GL_DEPTH_TEST);
    //// 重绘多边形,只绘制模板缓存值非0的像素
    glStencilFunc(GL_NOTEQUAL,0,0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

    // 再绘制一次所有的三角面片
    glEnableClientState(GL_VERTEX_ARRAY);
    for (int i = 0; i < vecBuffers.size(); ++i)
    {
        glVertexPointer(2, GL_FLOAT, 0, vecBuffers[i]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vecSize[i]);
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    glDisable(GL_STENCIL_TEST);

    for (int i = 0; i < vecBuffers.size(); ++i)
    {
        TPS_DEL_ARRAY(vecBuffers[i]);
    }
}

void SeriesTableRenderStrategy::CalcViewProjectionMatrix_i(){
    //////////////////////////////////////////////////////////////////////////
    //note:  copy from mpr render strategy of 3d engine, get the mpr projection matrix

    // get the normal of the plane in world coordinate
    Vector3D vNormalinWorld;
    mMPRGO->GetNormal(&vNormalinWorld);

    Point3D aVertexPoints[4];
    mMPRGO->GetVertices(aVertexPoints);

    // generate an orthogonal camera for the projection matrix
    // the camera is looking at the front face of the plane
    ProjectionCamera myCamera;
    myCamera.SetProjectionType(ProjectionCamera::PT_ORTHOGRAPHIC);
    myCamera.SetLookAt(mMPRGO->GetCenter());
    myCamera.SetEye(mMPRGO->GetCenter() + vNormalinWorld * 10.0);
    Vector3D vUp = aVertexPoints[3] - aVertexPoints[0];
    vUp.Normalize();
    myCamera.SetUpDirection(vUp);
    myCamera.SetNearClipDistance(8.0);
    myCamera.SetFarClipDistance(12.0);

    // check the aspect ratio
    // the camera should project the whole screen, or in the other word,
    // the camera should keep the same aspect ratio as the screen
    // the mpr plane should fit to the screen when keeping its own aspect ratio
    double dPlaneXinWorld = (aVertexPoints[1] - aVertexPoints[0]).Magnitude();
    double dPlaneYinWorld = (aVertexPoints[3] - aVertexPoints[0]).Magnitude();
    double dAspectRatioPlane = dPlaneXinWorld / dPlaneYinWorld;
    double dAspectRatioScreen = (double) mWidth / mHeight;
    Matrix4x4 matRescale;
    if (dAspectRatioScreen > dAspectRatioPlane)
    {
        myCamera.SetAspectRatio(dAspectRatioScreen);
        myCamera.SetOrthoWindow(dPlaneYinWorld * dAspectRatioScreen, dPlaneYinWorld);
        matRescale = MakeScale(Vector3D(dAspectRatioScreen / dAspectRatioPlane, 1.0, 1.0));
    }
    else
    {
        myCamera.SetAspectRatio(dAspectRatioScreen);
        myCamera.SetOrthoWindow(dPlaneXinWorld, dPlaneXinWorld / dAspectRatioScreen);
        matRescale = MakeScale(Vector3D(1.0, dAspectRatioPlane / dAspectRatioScreen, 1.0));
    }
    mViewProjectionMat = myCamera.GetViewProjectionMatrix();
    mNeedRecalcViewProj = false;
}
TPS_END_NAMESPACE  // end namespace tps