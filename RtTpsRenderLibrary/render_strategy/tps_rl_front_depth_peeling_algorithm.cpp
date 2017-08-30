////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_front_depth_peeling_algorithm.cpp
/// 
///  \brief class FrontDepthPeelingAlgorithm implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/3/1
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_front_depth_peeling_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm_program.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_node_interface.h"
#include "../tps_rl_shaders.h"

TPS_BEGIN_NAMESPACE

FrontDepthPeelingAlgorithm::FrontDepthPeelingAlgorithm()
    : mWidth(0), mHeight(0), mSampleCount(4)
{

}

FrontDepthPeelingAlgorithm::~FrontDepthPeelingAlgorithm()
{

}

void FrontDepthPeelingAlgorithm::Render(GLuint targetFboId, GLenum buff)
{
    if (!mInitialized) return;

    mPassNum = 0;
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    glDisable(GL_BLEND);

    //1. init pass
    glBindFramebuffer(GL_FRAMEBUFFER, mMSFboId[0]);
    CHECK_GL_ERRORS;
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearDepth(1.0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CHECK_GL_ERRORS;
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    for (unsigned int i = 0; i < mNodes.size(); ++i)
    {
        mInitPrograms[i]->Bind();
        mNodes[i]->Render(mInitPrograms[i]);
        mInitPrograms[i]->Unbind();
    }
    mPassNum++;
    CHECK_GL_ERRORS;

    //2. peeling pass
    int currId = 0;
    int prevId = 0;
    int numLayers = (mMaxPeelingNum - 1) * 2;
    for (int layer = 1; mUseOQ || layer < numLayers; layer++)
    {
        currId = layer % 2;
        prevId = 1 - currId;
        mPassNum++;

        glBindFramebuffer(GL_FRAMEBUFFER, mMSFboId[currId]);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        //copy texture
        mCopyTextureProgram->Bind();
        mCopyTextureProgram->BindTexture(0, GL_TEXTURE_2D_MULTISAMPLE, 
            mColorMSTexId[prevId], "ColorTex");
        glCallList(mQuadListId);
        mCopyTextureProgram->Unbind();

        glEnable(GL_DEPTH_TEST);
        //need blending for the pixels with same depth
        if (mUseOQ) 
        {
            glBeginQuery(GL_ANY_SAMPLES_PASSED, mQueryId);
        }

        for (unsigned int i = 0; i < mNodes.size(); ++i)
        {
            mPeelingPrograms[i]->Bind();
            mPeelingPrograms[i]->BindTexture(0, GL_TEXTURE_2D_MULTISAMPLE, 
                mDepthMSTexId[prevId], "DepthTex");
            mPeelingPrograms[i]->BindTexture(1, GL_TEXTURE_2D_MULTISAMPLE, 
                mColorMSTexId[prevId], "ColorTex");
            mPeelingPrograms[i]->SetUniformi("iSampleCount", &mSampleCount, 1);
            mNodes[i]->Render(mPeelingPrograms[i]);
            mPeelingPrograms[i]->Unbind();
        }

        if (mUseOQ) {
            glEndQuery(GL_ANY_SAMPLES_PASSED);
            GLuint any_passed;
            glGetQueryObjectuiv(mQueryId, GL_QUERY_RESULT, &any_passed);
            if (!any_passed) {
                break;
            }
        }

        CHECK_GL_ERRORS;
    }

    //3. final pass
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlenderFboId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMSFboId[currId]);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFboId);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glDrawBuffer(buff);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    mFinalProgram->Bind();
    mFinalProgram->BindTexture(0, GL_TEXTURE_RECTANGLE, 
        mBlenderTexId, "ColorTex");
    glCallList(mQuadListId);
    mFinalProgram->Unbind();
    mPassNum++;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    CHECK_GL_ERRORS;

// #ifdef _DEBUG
//     printf("\nGeometry Passes: %d", mPassNum);
// #endif
}

