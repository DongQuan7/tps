//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan mailto:yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measureanglestrategy.h
///  \brief   tps gallery measure angle strategy class
///
///  \version 1.0
///  \date    2015-03-09
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERYMEASUREANGLESTRATEGY_H_
#define TPS_RL_GALLERYMEASUREANGLESTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class GalleryMeasureGraphicObject;

class TPS_RL_DECLSPEC GalleryMeasureAngleStrategy : public TpsRenderStrategyBase
{
public:
    GalleryMeasureAngleStrategy();
    virtual ~GalleryMeasureAngleStrategy();

    virtual void Initialize() ;
    virtual void Finalize() ;
    virtual void Resize(int iWidth, int iHeight);

    virtual void RenderMeasureAngle(
        const std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& anglePointsList,
        const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints,
        const std::vector<bool>& isOnPlaneResultList,
        const float cursorX, const float cursorY, 
        const bool actionStarted, const bool firstSideDone, const bool secondSideDone);

    virtual int CacheChanges();
    //virtual bool IsToUpdate() const ;
    void SetGalleryMeasureAngleGraphicObject(std::shared_ptr<GalleryMeasureGraphicObject> galleryMeasureAngleGo);
    //Mcsf::MedViewer3D::Point2D ImageCoordinateToScreenCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);
protected:
    virtual void Render_i() ;
    //unsigned char*                                          Buffer_show;
    //float                                                  mBaseLineColor[4];

private:  
    std::shared_ptr<GalleryMeasureGraphicObject>        mGalleryMeasureAngleGraphicObject;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(GalleryMeasureAngleStrategy);
};


TPS_END_NAMESPACE
#endif