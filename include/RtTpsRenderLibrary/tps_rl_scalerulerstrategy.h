////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Shi Chunhui  chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_scalerulerstrategy.h
/// 
///  \brief class TpsScaleRulerStrategy
///     draw an image scale ruler
///  \version 1.0
/// 
///  \date    2014/4/14
////////////////////////////////////////////////////////////////

// (xOri, yOri+Height)                       (xDest, yDest+Height)
//       |       |       |       |       |       |
//       |___|___|___|___|___|___|___|___|___|___|
//       0       2       4       6       8       10
//  (xOri, yOri)                       (xDest, yDest)

#ifndef TPS_RL_SCALERULERSTRATEGY_H_
#define TPS_RL_SCALERULERSTRATEGY_H_

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE
class MPRGraphicObject;
class TpsTextOverlayGraphicObject;

class TPS_RL_DECLSPEC TpsScaleRulerStrategy : public TpsRenderStrategyBase {
public:
    TpsScaleRulerStrategy();
    virtual ~TpsScaleRulerStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);
    
    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);
    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);
    void SetRulerTextInfoGraphicObject(std::shared_ptr<TpsTextOverlayGraphicObject>);

protected:
    virtual void Render_i();

private:

    double      mPixelSpacing;//mm
    double      mTotalLen;//mm
    double      mTickLen; //mm

    std::shared_ptr<MPRGraphicObject>                       mMprGraphicObject;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolumeGraphicObject;
    std::shared_ptr<TpsTextOverlayGraphicObject>            mRulerTextGraphicObject;

private:
    bool CalculatePixelSpacing_i();
    virtual std::shared_ptr<IRenderStrategy> Clone() const { return nullptr; }
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsScaleRulerStrategy);

};

TPS_END_NAMESPACE

#endif