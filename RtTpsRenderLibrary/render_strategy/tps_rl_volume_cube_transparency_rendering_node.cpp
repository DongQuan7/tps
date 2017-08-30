////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_volume_cube_transparency_rendering_node.cpp
/// 
///  \brief class VolumeCubeTRNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/3/9
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_volume_cube_transparency_rendering_node.h"
#include "../tps_rl_shaders.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

VolumeCubeTRNode::VolumeCubeTRNode(int id) : PhongShadingTransprarencyNode(id)
{

}

VolumeCubeTRNode::~VolumeCubeTRNode()
{

}

bool VolumeCubeTRNode::Initialize_i()
{
    glGenBuffers(1, &mVboId);
    glGenBuffers(1, &mEboId);

    float vertices[24] = 
    {
        0.f, 0.f, 0.f,  1.f, 0.f, 0.f,  1.f, 1.f, 0.f,  0.f, 1.f, 0.f, 
        0.f, 0.f, 1.f,  1.f, 0.f, 1.f,  1.f, 1.f, 1.f,  0.f, 1.f, 1.f, 
    };
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int idx[24] = 
    {
        0, 1, 2, 3,  4, 7, 6, 5,  1, 5, 6, 2,  0, 3, 7, 4, 
        2, 6, 7, 3,  0, 4, 5, 1
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

void VolumeCubeTRNode::Finalize()
{
    glDeleteBuffers(1, &mVboId);
    glDeleteBuffers(1, &mEboId);
}

void VolumeCubeTRNode::PureRender_i(GLProgram* prog)
{
    if (mVolumeGO == nullptr)
    {
        return;
    }

    Matrix4x4 matModelView = mVolumeGO->GetModel2World();
    matModelView.Prepend(mCameraGO->GetViewMatrix());
    Matrix4x4 matProjection = mCameraGO->GetProjectionMatrix();

    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glEnableClientState(GL_VERTEX_ARRAY);

    if (prog)
    {
        float color[4] = {0.38f, 0.66f, 0.83f, 1.0f};
        prog->SetUniformf("vColor", color, 4);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboId);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void VolumeCubeTRNode::BuildShaders_i()
{
    PhongShadingTransprarencyNode::BuildShaders_i();
    mStrShadingFragmentShaders.push_back(
        Shaders::Get(IDR_SHADER_DepthPeelingCalcFragColorFrag).c_str());
}

TPS_END_NAMESPACE
