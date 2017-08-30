////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_gpu_marching_cubes.h
/// 
///  \brief class GPUMarchingCubes declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/11/17
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_GPU_MARCHING_CUBES_H_
#define TPS_RL_GPU_MARCHING_CUBES_H_

#include "tps_rl_defs.h"
#include "gl/glew.h"

TPS_BEGIN_NAMESPACE

class GLProgram;

struct MCData
{
    void* pData;
    int iDim[3];
    int iBytesPerPixel;
    float fIsoLevel;
    int iCalcDim[3];
    //for GL
    GLenum eType;
    GLenum eFormat;
    GLenum eInternalFormat;
    //smoothing paras
    bool bEnableSmoothing;
    int iSmoothedDim[3];
};

class TPS_RL_DECLSPEC GPUMarchingCubes
{
public:
    GPUMarchingCubes();
    ~GPUMarchingCubes();

    bool Initialize();

    void Finalize();

    void SetData(const MCData& data);

    void Calculate(GLuint vbo, GLuint nbo, unsigned int& vertexCount);
private:

    void PrepareVolumeData_i();
    void PreparePreprocessingData_i();
    void PrepareMCData_i(GLuint vbo, GLuint nbo, unsigned int vertexCount);
    void ReleaseResource_i();

    MCData mData;

    GLProgram* mSmoothingDataProgram;
    GLProgram* mPreprocessingProgram;
    GLProgram* mMCProgram;

    GLuint mEdgeTableTexId;
    GLuint mTriTableTexId;
    GLuint mVertexCountTableTexId;
    GLuint mEdgeVertexTexId;

    GLuint mOrgDataTexId;
    GLuint mDataTexId;
    //GLuint mDataTBOId;
    GLuint mVertexTexId;
    //GLuint mVertexTBOId;
    GLuint mNormalTexId;
    //GLuint mNormalTBOId;

    GLuint mCubeIndexCacheTexId;
    GLuint mCubeIndexCacheTBOId;
    GLuint mVertexCountSSBOId;
    GLuint mVertexCounterACBOId;
    GLuint mGaussianKernelSSBOId;
    //GLuint mVertexValuesSSBOId;

    //unsigned int mTotalVertexCount;
};

TPS_END_NAMESPACE

#endif // gpu_marching_cubes_h__
