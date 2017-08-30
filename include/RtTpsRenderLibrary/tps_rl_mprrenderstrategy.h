//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_mprrenderstrategy.h
///  \brief   MPR RenderStrategy
///
///  \version 1.0
///  \date    Nov. 21, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_MPRRENDERSTRATEGY_H_
#define TPS_RL_MPRRENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class MprAlgorithm;
class MPRGraphicObject;
typedef std::shared_ptr<MPRGraphicObject> MprGoPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumePtr;

class TPS_RL_DECLSPEC MPRRenderStrategy :public TpsRenderStrategyBase
{
public:
    MPRRenderStrategy(void);

    virtual ~MPRRenderStrategy(void);

    virtual void Initialize();
    virtual void Finalize();

    inline void SetVolumeGO(const VolumePtr& volumeGo)
    {
        if (mVolumeGO != volumeGo)
        {
            mVolumeGO = volumeGo;
            mDirty = true;
        }
    }

    inline void SetMPRGO(const MprGoPtr& mprGo)
    {
        if (mMprGO != mprGo)
        {
            mMprGO = mprGo;
            mDirty = true;
        }
    }

    virtual void Resize( int iWidth, int iHeight);

    virtual int CacheChanges();

protected:
    virtual void Render_i();

    bool PrepareAlgorithm_i();

protected:
    MprAlgorithm*   mMprAlgorithm;

    MprGoPtr        mMprGO;
    VolumePtr       mVolumeGO;

    static double MPR_TOLERANCE;
};

TPS_END_NAMESPACE

#endif  //TPS_RL_MPRRENDERSTRATEGY_H_
