////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_dose_transparency_rendering_node.h
/// 
///  \brief class DoseTransparencyRenderingNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/21
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSE_TRANSPARENCY_RENDERING_NODE_H_
#define TPS_RL_DOSE_TRANSPARENCY_RENDERING_NODE_H_

#include "tps_rl_defs.h"
#include "gl/glew.h"

#include <map>
#include <string>

#include "RtTpsRenderLibrary/tps_rl_phong_shading_transparency_rendering_node.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class DoseMeshGraphicObjectCollection;
typedef std::shared_ptr<DoseMeshGraphicObjectCollection> DoseMeshCollectionGOPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;

class TPS_RL_DECLSPEC DoseTransparencyRenderingNode : public PhongShadingTransprarencyNode
{
public:
    DoseTransparencyRenderingNode(int id);
    virtual ~DoseTransparencyRenderingNode();

    virtual void Finalize();

    //data source part
    inline void SetVolumeGO(const VolumeGOPtr& volumeGO)
    {
        mVolumeGO = volumeGO;
    }
    inline void SetDoseMeshCollectionGO(const DoseMeshCollectionGOPtr& doseMeshCollectionGO)
    {
        mDoseMeshCollectionGO = doseMeshCollectionGO;
    }

protected:
    virtual void PureRender_i(GLProgram* prog);
    virtual void BuildShaders_i();
    virtual bool Initialize_i();

private:

    VolumeGOPtr               mVolumeGO;
    DoseMeshCollectionGOPtr   mDoseMeshCollectionGO;

    GLuint mVboId;
    GLuint mNboId;
    GLuint mEboId;

    TPS_DISALLOW_COPY_AND_ASSIGN(DoseTransparencyRenderingNode);
};

TPS_END_NAMESPACE

#endif // tps_rl_dose_transparency_rendering_node_h__