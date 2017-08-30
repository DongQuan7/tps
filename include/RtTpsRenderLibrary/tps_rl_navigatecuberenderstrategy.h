
#ifndef TPS_RL_NAVIGATECUBERENDERSTRATEGY_H_
#define TPS_RL_NAVIGATECUBERENDERSTRATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE
class MPRGraphicObject;

class TPS_RL_DECLSPEC MPRNavigateCubeRenderstrategy : public TpsRenderStrategyBase {
public:
    MPRNavigateCubeRenderstrategy();
    ~MPRNavigateCubeRenderstrategy();
    //virtual methods
    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);
    //////////////////////////////////////////////////////////////////////////
    // \brief set volume go;
    void SetVolumeGraphicObj(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);

    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);

protected:
    virtual void Render_i();

private:
    unsigned int mVertexArrayID;
    unsigned int mBufferVertexID;
    unsigned int mTextureID;
    unsigned int mFboID;
    std::shared_ptr<MPRGraphicObject>                    mMprGraphicObject;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> mVolumeGraphicObject;
    Mcsf::MedViewer3D::Matrix4x4                         mMatViewProjection;

private:
    float GetCubeArea_i();
    void DrawCube_i(float z1, float z2, float xOffset, float yOffset);
    void DrawFace_i(float z, float xOffset, float yOffset);
        
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MPRNavigateCubeRenderstrategy);


};


TPS_END_NAMESPACE
#endif