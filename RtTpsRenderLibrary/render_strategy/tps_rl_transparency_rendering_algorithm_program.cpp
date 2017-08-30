////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_transparency_rendering_algorithm_program.cpp
/// 
///  \brief class TRALGProgram implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/03/01
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm_program.h"

TPS_BEGIN_NAMESPACE

TRALGProgram::TRALGProgram() : GLProgram()
{
    
}

TRALGProgram::~TRALGProgram()
{

}

void TRALGProgram::AttachShadingShaders(
    const std::vector<const char*>& vss, 
    const std::vector<const char*>& fss)
{
    for (int i = 0; i < vss.size(); ++i)
    {
        GLProgram::AttachShader(GLProgram::Vertex_Shader, vss[i]);
    }
    for (int i = 0; i < fss.size(); ++i)
    {
        GLProgram::AttachShader(GLProgram::Fragment_Shader, fss[i]);
    }
}

TPS_END_NAMESPACE