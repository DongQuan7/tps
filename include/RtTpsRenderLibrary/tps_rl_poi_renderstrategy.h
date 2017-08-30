////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_poi_renderstrategy.h
/// 
///  \brief class TpsPoiRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/11/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_POI_RNEDERSTRATEGY_H
#define TPS_RL_POI_RNEDERSTRATEGY_H

#include "tps_rl_defs.h"            // for TPS_RL_DECLSPEC

#include "RtTpsFramework/tps_fw_render_strategy_base.h"     // for Base class

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
        class Point3D;
    }
}

TPS_BEGIN_NAMESPACE
class MPRGraphicObject;
class TpsPoiGraphicObjectCollection;
class AntialiasedCirclePainter;

class TPS_RL_DECLSPEC TpsPoiRenderStrategy : public TpsRenderStrategyBase {
public:
    TpsPoiRenderStrategy();

    virtual ~TpsPoiRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetPoiGraphicObjectCollection(std::shared_ptr<TpsPoiGraphicObjectCollection>);

    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);

    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);

protected:
    virtual void Render_i();

private:
    AntialiasedCirclePainter*                            m_pPointPainter;
    std::shared_ptr<TpsPoiGraphicObjectCollection>       m_spPoiGraphicObjectCollection;
    std::shared_ptr<MPRGraphicObject>                    m_spMprGraphicObject;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> m_spVolumeGraphicObject;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPoiRenderStrategy);
};

TPS_END_NAMESPACE

#endif