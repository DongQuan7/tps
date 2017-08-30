////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_volume4d_bev_renderstrategy.h
/// 
///  \brief class TpsVolume4dBevRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOLUME4D_BEV_RENDERSTRATEGY_H_
#define TPS_RL_VOLUME4D_BEV_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include <vector>

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class Volume4dGraphicObject;
class DrrGraphicObejct;
class Ct2DensityGraphicObejct;
class CameraGraphicObject;
class StateControlGraphicObject;
class NormalGroupCollection;
class DrrGLGenerator;

class TPS_RL_DECLSPEC Volume4dBevRenderStrategy : public TpsRenderStrategyBase
{
public:

    Volume4dBevRenderStrategy();
    virtual ~Volume4dBevRenderStrategy();

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

    inline void SetVolume4dGraphicObject(std::shared_ptr<Volume4dGraphicObject> spVolume4dGO) {
        if (m_spVolume4dGO != spVolume4dGO) {
            m_spVolume4dGO = spVolume4dGO;
            mDirty = true;
        }
    }

    void SetDrrGO(std::shared_ptr<DrrGraphicObejct> drrGO){
        if (m_spDrrGO != drrGO){
            m_spDrrGO = drrGO;
            mDirty = true;
        }
    }
    void SetCt2DensityGO(std::shared_ptr<Ct2DensityGraphicObejct> spCtGO){
        if (m_spCt2DensityGO != spCtGO){
            m_spCt2DensityGO = spCtGO;
            mDirty = true;
        }
    }

    void SetStateControlGO(std::shared_ptr<StateControlGraphicObject> stateGo){
        if (m_spStateControlGO != stateGo){
            m_spStateControlGO = stateGo;
            mDirty = true;
        }
    }

    void SetCameraGO(std::shared_ptr<CameraGraphicObject> cameraGo){
        if (m_spCameraGO != cameraGo){
            m_spCameraGO = cameraGo;
            mDirty = true;
        }
    }

    void SetNormalGroupCollectionGO(std::shared_ptr<NormalGroupCollection> ngCollectionGO){
        if (m_spNormalGroupCollectionGO != ngCollectionGO){
            m_spNormalGroupCollectionGO = ngCollectionGO;
            mDirty = true;
        }
    }

protected:
    /// \brief  Virtual function for the rendering logic
    virtual void Render_i();

private:
    bool GenerateDrrImage_i();

private:
    bool                m_bInitialized;

    unsigned int        mTextureID;

    std::shared_ptr<Volume4dGraphicObject>                  m_spVolume4dGO;
    std::shared_ptr<DrrGraphicObejct>                       m_spDrrGO;
    std::shared_ptr<Ct2DensityGraphicObejct>                m_spCt2DensityGO;
    std::shared_ptr<StateControlGraphicObject>              m_spStateControlGO;
    std::shared_ptr<CameraGraphicObject>                    m_spCameraGO;
    std::shared_ptr<NormalGroupCollection>                  m_spNormalGroupCollectionGO;

    DrrGLGenerator*         m_pDrrGenerator;
    float*                  m_pDrrImage;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(Volume4dBevRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_VOLUME4D_MPR_RENDERSTRATEGY_H_
