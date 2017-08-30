////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mpr_hotspots_renderstrategy.h
/// 
///  \brief class MprHotSpotsRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/09/05
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPR_HOT_SPOTS_RENDERSTRATEGY_H_
#define TPS_RL_MPR_HOT_SPOTS_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class HotSpotsGraphicObject;
class MPRGraphicObject;
class ITpsDataRepository;

class TPS_RL_DECLSPEC MprHotSpotsRenderStrategy : public TpsRenderStrategyBase {
public:
    friend class MascotRenderStrategyUT_Render_Test;
public:
    MprHotSpotsRenderStrategy();
    MprHotSpotsRenderStrategy(std::shared_ptr<MPRGraphicObject> spMPRGO);
    virtual ~MprHotSpotsRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetName(std::string& strName);
    void SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject> spMPRGO);
    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO);
    void SetHotSpotsGraphicObject(std::shared_ptr<HotSpotsGraphicObject> spHotSpotsGO);
    void SetColdSpotsGraphicObject(std::shared_ptr<HotSpotsGraphicObject> spColdSpotsGO);
    void SetCurrentPlanUid(std::string& planUid);
protected:
    virtual void Render_i();
    std::shared_ptr<MPRGraphicObject> m_spMPRGO;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> m_spVolumeGO;
    std::shared_ptr<HotSpotsGraphicObject> m_spHotSpotsGO;
    std::shared_ptr<HotSpotsGraphicObject> m_spColdSpotsGO;

private:
    std::string m_strName;
    std::string m_strPlanUid;
    Mcsf::MedViewer3D::Vector3D m_vecUp;
    Mcsf::MedViewer3D::Vector3D m_vecView;

    //TPS_DISALLOW_COPY_AND_ASSIGN(MprHotSpotsRenderStrategy);
};

TPS_END_NAMESPACE

#endif //TPS_RL_MPR_HOT_SPOTS_RENDERSTRATEGY_H_