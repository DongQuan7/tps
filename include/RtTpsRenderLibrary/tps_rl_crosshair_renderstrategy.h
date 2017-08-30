////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_crosshair_renderstrategy.h
/// 
///  \brief class CrosshairRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CROSSHAIR_RENDERSTRATEGY_H_
#define TPS_RL_CROSSHAIR_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class CrosshairGraphicObject;

class TPS_RL_DECLSPEC CrosshairRenderStrategy : public TpsRenderStrategyBase {
public:
    CrosshairRenderStrategy();
    CrosshairRenderStrategy(std::shared_ptr<CrosshairGraphicObject> spCrosshairGO);
    virtual ~CrosshairRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetName(std::string& strName);
    void SetCrosshairGraphicObject(std::shared_ptr<CrosshairGraphicObject> spCrosshairGO);

protected:
    std::shared_ptr<CrosshairGraphicObject> m_spCrosshairGO;
    virtual void Render_i();

private:
    std::string m_strName;

    //TPS_DISALLOW_COPY_AND_ASSIGN(CrosshairRenderStrategy);
};

TPS_END_NAMESPACE

#endif //TPS_RL_CROSSHAIR_RENDERSTRATEGY_H_