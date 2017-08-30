////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprdoserenderstrategy.h
/// 
///  \brief class TpsMPRDoseRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/02/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPRDOSERENDERSTRATEGY_H_
#define TPS_RL_MPRDOSERENDERSTRATEGY_H_

#include "tps_rl_defs.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsMPRDoseGraphicObject;
class MPRGraphicObject;
class TpsDoseLineGraphicObject;
class GLProgram;

/// \class TpsMPRDoseRenderStrategy
/// 
/// \brief this strategy render dose on the mpr plane
class TPS_RL_DECLSPEC TpsMPRDoseRenderStrategy : public TpsRenderStrategyBase {
public:
    //the constructor and destructor
    TpsMPRDoseRenderStrategy();
    ~TpsMPRDoseRenderStrategy();

    ///interface from RenderStrategyGPU
    //dummy interface
    virtual void Initialize();
    virtual void Finalize();
    //test refresh 
    virtual int CacheChanges();
    //resize the window
    virtual void Resize(int iWidth, int iHeight);

    //////////////////////////////////////////////////////////////////////////
    // \brief set dose graphic object;
    // \param[in] the dose graphic object pointer;
    void SetDoseGraphicObject(std::shared_ptr<TpsMPRDoseGraphicObject> doseGO);

    //////////////////////////////////////////////////////////////////////////
    // \brief set mpr graphic object;
    // \param[in] the mpr graphic object pointer;
    void SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject> mprGO);

    //////////////////////////////////////////////////////////////////////////
    // \brief set dose line object;
    // \param[in] the dose line object pointer;
    void SetDoseLineObject(std::shared_ptr<TpsDoseLineGraphicObject> doseGO);

protected:
    //render
    virtual void Render_i();
private:
    void ResetScreenToDoseMatrix_i(int width, int height);
    void GetScreenBuffer(float* buffer, Mcsf::Point3f* miniVertices, int miniWidth, int miniHeight, 
                      float* screenBuffer, Mcsf::Point3f* screenVertices, int screenWidth, int screenHeight);
    void GetScreenVertices(Mcsf::Point3f* pVertices, Mcsf::Point3f* screenVertices);
    void CalcScreenToWorld_i();
private:
    std::shared_ptr<TpsMPRDoseGraphicObject>    mDoseGO;
    std::shared_ptr<MPRGraphicObject>           mMPRGO;
    std::shared_ptr<TpsDoseLineGraphicObject>   mDoseLineGo;

    Mcsf::MedViewer3D::Matrix4x4            mScreenToWorldMat;

    float                                   mScrToDoseMatrix[9];

    GLProgram*                              mProgram;
    unsigned int                            mDisplayValueLoca;
    unsigned int                            mDisplayColorLoca;
    unsigned int                            mDisplayCountLoca;
    unsigned int                            mDoseSampLoca;
    unsigned int                            mScrToDoseTexLoca;
    unsigned int                            mVisionModeLoca;

    unsigned int                            mVertexArrayID;
    unsigned int                            mArrayBufferID[2];
    unsigned int                            mTexID[3];
    float                                   mTransparencyLoca;
    bool                                    mNeedRecalcScreenToWorld;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMPRDoseRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif