////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_applicator_render_strategy.h
/// 
///  \brief class ApplicatorRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/06
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_APPLICATOR_RENDER_STRATEGY_H_
#define TPS_RL_APPLICATOR_RENDER_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class NormalGroupCollection;
class CameraGraphicObject;
class AntialiasedCirclePainter;
/// \class ApplicatorRenderStrategy
/// 
/// \brief the strategy for applicator/cone
class TPS_RL_DECLSPEC ApplicatorRenderStrategy : public TpsRenderStrategyBase{
public:
    //the constructor and destructor
    ApplicatorRenderStrategy();
    virtual ~ApplicatorRenderStrategy();

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
    // \brief set volume go;
    void SetVolumeGraphicObj(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);

    void SetCameraGO(std::shared_ptr<CameraGraphicObject>);

    void SetNormalGroupCollection(std::shared_ptr<NormalGroupCollection> normalGroupCollection);

protected:
    virtual void Render_i();

private:
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> mVolumeGO;
    std::shared_ptr<NormalGroupCollection>               mNormalGroupCollection;
    std::shared_ptr<CameraGraphicObject>                 mCameraGO;
    AntialiasedCirclePainter*                            mCirclePainter;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ApplicatorRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif