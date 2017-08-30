////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_transparency_scene_renderer.cpp
/// 
///  \brief class TransparencySceneRenderer implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_scene_renderer.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm_program.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_node_interface.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm.h"

#include <stdio.h>

TPS_BEGIN_NAMESPACE

TransparencySceneRenderer::TransparencySceneRenderer()
    : mIsInitialized(false), mImageWidth(0), mImageHeight(0)
{

}

TransparencySceneRenderer::~TransparencySceneRenderer()
{
    if (mAlgorithm) delete mAlgorithm;
}

bool TransparencySceneRenderer::Initialize(TransparencyRenderingAlgorithm* algorithm)
{
    if (mNodes.empty())
    {
        //if no node in renderer, return failure.
        TPS_LOG_DEV_ERROR<<"There is no rendering node in renderer.";
        return false;
    }

    if (mIsInitialized)
    {
        //already initialized.
        TPS_LOG_DEV_INFO<<"The renderer has already been initialized.";
        return true;
    }

    mAlgorithm = algorithm;
    if (mAlgorithm == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"TR algorithm is empty.";
        return false;
    }

    //clear old gl error
    glGetError();

    if (!InitNodes_i())
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize rendering nodes.";
        return false;
    }

    if (!mAlgorithm->Initialize(mNodes))
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize TR algorithm.";
        return false;
    }
    mIsInitialized = true;
    return true;
}

void TransparencySceneRenderer::Finalize()
{
    ///
    ClearNodes_i();
    mAlgorithm->Finalize();
    mIsInitialized = false;
}

void TransparencySceneRenderer::AddNode(ITransparencyRenderingNode* node)
{
    if (mIsInitialized)
    {
        //log renderer already initialized, can not add any node now.
        return;
    }
    mNodes.push_back(node);
}

void TransparencySceneRenderer::Resize(unsigned int width, unsigned int height)
{
    if (mImageWidth == width && mImageHeight == height) return;

    mImageWidth = width;
    mImageHeight = height;
    mAlgorithm->Resize(width, height);

    for (size_t i = 0; i < mNodes.size(); ++i)
    {
        mNodes[i]->Resize(width, height);
    }
}

void TransparencySceneRenderer::Render(GLuint targetFboId, GLenum buff)
{
    //clear old gl error
    glGetError();
    if (!mIsInitialized)
    {
        TPS_LOG_DEV_ERROR<<"The renderer has not been initialized yet.";
        return;
    }

    mAlgorithm->Render(targetFboId, buff);
}

bool TransparencySceneRenderer::InitNodes_i()
{
    for (size_t i = 0; i < mNodes.size(); ++i)
    {
        if (mNodes[i] == nullptr) continue;
        if (!mNodes[i]->Initialize())
        {
            //log
            return false;
        }
    }
    CHECK_GL_ERRORS;
    return true;
}

void TransparencySceneRenderer::ClearNodes_i()
{
    for (size_t i = 0; i < mNodes.size(); ++i)
    {
        mNodes[i]->Finalize();
        //Render不再负责nodes的生命周期
        //delete mNodes[i];
    }
    mNodes.clear();
}

void TransparencySceneRenderer::UseOQ(bool useOQ)
{
    mAlgorithm->SetUseOQ(useOQ);
}

void TransparencySceneRenderer::SetMaxPeelingNum(unsigned int num)
{
    mAlgorithm->SetMaxPeelingNum(num);
}

TPS_END_NAMESPACE