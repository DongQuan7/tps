#ifndef TPS_RL_MPR_ROI_RENDER_STRATEGY_H_
#define TPS_RL_MPR_ROI_RENDER_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include <vector>

namespace Mcsf 
{
    namespace MedViewer3D 
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class CSGOCollection;
//class ContourSetGraphicObject;
class MPRGraphicObject;
class TpsVOIGOCollection;
//class ComposableGraphicObject;
class Contour;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;
typedef std::shared_ptr<MPRGraphicObject> MprGOPtr;
//typedef std::shared_ptr<ContourSetGraphicObject> ContourSetGOPtr;
//typedef std::shared_ptr<CSGOCollection> CSGOCollectionPtr;
//typedef std::shared_ptr<ComposableGraphicObject> ComposableCSGOCollectionPtr;
typedef std::shared_ptr<TpsVOIGOCollection> VOIGOCollectionPtr;

class TPS_RL_DECLSPEC MprRoiRenderStrategy : public TpsRenderStrategyBase
{
public:
    //the constructor and destructor
    MprRoiRenderStrategy();
    virtual ~MprRoiRenderStrategy();

    virtual int CacheChanges();

    void SetVolumeGO(const VolumeGOPtr& volGO);
    void SetMPRGO(const MprGOPtr& mprGO);
    //void SetComposableCSGOCollection(const ComposableCSGOCollectionPtr& composableGO);
    void SetVOIGOCollection(const VOIGOCollectionPtr& voigoCollection);

protected:
    virtual void Render_i();
    void DrawFill_i(const std::vector<Contour>& contours);

private:
    VolumeGOPtr         mVolumeGO;
    MprGOPtr            mMprGO;
    //ComposableCSGOCollectionPtr   mComposableCsGOCollection;
    VOIGOCollectionPtr  mVOIGOCollection;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(MprRoiRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif