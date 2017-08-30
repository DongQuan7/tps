//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  wenlong.zhao mailto:wenlong.zhao@united-imaging.com
///
///  \file    tps_rl_easyplan_measuredistancestrategy.h
///  \brief   tps easyplan measure distance strategy class
///
///  \version 1.0
///  \date    2014-11-08
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMEASUREDISTANCESTRATEGY_H_
#define TPS_RL_EASYPLANMEASUREDISTANCESTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class EasyPlanMeasureGraphicObject;

class TPS_RL_DECLSPEC EasyPlanMeasureDistanceStrategy : public TpsRenderStrategyBase
{
public:
    EasyPlanMeasureDistanceStrategy();
    virtual ~EasyPlanMeasureDistanceStrategy();

    virtual void Initialize() ;
    virtual void Finalize() ;
    //virtual void Resize(int iWidth, int iHeight);

    virtual void RenderMeasureDistance(float startX, float startY, float cursorX, float cursorY, 
        bool actionStarted, bool actionDone);

    //virtual int CacheChanges();
    //virtual bool IsToUpdate() const ;
    void SetEasyPlanMeasureDistanceGraphicObject(std::shared_ptr<EasyPlanMeasureGraphicObject> easyPlanMeasureDistanceGo);
    //float IgrtDRRMeasureDistanceStrategy::CalculateDistance(float startX, float startY, float cursorX, float cursorY);
    void Resize( int iWidth, int iHeight );
    int CacheChanges();
protected:
    virtual void Render_i() ;
    //unsigned char*                                          Buffer_show;
    //float                                                  mBaseLineColor[4];

private:  
    std::shared_ptr<EasyPlanMeasureGraphicObject>        mEasyPlanMeasureDistanceGraphicObject;
    //std::vector<Line> mDrrDistancePoint;

    //std::vector<Line> mNewDrrDistancePoint;
    //std::vector<Line>::iterator mItNewDrrDistance;



private:
    TPS_DISALLOW_COPY_AND_ASSIGN(EasyPlanMeasureDistanceStrategy);
    bool TransformEasyPlanImageModelPointToScreenPoint( Mcsf::MedViewer3D::Point2D& ptIn, Mcsf::MedViewer3D::Point2D& ptOut);
};


TPS_END_NAMESPACE
#endif