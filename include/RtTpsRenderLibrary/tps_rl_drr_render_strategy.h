////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_drr_render_strategy.h
/// 
///  \brief class DrrRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/28
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DRR_RENDER_STRATEGY_H_
#define TPS_RL_DRR_RENDER_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
class DrrGraphicObejct;
class Ct2DensityGraphicObejct;
class Ct2DensityGraphicObejctCollection;
class CameraGraphicObject;
class StateControlGraphicObject;
class NormalGroupCollection;
class DrrGLGenerator;
class DRRSettingGraphicObject;
class DRRSettingGraphicObjectCollection;
class ITpsDataRepository;


/// \class DrrRenderStrategy
/// 
/// \brief *****
class TPS_RL_DECLSPEC DrrRenderStrategy : public TpsRenderStrategyBase{
public:
    //the constructor and destructor
    DrrRenderStrategy();
    virtual ~DrrRenderStrategy();

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
    void SetVolumeGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volumeGo);

    //////////////////////////////////////////////////////////////////////////
    // set DRR go;
    void SetDrrGO(std::shared_ptr<DrrGraphicObejct> drrGO);

    //////////////////////////////////////////////////////////////////////////
    // set ct 2 density graphic object
    void SetCt2DensityGOColletion(std::shared_ptr<Ct2DensityGraphicObejctCollection> ctGOCollection);

    void SetDrrSettingGoColletion(std::shared_ptr<DRRSettingGraphicObjectCollection> drrSettingGOColletion);

    //////////////////////////////////////////////////////////////////////////
    // set state control graphic object
    void SetStateControlGO(std::shared_ptr<StateControlGraphicObject> stateGo);

    //////////////////////////////////////////////////////////////////////////
    // set camera graphic object
    void SetCameraGO(std::shared_ptr<CameraGraphicObject> cameraGo);

    void SetNormalGroupCollectionGO(std::shared_ptr<NormalGroupCollection> ngCollectionGO);

	void SetDataRepository(ITpsDataRepository* dataRepository);
protected:
    virtual void Render_i();
private:
    bool GenerateDrrImage_i();
private:
    unsigned int        mTextureID;

    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolumeGO;
    std::shared_ptr<DrrGraphicObejct>                       mDrrGO;
    std::shared_ptr<Ct2DensityGraphicObejctCollection>      mCt2DensityGOCollection;
    std::shared_ptr<DRRSettingGraphicObjectCollection>      mDrrSettingGoCollection;
    std::shared_ptr<StateControlGraphicObject>              mStateControlGO;
    std::shared_ptr<CameraGraphicObject>                    mCameraGO;
    std::shared_ptr<NormalGroupCollection>                  mNormalGroupCollectionGO;

    DrrGLGenerator*  mDrrGenerator;
    float*                  mDrrImage;
	ITpsDataRepository*         mDataRepository;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DrrRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif