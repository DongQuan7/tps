////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_3d_window_render_strategy.h
/// 
///  \brief class Tps3DWindowRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/19
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_3D_WINDOW_RENDER_STRATEGY_H_
#define TPS_RL_3D_WINDOW_RENDER_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class TransparencySceneRenderer;
class VOITransparencyRenderingNode;
class BeamTransparencyRenderingNode;
class DoseTransparencyRenderingNode;
class MprTransparencyRenderingNode;
class VolumeCubeTRNode;

class CameraGraphicObject;
class ShadedVoiGraphicObject;
class TpsVOIGOCollection;
class NormalGroupCollection;
class DoseMeshGraphicObjectCollection;
class MPRGraphicObject;
typedef std::shared_ptr<CameraGraphicObject>                    CameraGOPtr;
typedef std::shared_ptr<ShadedVoiGraphicObject>                 ShadedVOIGOPtr;
typedef std::shared_ptr<TpsVOIGOCollection>                     VOICollectionGOPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    VolumeGOPtr;
typedef std::shared_ptr<NormalGroupCollection>                  NGCollectionGOPtr;
typedef std::shared_ptr<DoseMeshGraphicObjectCollection>        DoseMeshCollectionGOPtr;
typedef std::shared_ptr<MPRGraphicObject> MprGOPtr;

//Transparency rendering node type
enum TRNodeType
{
    VOITRNode = 0,
    BeamTRNode,
    DoseTRNode,
    MPRTRNode,
    CubeTRNode,
};

class TPS_RL_DECLSPEC Tps3DWindowRenderStrategy : public TpsRenderStrategyBase
{
public:
    //the constructor and destructor
    Tps3DWindowRenderStrategy();
    virtual ~Tps3DWindowRenderStrategy();

    /// \fun    CacheChanges
    /// \brief  Store all changed items locally for later rendering.
    ///         This is necessary because the rendering is asynchronous.
    //virtual int CacheChanges() = 0;
    virtual int CacheChanges();

    /// \fun    Initialize
    /// \brief  For GPU implementation, any GL resources specific to the
    ///         strategy or the render context should be created in the function
    virtual void Initialize();

    /// \fun    Finalize
    /// \brief  For GPU implementation, the resources created in the Initialize
    ///         function should be cleaned up
    virtual void Finalize();

    /// \fun    Resize
    /// \brief  The resize method is only needed in gpu implementation
    ///         to reallocate any gpu resource according to the new
    ///         size of the screen
    /// \param[in] iWidth The width of the screen in pixels
    /// \param[in] iHeight The height of the screen in pixels
    virtual void Resize(int iWidth, int iHeight);

    //////////////////////////////////////////////////////////////////////////
    // \brief set volume go
    void SetVolumeGO(const VolumeGOPtr& volumeGO);

    //////////////////////////////////////////////////////////////////////////
    // set camera graphic object
    void SetCameraGO(const CameraGOPtr& cameraGO);

    void SetShadedVOIGO(const ShadedVOIGOPtr& shadedVOIGO);

    void SetVOICollectionGO(const VOICollectionGOPtr& voiCollectionGO);

    void SetNGCollectionGO(const NGCollectionGOPtr& ngCollectionGO);

    void SetDoseMeshCollectionGO(const DoseMeshCollectionGOPtr& doseMeshCollectionGO);

    void SetMprGO(const MprGOPtr& axialGO, const MprGOPtr& coronalGO, 
        const MprGOPtr& sagittalGO);
protected:
    virtual void Render_i();

private:
    TransparencySceneRenderer* mTSRenderer;
    //this class will keep the pointers of rendering nodes to do data source operation
    //including set or remove data source
    //node that the render strategy only holds the references,
    //never delete the nodes here, this class do not take charge of the nodes' life circle.
    VOITransparencyRenderingNode*  mVOITRNode;
    BeamTransparencyRenderingNode* mBeamTRNode;
    DoseTransparencyRenderingNode* mDoseTRNode;
    MprTransparencyRenderingNode*  mMprTRNode;
    VolumeCubeTRNode*              mVolumeCubeNode;

    VolumeGOPtr             mVolumeGO;
    CameraGOPtr             mCameraGO;
    ShadedVOIGOPtr          mShadedVOIGO;
    VOICollectionGOPtr      mVOICollectionGO;
    NGCollectionGOPtr       mNGCollectionGO;
    DoseMeshCollectionGOPtr mDoseMeshCollectionGO;
    MprGOPtr                mMprAxialGO;
    MprGOPtr                mMprCoronalGO;
    MprGOPtr                mMprSagittalGO;
    TPS_DISALLOW_COPY_AND_ASSIGN(Tps3DWindowRenderStrategy);
};

TPS_END_NAMESPACE
#endif // TPS_RL_3D_WINDOW_RENDER_STRATEGY_H_