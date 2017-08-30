////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fluence_map_render_strategy.h
/// 
///  \brief class FluenceMapRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/06
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_FLUENCE_MAP_RENDER_STRATEGY_H_
#define TPS_RL_FLUENCE_MAP_RENDER_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class GLProgram;
class NormalGroupCollection;
class TpsBeamGraphicObjectCollection;
class CameraGraphicObject;
class StateControlGraphicObject;
class ITpsDataRepository;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;
typedef std::shared_ptr<CameraGraphicObject> CameraGOPtr;
typedef std::shared_ptr<NormalGroupCollection> NGCollectionGOPtr;
typedef std::shared_ptr<TpsBeamGraphicObjectCollection> NormalGroupPtr;
typedef std::shared_ptr<StateControlGraphicObject> StartControlGOPtr;

/// \class FluenceMapRenderStrategy
/// 
/// \brief *****
class TPS_RL_DECLSPEC FluenceMapRenderStrategy : public TpsRenderStrategyBase{
public:
    //the constructor and destructor
    FluenceMapRenderStrategy();
    virtual ~FluenceMapRenderStrategy();

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

    void SetVolumeGO(VolumeGOPtr volumeGO);
    void SetCameraGO(CameraGOPtr cameraGO);
    void SetNormalGroupCollectionGO(NGCollectionGOPtr ngCollectionGO);
    void SetStateControlGO(StartControlGOPtr stateControlGO);

	//void SetDataRepository(ITpsDataRepository* dataRepository);

protected:
    virtual void Render_i();

private:
    bool GetLutWWWL(NormalGroupPtr ng, float& ww, float& wl);

    VolumeGOPtr         mVolumeGO;
    NGCollectionGOPtr   mNGCollectionGO;
    CameraGOPtr         mCameraGO;
    StartControlGOPtr   mStateControlGO;

    GLProgram*          mGLProgram;
    unsigned int        mProgramLocations[8];
    unsigned int        mVertexArrayID;
    unsigned int        mBufferID[3];
    unsigned int        mTexID[3];

	ITpsDataRepository*         mDataRepository;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(FluenceMapRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif