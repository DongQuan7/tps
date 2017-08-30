////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_phong_shading_transparency_rendering_node.h
/// 
///  \brief class PhongShadingTransprarencyNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_PHONG_SHADING_TRANSPARENCY_RENDERING_NODE_H_
#define TPS_RL_PHONG_SHADING_TRANSPARENCY_RENDERING_NODE_H_

#include "tps_rl_defs.h"
#include "gl/glew.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_node_interface.h"

TPS_BEGIN_NAMESPACE

class ShadedVoiGraphicObject;
class CameraGraphicObject;
typedef std::shared_ptr<CameraGraphicObject> CameraGOPtr;
typedef std::shared_ptr<ShadedVoiGraphicObject> ShadedVOIGOPtr;

class TPS_RL_DECLSPEC PhongShadingTransprarencyNode : public ITransparencyRenderingNode
{
public:
    PhongShadingTransprarencyNode(int id);
    virtual ~PhongShadingTransprarencyNode();

    virtual void Render(GLProgram* prog = nullptr);

    inline void SetCameraGO(const CameraGOPtr& cameraGO)
    {
        mCameraGO = cameraGO;
    }
    inline void SetShadedVOIGO(const ShadedVOIGOPtr& shadedVOIGO)
    {
        mShadedVOIGO = shadedVOIGO;
    }
protected:
    //the pure rendering part excluding light setting
    virtual void PureRender_i(GLProgram* prog) = 0;

    virtual void BuildShaders_i();

    CameraGOPtr           mCameraGO;
    ShadedVOIGOPtr        mShadedVOIGO;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(PhongShadingTransprarencyNode);
};

TPS_END_NAMESPACE
#endif // tps_rl_phong_shading_transparency_rendering_node_h__