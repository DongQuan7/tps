////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_dummyrenderstrategy.h
/// 
///  \brief class DummyRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DUMMYRENDERSTRATEGY_H_
#define TPS_RL_DUMMYRENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class DummyGraphicObject;

/// \class DummyRenderStrategy
/// 
/// \brief About the usage of DummyRenderStrategy: render nothing, but will cause 
///        re-blending of overlays' FBO & CPU(mainly the text rendering) buffer
class TPS_RL_DECLSPEC DummyRenderStrategy : public TpsRenderStrategyBase{
public:
    //the constructor and destructor
    DummyRenderStrategy();
    virtual ~DummyRenderStrategy();

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

    inline void SetDummyGraphicObject(std::shared_ptr<DummyGraphicObject> go){
        if (mDummyGo != go){
            mDummyGo = go;
            mDirty = true;
        }
    }

protected:
    virtual void Render_i();

private:

    std::shared_ptr<DummyGraphicObject>     mDummyGo;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DummyRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif