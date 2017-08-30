////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Shi Chunhui  chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_colorspectrarenderstrategy.h
/// 
///  \brief class TpsColorSpectraRenderStrategy
///  
///  \version 1.0
/// 
///  \date    2014/4/30

#ifndef TPS_RL_COLORSPECTRASTRATEGY_H_
#define TPS_RL_COLORSPECTRASTRATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsDataAccess/tps_da_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace RTFWK
{
    class RTFWKDLDODoseGrid;
};

TPS_BEGIN_NAMESPACE
class TpsMPRDoseGraphicObject;
class TpsDoseLineGraphicObject;

class TPS_RL_DECLSPEC TpsColorSpectraRenderStrategy : public TpsRenderStrategyBase {

public:
    TpsColorSpectraRenderStrategy();
    virtual ~TpsColorSpectraRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);
    void SetDoseGraphicObject(std::shared_ptr<TpsDoseLineGraphicObject> doseGO);
protected:
    virtual void Render_i();
private:
    //std::shared_ptr<TpsMPRDoseGraphicObject>    mDoseGO;
    std::shared_ptr<TpsDoseLineGraphicObject> mDoseGO;

    float mOriginX, mOriginY, mOffsetX, mOffsetY;

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsColorSpectraRenderStrategy);
};
TPS_END_NAMESPACE
#endif
