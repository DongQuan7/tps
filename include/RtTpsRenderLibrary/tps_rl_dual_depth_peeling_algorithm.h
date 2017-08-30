////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_transparency_rendering_algorithm.h
/// 
///  \brief class TransparencyRenderingAlgorithm declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/3/1
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DUAL_DEPTH_PEELING_ALGORITHM_H_
#define TPS_RL_DUAL_DEPTH_PEELING_ALGORITHM_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm.h"

TPS_BEGIN_NAMESPACE

class GLProgram;

class DualDepthPeelingAlgorithm : public TransparencyRenderingAlgorithm
{
public: 
    DualDepthPeelingAlgorithm();
    virtual ~DualDepthPeelingAlgorithm();

    virtual void Render(GLuint targetFboId, GLenum buff);

    virtual void Resize(unsigned int width, unsigned int height);

private:
    bool InitRenderTargets_i();
    bool BuildPrograms_i();

    void ClearRenderTargets_i();
    void ClearPrograms_i();

    void InitPass_i();
    int PeelingPass_i(GLuint targetFboId, GLenum buff);
    void FinalPass_i(GLuint targetFboId, GLenum buff, int currId);

    //render targets
    GLuint mPeelingFboId;
    GLuint mDepthTexId[2];
    GLuint mFrontBlenderTexId[2];
    //GLuint mBackTempTexId[2];
    GLuint mBackTexId;

    GLProgram* mInitProgram;
    GLProgram* mBlendingProgram;
    std::vector<GLProgram*> mPeelingPrograms;
    GLProgram* mFinalProgram;
    //GLProgram* mTestTextureProgram;

    GLuint m_uiFbo;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(DualDepthPeelingAlgorithm);
};

TPS_END_NAMESPACE

#endif // TPS_RL_DUAL_DEPTH_PEELING_ALGORITHM_H_
