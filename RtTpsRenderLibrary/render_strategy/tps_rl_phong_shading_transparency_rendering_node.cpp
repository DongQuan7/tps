////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_phong_shading_transparency_rendering_node.cpp
/// 
///  \brief class PhongShadingTransprarencyNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/17
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_phong_shading_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "../tps_rl_shaders.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

PhongShadingTransprarencyNode::PhongShadingTransprarencyNode(int id)
    : ITransparencyRenderingNode(id)
{

}

PhongShadingTransprarencyNode::~PhongShadingTransprarencyNode()
{
    
}

void PhongShadingTransprarencyNode::Render(GLProgram* prog/* = nullptr*/)
{
    if (mCameraGO == nullptr || mShadedVOIGO == nullptr)
    {
        return;
    }

    //set default point light
    mCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
    //light setting
    float lightFactor[4] = 
    {
        mShadedVOIGO->GetAmbFactor(),
        mShadedVOIGO->GetDiffFactor(),
        mShadedVOIGO->GetSpecularFactor(),
        mShadedVOIGO->GetShininess()
    };
    //use white light
    float lightColor[3] = {1.0f, 1.0f, 1.0f};
    Point3D ptEye = mCameraGO->GetEye();
    Matrix4x4 matWorldView = mCameraGO->GetViewMatrix();
    Point3D ptLightPosition = matWorldView.Transform(ptEye);
    float lightPosition[3] = { ptLightPosition.x, ptLightPosition.y, ptLightPosition.z };

    if (prog)
    {
        prog->SetUniformf("vLightColor", lightColor, 3);
        prog->SetUniformf("vLightPosition", lightPosition, 3);
        prog->SetUniformf("vLightFactor", lightFactor, 4);
    }

    glViewport(0, 0, mWidth, mHeight);
    PureRender_i(prog);
    CHECK_GL_ERRORS;
}

void PhongShadingTransprarencyNode::BuildShaders_i()
{
    mStrShadingVertexShaders.push_back(Shaders::Get(IDR_SHADER_PhongShadingVert).c_str());
    mStrShadingFragmentShaders.push_back(Shaders::Get(IDR_SHADER_PhongShadingFrag).c_str());
}

TPS_END_NAMESPACE