////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Yuxuan Duan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_volume4d_mpr_renderstrategy.h
/// 
///  \brief class GalleryVolume4dMprRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/03/22
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_VOLUME4D_MPR_RENDERSTRATEGY_H_
#define TPS_RL_GALLERY_VOLUME4D_MPR_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class Volume4dGraphicObject;

class TPS_RL_DECLSPEC GalleryVolume4dMprRenderStrategy : public MPRRenderStrategy
{
public:

    GalleryVolume4dMprRenderStrategy();
    virtual ~GalleryVolume4dMprRenderStrategy();

    /// \fun    CacheChanges
    /// \brief  Store all changed items locally for later rendering.
    ///         This is necessary because the rendering is asynchronous.
    //virtual int CacheChanges() = 0;
    virtual int CacheChanges();

    inline void SetVolume4dGraphicObject(std::shared_ptr<Volume4dGraphicObject> spVolume4dGO) {
        if (m_spVolume4dGO != spVolume4dGO) {
            m_spVolume4dGO = spVolume4dGO;
            mDirty = true;
        }
    }

    inline void Enable4DMode(bool isEnabled) { mIs4DEnabled = isEnabled; }

protected:
    virtual void Render_i();

private:
    std::shared_ptr<Volume4dGraphicObject>      m_spVolume4dGO;
    bool mIs4DEnabled;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(GalleryVolume4dMprRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_GALLERY_VOLUME4D_MPR_RENDERSTRATEGY_H_
