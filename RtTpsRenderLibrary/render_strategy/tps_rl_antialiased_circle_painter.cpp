////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_antialiased_circle_painter.cpp
/// 
///  \brief class AntialiasedCirclePainter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/12/11
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_antialiased_circle_painter.h"

#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "../tps_rl_shaders.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

AntialiasedCirclePainter::AntialiasedCirclePainter()
    : mInitialized(false), mCount(0), mDrawShadow(true), mProgram(nullptr){
}

AntialiasedCirclePainter::~AntialiasedCirclePainter(){
    if (mInitialized){
        if (mProgram != nullptr){
            delete mProgram;
        }

        glDeleteBuffers(2, mBufferID);
        glDeleteVertexArrays(1, &mVAOID);
        glDeleteTextures(4, mTexID);
    }
}

bool AntialiasedCirclePainter::Initialize(){
    if (mInitialized) return true;

    mProgram = new GLProgram();
    mProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_AntialiasedCircleVert).c_str());
    mProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_AntialiasedCircleFrag).c_str());
    if (!mProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link anti-aliased circle program.";
        return false;
    }
    unsigned int progID = mProgram->ID();
    //get locations
    mUniformLocations[0] = glGetAttribLocation(progID, "vVertex");
    mUniformLocations[1] = glGetUniformLocation(progID, "sCenters");
    mUniformLocations[2] = glGetUniformLocation(progID, "sRadius");
    mUniformLocations[3] = glGetUniformLocation(progID, "sColors");
    mUniformLocations[4] = glGetUniformLocation(progID, "sIsFilled"); 
    mUniformLocations[5] = glGetUniformLocation(progID, "iCount");
    mUniformLocations[6] = glGetUniformLocation(progID, "bDrawShadow");
    //mUniformLocations[5] = glGetUniformLocation(mProgramID, "isFilled"); 

    //init VAO
    GLfloat aVertex[12] = { -1.0, -1.0, 0.0, 
                            1.0, -1.0, 0.0, 
                            1.0, 1.0, 0.0, 
                            -1.0, 1.0, 0.0 };
    GLuint aIndex[6] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &mVAOID);
    glBindVertexArray(mVAOID);

    glGenBuffers(2, mBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aVertex), aVertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(mUniformLocations[0]);
    glVertexAttribPointer(mUniformLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aIndex), aIndex, GL_STATIC_DRAW);
    glBindVertexArray(0);

    //textures
    glGenTextures(4, mTexID);
    //centers
    glBindTexture(GL_TEXTURE_1D, mTexID[0]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //radius
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //colors
    glBindTexture(GL_TEXTURE_1D, mTexID[2]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_1D, 0);
    //is filled flag
    glBindTexture(GL_TEXTURE_1D, mTexID[3]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_1D, 0);

    mInitialized = true;
    return true;
}

//centers(x, y), radius(x), colors(r, g, b)
void AntialiasedCirclePainter::SetParas(float* centers, float* radius, 
    float* colors, bool* isFilledFlag, int count, bool drawShadow/* = true*/){
    
    glBindTexture(GL_TEXTURE_1D, mTexID[0]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, count, 0, GL_RG, GL_FLOAT, centers);
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, count, 0, GL_RED, GL_FLOAT, radius);
    glBindTexture(GL_TEXTURE_1D, mTexID[2]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, count, 0, GL_RGB, GL_FLOAT, colors);
    glBindTexture(GL_TEXTURE_1D, mTexID[3]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R8UI, count, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, isFilledFlag);
    glBindTexture(GL_TEXTURE_1D, 0);

    mCount = count;
    mDrawShadow = drawShadow;
}

void AntialiasedCirclePainter::Render(){
    if (!mInitialized || mCount == 0 || mProgram == nullptr) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glUseProgram(mProgram->ID());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, mTexID[0]);
    glUniform1i(mUniformLocations[1], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);
    glUniform1i(mUniformLocations[2], 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, mTexID[2]);
    glUniform1i(mUniformLocations[3], 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, mTexID[3]);
    glUniform1i(mUniformLocations[4], 3);

    glUniform1i(mUniformLocations[5], mCount);
    glUniform1i(mUniformLocations[6], mDrawShadow);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    /*glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(mVAOID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);

    glUseProgram(0);
    glPopAttrib();
}
TPS_END_NAMESPACE  // end namespace tps