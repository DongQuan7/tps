////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Qiangqiang ZHOU qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_voi3d_renderstrategy.h
/// 
///  \brief class VOI3DRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    Aug 6, 2014
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOI3D_RENDERSTRATEGY_H_
#define TPS_RL_VOI3D_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class CameraGraphicObject;
class StateControlGraphicObject;
class TpsVOIGOCollection;

//for BEV & shaded 3D windows
class TPS_RL_DECLSPEC VOI3DRenderStrategy :
    public TpsRenderStrategyBase {
public:
    VOI3DRenderStrategy(WINDOW_TYPE  windowType);
    virtual ~VOI3DRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetStateControlGO(std::shared_ptr<StateControlGraphicObject>);
    void SetCameraGO(std::shared_ptr<CameraGraphicObject>);
    void SetVolumeGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);
    //////////////////////////////////////////////////////////////////////////
    // \brief set VOI collection graphic object
    // \param[in] voiCollection  VOI collection graphic object pointer
    void SetVOICollectionGO(std::shared_ptr<TpsVOIGOCollection> voiCollection);

protected:
    virtual void Render_i();

private:
    std::shared_ptr<StateControlGraphicObject>              mStateGO;
    std::shared_ptr<TpsVOIGOCollection>                     mVoiCollectionGO;
    std::shared_ptr<CameraGraphicObject>                    mCameraGO;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolumeGO;
    WINDOW_TYPE                                             mWindowType;

    TPS_DISALLOW_COPY_AND_ASSIGN(VOI3DRenderStrategy);

};

TPS_END_NAMESPACE

#endif