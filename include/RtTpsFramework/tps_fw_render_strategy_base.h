////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_render_strategy_base.h
/// 
///  \brief class TpsRenderStrategyBase declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/30
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_RENDER_STRATEGY_BASE_H_
#define TPS_FW_RENDER_STRATEGY_BASE_H_

#include "RtTpsFramework/tps_fw_defs.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_render_strategy_gpu.h"
#include <memory>

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsRenderStrategyBase
/// 
/// \brief *****
class TPS_FW_DECLSPEC TpsRenderStrategyBase : public Mcsf::MedViewer3D::RenderStrategyGPU 
{
public:
    //the constructor and destructor
    TpsRenderStrategyBase();
    ~TpsRenderStrategyBase();

    /// \brief  Virtual function for the rendering logic
    virtual void Render();

    /// \fun    IsToUpdate
    /// \brief  Indicate whether the render strategy needs to be invoked
    ///         for each frame
    bool IsToUpdate() const;

    void SetIsVisible(bool visible);
    bool IsVisible() const;

    void SetIsActive(bool isActive);
    bool IsActive() const;

    /// \fun    Initialize
    /// \brief  For GPU implementation, any GL resources specific to the
    ///         strategy or the render context should be created in the function
    virtual void Initialize() { }

    /// \fun    Finalize
    /// \brief  For GPU implementation, the resources created in the Initialize
    ///         function should be cleaned up
    virtual void Finalize() { }

    /// \fun    Resize
    /// \brief  The resize method is only needed in gpu implementation
    ///         to reallocate any gpu resource according to the new
    ///         size of the screen
    /// \param[in] iWidth The width of the screen in pixels
    /// \param[in] iHeight The height of the screen in pixels
    virtual void Resize(int iWidth, int iHeight);
protected:
    virtual void Render_i() = 0;

    bool    mDirty;
    //visible与active的区别
    //visible是指可不可见，不可见绘制全黑图
    //active指是否激活，未激活时不进行绘制
    //同时为True才可以画出图像
    bool    mIsVisible;
    bool    mIsActive;
    bool    mIsLazyPass;

    int     mWidth;
    int     mHeight;

private:
    virtual std::shared_ptr<IRenderStrategy> Clone() const { return nullptr; }

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRenderStrategyBase);
};

TPS_END_NAMESPACE  // end namespace tps

#endif