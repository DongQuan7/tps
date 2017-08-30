//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan mailto:yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measuredistancestrategy.h
///  \brief   tps gallery measure distance strategy class
///
///  \version 1.0
///  \date    2015-04-23
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERYMEASUREDISTANCESTRATEGY_H_
#define TPS_RL_GALLERYMEASUREDISTANCESTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class GalleryMeasureGraphicObject;

class TPS_RL_DECLSPEC GalleryMeasureDistanceStrategy : public TpsRenderStrategyBase
{
public:
    GalleryMeasureDistanceStrategy();
    virtual ~GalleryMeasureDistanceStrategy();

    virtual void Initialize() ;
    virtual void Finalize() ;
    virtual void Resize(int iWidth, int iHeight);

    virtual void RenderMeasureDistance(
        const std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& distancePointsList,
        const std::list<Mcsf::MedViewer3D::Point2D>& distancePoints,
        const std::vector<bool>& isOnPlaneResultList,
        const int hitIndex/*,
        const bool actionStarted, const bool actionDone*/);

    virtual int CacheChanges();
    //virtual bool IsToUpdate() const ;
    void SetGalleryMeasureDistanceGraphicObject(std::shared_ptr<GalleryMeasureGraphicObject> galleryMeasureDistanceGo);
    //Mcsf::MedViewer3D::Point2D ImageCoordinateToScreenCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);
protected:
    virtual void Render_i() ;
    //unsigned char*                                          Buffer_show;
    //float                                                  mBaseLineColor[4];

private:  
    std::shared_ptr<GalleryMeasureGraphicObject>        mGalleryMeasureDistanceGraphicObject;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(GalleryMeasureDistanceStrategy);
};


TPS_END_NAMESPACE
#endif