void FrontDepthPeelingAlgorithm::Resize(unsigned int width, unsigned int height)
{
    mWidth = width;
    mHeight = height;

    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepthMSTexId[i]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSampleCount, 
            GL_DEPTH32F_STENCIL8, width, height,GL_FALSE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorMSTexId[i]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 
            mSampleCount, GL_RGBA, width, height,GL_FALSE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    }

    glBindTexture(GL_TEXTURE_RECTANGLE, mBlenderTexId);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA,
        width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
}

bool FrontDepthPeelingAlgorithm::InitRenderTargets_i()
{
    glGenTextures(2, mDepthMSTexId);
    glGenTextures(2, mColorMSTexId);
    glGenFramebuffers(2, mMSFboId);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepthMSTexId[i]);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorMSTexId[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, mMSFboId[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT/*GL_DEPTH_ATTACHMENT*/,
            GL_TEXTURE_2D_MULTISAMPLE, mDepthMSTexId[i], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D_MULTISAMPLE, mColorMSTexId[i], 0);
    }

    glGenTextures(1, &mBlenderTexId);
    glBindTexture(GL_TEXTURE_RECTANGLE, mBlenderTexId);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &mBlenderFboId);
    glBindFramebuffer(GL_FRAMEBUFFER, mBlenderFboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_RECTANGLE, mBlenderTexId, 0);

    this->Resize(0, 0);
    CHECK_GL_ERRORS;
    return true;
}

bool FrontDepthPeelingAlgorithm::BuildPrograms_i()
{
    TRALGProgram* program;
    std::vector<const char*> vss, fss;
    for (unsigned int i = 0; i < mNodes.size(); ++i)
    {
        mNodes[i]->GetShadingShaders(vss, fss);
        program = this->CreateProgram();
        program->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FrontPeelingInitVert).c_str());
        program->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_FrontPeelingInitFrag).c_str());
        program->AttachShadingShaders(vss, fss);
        if (!program->Link())
        {
            TPS_LOG_DEV_ERROR<<"Failed to link init program.";
            printf("Failed to link init program.");
            delete program;
            return false;
        }
        mInitPrograms.push_back(program);

        program = this->CreateProgram();
        program->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FrontPeelingVert).c_str());
        program->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_FrontPeelingFrag).c_str());
        program->AttachShadingShaders(vss, fss);
        if (!program->Link())
        {
            TPS_LOG_DEV_ERROR<<"Failed to link peeling program.";
            printf("Failed to link peeling program.");
            delete program;
            return false;
        }
        mPeelingPrograms.push_back(program);
    }

    mFinalProgram = new GLProgram();
    mFinalProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FrontPeelingFinalVert).c_str());
    mFinalProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_FrontPeelingFinalFrag).c_str());
    if (!mFinalProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link final pass program.";
        printf("Failed to link final pass program.");
        return false;
    }

    mCopyTextureProgram = new GLProgram();
    mCopyTextureProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FrontPeelingCopyTexVert).c_str());
    mCopyTextureProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_FrontPeelingCopyTexFrag).c_str());
    if (!mCopyTextureProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link copy texture program.";
        printf("Failed to link copy texture program.");
        return false;
    }

    CHECK_GL_ERRORS;
    return true;
}

void FrontDepthPeelingAlgorithm::ClearRenderTargets_i()
{
    glDeleteFramebuffers(2, mMSFboId);
    glDeleteFramebuffers(1, &mBlenderFboId);
    glDeleteTextures(2, mDepthMSTexId);
    glDeleteTextures(2, mColorMSTexId);
    glDeleteTextures(1, &mBlenderTexId);
}

void FrontDepthPeelingAlgorithm::ClearPrograms_i()
{
    for (unsigned int i = 0; i < mInitPrograms.size(); ++i)
    {
        delete mInitPrograms[i];
        delete mPeelingPrograms[i];
    }
    mInitPrograms.clear();
    mPeelingPrograms.clear();
    delete mFinalProgram; mFinalProgram = nullptr;
    delete mCopyTextureProgram; mCopyTextureProgram = nullptr;
}

TPS_END_NAMESPACE