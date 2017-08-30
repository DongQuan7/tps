////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_beam_transparency_rendering_node.h
/// 
///  \brief class BeamTransparencyRenderingNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/21
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEAM_TRANSPARENCY_RENDERING_NODE_H_
#define TPS_RL_BEAM_TRANSPARENCY_RENDERING_NODE_H_

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

class NormalGroupCollection;
class TpsSegmentGraphicObject;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;
typedef std::shared_ptr<NormalGroupCollection> NGCollectionGOPtr;
typedef std::shared_ptr<TpsSegmentGraphicObject> SegmentGOPtr;

class TPS_RL_DECLSPEC BeamTransparencyRenderingNode : public PhongShadingTransprarencyNode
{
public:
    BeamTransparencyRenderingNode(int id);
    virtual ~BeamTransparencyRenderingNode();

    virtual void Finalize();

    //data source part
    inline void SetVolumeGO(const VolumeGOPtr& volumeGO)
    {
        mVolumeGO = volumeGO;
    }
    inline void SetNGCollectionGO(const NGCollectionGOPtr& ngCollectionGO)
    {
        mNGCollectionGO = ngCollectionGO;
    }
protected:
    virtual void PureRender_i(GLProgram* prog);
    virtual void BuildShaders_i();
    virtual bool Initialize_i();

private:
    void DrawLeaves_i(GLProgram* prog, SegmentGOPtr segmentGO, double scale);
    void DrawJaw_i(GLProgram* prog, SegmentGOPtr segmentGO, double sad, double scale);
    void DrawBeamFace_i(GLProgram* prog, SegmentGOPtr segmentGO, bool isActive, double scale);
    void DrawIsoCenter_i();
    void DrawCentralLine_i(double sad);

    VolumeGOPtr         mVolumeGO;
    NGCollectionGOPtr   mNGCollectionGO;

    GLuint mVboId;
    TPS_DISALLOW_COPY_AND_ASSIGN(BeamTransparencyRenderingNode);
};

TPS_END_NAMESPACE

#endif // tps_rl_beam_transparency_rendering_node_h__