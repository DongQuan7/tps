////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_transparency_rendering_node_interface.h
/// 
///  \brief class ITransparencyRenderingNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_TRANSPARENCY_RENDERING_NODE_INTERFACE_H_
#define TPS_RL_TRANSPARENCY_RENDERING_NODE_INTERFACE_H_

#include "tps_rl_defs.h"

#include <vector>

#define TR_NODE_START_TEX_UNIT 2

TPS_BEGIN_NAMESPACE

class GLProgram;

class ITransparencyRenderingNode
{
public:
    ITransparencyRenderingNode(int id) : 
        mNodeID(id), mWidth(0), mHeight(0){}
    virtual ~ITransparencyRenderingNode() {}

    inline virtual bool Initialize()
    {
        BuildShaders_i();
        return Initialize_i();
    }

    inline int GetNodeID() { return mNodeID; }

    inline void GetShadingShaders(
        std::vector<const char*>& vShaders, 
        std::vector<const char*>& fShaders)
    {
        vShaders = mStrShadingVertexShaders;
        fShaders = mStrShadingFragmentShaders;
    }

    virtual void Finalize() = 0;

    virtual void Render(GLProgram* prog = nullptr) = 0;

    inline virtual void Resize(unsigned int width, unsigned int height)
    {
        mWidth = width;
        mHeight = height;
    }

protected:
    virtual void BuildShaders_i() = 0;
    virtual bool Initialize_i() = 0;

    int mNodeID;

    std::vector<const char*> mStrShadingVertexShaders;
    std::vector<const char*> mStrShadingFragmentShaders;
    unsigned int mWidth;
    unsigned int mHeight;
};

TPS_END_NAMESPACE
#endif // TPS_RL_TRANSPARENCY_RENDERING_NODE_INTERFACE_H_
