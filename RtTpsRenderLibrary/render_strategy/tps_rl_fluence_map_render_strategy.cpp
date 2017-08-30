////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fluence_map_render_strategy.cpp
/// 
///  \brief class FluenceMapRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/06
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <memory>
#include "RtTpsRenderLibrary/tps_rl_fluence_map_render_strategy.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"

#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "../tps_rl_shaders.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

//initialize lut parameters
//currently we hard coded this, it will be configurable in version 2.0
const int lutParaCount = 11;
const float lutValues[lutParaCount] = {
    0.f, 0.02f, 0.15f, 0.24f, 0.35f, 0.46f, 0.56f, 0.67f, 0.78f, 0.89f, 1.f };
const unsigned char lutColors[lutParaCount * 3] = {
    0, 0, 0,       /*|*/ 69, 5, 11,    /*|*/ 186, 16, 29,  /*|*/ 
    237, 41, 36,   /*|*/ 237, 79, 36,  /*|*/ 231, 125, 33, /*|*/
    240, 186, 8,   /*|*/ 252, 243, 2,  /*|*/ 255, 255, 73, /*|*/
    255, 255, 203, /*|*/ 255, 255, 255 };

FluenceMapRenderStrategy::FluenceMapRenderStrategy()
{

}

FluenceMapRenderStrategy::~FluenceMapRenderStrategy()
{

}

void CheckGLStatus() 
{
    GLenum err = glGetError(); 
    switch (err) 
    { 
    case GL_NO_ERROR: 
        printf("\nNo error."); 
        break; 
    case GL_INVALID_ENUM: 
        printf("\nInvalid enum."); 
        break; 
    case GL_INVALID_VALUE: 
        printf("\nInvalid value."); 
        break; 
    case GL_INVALID_OPERATION: 
        printf("\nInvalid operation."); 
        break; 
    case GL_INVALID_FRAMEBUFFER_OPERATION: 
        printf("\nInvalid frame operation."); 
        break; 
    case GL_OUT_OF_MEMORY: 
        printf("\nOut of memory."); 
        break; 
    case GL_STACK_UNDERFLOW: 
        printf("\nStack underflow."); 
        break; 
    case GL_STACK_OVERFLOW: 
        printf("\nStack overflow."); 
        break; 
    default: 
        printf("\nUnknown error."); 
        break;
    }
}

