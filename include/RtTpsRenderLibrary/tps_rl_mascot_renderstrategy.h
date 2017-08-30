////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mascot_renderstrategy.h
/// 
///  \brief class MascotRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/09/05
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MASCOT_RENDERSTRATEGY_H_
#define TPS_RL_MASCOT_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"

TPS_BEGIN_NAMESPACE

class MascotGraphicObject;
class MPRGraphicObject;

class TPS_RL_DECLSPEC MascotRenderStrategy : public TpsRenderStrategyBase {
public:
    friend class MascotRenderStrategyUT_Render_Test;
public:
    MascotRenderStrategy();
    MascotRenderStrategy(std::shared_ptr<MPRGraphicObject> spMPRGO);
    virtual ~MascotRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetName(std::string& strName);
    void SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject> spMPRGO);

protected:
    virtual void Render_i();

    std::shared_ptr<MPRGraphicObject> m_spMPRGO;
    std::shared_ptr<MascotGraphicObject> m_spMascotGO;

private:
    std::string m_strName;

    Mcsf::MedViewer3D::Vector3D m_vecUp;
    Mcsf::MedViewer3D::Vector3D m_vecView;

    //TPS_DISALLOW_COPY_AND_ASSIGN(MascotRenderStrategy);
};

TPS_END_NAMESPACE

#endif //TPS_RL_MASCOT_RENDERSTRATEGY_H_