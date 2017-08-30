////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_mpr_transparency_rendering_node.cpp
/// 
///  \brief class MprTransparencyRenderingNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/21
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_transparency_rendering_node.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "../tps_rl_shaders.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsROIUtility/tps_ru_marching_cubes_tables.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

using namespace Mcsf::MedViewer3D;

double MPR_TOLERANCE = 0.01;

TPS_BEGIN_NAMESPACE

MprTransparencyRenderingNode::MprTransparencyRenderingNode(int id)
    : ITransparencyRenderingNode(id)
{

}

MprTransparencyRenderingNode::~MprTransparencyRenderingNode()
{

}

bool MprTransparencyRenderingNode::Initialize_i()
{
    GLfloat aVertex[12] = { -0.5, -0.5, 0.0, 
        0.5, -0.5, 0.0, 
        0.5, 0.5, 0.0, 
        -0.5, 0.5, 0.0 };
    GLuint aIndex[6] = { 0, 1, 3, 1, 2, 3 };
    glGenBuffers(1, &mVboId);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aVertex), aVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mEboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aIndex), aIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

void MprTransparencyRenderingNode::Finalize()
{
    glDeleteBuffers(1, &mVboId);
    glDeleteBuffers(1, &mEboId);
}

void MprTransparencyRenderingNode::Render(GLProgram* prog/* == nullptr*/)
{
    if (mShadedVOIGO == nullptr || mVolumeGO == nullptr)
    {
        return;
    }

    int section = mShadedVOIGO->GetMprSection();
    MprGOPtr mprGO = section == AXIAL ? mMprAxialGO : 
        (section == CORONAL ? mMprCoronalGO : mMprSagittalGO);
    if (mprGO == nullptr) return;
    float alpha = 1.0f - mShadedVOIGO->GetMprTransparency3D();
    if(alpha < 1e-6) return;

    int vertexCount = PrepareVertexBuffer_i(mprGO);
    if (vertexCount == 0) return;
    if (prog)
    {
        this->PrepareProgram_i(prog, mprGO);
    }

    mCameraGO->AdaptFovViaAspect((double)mWidth/mHeight);
    Matrix4x4 matModel2World = mprGO->GetModel2World();
    Matrix4x4 matModelView = matModel2World;
    matModelView.Prepend(mCameraGO->GetViewMatrix());
    Matrix4x4 matProj = mCameraGO->GetProjectionMatrix();
    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProj._m);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)NULL);
    glEnableClientState(GL_VERTEX_ARRAY);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void MprTransparencyRenderingNode::BuildShaders_i()
{
    mStrShadingVertexShaders.push_back(Shaders::Get(IDR_SHADER_Mpr3DVert).c_str());
    mStrShadingVertexShaders.push_back(Shaders::Get(IDR_SHADER_MprCastingVert).c_str());
    mStrShadingFragmentShaders.push_back(Shaders::Get(IDR_SHADER_Mpr3DFrag).c_str());
    mStrShadingFragmentShaders.push_back(Shaders::Get(IDR_SHADER_MprCastingFrag).c_str());
}

