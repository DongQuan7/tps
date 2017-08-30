////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_gpu_marching_cubes.cpp
/// 
///  \brief class GPUMarchingCubes implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/11/17
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gpu_marching_cubes.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "RtTpsROIUtility/tps_ru_marching_cubes_tables.h"
#include "../tps_rl_shaders.h"

TPS_BEGIN_NAMESPACE

//sigma = 1
float aGaussianKernel[27] = 
{
    0.0206f, 0.0339f, 0.0206f,
    0.0339f, 0.0560f, 0.0339f,
    0.0206f, 0.0339f, 0.0206f,

    0.0339f, 0.0560f, 0.0339f,
    0.0560f, 0.0923f, 0.0560f,
    0.0339f, 0.0560f, 0.0339f,

    0.0206f, 0.0339f, 0.0206f,
    0.0339f, 0.0560f, 0.0339f,
    0.0206f, 0.0339f, 0.0206f,
};

GPUMarchingCubes::GPUMarchingCubes()
{

}

GPUMarchingCubes::~GPUMarchingCubes()
{

}

bool GPUMarchingCubes::Initialize()
{
    //edge table texture
    glGenTextures(1, &mEdgeTableTexId);
    glBindTexture(GL_TEXTURE_1D, mEdgeTableTexId);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R16UI, 256, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, MarchingCubesTables::mEdgeTable);
    glBindTexture(GL_TEXTURE_1D, 0);

    //glGetError();
    //unsigned int size = 256;
    //unsigned short* edgeBuffer = new unsigned short[size];
    //memset(edgeBuffer, 0, size * sizeof(unsigned short));
    //glBindTexture(GL_TEXTURE_1D, mEdgeTableTexId);
    //glGetTexImage(GL_TEXTURE_1D, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, edgeBuffer);
    //glBindTexture(GL_TEXTURE_1D, 0);

    //tri-table texture
    glGenTextures(1, &mTriTableTexId);
    glBindTexture(GL_TEXTURE_2D, mTriTableTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 16, 256, 0, GL_RED_INTEGER, GL_BYTE, MarchingCubesTables::mTriTable);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &mEdgeVertexTexId);
    glBindTexture(GL_TEXTURE_1D, mEdgeVertexTexId);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R8UI, 12, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, MarchingCubesTables::mEdgeVertexTable);
    glBindTexture(GL_TEXTURE_1D, 0);

    glGenTextures(1, &mVertexCountTableTexId);
    glBindTexture(GL_TEXTURE_1D, mVertexCountTableTexId);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R8UI, 256, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, MarchingCubesTables::mVertexCountTable);
    glBindTexture(GL_TEXTURE_1D, 0);

    //volume texture buffer
    glGenTextures(1, &mOrgDataTexId);
    glBindTexture(GL_TEXTURE_3D, mOrgDataTexId);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    glGenTextures(1, &mDataTexId);
    glBindTexture(GL_TEXTURE_3D, mDataTexId);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    //glGenBuffers(1, &mDataTBOId);
    //glBindBuffer(GL_TEXTURE_BUFFER, mDataTBOId);
    //glBindBuffer(GL_TEXTURE_BUFFER, 0);
    //glGenTextures(1, &mDataTexId);
    //glBindTexture(GL_TEXTURE_BUFFER, mDataTexId);
    //glBindTexture(GL_TEXTURE_BUFFER, 0);

    //buffer object, gaussian kernel
    glGenBuffers(1, &mGaussianKernelSSBOId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mGaussianKernelSSBOId);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 27, aGaussianKernel, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //buffer object, indicates the whole vertex count
    glGenBuffers(1, &mVertexCountSSBOId);
    //vertex counter
    glGenBuffers(1, &mVertexCounterACBOId);

    //image buffer, table index cache
    glGenBuffers(1, &mCubeIndexCacheTBOId);
    glBindBuffer(GL_TEXTURE_BUFFER, mCubeIndexCacheTBOId);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glGenTextures(1, &mCubeIndexCacheTexId);
    glBindTexture(GL_TEXTURE_BUFFER, mCubeIndexCacheTexId);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    //output, vertex
//     glGenBuffers(1, &mVertexTBOId);
//     glBindBuffer(GL_TEXTURE_BUFFER, mVertexTBOId);
//     glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glGenTextures(1, &mVertexTexId);
    glBindTexture(GL_TEXTURE_BUFFER, mVertexTexId);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    //output, normal
//     glGenBuffers(1, &mNormalTBOId);
//     glBindBuffer(GL_TEXTURE_BUFFER, mNormalTBOId);
//     glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glGenTextures(1, &mNormalTexId);
    glBindTexture(GL_TEXTURE_BUFFER, mNormalTexId);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    //program
    mSmoothingDataProgram = new GLProgram();
    mSmoothingDataProgram->AttachShader(GLProgram::Compute_Shader, 
        Shaders::Get(IDR_SHADER_DataSmoothingComp).c_str());
    if (!mSmoothingDataProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link smoothing program.";
        printf("Failed to link smoothing program.");
        return false;
    }

    mPreprocessingProgram = new GLProgram();
    mPreprocessingProgram->AttachShader(GLProgram::Compute_Shader, 
        Shaders::Get(IDR_SHADER_GpuMCPreprocessingComp).c_str());
    if (!mPreprocessingProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link preprocessing program.";
        printf("Failed to link preprocessing program.");
        return false;
    }

    mMCProgram = new GLProgram();
    mMCProgram->AttachShader(GLProgram::Compute_Shader, 
        Shaders::Get(IDR_SHADER_GpuMCComp).c_str());
    if (!mMCProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link marching cubes program.";
        printf("Failed to link marching cubes program.");
        return false;
    }
    CHECK_GL_ERRORS;
    return true;
}

void GPUMarchingCubes::Finalize()
{
    glDeleteTextures(1, &mEdgeTableTexId);
    glDeleteTextures(1, &mTriTableTexId);
    glDeleteTextures(1, &mEdgeVertexTexId);
    glDeleteTextures(1, &mVertexCountTableTexId);
    glDeleteTextures(1, &mOrgDataTexId);
    glDeleteTextures(1, &mDataTexId);
    glDeleteTextures(1, &mVertexTexId);
    glDeleteTextures(1, &mNormalTexId);
    glDeleteTextures(1, &mCubeIndexCacheTexId);

    //glDeleteBuffers(1, &mDataTBOId);
    glDeleteBuffers(1, &mVertexCountSSBOId);
    glDeleteBuffers(1, &mVertexCounterACBOId);
    //glDeleteBuffers(1, &mVertexTBOId);
    //glDeleteBuffers(1, &mNormalTBOId);
    glDeleteBuffers(1, &mCubeIndexCacheTBOId);
    glDeleteBuffers(1, &mGaussianKernelSSBOId);

    TPS_DEL_PTR(mSmoothingDataProgram);
    TPS_DEL_PTR(mPreprocessingProgram);
    TPS_DEL_PTR(mMCProgram);
}

void DumpMemorySummary()
{
    GLint total_mem_kb, cur_avail_mem_kb;
    glGetIntegerv(
        GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,
        &total_mem_kb);
    glGetIntegerv(
        GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
        &cur_avail_mem_kb);
    printf("\nGraphic memory summary: total %d, used: %d (MB).", 
        total_mem_kb / 1024, (total_mem_kb - cur_avail_mem_kb) / 1024);
}

