#ifndef TPS_RL_BEVBEAMRENDERSTRATEGY_H
#define TPS_RL_BEVBEAMRENDERSTRATEGY_H
#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"
namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE
class TpsBeamGraphicObjectCollection;
//class ShadedVoiGraphicObject;
class NormalGroupCollection;
class CameraGraphicObject;
class TPS_RL_DECLSPEC BevBeamRenderStrategy : public TpsRenderStrategyBase {
public:
    BevBeamRenderStrategy();
    virtual ~BevBeamRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    //void SetBeamGraphicObjectCollection(std::shared_ptr<TpsBeamGraphicObjectCollection>);
    void SetNormalGroupCollection(std::shared_ptr<NormalGroupCollection> normalGroupCollection);

    void SetCameraGO(std::shared_ptr<CameraGraphicObject>);
    //void SetShadedSurfaceGraphicObject(std::shared_ptr<ShadedVoiGraphicObject>);

    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);

protected:
    virtual void Render_i();
private:
    bool     IsBeamView_i();
private:
    std::shared_ptr<NormalGroupCollection>                             mNormalGroupCollection;
    std::shared_ptr<CameraGraphicObject>                            mCameraGO;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>               mVolumeGo;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(BevBeamRenderStrategy);
};

TPS_END_NAMESPACE


#endif