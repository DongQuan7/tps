////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measureanglevaluestrategy.h
/// 
///  \brief class GalleryMeasureAngleValStrategy
/// 
///  \version 1.0
/// 
///  \date    2015/3/23
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_GALLERYMEASUREANGLEVALSTRATEGY_H_
#define TPS_RL_GALLERYMEASUREANGLEVALSTRATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_cpurenderstrategy.h"

TPS_BEGIN_NAMESPACE

//class TpsTextOverlayGraphicObject;
class GalleryMeasureValGraphicObject;

class TPS_RL_DECLSPEC GalleryMeasureAngleValStrategy : public CPURenderStrategy
{
public:
    GalleryMeasureAngleValStrategy();
    virtual ~GalleryMeasureAngleValStrategy();

    virtual bool Render(unsigned char*& buffer, int bufWidth, int bufHeight);

    void SetMeasureValGraphicObject(std::shared_ptr<GalleryMeasureValGraphicObject> measureVal);

    Mcsf::MedViewer3D::Point2D ImageCoordinateToScreenCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);

private:
    bool                                                    mIsInitialed;
    bool                                                    mIsDirty;
    std::shared_ptr<GalleryMeasureValGraphicObject>            mMeasureValGraphicObject;
};

TPS_END_NAMESPACE  // end namespace tps

#endif