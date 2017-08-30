////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_bev_dose_shaded_strategy.h
/// 
///  \brief class TpsBevDoseShadedStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/06/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEV_DOSE_SHADED_STRATEGY_H_
#define TPS_RL_BEV_DOSE_SHADED_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"


TPS_BEGIN_NAMESPACE

class CameraGraphicObject;
class DoseMeshGraphicObjectCollection;

class TPS_RL_DECLSPEC TpsBevDoseShadedStrategy : public TpsRenderStrategyBase {
public:
    TpsBevDoseShadedStrategy();
    virtual ~TpsBevDoseShadedStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetDoseMeshGOCollection(std::shared_ptr<DoseMeshGraphicObjectCollection>);
    void SetCameraGO(std::shared_ptr<CameraGraphicObject>);

protected:
    virtual void Render_i();

private:
    void RenderBevDoseSurface();
    void RenderBevDoseLine();
    void RenderBevDoseMesh();

private:
    bool                        m_bRenderInLine;    // default to false; true when debug line
    bool                        m_bRenderInMesh;    // default to false; true when debug mesh
    std::shared_ptr<CameraGraphicObject>     m_spCameraGO;
    std::shared_ptr<DoseMeshGraphicObjectCollection>      m_spDoseMeshGOCollection;
};

TPS_END_NAMESPACE

#endif //TPS_RL_BEV_DOSE_SHADED_STRATEGY_H_
