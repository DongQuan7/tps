////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_bev_window_tansparency_render_strategy.h
/// 
///  \brief class BevWindowTransparencyRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/19
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_BEV_WINDOW_TANSPARENCY_RENDER_STRATEGY_H_
#define TPS_RL_BEV_WINDOW_TANSPARENCY_RENDER_STRATEGY_H_

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
class DoseTransparencyRenderingNode;

class CameraGraphicObject;
class ShadedVoiGraphicObject;
class TpsVOIGOCollection;
class DoseMeshGraphicObjectCollection;
typedef std::shared_ptr<CameraGraphicObject>                    CameraGOPtr;
typedef std::shared_ptr<ShadedVoiGraphicObject>                 ShadedVOIGOPtr;
typedef std::shared_ptr<TpsVOIGOCollection>                     VOICollectionGOPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    VolumeGOPtr;
typedef std::shared_ptr<DoseMeshGraphicObjectCollection>        DoseMeshCollectionGOPtr;

//Transparency rendering node type
enum TRNodeType
{
    VOITRNode = 0,
    BeamTRNode,
    DoseTRNode,
    MPRTRNode,
    CubeTRNode,
};

class TPS_RL_DECLSPEC BevWindowTransparencyRenderStrategy : public TpsRenderStrategyBase
{
public:
    //the constructor and destructor
    BevWindowTransparencyRenderStrategy();
    virtual ~BevWindowTransparencyRenderStrategy();

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

    void SetDoseMeshCollectionGO(const DoseMeshCollectionGOPtr& doseMeshCollectionGO);

protected:
    virtual void Render_i();

private:
    TransparencySceneRenderer* mTSRenderer;
    //this class will keep the pointers of rendering nodes to do data source operation
    //including set or remove data source
    //node that the render strategy only holds the references,
    //never delete the nodes here, this class do not take charge of the nodes' life circle.
    VOITransparencyRenderingNode*  mVOITRNode;
    DoseTransparencyRenderingNode* mDoseTRNode;

    VolumeGOPtr             mVolumeGO;
    CameraGOPtr             mCameraGO;
    ShadedVOIGOPtr          mShadedVOIGO;
    VOICollectionGOPtr      mVOICollectionGO;
    DoseMeshCollectionGOPtr mDoseMeshCollectionGO;
    TPS_DISALLOW_COPY_AND_ASSIGN(BevWindowTransparencyRenderStrategy);
};

TPS_END_NAMESPACE
#endif // TPS_RL_3D_WINDOW_RENDER_STRATEGY_H_