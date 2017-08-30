////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_shadedvoistrategy.h
/// 
///  \brief class TpsShadedVoiStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/2/26
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHADEDVOISTRATEGY_H_
#define TPS_RL_SHADEDVOISTRATEGY_H_

#include "tps_rl_defs.h"

#include <map>
#include <vector>
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
        class Matrix4x4;
    }
}

class GLSLProgramObject;

TPS_BEGIN_NAMESPACE
class CameraGraphicObject;
class TpsBeamGraphicObjectCollection;
class NormalGroupCollection;
class ShadedVoiGraphicObject;
class StateControlGraphicObject;
class ITpsDataRepository;
class TpsVOIGOCollection;
class DoseMeshGraphicObjectCollection;
class MPRGraphicObject;

class TPS_RL_DECLSPEC TpsShadedVoiStrategy:
    public TpsRenderStrategyBase {
public:
    TpsShadedVoiStrategy(WINDOW_TYPE  mWindowType);
    virtual ~TpsShadedVoiStrategy();

    virtual void Initialize();
    virtual void Finalize();
    void SetDataRepository(ITpsDataRepository* repos) {mDataRepository = repos;}
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetShadedSurfaceGraphicObject(std::shared_ptr<ShadedVoiGraphicObject>);
    void SetCameraGO(std::shared_ptr<CameraGraphicObject>);
    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);
    void SetNormalGroupCollection(std::shared_ptr<NormalGroupCollection> normalGroupCollection);
    void SetStateControlGO(std::shared_ptr<StateControlGraphicObject> stateControlGO);
    void SetVOICollectionGO(std::shared_ptr<TpsVOIGOCollection> voiCollection);
    void SetDoseMeshGOCollection(std::shared_ptr<DoseMeshGraphicObjectCollection>);
    void SetMprGo(std::shared_ptr<MPRGraphicObject>, std::shared_ptr<MPRGraphicObject>, std::shared_ptr<MPRGraphicObject>);
protected:
    virtual void Render_i();

private:
    //bool    IsBeamView_i();
    //void    LoadShader_i();

    virtual void RenderOpenMesh_i();
    virtual void RenderPrism_i();

    void InitDualPeelingRenderTargets();
    void DeleteDualPeelingRenderTargets();
    void DrawModel(std::string voiUid, bool showBeams);
    void BuildShaders();
    void DestroyShaders();
    void RenderDualPeeling(bool showBeams);
    void reshape(int w, int h);
    void idle();

    void DrawDoseMesh(std::string uid);
    void Draw3DSegment(std::string uid);
    void DrawMpr(std::string uid);
    void DrawVolume(std::string uid);

    bool GenerateBeamBuffers();
    void ReleaseBeamBuffers();

    void GenerateMprTextureBuffer();

private:
    std::shared_ptr<CameraGraphicObject>                 mCameraGO;
    std::shared_ptr<ShadedVoiGraphicObject>                 mShadedSurfaceGO;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolumeGO;
    std::shared_ptr<MPRGraphicObject>                       mMprAxialGO;
    std::shared_ptr<MPRGraphicObject>                       mMprCoronalGO;
    std::shared_ptr<MPRGraphicObject>                       mMprSagittalGO;
    std::shared_ptr<MPRGraphicObject>                       mMprGO;
    std::shared_ptr<StateControlGraphicObject>              mStateGO;
    std::shared_ptr<DoseMeshGraphicObjectCollection>        m_spDoseMeshGOCollection;     // for Dose surface
    ITpsDataRepository          *mDataRepository;
    std::shared_ptr<NormalGroupCollection>                  mNormalGroupCollection;
    std::shared_ptr<TpsVOIGOCollection>                     mVoiCollectionGO;
    GLSLProgramObject                                       *g_shaderDualInit;
    GLSLProgramObject                                       *g_shaderDualPeel;
    GLSLProgramObject                                       *g_shaderDualFinal;
    GLuint                                                  g_dualPeelingSingleFboId;
    GLuint                                                  g_dualDepthTexId[2];
    GLuint                                                  g_dualFrontBlenderTexId[2];

    unsigned int mVertexArrayID;

    unsigned int mVertexBufferID;
    unsigned int mIndexBufferID;
    unsigned int mNormalBufferID;
    unsigned int mColorBufferID;

    int   mTotalFace;

    unsigned int mAttribLocation[3];
    unsigned int mUniformLocation[2];

    WINDOW_TYPE  mWindowType;

    int          mTotalVertexSize;
    int          mTotalIndexSize;
    std::map<std::string, float*> mVertexBufferMap;
    std::map<std::string, float*> mNormalBufferMap;
    std::map<std::string, unsigned int*> mIndexBufferMap;
    std::map<std::string, int> mTotalTriangleSizeMap;

    std::vector<std::string> mVoiMap;

    std::map<std::string, float*> mBeamEdgeMap;

    std::map<std::string, float*> mBeamFurtherEdgeMap;

    std::map<std::string, float*> mBeamSideLineMap;

    std::map<std::string, float*> mBeamSideWallMap;

    std::map<std::string, float*> mBeamColorMap;

    std::map<std::string, int> mBeamEdgeFloatNumMap;

    std::map<std::string, int> mBeamFurtherEdgeFloatNumMap;

    std::map<std::string, int> mBeamSideLineFloatNumMap;

    std::map<std::string, int> mBeamSideWallFloatNumMap;

    std::map<std::string, unsigned int*> mBeamEdgeIndexMap;

    std::map<std::string, unsigned int*> mBeamFurtherEdgeIndexMap;

    std::map<std::string, unsigned int*> mBeamSideLineIndexMap;

    std::map<std::string, unsigned int*> mBeamSideWallIndexMap;

	std::map<std::string, float*> mBeamCenterLineMap;

    Mcsf::MedViewer3D::Matrix4x4       mScreenToTextMat;

    unsigned int mMVPUniformLocation;

    unsigned int mMPRTextureID;

    unsigned int mMPRTextureVertID;

    unsigned char currentSection;

    bool needRefreshBeamBuffer;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsShadedVoiStrategy);



};



TPS_END_NAMESPACE

#endif
