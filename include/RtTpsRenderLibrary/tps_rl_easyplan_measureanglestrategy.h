//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  wenlong.zhao mailto:wenlong.zhao@united-imaging.com
///
///  \file    tps_rl_easyplan_measureanglestrategy.h
///  \brief   tps easyplan measure angle strategy class
///
///  \version 1.0
///  \date    2014-11-11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMEASUREANGLESTRATEGY_H_
#define TPS_RL_EASYPLANMEASUREANGLESTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"
//ZHENGHE
#include "McsfMedViewer3DGraphicObject/graphic_object_plane.h"


namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
        class RenderStrategyGPU;
    }
}


TPS_BEGIN_NAMESPACE

class EasyPlanMeasureGraphicObject;

class TPS_RL_DECLSPEC EasyPlanMeasureAngleStrategy : public TpsRenderStrategyBase
{
public:
    EasyPlanMeasureAngleStrategy();
    virtual ~EasyPlanMeasureAngleStrategy();

    virtual void Initialize() ;
    virtual void Finalize() ;
    //virtual void Resize(int iWidth, int iHeight);

    virtual void RenderMeasureAngle(const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints,
        const float cursorX, const float cursorY, 
        const bool actionStarted, const bool firstSideDone, const bool secondSideDone);

    //virtual int CacheChanges();
    //virtual bool IsToUpdate() const ;
    void SetEasyPlanMeasureAngleGraphicObject(
        std::shared_ptr<EasyPlanMeasureGraphicObject> easyPlanMeasureAngleGo);
    void Resize( int iWidth, int iHeight );
    int CacheChanges();
protected:
    virtual void Render_i();
    //unsigned char*										  Buffer_show;
    //float												  mBaseLineColor[4];

private:  
    std::shared_ptr<EasyPlanMeasureGraphicObject>        mEasyPlanMeasureAngleGraphicObject;
    //std::vector<Line> mDrrDistancePoint;

    //std::vector<Line> mNewDrrDistancePoint;
    //std::vector<Line>::iterator mItNewDrrDistance;



private:
    TPS_DISALLOW_COPY_AND_ASSIGN(EasyPlanMeasureAngleStrategy);
};


TPS_END_NAMESPACE
#endif