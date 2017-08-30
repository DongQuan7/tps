////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_textstrategy.h
/// 
///  \brief class GalleryTextStrategy
/// 
///  \version 1.01
/// 
///  \date    2015/6/15
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_GALLERYTEXTSTRATEGY_H_
#define TPS_RL_GALLERYTEXTSTRATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_cpurenderstrategy.h"

TPS_BEGIN_NAMESPACE

//class TpsTextOverlayGraphicObject;
class GalleryTextGraphicObject;

class TPS_RL_DECLSPEC GalleryTextStrategy : public CPURenderStrategy
{
public:
    GalleryTextStrategy();
    virtual ~GalleryTextStrategy();

    virtual bool Render(unsigned char*& buffer, int bufWidth, int bufHeight);

    void SetTextGraphicObject(std::shared_ptr<GalleryTextGraphicObject> textGO);

    Mcsf::MedViewer3D::Point2D ImageCoordinateToScreenCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);

private:
    bool                                                    mIsInitialed;
    bool                                                    mIsDirty;
    std::shared_ptr<GalleryTextGraphicObject>               mTextGraphicObject;
};

TPS_END_NAMESPACE  // end namespace tps

#endif