void MprTransparencyRenderingNode::PrepareProgram_i(GLProgram* prog, MprGOPtr mprGO)
{
    if (prog == nullptr || mprGO == nullptr) return;

    Matrix4x4 matModel2World = mprGO->GetModel2World();
    Matrix4x4 matWorld2VolumeM = mVolumeGO->GetModel2World().Inverse();
    Matrix4x4 matMprModel2VolumeModel = matWorld2VolumeM;
    matMprModel2VolumeModel.Append(matModel2World);
    float matMprM2VolumeM[16];
    for (int i=0; i<16; ++i){
        matMprM2VolumeM[i] = static_cast<float>(matMprModel2VolumeModel._m[i]);
    }
    prog->SetUniformf("matMPRModel2VolumeModel", matMprM2VolumeM, 16);

    //set casting vector
    Vector3D normal;
    mprGO->GetNormal(&normal);
    double thickness = mprGO->GetThickness();
    Vector3D castingVector = thickness * normal;
    float castingV[4] = { castingVector.x, castingVector.y, castingVector.z, 0.f };
    prog->SetUniformf("vCastingVector", castingV, 4);

    float fSteps = 1.0;
    double dMinThicknessInWorld = 0.0;
    TpsMprHelper::GetMinThickness(mVolumeGO, mprGO, dMinThicknessInWorld);
    if (thickness < dMinThicknessInWorld *(1 +  MPR_TOLERANCE)){
        fSteps = 1.0;
    }
    else{
        double dVoxelSpacingInWorld = 0.0;
        double dVoxelSpacingP = 0.0;
        MPRAPI::GetMinSpacing(mVolumeGO, mprGO, &dVoxelSpacingInWorld, &dVoxelSpacingP);
        fSteps = (float)std::max(1.0, std::ceil((thickness / dVoxelSpacingInWorld) - MPR_TOLERANCE));
    }
    prog->SetUniformf("fSteps", &fSteps, 1);

    std::shared_ptr<Texture3D> texVolume = mVolumeGO->GetTexture3D();
    if (texVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"The volume's texture is null.";
        return;
    }
    prog->BindTexture(TR_NODE_START_TEX_UNIT, GL_TEXTURE_3D, texVolume->GetID(), "sVolumeData");
    double wl[2];
    mprGO->GetWindowing(wl[0], wl[1]);
    mVolumeGO->RegulateWindowing(&wl[0], &wl[1]);
    float fWL[2] = {wl[0], wl[1]};
    prog->SetUniformf("vWindowLevel", fWL, 2);
    unsigned int pseudocolorTexID = mprGO->GetPseudoColorTexID();
    prog->BindTexture(TR_NODE_START_TEX_UNIT + 1, GL_TEXTURE_1D, 
        pseudocolorTexID, "sPseudoColorTable");
    unsigned int pseudocolorTableSize = mprGO->GetPseudoColorTexSize();
    prog->SetUniformui("uiPseudoColorTableSize", &pseudocolorTableSize, 1);

    //set color inverted
    int iInverted = mprGO->IsColorInverted();
    prog->SetUniformi("isInverted", &iInverted, 1);
    float alpha = 1.0f - mShadedVOIGO->GetMprTransparency3D();
    prog->SetUniformf("fAlpha", &alpha, 1);
    int iDrawEdge = 0;
    prog->SetUniformi("bDrawEdge", &iDrawEdge, 1);
}

int MprTransparencyRenderingNode::PrepareVertexBuffer_i(MprGOPtr mprGO)
{
    Matrix4x4 matVolModelToMprModel = mVolumeGO->GetModel2World();
    matVolModelToMprModel.Prepend(mprGO->GetModel2World().Inverse());
    //“ª¥ŒMCÀ„∑®
    Point3D vertices[8] = 
    {
        Point3D(0, 0, 0), //
        Point3D(1, 0, 0), //
        Point3D(1, 1, 0), //
        Point3D(0, 1, 0), //
        Point3D(0, 0, 1), //
        Point3D(1, 0, 1), //
        Point3D(1, 1, 1), //
        Point3D(0, 1, 1)  //
    };
    int cubeIndex = 0;
    for (int i = 0; i < 8; ++i)
    {
        vertices[i] = matVolModelToMprModel.Transform(vertices[i]);
        if (vertices[i].z > 0.0)
        {
            cubeIndex |= (0x01 << i);
        }
    }
    if (cubeIndex == 0 || cubeIndex == 255) return 0;
    float vertexBuffer[45];
    int idx;
    Point3D vert;
    int vertexCount = 0;
    for (int i = 0; i < 16; ++i)
    {
        idx = MarchingCubesTables::mTriTable[cubeIndex][i];
        if (idx == -1)
        {
            break;
        }
        vert = this->VertexInterpolation_i(vertices, idx);
        vertexBuffer[3 * vertexCount] = static_cast<float>(vert.x);
        vertexBuffer[3 * vertexCount + 1] = static_cast<float>(vert.y);
        vertexBuffer[3 * vertexCount + 2] = static_cast<float>(vert.z);
        vertexCount++;
    }
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertexCount * sizeof(float), vertexBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vertexCount;
}

Point3D MprTransparencyRenderingNode::VertexInterpolation_i(
    const Point3D* vertices, int index)
{
    Point3D start, end;
    switch(index)
    {
    case 0:
        start = vertices[0];
        end = vertices[1];
        break;
    case 1:
        start = vertices[1];
        end = vertices[2];
        break;
    case 2:
        start = vertices[2];
        end = vertices[3];
        break;
    case 3:
        start = vertices[0];
        end = vertices[3];
        break;
    case 4:
        start = vertices[4];
        end = vertices[5];
        break;
    case 5:
        start = vertices[5];
        end = vertices[6];
        break;
    case 6:
        start = vertices[6];
        end = vertices[7];
        break;
    case 7:
        start = vertices[4];
        end = vertices[7];
        break;
    case 8:
        start = vertices[0];
        end = vertices[4];
        break;
    case 9:
        start = vertices[1];
        end = vertices[5];
        break;
    case 10:
        start = vertices[2];
        end = vertices[6];
        break;
    case 11:
        start = vertices[3];
        end = vertices[7];
        break;
    }

    Vector3D vec = (0.0 - start.z) / (end.z - start.z) * (end - start);
    return start + vec;
}
TPS_END_NAMESPACE