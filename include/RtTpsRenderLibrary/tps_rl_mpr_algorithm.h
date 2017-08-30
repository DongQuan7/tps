////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mpr_algorithm.h
/// 
///  \brief class TpsFusionAlgorithm declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_FUSIONALGORITHM_H_
#define TPS_RL_FUSIONALGORITHM_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

#define FUSION_ALG_LOCATION_COUNT 11
#define FUSION_UNIFORM_COUNT 10

class GLProgram;
/// \class TpsFusionAlgorithm
/// 
/// \brief *****
class TPS_RL_DECLSPEC MprAlgorithm{
public:
    enum MprAlgorithmType{
        MPR_ALGO_AVERAGE,
        MPR_ALGO_MIP,
        MPR_ALGO_MinIP,
    };

public:
    //the constructor and destructor
    MprAlgorithm();
    ~MprAlgorithm();

    //set mpr model 2 volume model
    void SetMprModel2VolumeModel(const Mcsf::MedViewer3D::Matrix4x4& matrix);
    //set model view projection matrix
    void SetMVPMatrix(const Mcsf::MedViewer3D::Matrix4x4& matrix);
    //set mpr casting vector in world c.s.
    void SetCastingVector(const Mcsf::MedViewer3D::Vector3D& vector);
    //set mpr casting step number
    void SetCastingStepNum(float stepNum);
    float GetCastingStepNum() const;
    //set window level
    void SetWindowLevel(double windowCenter, double windowWidth);
    //set opacity
    //void SetAlpha(float alpha);
    //set is color inverted flag
    void SetIsColorInverted(bool isInverted);
    //set mpr algorithm type
    void SetMprAlgorithmType(MprAlgorithmType eAlgorithmType);
    //set fusion type
    void SetFusionType(FUSION_TYPE eFusionType);

    //set volume data 3d texture id
    void SetVolumeDataTexID(unsigned int id);
    //set pseudo color table texture id
    void SetPseudocolorTableTexID(unsigned int id);
    //set pseudo color table size
    void SetPseudocolorTableSize(unsigned int size);

    void SetVPMat(const Mcsf::MedViewer3D::Matrix4x4& matrix);
    Mcsf::MedViewer3D::Matrix4x4 GetVPMat() const;
    void SetModel2WorldMat(const Mcsf::MedViewer3D::Matrix4x4& matrix);
    Mcsf::MedViewer3D::Matrix4x4 GetModel2WorldMat() const;

    void Render();

    bool Initialize();

    void Finalize();

private:
    bool InitProgram_i();
    bool GetLocations_i();
    bool InitVAO_i();
    void ClearProgram_i();
    void ClearVAO_i();
    void SetUniformValue_i();

private:
    Mcsf::MedViewer3D::Matrix4x4        mMprModel2VolumeModel;
    Mcsf::MedViewer3D::Matrix4x4        mMVPMatrix;

    Mcsf::MedViewer3D::Vector3D         mCastingVector;
    float                               mCastingStepNum;
    double                              mWindowLevel[2];
    //float                               mAlpha;
    bool                                mIsColorInverted;
    MprAlgorithmType                    mMprAlgorithm;
    FUSION_TYPE                         mFusionType;

    GLProgram*                          mProgram;
    unsigned int                        mVolumeDataTexID;
    unsigned int                        mPseudocolorTableTexID;
    unsigned int                        mPseudocolorTableSize;

    unsigned int                        mLocations[FUSION_ALG_LOCATION_COUNT];
    unsigned int                        mVAOID;
    unsigned int                        mVertexBufferID;
    unsigned int                        mElementIdxBufferID;

    Mcsf::MedViewer3D::Matrix4x4        mVPMat;
    Mcsf::MedViewer3D::Matrix4x4        mM2WMat;

    static const char* aFusionProgramUniformNames[FUSION_UNIFORM_COUNT];
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(MprAlgorithm);
};

TPS_END_NAMESPACE  // end namespace tps

#endif