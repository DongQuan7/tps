////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_volume_cube_transparency_rendering_node.h
/// 
///  \brief class VolumeCubeTRNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/3/9
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOLUME_CUBE_TRANSPARENCY_RENDERING_NODE_H_
#define TPS_RL_VOLUME_CUBE_TRANSPARENCY_RENDERING_NODE_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_phong_shading_transparency_rendering_node.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;

class TPS_RL_DECLSPEC VolumeCubeTRNode : public PhongShadingTransprarencyNode
{
public:
    VolumeCubeTRNode(int id);
    virtual ~VolumeCubeTRNode();

    virtual void Finalize();

    //data source part
    inline void SetVolumeGO(const VolumeGOPtr& volumeGO)
    {
        mVolumeGO = volumeGO;
    }
protected:
    virtual void PureRender_i(GLProgram* prog);
    virtual void BuildShaders_i();
    virtual bool Initialize_i();

private:
    VolumeGOPtr           mVolumeGO;

    unsigned int mVboId;
    unsigned int mEboId;
    TPS_DISALLOW_COPY_AND_ASSIGN(VolumeCubeTRNode);
};

TPS_END_NAMESPACE

#endif // TPS_RL_VOLUME_CUBE_TRANSPARENCY_RENDERING_NODE_H_
