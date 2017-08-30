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

#ifndef TPS_RL_TRANSPARENCY_RENDERING_ALGORITHM_H_
#define TPS_RL_TRANSPARENCY_RENDERING_ALGORITHM_H_

#include "tps_rl_defs.h"
#include <vector>
#include "gl/glew.h"

TPS_BEGIN_NAMESPACE

class ITransparencyRenderingNode;
class TRALGProgram;

class TransparencyRenderingAlgorithm
{
public:
    TransparencyRenderingAlgorithm();
    virtual ~TransparencyRenderingAlgorithm();

    virtual bool Initialize(const std::vector<ITransparencyRenderingNode*>& nodes);
    virtual void Finalize();

    virtual void Render(GLuint targetFboId, GLenum buff) = 0;

    virtual void Resize(unsigned int width, unsigned int height) = 0;

    virtual TRALGProgram* CreateProgram();

    unsigned int GetPassNum() const { return mPassNum; }
    void SetUseOQ( bool useOQ) { mUseOQ = useOQ; }
    bool GetUseOQ() const { return mUseOQ; }
    void SetMaxPeelingNum(unsigned int maxNum) { mMaxPeelingNum = maxNum; }
    unsigned int GetMaxPeelingNum() const { return mMaxPeelingNum; }
protected:
    void MakeFullScreenQuad_i();

    virtual bool InitRenderTargets_i() { return true; }
    virtual bool BuildPrograms_i() { return true; }

    virtual void ClearRenderTargets_i() {}
    virtual void ClearPrograms_i() {}

    std::vector<ITransparencyRenderingNode*> mNodes;

    unsigned int mPassNum;
    bool mUseOQ;
    unsigned int mMaxPeelingNum;

    GLuint mQuadListId;
    GLuint mQueryId;
    bool mInitialized;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TransparencyRenderingAlgorithm);
};

TPS_END_NAMESPACE
#endif // TPS_RL_TRANSPARENCY_RENDERING_ALGORITHM_H_
