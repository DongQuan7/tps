#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_contour_set_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"

#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"
//#include "RtTpsFramework/tps_fw_composable_graphic_object.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

MprRoiRenderStrategy::MprRoiRenderStrategy()
{

}

MprRoiRenderStrategy::~MprRoiRenderStrategy()
{

}

void MprRoiRenderStrategy::Render_i()
{
    if (mVolumeGO == nullptr || mMprGO == nullptr || /*mComposableCsGOCollection == nullptr || */
        mVOIGOCollection == nullptr)
    {
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glViewport(0, 0, mWidth, mHeight);
    glUseProgram(0);

    Camera camera;
    mMprGO->GetProjectionCamera(mWidth, mHeight, camera);
    auto matView = camera.GetViewMatrix();
    auto matPrj = camera.GetProjectionMatrix();
    auto matW2P = mVolumeGO->GetWorld2Patient();
    matView.Append(matW2P.Inverse());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matPrj._m);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matView._m);

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2.0f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

    Vector3D normal;
    mMprGO->GetNormal(&normal);
    normal = matW2P.Transform(normal);
    Point3D center = mMprGO->GetCenter();

    size_t dim[3];
    mVolumeGO->GetDimension(dim);
    Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
        0, dim[1], 0, 0,
        0, 0, dim[2], 0,
        -0.5, -0.5, -0.5, 1);
    Matrix4x4 matW2V = mVolumeGO->GetModel2World().Inverse();
    matW2V.Prepend(modelToVolume);
    Vector3D v = matW2V.Transform(normal);
    v.Normalize();
    bool isAxial = fabs(fabs(v.z) - 1.0) <= 1e-6;
    int slice_no = TPS_ROUND(matW2V.Transform(center).z);

    float color[4] = { 0.f, 0.f, 0.f, 1.f };
    auto voiList = mVOIGOCollection->GetVOIList();
    for (auto it = voiList.begin(); it != voiList.end(); ++it)
    {
        if (it->second == nullptr || !it->second->GetVisibility()) continue;

        it->second->GetColor(color);
        auto cs = it->second->GetContourSet();
        std::vector<Contour> contours;
        if (cs->normal.IsParallel2(normal))
        {
            auto matW2CS = matW2P;
            matW2CS.Prepend(cs->pat_to_vol);

            auto center_cs = matW2CS.Transform(center);
            int slice_id = int(TPS_ROUND(center_cs.z));
            auto it_pc = cs->page_contours.find(slice_id);
            if (it_pc == cs->page_contours.end())
            {
                //printf("\nno contours on current page.");
                continue;
            }
            contours = it_pc->second->contour_list;
        }
        else
        {
            Point3D vertices[4];
            mMprGO->GetVertices(vertices);
            for (int i = 0; i < 4; ++i)
            {
                vertices[i] = matW2P.Transform(vertices[i]);
            }
            PLANE plane(vertices[0], vertices[3], vertices[1]);
            auto sdf = it->second->GetSDF();
            //ROIShapeConverter::SliceSDFToContours(*sdf, plane, contours);
            ROIShapeConverter::SliceSDFToContours_CPU(*sdf, plane, contours);
            if (contours.empty()) continue;
        }

        if (isAxial && it->second->GetIsInterpolate(slice_no))
        {
            glLineStipple(1, 0x0F0F);
            glEnable(GL_LINE_STIPPLE);
        }

        it->second->GetColor(color);
        glColor4f(color[0], color[1], color[2], 1.f);
        for (int i = 0; i < contours.size(); ++i)
        {
            glVertexPointer(3, GL_DOUBLE, 0, contours[i].pt._Myfirst);
            glDrawArrays(GL_LINE_LOOP, 0, contours[i].VertexCount());
        }
        if(color[3] > 1e-6)
        {
            glColor4f(color[0], color[1],color[2],color[3]);
            DrawFill_i(contours);
        }
        glDisable(GL_LINE_STIPPLE);
    }

    glLineWidth(1.0f);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void MprRoiRenderStrategy::DrawFill_i(const std::vector<Contour>& contours)
{
    std::vector<double*> vecBuffers;
    std::vector<int> vecSize;
    vecBuffers.reserve(contours.size());
    vecSize.reserve(contours.size());
    for (int i = 0; i < contours.size(); ++i)
    {
        if (contours[i].VertexCount() < 3)
        {
            continue;
        }

        int size = contours[i].VertexCount() + 2;
        double* buffer = new double[size * 3];
        memcpy(buffer, contours[i].pt._Myfirst, 3 * sizeof(double));
        memcpy(buffer + 3, contours[i].pt._Myfirst, 3 * contours[i].VertexCount() * sizeof(double));
        memcpy(buffer + 3 * (size - 1), contours[i].pt._Myfirst, 3 * sizeof(double));
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
    for (int i = 0; i < vecBuffers.size(); ++i)
    {
        glVertexPointer(3, GL_DOUBLE, 0, vecBuffers[i]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vecSize[i]);
    }

    ////glEnable(GL_DEPTH_TEST);
    //// 重绘多边形,只绘制模板缓存值非0的像素
    glStencilFunc(GL_NOTEQUAL,0,0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

    // 再绘制一次所有的三角面片
    for (int i = 0; i < vecBuffers.size(); ++i)
    {
        glVertexPointer(3, GL_DOUBLE, 0, vecBuffers[i]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vecSize[i]);
    }

    glDisable(GL_STENCIL_TEST);
    for (int i = 0; i < vecBuffers.size(); ++i)
    {
        TPS_DEL_ARRAY(vecBuffers[i]);
    }
}

int MprRoiRenderStrategy::CacheChanges()
{
    if (mVolumeGO == nullptr || mMprGO == nullptr || 
        /*mComposableCsGOCollection == nullptr || */mVOIGOCollection == nullptr)
    {
        return -1;
    }

    if (mVolumeGO->IsDirty() || mMprGO->IsDirty() || 
        /*mComposableCsGOCollection->IsDirty() || */mVOIGOCollection->IsDirty())
    {
        mDirty = true;
    }
    return 0;
}

void MprRoiRenderStrategy::SetVolumeGO(const VolumeGOPtr& volGO)
{
    if (mVolumeGO != volGO)
    {
        mVolumeGO = volGO;
        mDirty = true;
    }
}

void MprRoiRenderStrategy::SetMPRGO(const MprGOPtr& mprGO)
{
    if (mMprGO != mprGO)
    {
        mMprGO = mprGO;
        mDirty = true;
    }
}

// void MprRoiRenderStrategy::SetComposableCSGOCollection(
//     const ComposableCSGOCollectionPtr& composableGO)
// {
//     if (mComposableCsGOCollection != composableGO)
//     {
//         mComposableCsGOCollection = composableGO;
//         mDirty = true;
//     }
// }

void MprRoiRenderStrategy::SetVOIGOCollection(const VOIGOCollectionPtr& voigoCollection)
{
    if (mVOIGOCollection != voigoCollection)
    {
        mVOIGOCollection = voigoCollection;
        mDirty = true;
    }
}
TPS_END_NAMESPACE