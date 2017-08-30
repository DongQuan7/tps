////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fusionalgorithm.cpp
/// 
///  \brief class TpsFusionAlgorithm implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_algorithm.h"
#include "../tps_rl_shaders.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

const char* MprAlgorithm::aFusionProgramUniformNames[FUSION_UNIFORM_COUNT] = 
{
    "matMVP",                               //0
    "matMPRModel2VolumeModel",              //1
    "vCastingVector",                       //2
    "sVolumeData",                          //3
    "vWindowLevel",                         //4
    "fSteps",                               //5
    "sPseudoColorTable",                    //6
    "uiPseudoColorTableSize",               //7
    "iAlgorithmType",                       //8
    "isInverted",                           //9
};

MprAlgorithm::MprAlgorithm()
: mCastingVector(0.0, 0.0, 0.0), mCastingStepNum(0.0)/*, mAlpha(1.0)*/
, mIsColorInverted(false), mMprAlgorithm(MPR_ALGO_MIP)
, mFusionType(FUSION_TYPE_BLEND), mProgram(nullptr)
{
    mMprModel2VolumeModel.SetIdintity();
    mMVPMatrix.SetIdintity();
    //mWorld2VolumeModel.SetIdintity();
    memset(mLocations, -1, FUSION_ALG_LOCATION_COUNT * sizeof(unsigned int));
}

MprAlgorithm::~MprAlgorithm(){

}

void MprAlgorithm::Render(){
    if (mProgram == nullptr) return;

    glUseProgram(mProgram->ID());

    SetUniformValue_i();

    /// Draw VAO
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(mVAOID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

bool MprAlgorithm::Initialize(){

    if (!InitProgram_i() || !GetLocations_i() || !InitVAO_i()){
        return false;
    }
    return true;
}

void MprAlgorithm::Finalize(){

    ClearProgram_i();
    ClearVAO_i();
}

bool MprAlgorithm::InitProgram_i()
{
    mProgram = new GLProgram();
    mProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_MprVert).c_str());
    mProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_MprCastingVert).c_str());
    mProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_MprFrag).c_str());
    mProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_MprCastingFrag).c_str());
    if (!mProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize program.";
        return false;
    }
    return true;
}

bool MprAlgorithm::GetLocations_i(){

    mLocations[0] = glGetAttribLocation(mProgram->ID(), "inVertex");
    if (mLocations[0] == -1){
        TPS_LOG_DEV_ERROR<<"Failed to get the ID of attribute 'inVertex'.";
        return false;
    }

    const char* uniformName;
    for (int i=0; i < FUSION_UNIFORM_COUNT; ++i){
        uniformName = aFusionProgramUniformNames[i];
        mLocations[i + 1] = glGetUniformLocation(mProgram->ID(), uniformName);
        if (mLocations[i + 1] == -1){
            TPS_LOG_DEV_WARNING<<"Failed to get the ID of uniform '"<<uniformName<<"'.";
        }
    }
    return true;
}

bool MprAlgorithm::InitVAO_i(){

    GLfloat aVertex[12] = { -0.5, -0.5, 0.0, 
                            0.5, -0.5, 0.0, 
                            0.5, 0.5, 0.0, 
                            -0.5, 0.5, 0.0 };
    GLuint aIndex[6] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &mVAOID);
    glBindVertexArray(mVAOID);

    glGenBuffers(1, &mVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aVertex), aVertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(mLocations[0]);
    glVertexAttribPointer(mLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &mElementIdxBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementIdxBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aIndex), aIndex, GL_STATIC_DRAW);

    glBindVertexArray(0);
    return true;
}

void MprAlgorithm::ClearProgram_i(){

    if (mProgram){
        delete mProgram;
        memset(mLocations, 0, FUSION_ALG_LOCATION_COUNT * sizeof(unsigned int));
    }
}

void MprAlgorithm::ClearVAO_i(){

    if (mVAOID){
        glDeleteBuffers(1, &mVertexBufferID);
        glDeleteBuffers(1, &mElementIdxBufferID);
        glDeleteVertexArrays(1, &mVAOID);
        mVAOID = 0;
    }
}

void MprAlgorithm::SetUniformValue_i(){
    
    float matMVP[16];
    for (int i=0; i<16; ++i){
        matMVP[i] = static_cast<float>(mMVPMatrix._m[i]);
    }
    glUniformMatrix4fv(mLocations[1], 1, GL_FALSE, matMVP);

    float matMprM2VolumeM[16];
    for (int i=0; i<16; ++i){
        matMprM2VolumeM[i] = static_cast<float>(mMprModel2VolumeModel._m[i]);
    }
    glUniformMatrix4fv(mLocations[2], 1, GL_FALSE, matMprM2VolumeM);

    glUniform4f(mLocations[3], mCastingVector.x, 
        mCastingVector.y, mCastingVector.z, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mVolumeDataTexID);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(mLocations[4], 0);
    glUniform2f(mLocations[5], static_cast<float>(mWindowLevel[0]), 
        static_cast<float>(mWindowLevel[1]));
    glUniform1f(mLocations[6], mCastingStepNum);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, mPseudocolorTableTexID);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(mLocations[7], 1);
    glUniform1ui(mLocations[8], mPseudocolorTableSize);
    //glUniform1f(mLocations[10], mAlpha);

    glUniform1i(mLocations[9], mMprAlgorithm);
    glUniform1i(mLocations[10], mIsColorInverted);
}

void MprAlgorithm::SetMprModel2VolumeModel(const Matrix4x4& matrix){
    mMprModel2VolumeModel = matrix;
}

void MprAlgorithm::SetMVPMatrix(const Matrix4x4& matrix){
    mMVPMatrix = matrix;
}

void MprAlgorithm::SetCastingVector(const Vector3D& vector){
    mCastingVector = vector;
}

void MprAlgorithm::SetCastingStepNum(float stepNum){
    mCastingStepNum = stepNum;
}

float MprAlgorithm::GetCastingStepNum() const
{
    return mCastingStepNum;
}

void MprAlgorithm::SetWindowLevel(double windowCenter, double windowWidth){
    mWindowLevel[0] = windowWidth;
    mWindowLevel[1] = windowCenter;
}

void MprAlgorithm::SetIsColorInverted(bool isInverted){
    mIsColorInverted = isInverted;
}

void MprAlgorithm::SetMprAlgorithmType(MprAlgorithm::MprAlgorithmType eAlgorithmType){
    mMprAlgorithm = eAlgorithmType;
}

void MprAlgorithm::SetFusionType(FUSION_TYPE eFusionType){
    mFusionType = eFusionType;
}

void MprAlgorithm::SetVolumeDataTexID(unsigned int id){
    mVolumeDataTexID = id;
}

void MprAlgorithm::SetPseudocolorTableTexID(unsigned int id){
    mPseudocolorTableTexID = id;
}

void MprAlgorithm::SetPseudocolorTableSize(unsigned int size){
    mPseudocolorTableSize = size;
}

void MprAlgorithm::SetVPMat(const Mcsf::MedViewer3D::Matrix4x4& matrix)
{
    mVPMat = matrix;
}

Mcsf::MedViewer3D::Matrix4x4 MprAlgorithm::GetVPMat() const
{
    return mVPMat;
}

void MprAlgorithm::SetModel2WorldMat(const Mcsf::MedViewer3D::Matrix4x4& matrix)
{
    mM2WMat = matrix;
}

Mcsf::MedViewer3D::Matrix4x4 MprAlgorithm::GetModel2WorldMat() const
{
    return mM2WMat;
}
TPS_END_NAMESPACE  // end namespace tps
