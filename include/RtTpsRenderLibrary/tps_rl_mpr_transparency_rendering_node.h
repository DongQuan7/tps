////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_mpr_transparency_rendering_node.h
/// 
///  \brief class MprTransparencyRenderingNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/21
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPR_TRANSPARENCY_RENDERING_NODE_H_
#define TPS_RL_MPR_TRANSPARENCY_RENDERING_NODE_H_

#include "tps_rl_defs.h"
#include "gl/glew.h"

#include <map>
#include <string>

#include "RtTpsRenderLibrary/tps_rl_transparency_rendering_node_interface.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
        class Point3D;
    }
}

TPS_BEGIN_NAMESPACE

class MPRGraphicObject;
class ShadedVoiGraphicObject;
class CameraGraphicObject;
typedef std::shared_ptr<CameraGraphicObject> CameraGOPtr;
typedef std::shared_ptr<ShadedVoiGraphicObject> ShadedVOIGOPtr;
typedef std::shared_ptr<MPRGraphicObject> MprGOPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;

class TPS_RL_DECLSPEC MprTransparencyRenderingNode : public ITransparencyRenderingNode
{
public:
    MprTransparencyRenderingNode(int id);
    virtual ~MprTransparencyRenderingNode();

    virtual void Finalize();

    virtual void Render(GLProgram* prog = nullptr);

    //data source part
    inline void SetVolumeGO(const VolumeGOPtr& volumeGO)
    {
        mVolumeGO = volumeGO;
    }
    inline void SetMprGO(const MprGOPtr& axialGO, const MprGOPtr& coronalGO, 
        const MprGOPtr& sagittalGO)
    {
        mMprAxialGO = axialGO;
        mMprCoronalGO = coronalGO;
        mMprSagittalGO = sagittalGO;
    }

    inline void SetCameraGO(const CameraGOPtr& cameraGO)
    {
        mCameraGO = cameraGO;
    }
    inline void SetShadedVOIGO(const ShadedVOIGOPtr& shadedVOIGO)
    {
        mShadedVOIGO = shadedVOIGO;
    }
protected:
    virtual void BuildShaders_i();
    virtual bool Initialize_i();

private:
    void PrepareProgram_i(GLProgram* prog, MprGOPtr mprGO);
    int PrepareVertexBuffer_i(MprGOPtr mprGO);
    Mcsf::MedViewer3D::Point3D VertexInterpolation_i(
        const Mcsf::MedViewer3D::Point3D* vertices, int index);

    VolumeGOPtr     mVolumeGO;
    CameraGOPtr     mCameraGO;
    ShadedVOIGOPtr  mShadedVOIGO;
    MprGOPtr        mMprAxialGO;
    MprGOPtr        mMprCoronalGO;
    MprGOPtr        mMprSagittalGO;

    GLuint mVboId;
    GLuint mEboId;

    TPS_DISALLOW_COPY_AND_ASSIGN(MprTransparencyRenderingNode);
};

TPS_END_NAMESPACE

#endif // tps_rl_mpr_transparency_rendering_node_h__