////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_front_depth_peeling_algorithm.h
/// 
///  \brief class FrontDepthPeelingAlgorithm declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/3/1
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_FRONT_DEPTH_PEELING_ALGORITHM_H_
#define TPS_RL_FRONT_DEPTH_PEELING_ALGORITHM_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_algorithm.h"

TPS_BEGIN_NAMESPACE

class GLProgram;

class FrontDepthPeelingAlgorithm : public TransparencyRenderingAlgorithm
{
public:
    FrontDepthPeelingAlgorithm();
    virtual ~FrontDepthPeelingAlgorithm();

    virtual void Render(GLuint targetFboId, GLenum buff);

    virtual void Resize(unsigned int width, unsigned int height);

private:

    bool InitRenderTargets_i();
    bool BuildPrograms_i();

    void ClearRenderTargets_i();
    void ClearPrograms_i();

    int mSampleCount;

    std::vector<GLProgram*> mInitPrograms;
    std::vector<GLProgram*> mPeelingPrograms;
    GLProgram* mFinalProgram;
    GLProgram* mCopyTextureProgram;

    GLuint mDepthMSTexId[2];
    GLuint mColorMSTexId[2];
    GLuint mMSFboId[2];
    GLuint mBlenderFboId;
    GLuint mBlenderTexId;

    unsigned int mWidth;
    unsigned int mHeight;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(FrontDepthPeelingAlgorithm);
};

TPS_END_NAMESPACE

#endif // TPS_RL_FRONT_DEPTH_PEELING_ALGORITHM_H_
