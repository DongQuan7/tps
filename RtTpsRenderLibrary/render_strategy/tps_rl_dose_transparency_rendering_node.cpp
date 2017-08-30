////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_dose_transparency_rendering_node.cpp
/// 
///  \brief class DoseTransparencyRenderingNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/21
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dose_transparency_rendering_node.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "../tps_rl_shaders.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

DoseTransparencyRenderingNode::DoseTransparencyRenderingNode(int id)
    : PhongShadingTransprarencyNode(id)
{

}

DoseTransparencyRenderingNode::~DoseTransparencyRenderingNode()
{

}

bool DoseTransparencyRenderingNode::Initialize_i()
{
    glGenBuffers(1, &mVboId);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mNboId);
    glBindBuffer(GL_ARRAY_BUFFER, mNboId);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mEboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

void DoseTransparencyRenderingNode::Finalize()
{
    glDeleteBuffers(1, &mVboId);
    glDeleteBuffers(1, &mNboId);
    glDeleteBuffers(1, &mEboId);
}

//rendering logic is copied from old ShadedVOIStrategy, need refined later
void DoseTransparencyRenderingNode::PureRender_i(GLProgram* prog)
{
    if (mVolumeGO == nullptr || mDoseMeshCollectionGO == nullptr)
    {
        return;
    }

    Matrix4x4 matP2W = mVolumeGO->GetWorld2Patient().Inverse();
    Matrix4x4 matModelView = mCameraGO->GetViewMatrix();
    matModelView.Append(matP2W);
    Matrix4x4 matProjection = mCameraGO->GetProjectionMatrix();

    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    auto doseMeshCollection = mDoseMeshCollectionGO->GetAllDoseMeshGo();
    for (int index = 0; index < doseMeshCollection.size();++index)
    {
        auto m_spDoseMeshGO = doseMeshCollection[index];
        if(!m_spDoseMeshGO->GetValidFlag()) continue;
        int nQuadPoint = 0;
        float* pVertex = nullptr;
        float* pNormal = nullptr;
        unsigned int* pIndex = nullptr;
        nQuadPoint = m_spDoseMeshGO->GetElementBuffer(&pVertex, &pNormal, &pIndex);
        if (nQuadPoint > 0)
        {
            if(prog) 
            {
                float color[4];
                float fRed, fGreen, fBlue, fAlpha;
                m_spDoseMeshGO->GetDoseColor(&fRed, &fGreen, &fBlue, &fAlpha);
                color[0] = fRed;
                color[1] = fGreen;
                color[2] = fBlue;
                color[3] = fAlpha;
                prog->SetUniformf("vColor", color, 4);
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mVboId);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nQuadPoint * 3, pVertex, GL_STATIC_DRAW);
            glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)NULL);
            glEnableClientState(GL_VERTEX_ARRAY);

            glBindBuffer(GL_ARRAY_BUFFER, mNboId);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nQuadPoint * 3, pNormal, GL_STATIC_DRAW);
            glNormalPointer(GL_FLOAT, 0, (GLfloat*)NULL);
            glEnableClientState(GL_NORMAL_ARRAY);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* nQuadPoint, pIndex, GL_STATIC_DRAW);

            glDrawElements(GL_QUAD_STRIP, nQuadPoint, GL_UNSIGNED_INT, NULL);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
        }
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void DoseTransparencyRenderingNode::BuildShaders_i()
{
    PhongShadingTransprarencyNode::BuildShaders_i();
    mStrShadingFragmentShaders.push_back(Shaders::Get(IDR_SHADER_DepthPeelingCalcFragColorFrag).c_str());
}
TPS_END_NAMESPACE