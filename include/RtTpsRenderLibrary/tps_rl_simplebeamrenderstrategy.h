//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_simplebeamrenderstrategy.h
///  \brief  
///  \
///  class  TpsSimpleBeamRenderStrategy
///  \version 1.0
///  \date  2014/11/25
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SIMPLEBEAM_RENDERSTRATEGY_H
#define TPS_RL_SIMPLEBEAM_RENDERSTRATEGY_H

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include <vector>

namespace Viewer3D = Mcsf::MedViewer3D;
namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}
TPS_BEGIN_NAMESPACE
class TpsBeamTemplateGraphicObject;
class TpsSimpleBeamGraphicObject;
class MPRGraphicObject;

class TPS_RL_DECLSPEC TpsSimpleBeamRenderStrategy : public TpsRenderStrategyBase
{
public:
    //////////////////////////////////////////////////////////////////////////
    // \brief constructor
    TpsSimpleBeamRenderStrategy();

    //////////////////////////////////////////////////////////////////////////
    // \brief destructor
    virtual ~TpsSimpleBeamRenderStrategy(void);

    //////////////////////////////////////////////////////////////////////////
    // \brief virtual methods derived from base class
    virtual void Initialize();
    virtual void Resize(int iWidth, int iHeight);
    virtual int CacheChanges();

    //////////////////////////////////////////////////////////////////////////
    // \brief set related mpr graphic object
    // \param[in] shared pointer of mpr graphic object;
    void SetMprGO(std::shared_ptr<MPRGraphicObject>);

    //////////////////////////////////////////////////////////////////////////
    // \brief set related volume graphic object;
    // \param[in] shared pointer of volume graphic;
    void SetVolumeGO(std::shared_ptr<Viewer3D::GraphicObjVolume>);

    //////////////////////////////////////////////////////////////////////////
    // \brief set beam template preview graphic object;
    // \param[in] shared pointer of beam collection graphic
    //void SetBeamCollGraphicObject(std::shared_ptr<TpsBeamGraphicObjectCollection>);
    void SetBeamTemplateGO(std::shared_ptr<TpsBeamTemplateGraphicObject> btGo);

protected:
    virtual void Render_i();

private:
    //hide the clone interface from IRenderStrategy
    virtual std::shared_ptr<IRenderStrategy> Clone() const { return nullptr; }

    std::vector<std::pair<Viewer3D::Point3D,Viewer3D::Point3D>> ComputeIntersectPairList_i(const Viewer3D::Matrix4x4 &);

    static int ComputeIntersect_i(const Viewer3D::Point3D &, const Viewer3D::Point3D &,
                                    Viewer3D::Point3D *);

    //std::vector<std::pair<Viewer3D::Point3D,Viewer3D::Point3D>> ComputeIntersectPairList_i(std::shared_ptr<TpsSegmentGraphicObject>,const Viewer3D::Matrix4x4 &);

    // \brief calculate world to view transformation matrix based on MPR plane
    void CalculateWorld2ViewTM_i();

private:
    //std::shared_ptr<TpsBeamTemplateGraphicObject>           mBeamTemplate;
    std::shared_ptr<MPRGraphicObject>                           mMprGO;
    std::shared_ptr<Viewer3D::GraphicObjVolume>                mVolumeGO;
    std::shared_ptr<TpsBeamTemplateGraphicObject>             mBeamTemplateGO;

    Viewer3D::Matrix4x4                                        mWorld2View;
    typedef std::vector<std::pair<Viewer3D::Point3D,Viewer3D::Point3D>> BeamLine;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSimpleBeamRenderStrategy);
};
TPS_END_NAMESPACE
#endif