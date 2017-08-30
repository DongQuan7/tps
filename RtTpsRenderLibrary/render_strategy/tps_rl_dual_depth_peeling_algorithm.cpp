////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_dual_depth_peeling_algorithm.cpp
/// 
///  \brief class DualDepthPeelingAlgorithm implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/3/1
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dual_depth_peeling_algorithm.h"

#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm_program.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_node_interface.h"
#include "../tps_rl_shaders.h"

#define MAX_DEPTH 1.0

GLenum aDrawBuffers[] = 
{
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
};

TPS_BEGIN_NAMESPACE

DualDepthPeelingAlgorithm::DualDepthPeelingAlgorithm()
{

}

DualDepthPeelingAlgorithm::~DualDepthPeelingAlgorithm()
{

}

void DualDepthPeelingAlgorithm::Render(GLuint targetFboId, GLenum buff)
{
    if (!mInitialized) return;

    mPassNum = 0;

    m_uiFbo = targetFboId;

    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    InitPass_i();
    int currId = PeelingPass_i(targetFboId, buff);
    FinalPass_i(targetFboId, buff, currId);
    #ifdef _DEBUG
    printf("\nGeometry Passes: %d", mPassNum);
    #endif
}

void DualDepthPeelingAlgorithm::Resize(unsigned int width, unsigned int height)
{
    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE, mDepthTexId[i]);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, width, height,
            0, GL_RGB, GL_FLOAT, 0);
        glBindTexture(GL_TEXTURE_RECTANGLE, mFrontBlenderTexId[i]);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, width, height,
            0, GL_RGBA, GL_FLOAT, 0);
    }

    glBindTexture(GL_TEXTURE_RECTANGLE, mBackTexId);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
}

bool DualDepthPeelingAlgorithm::InitRenderTargets_i()
{
    glGenTextures(2, mDepthTexId);
    glGenTextures(2, mFrontBlenderTexId);
    glGenTextures(1, &mBackTexId);
    glGenFramebuffers(1, &mPeelingFboId);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE, mDepthTexId[i]);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_RECTANGLE, mFrontBlenderTexId[i]);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_RECTANGLE, mBackTexId);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    this->Resize(0, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, mPeelingFboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_RECTANGLE, mDepthTexId[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_RECTANGLE, mFrontBlenderTexId[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
        GL_TEXTURE_RECTANGLE, mDepthTexId[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
        GL_TEXTURE_RECTANGLE, mFrontBlenderTexId[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4,
        GL_TEXTURE_RECTANGLE, mBackTexId, 0);

    CHECK_GL_ERRORS;
    return true;
}

bool DualDepthPeelingAlgorithm::BuildPrograms_i()
{
    mInitProgram = new GLProgram();
    mInitProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FixedVert).c_str());
    mInitProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_DepthPeelingInitFrag).c_str());
    if (!mInitProgram->Link())
    {
        printf("Failed to link init pass program.");
        return false;
    }

    mBlendingProgram = new GLProgram();
    mBlendingProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FixedVert).c_str());
    mBlendingProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_RectTexMappingFrag).c_str());
    if (!mBlendingProgram->Link())
    {
        printf("Failed to link blending program.");
        return false;
    }

    mFinalProgram = new GLProgram();
    mFinalProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FixedVert).c_str());
    mFinalProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_DepthPeelingFinalFrag).c_str());
    if (!mFinalProgram->Link())
    {
        printf("Failed to link final program.");
        return false;
    }

    //get the shaders from node
    TRALGProgram* program;
    std::vector<const char*> vss, fss;
    for (unsigned int i = 0; i < mNodes.size(); ++i)
    {
        mNodes[i]->GetShadingShaders(vss, fss);
        program = this->CreateProgram();
        program->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_DepthPeelingPeelVert).c_str());
        program->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_DepthPeelingPeelFrag).c_str());
        program->AttachShadingShaders(vss, fss);
        if (!program->Link())
        {
            printf("Failed to link peeling program.");
            delete program;
            return false;
        }
        mPeelingPrograms.push_back(program);
    }

    //mTestTextureProgram = new GLProgram();
    //mTestTextureProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::strTestTextureVertShader.c_str());
    //mTestTextureProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::strTestTextureFragShader.c_str());
    //if (!mTestTextureProgram->Link())
    //{
    //    printf("Failed to link test program.");
    //    return false;
    //}
    CHECK_GL_ERRORS;
    return true;
}

void DualDepthPeelingAlgorithm::ClearRenderTargets_i()
{
    glDeleteTextures(2, mDepthTexId);
    glDeleteTextures(2, mFrontBlenderTexId);
    glDeleteTextures(1, &mBackTexId);
    glDeleteFramebuffers(1, &mPeelingFboId);
}

