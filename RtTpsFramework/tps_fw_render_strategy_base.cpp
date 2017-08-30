////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_render_strategy_base.cpp
/// 
///  \brief class TpsRenderStrategyBase implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/30
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsRenderStrategyBase::TpsRenderStrategyBase()
: Mcsf::MedViewer3D::RenderStrategyGPU(0)
, mWidth(0), mHeight(0), mDirty(false), mIsVisible(true), mIsActive(true), mIsLazyPass(false)
{

}

TpsRenderStrategyBase::~TpsRenderStrategyBase()
{

}

void TpsRenderStrategyBase::SetIsVisible(bool visible)
{
    if (mIsVisible != visible) mDirty = true;
    mIsVisible = visible;
}

bool TpsRenderStrategyBase::IsVisible() const
{
    return mIsVisible;
}

void TpsRenderStrategyBase::SetIsActive(bool isActive)
{
    mIsActive = isActive;
}

bool TpsRenderStrategyBase::IsActive() const
{
    return mIsActive;
}

void TpsRenderStrategyBase::Render()
{
    if (!mIsActive) return;
    Scopped_SetValue<bool, false> setFalse(&mDirty);

    if (mWidth <= 0 || mHeight <= 0) return;
    if (!mIsVisible) return;
    Render_i();
}

bool TpsRenderStrategyBase::IsToUpdate() const
{
    return mIsActive ? mDirty : false;
}

void TpsRenderStrategyBase::Resize(int iWidth, int iHeight)
{
    mWidth = iWidth;
    mHeight = iHeight;
    mDirty = true;
}
TPS_END_NAMESPACE  // end namespace tps