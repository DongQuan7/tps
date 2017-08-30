////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_voi_transparency_rendering_node.h
/// 
///  \brief class VOITransparencyRenderingNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOI_TRANSPARENCY_RENDERING_NODE_H_
#define TPS_RL_VOI_TRANSPARENCY_RENDERING_NODE_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_phong_shading_transparency_rendering_node.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
        class Matrix4x4;
    }
}

TPS_BEGIN_NAMESPACE

class TpsVOIGraphicObject;
class TpsVOIGOCollection;
class GPUMarchingCubes;
typedef std::shared_ptr<TpsVOIGraphicObject> VOIGOPtr;
typedef std::shared_ptr<TpsVOIGOCollection> VOICollectionGOPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;

class TPS_RL_DECLSPEC VOITransparencyRenderingNode : public PhongShadingTransprarencyNode
{
public:
    VOITransparencyRenderingNode(int id);
    virtual ~VOITransparencyRenderingNode();

    virtual void Finalize();

    //data source part
    inline void SetVolumeGO(const VolumeGOPtr& volumeGO)
    {
        mVolumeGO = volumeGO;
    }
    inline void SetVOICollectionGO(const VOICollectionGOPtr& voiCollectionGO)
    {
        mVOICollectionGO = voiCollectionGO;
    }
protected:
    virtual void PureRender_i(GLProgram* prog);
    virtual void BuildShaders_i();
    virtual bool Initialize_i();

    void CalcMesh_i(VOIGOPtr voi);
    void RenderVoiMesh_i(
        GLProgram* prog, 
        const VOIGOPtr& voi, 
        unsigned int vboID, 
        unsigned int nboID, 
        unsigned int vertexCount, 
        const Mcsf::MedViewer3D::Matrix4x4& matVolume2View, 
        const Mcsf::MedViewer3D::Matrix4x4& matProjection);

private:
    VolumeGOPtr           mVolumeGO;
    VOICollectionGOPtr    mVOICollectionGO;
    GPUMarchingCubes*     mGpuMC;
    TPS_DISALLOW_COPY_AND_ASSIGN(VOITransparencyRenderingNode);
};

TPS_END_NAMESPACE

#endif // tps_rl_voi_transparency_rendering_node_h__
