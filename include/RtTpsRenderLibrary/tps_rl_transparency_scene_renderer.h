////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_transparency_scene_renderer.h
/// 
///  \brief class TransparencySceneRenderer declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/18
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_TRANSPARENCY_SCENE_RENDERER_H_
#define TPS_RL_TRANSPARENCY_SCENE_RENDERER_H_

#include "tps_rl_defs.h"
#include <vector>
#include "gl/glew.h"

TPS_BEGIN_NAMESPACE

class ITransparencyRenderingNode;
class TransparencyRenderingAlgorithm;

//transparency rendering using dual depth peeling
class TPS_RL_DECLSPEC TransparencySceneRenderer
{
public:
    TransparencySceneRenderer();
    ~TransparencySceneRenderer();

    bool Initialize(TransparencyRenderingAlgorithm* algorithm);
    void Finalize();

    //note that all nodes must be added before renderer initialization.
    void AddNode(ITransparencyRenderingNode* node);
    //bool HasNode(int nodeID);
    //void RemoveNode(int nodeID);

    void Resize(unsigned int width, unsigned int height);
    void Render(GLuint targetFboId = 0, GLenum buff = GL_BACK);

    //if use occlusion queries
    void UseOQ(bool useOQ);
    void SetMaxPeelingNum(unsigned int num);
private:
    bool InitNodes_i();
    void ClearNodes_i();

private:
    bool mIsInitialized;

    unsigned int mImageWidth;
    unsigned int mImageHeight;

    std::vector<ITransparencyRenderingNode*> mNodes;
    TransparencyRenderingAlgorithm* mAlgorithm;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TransparencySceneRenderer);
};

TPS_END_NAMESPACE
#endif // TPS_RL_TRANSPARENCY_SCENE_RENDERER_H_