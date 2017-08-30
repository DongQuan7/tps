////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poitextrenderstrategy.h
/// 
///  \brief class POITypeTextRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/14
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_POITEXTRENDERSTRATEGY_H_
#define TPS_RL_POITEXTRENDERSTRATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_cpurenderstrategy.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GeometryTechnique;
        class GraphicObjVolume;
    }
}


TPS_BEGIN_NAMESPACE

class MPRGraphicObject;
class TpsPoiGraphicObjectCollection;
class ITpsDataRepository;

class TPS_RL_DECLSPEC POITypeTextRenderStrategy : public CPURenderStrategy {
public:
    POITypeTextRenderStrategy();
    virtual ~POITypeTextRenderStrategy();

    void SetDataRepository(ITpsDataRepository* repos) {mDataRepository = repos;}

    virtual bool Render(unsigned char*& buffer, int bufWidth, int bufHeight);

    void SetPoiGraphicObjectCollection(std::shared_ptr<TpsPoiGraphicObjectCollection>);

    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);

    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);

private:

    std::shared_ptr<TpsPoiGraphicObjectCollection>       mPoiGraphicObejctColl;
    std::shared_ptr<MPRGraphicObject>                    mMprGraphicObject;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> mVolumeGraphicObject;

    ITpsDataRepository          *mDataRepository;


private:
    double GetSpacing_i();

};

TPS_END_NAMESPACE

#endif