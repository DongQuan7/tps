////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprdoserenderstrategy.cpp
/// 
///  \brief class TpsMPRDoseRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/02/13
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprdoserenderstrategy.h"

#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "tps_rl_shaders.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"

//for test
//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsMPRDoseRenderStrategy::TpsMPRDoseRenderStrategy()
    : mDoseGO(nullptr), mMPRGO(nullptr), mDoseLineGo(nullptr)
    , mNeedRecalcScreenToWorld(true), mProgram(nullptr){

    for (int i=0; i<3; ++i){
        for (int j=0; j<3; ++j){
            mScrToDoseMatrix[3*i+j] = i==j ? 1.0f : 0.0f;
        }
    }
}

TpsMPRDoseRenderStrategy::~TpsMPRDoseRenderStrategy(){

}

void TpsMPRDoseRenderStrategy::Initialize(){

    mProgram = new GLProgram();
    mProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_MprDoseLineVert).c_str());
    mProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_MprDoseLineFrag).c_str());
    if (!mProgram->Link())
    {
        TPS_LOG_TRACE_ERROR<<"Failed to link mpr dose program.";
        return;
    }

    unsigned int progID = mProgram->ID();
    GLuint vertexLoca = glGetAttribLocation(progID, "vVertex");
    mDisplayValueLoca = glGetUniformLocation(progID, "texDisplayValue");
    mDisplayColorLoca = glGetUniformLocation(progID, "texDisplayColor");
    mDisplayCountLoca = glGetUniformLocation(progID, "uiDisplayCount");
    mDoseSampLoca = glGetUniformLocation(progID, "texDose");
    mScrToDoseTexLoca = glGetUniformLocation(progID, "matScrToDoseTex");
    mVisionModeLoca = glGetUniformLocation(progID, "uiVisionMode");
    mTransparencyLoca = glGetUniformLocation(progID, "fTransparency");
    GLuint fragOutputColorLoc = 0;
    glBindFragDataLocation(progID, fragOutputColorLoc, "vFragColor");

    glGenVertexArrays(1, &mVertexArrayID);
    glBindVertexArray(mVertexArrayID);

    float aVertex[12] = { -1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0 };
    glGenBuffers(2, mArrayBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mArrayBufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, aVertex, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vertexLoca);
    glVertexAttribPointer(vertexLoca, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned short aIndex[6] = {0, 1, 2, 2, 3, 0};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mArrayBufferID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6, aIndex, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //create texture
    glGenTextures(3, mTexID);
    //dose texture
    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    float border[4] = {0.f, 0.f, 0.f, 0.f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //display values & display colors texture
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_1D, 0);

    glBindTexture(GL_TEXTURE_1D, mTexID[2]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void TpsMPRDoseRenderStrategy::Finalize(){

    if (mProgram) delete mProgram;

    glDeleteBuffers(2, mArrayBufferID);
    glDeleteVertexArrays(1, &mVertexArrayID);
    glDeleteTextures(3, mTexID);
}

/*!\fn TpsMPRDoseRenderStrategy::Render()
* <key> \n
* PRA: Yes \n
* Name:void TpsMPRDoseRenderStrategy::Render() \n
* Traced from: SSFS_PRA_TPSDoseLineDisplay \n
* Description: render dose line \n
* Short Description: RenderDoseline \n
* Component: TPS \n
* </key> \n
*******************************************************************************/
void TpsMPRDoseRenderStrategy::Render_i(){
    if (mDoseGO == nullptr || mMPRGO == nullptr || mDoseLineGo == nullptr || mProgram == nullptr){
        return;
    }

    float oneHundredDoseValue = mDoseLineGo->GetOneHundredDoseValue();
    if (fabs(oneHundredDoseValue - INVALID_DOSE_VALUE) <= 1e-6 || 
        oneHundredDoseValue < 1e-6)
    {
        TPS_LOG_TRACE_ERROR<<"Invalid one hundred dose value.";
        return;
    }

    float transparency =  mDoseLineGo->GetTransparency();
    DOSELINE_RENDERMODE renderMode = mDoseLineGo->GetVisionMode();
    std::vector<DoseLineAttribute> vecDoseLine;
    mDoseLineGo->GetDisplayAttributes(vecDoseLine);
    //For test
    int count = 0;
    std::vector<float> displayValues;
    float* colors = new float[vecDoseLine.size() * 3];

    for(auto iter = vecDoseLine.begin(); iter != vecDoseLine.end();++iter){
        if(iter->mVisible){
            colors[3*count] = iter->mColor[0];
            colors[3*count + 1] = iter->mColor[1];
            colors[3*count + 2] = iter->mColor[2];
            displayValues.push_back((iter->mDosePercent ) * oneHundredDoseValue);
            count++;
        }
    }

    // check if need render
    if (renderMode == NONE || displayValues.empty()){
        if (nullptr != colors){
            delete [] colors; 
            colors = nullptr;
        } 
        return;
    }

    float * buffer;
    int width, height;
    Mcsf::MedViewer3D::Matrix4x4 screenToNormalPlane;
    mDoseGO->GetDoseGrid(buffer, height, width, screenToNormalPlane);

    //////////////////////////////////////////////////////////////////////////
    //for test
    //char fileName[256];
    //sprintf_s(fileName, "d:\\temp1\\TpsMPRDoseRenderStrategy_dose_result.bmp");
    //SubVolumeHelperAPI::SaveToTrueColorBitmapFloat(fileName, buffer, height, width);

    if (mNeedRecalcScreenToWorld) CalcScreenToWorld_i();
    screenToNormalPlane.Append(mScreenToWorldMat);
    float matrix[16];
    for (int i = 0; i < 16; ++i) {
        matrix[i] = screenToNormalPlane._m[i];
    }

    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, 
        width, height, 0, GL_LUMINANCE, GL_FLOAT, buffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_1D, mTexID[1]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE32F_ARB, 
        count, 0, GL_LUMINANCE, GL_FLOAT, displayValues._Myfirst);
    glBindTexture(GL_TEXTURE_1D, 0);

    glBindTexture(GL_TEXTURE_1D, mTexID[2]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 
        count, 0, GL_RGB, GL_FLOAT, colors);
    glBindTexture(GL_TEXTURE_1D, 0);
    if (nullptr != colors){
        delete [] colors; 
        colors = nullptr;
    } 

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);

    glPushAttrib(GL_ENABLE_BIT);
    glUseProgram(mProgram->ID());

    glDisable(GL_DEPTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
        //GL_ONE, GL_ZERO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexID[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, mTexID[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, mTexID[2]);

    glUniform1i(mDoseSampLoca, 0);
    glUniform1i(mDisplayValueLoca, 1);
    glUniform1i(mDisplayColorLoca, 2);

    glUniform1ui(mDisplayCountLoca, count);
    glUniformMatrix4fv(mScrToDoseTexLoca, 1, GL_FALSE, matrix);
    glUniform1ui(mVisionModeLoca, renderMode);
    glUniform1f(mTransparencyLoca, transparency);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(mVertexArrayID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glUseProgram(0);
    glPopAttrib();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void TpsMPRDoseRenderStrategy::SetDoseGraphicObject(
    std::shared_ptr<TpsMPRDoseGraphicObject> doseGO) {
    if (mDoseGO != doseGO){
        mDoseGO = doseGO;
        mDirty = true;
    }
}

void TpsMPRDoseRenderStrategy::SetMPRGraphicObject(
    std::shared_ptr<MPRGraphicObject> mprGO){
    if (mMPRGO != mprGO){
        mMPRGO = mprGO;
        mDirty = true;
        if (mMPRGO) mNeedRecalcScreenToWorld = true;
    }
}

void TpsMPRDoseRenderStrategy::SetDoseLineObject(
    std::shared_ptr<TpsDoseLineGraphicObject> doseLineGO){
    if (mDoseLineGo != doseLineGO){
        mDoseLineGo = doseLineGO;
        mDirty = true;
    }
}

void TpsMPRDoseRenderStrategy::Resize(int x,int y) {
    mWidth = x;
    mHeight = y;
    mDirty = true;
    mNeedRecalcScreenToWorld = true;
}

int TpsMPRDoseRenderStrategy::CacheChanges() {

    if (mDoseGO == nullptr || mMPRGO == nullptr || mDoseLineGo == nullptr){
        return -1;
    }

    if (mMPRGO->IsDirty()){
        mNeedRecalcScreenToWorld = true;
        mDirty = true;
    }

    if (mDoseGO->IsDirty() || mDoseLineGo->IsDirty()){
        mDirty = true;
    }

    return 0;
}

void TpsMPRDoseRenderStrategy::CalcScreenToWorld_i(){
    if (mMPRGO == nullptr) return;
    Mcsf::MedViewer3D::Point3D vertices[4];
    mMPRGO->GetVertices(vertices);

    Viewer3D::Vector3D vecY = vertices[3] - vertices[0];
    vecY.Normalize();
    Viewer3D::Vector3D vecZ; 
    mMPRGO->GetNormal(&vecZ); vecZ *= -1;
    Viewer3D::Vector3D vecX = vecZ.CrossProduct(vecY);
    vecY = vecX.CrossProduct(vecZ);

    double scale, ratio;
    double planeWidth = (vertices[1] - vertices[0]).Magnitude();
    double planeHeight = (vertices[3] - vertices[0]).Magnitude();
    Viewer3D::Point3D ptOrigin = vertices[0];
    double ratioPlane = planeWidth / planeHeight;
    double ratioView = (double)mWidth / mHeight;
    if (ratioPlane <= ratioView){
        scale = mHeight / planeHeight;
        ratio = ratioView / ratioPlane;
        ptOrigin += (vertices[0] - vertices[1]) * (ratio - 1) * 0.5;
    }
    else{
        scale = mWidth / planeWidth;
        ratio = ratioPlane / ratioView;
        ptOrigin += (vertices[0] - vertices[3]) * (ratio - 1) * 0.5;
    }

    Viewer3D::Vector3D temp(ptOrigin.x, ptOrigin.y, ptOrigin.z);
    Viewer3D::Matrix4x4 matWorldToScreen(vecX.x, vecY.x, vecZ.x, 0.0, 
        vecX.y, vecY.y, vecZ.y, 0.0, 
        vecX.z, vecY.z, vecZ.z, 0.0, 
        -1 * temp.DotProduct(vecX), 
        -1 * temp.DotProduct(vecY), 
        -1 * temp.DotProduct(vecZ), 1.0);
    matWorldToScreen.Prepend(MakeScale(Viewer3D::Vector3D(scale, scale, scale)));
    mScreenToWorldMat = matWorldToScreen.Inverse();
    mNeedRecalcScreenToWorld = false;
}
TPS_END_NAMESPACE  // end namespace tps