void GPUMarchingCubes::Calculate(GLuint vbo, GLuint nbo, unsigned int& vertexCount)
{
    //int* calc_dim = mData.iCalcDim;
    int* calc_dim = mData.bEnableSmoothing ? mData.iSmoothedDim : mData.iDim;
    if (calc_dim[0] <= 0 || calc_dim[1] <= 0 || calc_dim[2] <= 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        vertexCount = 0;
        return;
    }

    DumpMemorySummary();

    //glPushAttrib(GL_ALL_ATTRIB_BITS);
    glActiveTexture(GL_TEXTURE0);
    PrepareVolumeData_i();
    PreparePreprocessingData_i();

    //pre-processing
    mPreprocessingProgram->Bind();
    //invocation space is calc_dim + 2;
    glDispatchCompute(calc_dim[0] / 2, calc_dim[1] / 2, calc_dim[2] - 1);
    //glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    mPreprocessingProgram->Unbind();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVertexCountSSBOId);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &vertexCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //glGetError();
    //unsigned int size = mData.iDim[0] * mData.iDim[1] * mData.iDim[2];
    //unsigned short* tableIndexBuffer = new unsigned short[size];
    //memset(tableIndexBuffer, 0, size * sizeof(unsigned short));
    //glBindBuffer(GL_TEXTURE_BUFFER, mCubeIndexCacheTBOId);
    //glGetBufferSubData(GL_TEXTURE_BUFFER, 0, size*sizeof(unsigned short), tableIndexBuffer);
    //glBindBuffer(GL_TEXTURE_BUFFER, 0);
    //CHECK_GL_ERRORS;

    //calculate tri-mesh
    PrepareMCData_i(vbo, nbo, vertexCount);

    mMCProgram->Bind();
    glDispatchCompute(calc_dim[0] / 2, calc_dim[1] / 2, calc_dim[2] - 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    mMCProgram->Unbind();

    //float* vertexBuffer = new float[vertexCount * 3];
    //memset(vertexBuffer, 0, vertexCount * 3 * sizeof(float));
    //glBindBuffer(GL_TEXTURE_BUFFER, vbo);
    //glGetBufferSubData(GL_TEXTURE_BUFFER, 0, vertexCount * 3 * sizeof(float), vertexBuffer);
    //glBindBuffer(GL_TEXTURE_BUFFER, 0);
    //float* normalBuffer = new float[vertexCount * 3];
    //memset(normalBuffer, 0, vertexCount * 3 * sizeof(float));
    //glBindBuffer(GL_TEXTURE_BUFFER, nbo);
    //glGetBufferSubData(GL_TEXTURE_BUFFER, 0, vertexCount * 3 * sizeof(float), normalBuffer);
    //glBindBuffer(GL_TEXTURE_BUFFER, 0);
    //delete[] vertexBuffer;
    //delete[] normalBuffer;

    ReleaseResource_i();
    //glPopAttrib();
}

void GPUMarchingCubes::SetData(const MCData& data)
{
    mData = data;
}

void GPUMarchingCubes::PrepareVolumeData_i()
{
    glGetError();

    GLuint targetTex = mData.bEnableSmoothing ? mOrgDataTexId : mDataTexId;
    //upload original volume data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_3D, targetTex);
    glTexImage3D(
        GL_TEXTURE_3D, 
        0, 
        mData.eInternalFormat, 
        mData.iDim[0], 
        mData.iDim[1],
        mData.iDim[2], 
        0, 
        mData.eFormat, 
        mData.eType, 
        mData.pData);
    glBindTexture(GL_TEXTURE_3D, 0);
    //if need not smooth, return
    if (!mData.bEnableSmoothing)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        return;
    }

    glBindTexture(GL_TEXTURE_3D, mDataTexId);
    glTexImage3D(GL_TEXTURE_3D, 
        0, 
        GL_R32F, 
        mData.iDim[0], 
        mData.iDim[1],
        mData.iDim[2], 
        0, 
        GL_RED, 
        GL_FLOAT, 
        NULL);
    glBindTexture(GL_TEXTURE_3D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //三维纹理做为image时，layered必须为true
    glBindImageTexture(0, mDataTexId, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mGaussianKernelSSBOId);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mGaussianKernelSSBOId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    mSmoothingDataProgram->Bind();
    mSmoothingDataProgram->SetUniformi("volumeDim", mData.iDim, 3);
    mSmoothingDataProgram->BindTexture(0, GL_TEXTURE_3D, mOrgDataTexId, "texOrgData");

    glDispatchCompute(mData.iDim[0], mData.iDim[1], mData.iDim[2]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    mSmoothingDataProgram->Unbind();
    CHECK_GL_ERRORS;
}

void GPUMarchingCubes::PreparePreprocessingData_i()
{
    int* calc_dim = mData.iCalcDim;
    //int* calc_dim = mData.bEnableSmoothing ? mData.iSmoothedDim : mData.iDim;
    //table index cache image buffer
    glBindBuffer(GL_TEXTURE_BUFFER, mCubeIndexCacheTBOId);
    glBufferData(GL_TEXTURE_BUFFER, (calc_dim[0] + 2) * (calc_dim[1] + 2) * 
        (calc_dim[2] + 2)*sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glBindTexture(GL_TEXTURE_BUFFER, mCubeIndexCacheTexId);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R16UI, mCubeIndexCacheTBOId);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindImageTexture(0, mCubeIndexCacheTexId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);

    //init total vertex count SSBO
    unsigned int vertexCount = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVertexCountSSBOId);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &vertexCount, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mVertexCountSSBOId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    mPreprocessingProgram->Bind();
    mPreprocessingProgram->SetUniformf("isoLevel", &mData.fIsoLevel, 1);
    mPreprocessingProgram->SetUniformi("volumeDim", mData.iDim, 3);
    mPreprocessingProgram->SetUniformi("calcSpaceDim", calc_dim, 3);
    mPreprocessingProgram->BindTexture(0, GL_TEXTURE_3D, mDataTexId, "texVolumeData");
    mPreprocessingProgram->BindTexture(1, GL_TEXTURE_1D, mVertexCountTableTexId, "texVertexCountTable");
    mPreprocessingProgram->Unbind();
}

void GPUMarchingCubes::PrepareMCData_i(GLuint vbo, GLuint nbo, unsigned int vertexCount)
{
    glGetError();
    //initialize vertex atomic counter
    int counter = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, mVertexCounterACBOId);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &counter, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, mVertexCounterACBOId);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    //triangle vertices image buffer
    glBindBuffer(GL_TEXTURE_BUFFER, vbo);
    glBufferData(GL_TEXTURE_BUFFER, vertexCount * 3 * sizeof(float), 
        NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glBindTexture(GL_TEXTURE_BUFFER, mVertexTexId);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, vbo);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindImageTexture(0, mVertexTexId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    //vertex normals image buffer
    glBindBuffer(GL_TEXTURE_BUFFER, nbo);
    glBufferData(GL_TEXTURE_BUFFER, vertexCount * 3 * sizeof(float), 
        NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glBindTexture(GL_TEXTURE_BUFFER, mNormalTexId);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, nbo);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindImageTexture(1, mNormalTexId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    //CHECK_GL_ERRORS;

//     unsigned int count = 0;
//     glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVertexCountSSBOId);
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &count, GL_DYNAMIC_READ);
//     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mVertexCountSSBOId);
//     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //int* calc_dim = mData.iCalcDim;
    int* calc_dim = mData.bEnableSmoothing ? mData.iSmoothedDim : mData.iDim;
    mMCProgram->Bind();
    mMCProgram->SetUniformf("isoLevel", &mData.fIsoLevel, 1);
    mMCProgram->SetUniformi("volumeDim", mData.iDim, 3);
    mMCProgram->SetUniformi("calcSpaceDim", calc_dim, 3);
    mMCProgram->BindTexture(0, GL_TEXTURE_3D, mDataTexId, "texVolumeData");
    mMCProgram->BindTexture(1, GL_TEXTURE_1D, mEdgeTableTexId, "texEdgeTable");
    mMCProgram->BindTexture(2, GL_TEXTURE_2D, mTriTableTexId, "texTriTable");
    mMCProgram->BindTexture(3, GL_TEXTURE_1D, mEdgeVertexTexId, "texEdgeVertexTable");
    mMCProgram->BindTexture(4, GL_TEXTURE_BUFFER, mCubeIndexCacheTexId, "texCubeIndexCache");
    mMCProgram->Unbind();
}

void GPUMarchingCubes::ReleaseResource_i()
{
    glBindTexture(GL_TEXTURE_3D, mDataTexId);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 0, 0, 0, 0, GL_RED, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_3D, 0);

    glBindTexture(GL_TEXTURE_3D, mOrgDataTexId);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 0, 0, 0, 0, GL_RED, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_3D, 0);

    glBindBuffer(GL_TEXTURE_BUFFER, mCubeIndexCacheTBOId);
    glBufferData(GL_TEXTURE_BUFFER, 0, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
}
TPS_END_NAMESPACE