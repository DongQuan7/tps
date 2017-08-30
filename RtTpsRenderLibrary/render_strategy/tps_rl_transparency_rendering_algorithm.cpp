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
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm_program.h"

TPS_BEGIN_NAMESPACE

TransparencyRenderingAlgorithm::TransparencyRenderingAlgorithm()
    : mPassNum(0), mUseOQ(true), mMaxPeelingNum(5), mInitialized(false) {}

TransparencyRenderingAlgorithm::~TransparencyRenderingAlgorithm() {}

bool TransparencyRenderingAlgorithm::Initialize(const std::vector<ITransparencyRenderingNode*>& nodes)
{
    if (mInitialized) return true;

    mNodes = nodes;

    //clear old gl error
    glGetError();

    if (!InitRenderTargets_i())
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize render targets.";
        return false;
    }

    if (!BuildPrograms_i())
    {
        TPS_LOG_DEV_ERROR<<"Failed to build common GL programs.";
        return false;
    }
    MakeFullScreenQuad_i();
    glGenQueries(1, &mQueryId);
    mInitialized = true;
    return true;
}

void TransparencyRenderingAlgorithm::Finalize()
{
    if (!mInitialized) return;
    ///
    ClearRenderTargets_i();
    ClearPrograms_i();
    glDeleteQueries(1, &mQueryId);
    glDeleteLists(mQuadListId, 1);
    mInitialized = false;
}

void TransparencyRenderingAlgorithm::MakeFullScreenQuad_i()
{
    mQuadListId = glGenLists(1);
    glNewList(mQuadListId, GL_COMPILE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0); 
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glEndList();
}

TRALGProgram* TransparencyRenderingAlgorithm::CreateProgram()
{
    return new TRALGProgram();
}

TPS_END_NAMESPACE