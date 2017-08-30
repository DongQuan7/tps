////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  SHI chunhui chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_cornertextinforenderstrategy.h
/// 
///  \brief class CornerTextInfoRenderStrategy
/// 
///  \version 1.0
/// 
///  \date    2014/6/19
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_CORNER_TEXT_INFO_RENDERSTRATEGY_H
#define TPS_RL_CORNER_TEXT_INFO_RENDERSTRATEGY_H

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_cpurenderstrategy.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class MPRGraphicObject;
class TpsTextOverlayGraphicObject;

class TPS_RL_DECLSPEC CornerTextInfoRenderStrategy : public CPURenderStrategy
{
public:
    CornerTextInfoRenderStrategy();
    virtual ~CornerTextInfoRenderStrategy();

    virtual bool Render(unsigned char*& buffer, int bufWidth, int bufHeight);
    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);
    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);
    void SetCornerTextInfoGraphicObject(std::shared_ptr<TpsTextOverlayGraphicObject>);

private:
    bool                                                    mIsInitialed;
    bool                                                    mIsDirty;
    std::shared_ptr<MPRGraphicObject>                       mMprGraphicObject;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolumeGraphicObject;
    std::shared_ptr<TpsTextOverlayGraphicObject>            mCornerTextInfoGraphicObject;
};

TPS_END_NAMESPACE  // end namespace tps

#endif