void DualDepthPeelingAlgorithm::ClearPrograms_i()
{
    delete mInitProgram; mInitProgram = nullptr;
    delete mBlendingProgram; mBlendingProgram = nullptr;
    delete mFinalProgram; mFinalProgram = nullptr;
    for (unsigned int i = 0; i < mPeelingPrograms.size(); ++i)
    {
        delete mPeelingPrograms[i];
    }
    mPeelingPrograms.clear();
}

void DualDepthPeelingAlgorithm::InitPass_i()
{
    // ---------------------------------------------------------------------
    // Initialize Min-Max Depth Buffer
    // ---------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, mPeelingFboId);

    // Render targets 1 and 2 store the front and back colors
    // Clear to 0.0 and use MAX blending to filter written color
    // At most one front color and one back color can be written every pass
    glDrawBuffers(2, &aDrawBuffers[1]);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
    glDrawBuffer(aDrawBuffers[0]);
    glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendEquation(GL_MAX);

    mInitProgram->Bind();
    for (size_t i = 0; i < mNodes.size(); ++i)
    {
        mNodes[i]->Render();
    }
    mInitProgram->Unbind();

    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    //mTestTextureProgram->Bind();
    //mTestTextureProgram->BindTexture(0, GL_TEXTURE_RECTANGLE, 
    //    mDepthTexId[0], "tex");
    //glCallList(mQuadListId);
    //mTestTextureProgram->Unbind();
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    mPassNum++;
    CHECK_GL_ERRORS
}

int DualDepthPeelingAlgorithm::PeelingPass_i(GLuint targetFboId, GLenum buff)
{
    int currId = 0;
    for (unsigned int pass = 1; mUseOQ || pass < mMaxPeelingNum; pass++)
    {
        mPassNum++;
        currId = pass % 2;
        int prevId = 1 - currId;
        int bufId = currId * 3;

        glBindFramebuffer(GL_FRAMEBUFFER, mPeelingFboId);
        glDrawBuffers(2, &aDrawBuffers[bufId+1]);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawBuffer(aDrawBuffers[bufId+0]);
        glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render target 0: RG32F MAX blending
        // Render target 1: RGBA MAX blending
        // Render target 2: RGBA MAX blending
        glDrawBuffers(3, &aDrawBuffers[bufId+0]);
        glBlendEquation(GL_MAX);

        for (size_t i = 0; i < mNodes.size(); ++i)
        {
            mPeelingPrograms[i]->Bind();
            mPeelingPrograms[i]->BindTexture(0, GL_TEXTURE_RECTANGLE, 
                mDepthTexId[prevId], "DepthBlenderTex");
            mPeelingPrograms[i]->BindTexture(1, GL_TEXTURE_RECTANGLE, 
                mFrontBlenderTexId[prevId], "FrontBlenderTex");
            mNodes[i]->Render(mPeelingPrograms[i]);
            mPeelingPrograms[i]->Unbind();
        }

        // Full screen pass to alpha-blend the back color
        // Over blending with the target frame buffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFboId);
        glDrawBuffer(buff);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (mUseOQ)
        {
            glBeginQuery(GL_SAMPLES_PASSED, mQueryId);
        }

        mBlendingProgram->Bind();
        mBlendingProgram->BindTexture(0, GL_TEXTURE_RECTANGLE, mBackTexId, "Tex");
        glCallList(mQuadListId);
        mBlendingProgram->Unbind();

        if (mUseOQ)
        {
            glEndQuery(GL_SAMPLES_PASSED);
            GLuint sample_count;
            glGetQueryObjectuiv(mQueryId, GL_QUERY_RESULT, &sample_count);
            if (sample_count == 0) break;
        }
    }

    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    //mTestTextureProgram->Bind();
    //mTestTextureProgram->BindTexture(0, GL_TEXTURE_RECTANGLE, 
    //    mFrontBlenderTexId[currId], "tex");
    //glCallList(mQuadListId);
    //mTestTextureProgram->Unbind();
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    CHECK_GL_ERRORS;
    return currId;
}

void DualDepthPeelingAlgorithm::FinalPass_i(GLuint targetFboId, GLenum buff, int currId)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFboId);
    glDrawBuffer(buff);

    // Final step of under blending, front color with back color
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    mFinalProgram->Bind();
    mFinalProgram->BindTexture(0, GL_TEXTURE_RECTANGLE, 
        mFrontBlenderTexId[currId], "FrontBlenderTex");
    glCallList(mQuadListId);
    mFinalProgram->Unbind();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    mPassNum++;

    CHECK_GL_ERRORS;
}

TPS_END_NAMESPACE