void FluenceMapRenderStrategy::Render_i()
{
    using namespace Mcsf::MedViewer3D;
    //check again
    if (mVolumeGO == nullptr || mCameraGO == nullptr || 
        mNGCollectionGO == nullptr || mStateControlGO == nullptr) return;
    //check display state
    if(mStateControlGO->GetType() == DRR) return;

    //get active normal group
    std::string activeNGUid = mNGCollectionGO->GetActiveNormalGroupUID();
    auto activeNG = mNGCollectionGO->GetNormalGroup(activeNGUid);
    if (activeNG == nullptr){
        TPS_LOG_DEV_WARNING<<"There is no active normal group!";
        return;
    }
    double sad = activeNG->GetSAD();
    //get active beam
    std::shared_ptr<TpsBeamGraphicObject> beamGO = activeNG->GetActiveBeamGo();
    if(nullptr == beamGO) {
        TPS_LOG_DEV_WARNING<<"There is no active beam!";
        return;
    }
    if (!beamGO->GetVisible()) return; //check if need hide fluence map.
    //get active segment
    std::shared_ptr<TpsSegmentGraphicObject> segmentGo = nullptr;
    segmentGo = beamGO->GetActiveSegmentGo();
    if(segmentGo == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get active segment.";
        return;
    }
    //get fluence map
    auto fluenceMap = beamGO->GetFluenceMapEntity();
    if (!fluenceMap->IsValid()){
        TPS_LOG_DEV_WARNING<<"Invalid fluence map, return.";
        return;
    }
    int mapSize[2];
    float* mapBuffer = nullptr;
    if (!fluenceMap->GetIntensifyBuffer(mapSize[0], mapSize[1], mapBuffer)){
        TPS_LOG_DEV_ERROR<<"Failed to get fluence map buffer.";
        return;
    }
    float spacing[2], start[2];
    if (!fluenceMap->GetSpacing(spacing[0], spacing[1]) || 
        !fluenceMap->GetStart(start[0], start[1]))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get fluence map spacing or start point.";
        delete[] mapBuffer;
        return;
    }

    //calculate texture coordinate for 4 corners
    Matrix4x4 matWorld2Collimator;
    segmentGo->GetStartCollimatorToPatMatrix(&matWorld2Collimator);
    matWorld2Collimator = matWorld2Collimator.Inverse();
    matWorld2Collimator.Append(mVolumeGO->GetWorld2Patient());

    mCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
    const Point3D* corners = mCameraGO->GetWorldFrustumCorners();
    double sadInWorld = matWorld2Collimator.Inverse(
        ).Transform(Vector3D(sad, 0.0, 0.0)).Magnitude();
    double nearClip = mCameraGO->GetNearClipDistance();
    double farClip = mCameraGO->GetFarClipDistance();
    double ratioIsoPlane = (sadInWorld - nearClip) / (farClip - nearClip);
    Point3D isoCornersInC[4] = {
        matWorld2Collimator.Transform(corners[0] + ratioIsoPlane * (corners[4] - corners[0])),
        matWorld2Collimator.Transform(corners[1] + ratioIsoPlane * (corners[5] - corners[1])),
        matWorld2Collimator.Transform(corners[2] + ratioIsoPlane * (corners[6] - corners[2])),
        matWorld2Collimator.Transform(corners[3] + ratioIsoPlane * (corners[7] - corners[3]))};
    float aTexCoor[8];
    for (int i = 0; i < 4; ++i){
        aTexCoor[2*i] = (isoCornersInC[i].x - start[0]) / (mapSize[0] * spacing[0]);
        aTexCoor[2*i + 1] = (isoCornersInC[i].y - start[1]) / (mapSize[1] * spacing[1]);
    }

    float fWW, fWL;
    if (!this->GetLutWWWL(activeNG, fWW, fWL)){
        TPS_LOG_DEV_ERROR<<"Failed to get lut ww/wl;";
        return;
    }

    //MVP
    //Matrix4x4 matMVP;
    //matMVP.SetIdintity();
    //float aMvp[16];
    //for(int i = 0; i < 16; ++i){
        //aMvp[i] = matMVP._m[i];
    //}

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    //set view port
    glViewport(0, 0, mWidth, mHeight);
    glPushAttrib(GL_ENABLE_BIT);
    glUseProgram(mGLProgram->ID());

    glBindVertexArray(mVertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aTexCoor), aTexCoor, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mapSize[0], mapSize[1], 0, GL_RED, GL_FLOAT, mapBuffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, mTexID[2]);

    //glUniformMatrix4fv(mProgramLocations[2], 1, GL_FALSE, aMvp);
    glUniform1i(mProgramLocations[3], 0);
    glUniform1i(mProgramLocations[4], 1);
    glUniform1i(mProgramLocations[5], 2);
    glUniform1f(mProgramLocations[6], fWW);
    glUniform1f(mProgramLocations[7], fWL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(mVertexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID[2]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);
    glPopAttrib();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

int FluenceMapRenderStrategy::CacheChanges()
{
    if (mVolumeGO == nullptr || mCameraGO == nullptr || 
        mNGCollectionGO == nullptr || mStateControlGO == nullptr)
    {
        return -1;
    }

    if (mVolumeGO->IsDirty() || mCameraGO->IsDirty() || 
        mNGCollectionGO->IsDirty() || mStateControlGO->IsDirty())
    {
        mDirty = true;
    }
    return 0;
}

void FluenceMapRenderStrategy::Initialize()
{
    mGLProgram = new GLProgram();
    mGLProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_FluenceMapVert).c_str());
    mGLProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_FluenceMapFrag).c_str());
    if (!mGLProgram->Link())
    {
        TPS_LOG_DEV_ERROR<<"Failed to link fluence map program.";
        return;
    }

    GLuint progID = mGLProgram->ID();
    mProgramLocations[0] = glGetAttribLocation(progID, "vVertex");
    mProgramLocations[1] = glGetAttribLocation(progID, "vTexCoord");
    mProgramLocations[2] = glGetUniformLocation(progID, "matMVP");
    mProgramLocations[3] = glGetUniformLocation(progID, "texFluenceMap");
    mProgramLocations[4] = glGetUniformLocation(progID, "texLutValue");
    mProgramLocations[5] = glGetUniformLocation(progID, "texLutColor");
    mProgramLocations[6] = glGetUniformLocation(progID, "fWW");
    mProgramLocations[7] = glGetUniformLocation(progID, "fWL");

    glGenVertexArrays(1, &mVertexArrayID);
    glBindVertexArray(mVertexArrayID);

    glGenBuffers(3, mBufferID);
    //vertex buffer
    float vertBuffer[8] = { -1.f, -1.f, 1.f, -1.f, 1.f, 1.f, -1.f, 1.f};
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertBuffer), vertBuffer, GL_STATIC_DRAW);
    glVertexAttribPointer(mProgramLocations[0], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mProgramLocations[0]);

    //texture coordinate buffer
    //float texCoord[8] = {0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
    glVertexAttribPointer(mProgramLocations[1], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mProgramLocations[1]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //element buffer
    unsigned short cubeIndex[] = {0, 1, 2, 0, 2, 3};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndex), cubeIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(3, mTexID);
    //intensity map texture
    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*GL_LINEAR*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST/*GL_LINEAR*/);
    glBindTexture(GL_TEXTURE_2D, 0);
    //lut values texture
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R16F, lutParaCount, 0, GL_RED, GL_FLOAT, lutValues);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //lut colors texture
    glBindTexture(GL_TEXTURE_1D, mTexID[2]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, lutParaCount, 0, GL_RGB, GL_UNSIGNED_BYTE, lutColors);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void FluenceMapRenderStrategy::Finalize()
{
    if (mGLProgram){
        delete mGLProgram;
    }
    glDeleteBuffers(3, mBufferID);
    glDeleteVertexArrays(1, &mVertexArrayID);
    glDeleteTextures(3, mTexID);
}

