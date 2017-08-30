////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_voi_transparency_rendering_node.cpp
/// 
///  \brief class VOITransparencyRenderingNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_voi_transparency_rendering_node.h"
#include "../tps_rl_shaders.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"

#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_vertex_array_buffer.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_vertex_element_buffer.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "RtTpsRenderLibrary/tps_rl_gpu_marching_cubes.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

VOITransparencyRenderingNode::VOITransparencyRenderingNode(int id)
    : PhongShadingTransprarencyNode(id)
{

}

VOITransparencyRenderingNode::~VOITransparencyRenderingNode()
{

}

bool VOITransparencyRenderingNode::Initialize_i()
{
    mGpuMC = new GPUMarchingCubes();
    if (!mGpuMC->Initialize())
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize GPUMarchingCubes.";
        return false;
    }
    return true;
}

void VOITransparencyRenderingNode::Finalize()
{
    mGpuMC->Finalize();
    TPS_DEL_PTR(mGpuMC);
}

void VOITransparencyRenderingNode::PureRender_i(GLProgram* prog)
{
    if (mVolumeGO == nullptr || mVOICollectionGO == nullptr)
    {
        return;
    }

    Matrix4x4 matPatient2View = mCameraGO->GetViewMatrix();
    matPatient2View.Append(mVolumeGO->GetWorld2Patient().Inverse());
    Matrix4x4 matProjection = mCameraGO->GetProjectionMatrix();

    unsigned int vbo, nbo, vertexCount;
    auto voiList = mVOICollectionGO->GetVOIList();
    for (auto it = voiList.begin(); it != voiList.end(); ++it)
    {
        if (it->second == nullptr || !it->second->GetVisibility() || 
            !it->second->IsShaded() || it->second->GetAlpha3D() <= 0.01)
        {
            continue;
        }

        if (it->second->IsMeshDataDirty())
        {
            CalcMesh_i(it->second);
        }
        it->second->GetMeshBuffer(vbo, nbo);
        it->second->GetMeshVertexCount(vertexCount);
        RenderVoiMesh_i(prog, it->second, vbo, nbo, 
            vertexCount, matPatient2View, matProjection);
    }
}

void VOITransparencyRenderingNode::BuildShaders_i()
{
    PhongShadingTransprarencyNode::BuildShaders_i();
    mStrShadingFragmentShaders.push_back(
        Shaders::Get(IDR_SHADER_DepthPeelingCalcFragColorFrag).c_str());
}

void VOITransparencyRenderingNode::CalcMesh_i(VOIGOPtr voi)
{
    auto sdf = voi->GetSDF();
    MCData mcData;
    mcData.fIsoLevel = sdf->GetIsoLevel();
    sdf->GetSize(mcData.iDim);
    sdf->GetSize(mcData.iCalcDim);
    sdf->GetGLFormat(mcData.eType, mcData.eFormat, mcData.eInternalFormat);
    mcData.iBytesPerPixel = sizeof(float);
    mcData.bEnableSmoothing = false;
    mcData.pData = sdf->GetData();

//    auto subVolume = voi->GetSubVolume();
//    MCData mcData;
//    mcData.iBytesPerPixel = sizeof(char);
//    mcData.fIsoLevel = 0.5f;
//    mcData.eInternalFormat = GL_R8_SNORM;
//    mcData.eFormat = GL_RED;
//    mcData.eType = GL_BYTE;
//    //mcData.eInternalFormat = GL_R8I;
//    //mcData.eFormat = GL_RED_INTEGER;
//    //mcData.eType = GL_BYTE;
//    mcData.pData = subVolume->GetData();
//    subVolume->GetSize(mcData.iDim[0], mcData.iDim[1], mcData.iDim[2]);
//    //mcData.bEnableSmoothing = false;
//    //mcData.iCalcDim[0] = mcData.iDim[0];
//    //mcData.iCalcDim[1] = mcData.iDim[1];
//    //mcData.iCalcDim[2] = mcData.iDim[2];
//
//    mcData.bEnableSmoothing = true;
//    mcData.iSmoothedDim[0] = mcData.iDim[0];
//    mcData.iSmoothedDim[1] = mcData.iDim[1];
//    mcData.iSmoothedDim[2] = mcData.iDim[2];
////     double spacing[3];
////     mVolumeGO->GetSpacing(spacing);
////     double min_spacing = TPS_MIN(TPS_MIN(spacing[0], spacing[1]), spacing[2]);
////     mcData.iSmoothedDim[0] = ceil(mcData.iDim[0] * spacing[0] / min_spacing);
////     mcData.iSmoothedDim[1] = ceil(mcData.iDim[1] * spacing[1] / min_spacing);
////     mcData.iSmoothedDim[2] = ceil(mcData.iDim[2] * spacing[2] / min_spacing);

    GLint currPrg;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currPrg);
    unsigned int vbo, nbo, vertexCount;
    voi->GetMeshBuffer(vbo, nbo);
    mGpuMC->SetData(mcData);
    mGpuMC->Calculate(vbo, nbo, vertexCount);
    voi->SetMeshVertexCount(vertexCount);
    voi->SetMeshDataDirty(false);
    glUseProgram(currPrg);
}

void VOITransparencyRenderingNode::RenderVoiMesh_i(
    GLProgram* prog, const VOIGOPtr& voi, unsigned int vboID, 
    unsigned int nboID, unsigned int vertexCount, 
    const Matrix4x4& matPatient2View, const Matrix4x4& matProjection)
{
    Matrix4x4 matModelView = matPatient2View;
    matModelView.Append(voi->GetSDF()->GetPatToSDF().Inverse());

    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    int displayMode = mShadedVOIGO->GetDisplayMode();
    glPolygonMode(GL_FRONT_AND_BACK, displayMode == 1 ? GL_FILL : GL_LINE);

    if (prog)
    {
        float color[4];
        //need not do null validation again
        voi->GetColor(color);
        color[3] = voi->GetAlpha3D();
        prog->SetUniformf("vColor", color, 4);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)NULL);
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, nboID);
    glNormalPointer(GL_FLOAT, 0, (GLfloat*)NULL);
    glEnableClientState(GL_NORMAL_ARRAY);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
TPS_END_NAMESPACE