void FluenceMapRenderStrategy::Resize(int iWidth, int iHeight)
{
    mWidth = iWidth;
    mHeight = iHeight;
}

void FluenceMapRenderStrategy::SetVolumeGO(
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volumeGO)
{
    if (volumeGO != mVolumeGO){
        mVolumeGO = volumeGO;
        mDirty = true;
    }
}

void FluenceMapRenderStrategy::SetCameraGO(
    std::shared_ptr<CameraGraphicObject> cameraGO)
{
    if (cameraGO != mCameraGO){
        mCameraGO = cameraGO;
        mDirty = true;
    }
}

void FluenceMapRenderStrategy::SetNormalGroupCollectionGO(
    std::shared_ptr<NormalGroupCollection> ngCollectionGO)
{
    if (ngCollectionGO != mNGCollectionGO){
        mNGCollectionGO = ngCollectionGO;
        mDirty = true;
    }
}

void FluenceMapRenderStrategy::SetStateControlGO(
    std::shared_ptr<StateControlGraphicObject> stateControlGO)
{
    if (stateControlGO != mStateControlGO){
        mStateControlGO = stateControlGO;
        mDirty = true;
    }
}

bool FluenceMapRenderStrategy::GetLutWWWL(NormalGroupPtr ng, float& ww, float& wl)
{
    std::shared_ptr<FluenceMapEntity> fluenceMap = nullptr;
    auto beamMap = ng->GetBeamMap();
    float mu, min, max;
    float maxMu(std::numeric_limits<float>::lowest());
    float minMu(std::numeric_limits<float>::max());
    for (auto it = beamMap.begin(); it != beamMap.end(); ++it){
        if (it->second == nullptr){
            TPS_LOG_DEV_ERROR<<"The beam graphic object is null.";
            return false;
        }
        mu = it->second->GetMU();
        fluenceMap = it->second->GetFluenceMapEntity();
        if (fluenceMap == nullptr){
            TPS_LOG_DEV_ERROR<<"Fluence map in the normal group is null.";
            return false;
        }
        if (!fluenceMap->GetMaxValue(max) || !fluenceMap->GetMinValue(min)){
            TPS_LOG_DEV_ERROR<<"Failed to get max/min value of fluence map.";
            return false;
        }
        maxMu = TPS_MAX(maxMu, max * mu);
        minMu = TPS_MIN(minMu, min * mu);
    }
    mu = ng->GetActiveBeamGo()->GetMU();
    //当mu为0时，fluence map显示为全黑
    if (abs(mu) <= 1e-6){
        TPS_LOG_DEV_INFO<<"Active beam graphic object's MU is 0.";
        return false;
    }
    max = maxMu / mu;
    min = minMu / mu;
    ww = max - min;
    wl = min + ww / 2;
    return true;
}
TPS_END_NAMESPACE  // end namespace tps