////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_drr_render_strategy.cpp
/// 
///  \brief class DrrRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_drr_render_strategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DDataRepresentation/data_config.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_1d.h"
#include "RtTpsRenderLibrary/drr_gl_generator.h"

#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsAlgorithmProxy/tps_ap_drr_algorithm_proxy.h"
#include "RtTpsRenderLibrary/tps_rl_ct2densitygocollection.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggo.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollection.h"
#ifdef _DEBUG
    #include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#endif

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE  // begin namespace tps

DrrRenderStrategy::DrrRenderStrategy() : mVolumeGO(nullptr), mDrrGO(nullptr), 
mCt2DensityGOCollection(nullptr), mStateControlGO(nullptr), mCameraGO(nullptr), 
mNormalGroupCollectionGO(nullptr), mTextureID(0), mDrrImage(nullptr), mDrrSettingGoCollection(nullptr)
{
    mDrrGenerator = new DrrGLGenerator();
}

DrrRenderStrategy::~DrrRenderStrategy(){
    if (mDrrGenerator){
        delete mDrrGenerator;
        mDrrGenerator = nullptr;
    }

    if (mDrrImage){
        delete[] mDrrImage;
        mDrrImage = nullptr;
    }
}

void DrrRenderStrategy::Initialize(){

    mDrrGenerator->Initialize();

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrrRenderStrategy::Finalize(){
    if (mTextureID){
        glDeleteTextures(1, &mTextureID);
    }

    mDrrGenerator->Finialize();
}

void DrrRenderStrategy::Render_i(){
    //check again
    if (nullptr == mDrrGO || nullptr == mVolumeGO || 
        nullptr == mCt2DensityGOCollection  || nullptr == mStateControlGO || mCameraGO == nullptr
        ||mDrrSettingGoCollection == nullptr) {
        return;
    }

    if (!mStateControlGO->GetDrrVisibility()) return;
    if(FLUENCE_MAP == mStateControlGO->GetType()) return;
    if (!GenerateDrrImage_i()){
        TPS_LOG_DEV_ERROR<<"Failed to generate drr image.";
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, mWidth, mHeight);
   
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, mWidth, mHeight, 0, 
        GL_LUMINANCE, GL_FLOAT, mDrrImage);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f); glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f); glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f); glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f); glVertex2f(-1.f, 1.f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

//     #ifdef _DEBUG
//         unsigned char* bmpBuffer = new unsigned char[mWidth * mHeight * 4];
//         for (int j=0; j < mWidth * mHeight; ++j){
//             bmpBuffer[4 * j] = mDrrImage[j] * 255;
//             bmpBuffer[4 * j + 1] = mDrrImage[j] * 255;
//             bmpBuffer[4 * j + 2] = mDrrImage[j] * 255;
//             bmpBuffer[4 * j + 3] = 255;
//         }
//         char fileName[256];
//         sprintf_s(fileName, "d:\\temp\\drr_result.bmp");
//         SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, mHeight, mWidth);
//         delete []bmpBuffer;
//     #endif
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool DrrRenderStrategy::GenerateDrrImage_i(){
    mDrrGenerator->SetViewportRegion(0, 0, mWidth, mHeight);
  
    mCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
    Matrix4x4 mvp = mCameraGO->GetViewProjectionMatrix();
    mvp.Append(mVolumeGO->GetModel2World());
    float matrix[16];
    for (int i = 0; i < 16; ++i){
        matrix[i] = mvp._m[i];
    }
    mDrrGenerator->SetMVP(matrix);
    mDrrGenerator->SetVolumeTexture(mVolumeGO->GetTexture3D()->GetID());
  
    Matrix4x4 model2Patient = mVolumeGO->GetModel2PatientMat();
    for (int i = 0; i < 16; ++i){
        matrix[i] = model2Patient._m[i];
    }
    mDrrGenerator->SetModel2Patient(matrix);
  
    int ct2densitySize = 0;
    float* table;
    int startCt = -1024;
    std::string activeNormalUid = mNormalGroupCollectionGO->GetActiveNormalGroupUID();
    if(activeNormalUid == "") return false;

    auto drrsettingGo = mDrrSettingGoCollection->GetDrrSettingGo(activeNormalUid);
    if(drrsettingGo == nullptr) 
    {
        TPS_LOG_DEV_ERROR<<"The drr setting of normalgroup"<<activeNormalUid<< "is null.";
        return false;
    }
    std::string ct2densityUid = drrsettingGo->GetCT2DensityTableUid();
    float thresholdMin, thresholdMax;
    drrsettingGo->GetCTThresholdMin(thresholdMin);
    drrsettingGo->GetCTThresholdMax(thresholdMax);
    auto ct2densityGo = mCt2DensityGOCollection->GetCt2DensityGo(ct2densityUid);
	if(ct2densityGo == nullptr) return false;
    ct2densityGo->GetCt2Density(&table, &ct2densitySize);
    ct2densityGo->LoadTexture(thresholdMin, thresholdMax);
    std::shared_ptr<Texture1D> ct2Density = ct2densityGo->GetTexture();
    if (ct2Density == nullptr){
        TPS_LOG_DEV_ERROR<<"The ct 2 density texture is null.";
        return false;
    }
    mDrrGenerator->SetCt2Density(startCt, ct2densitySize, ct2Density->GetID());
  
    size_t dim[3];
    mVolumeGO->GetDimension(dim);
    Matrix4x4 volume2Model(dim[0], 0, 0, 0, /*|*/ 0, dim[1], 0, 0, 
        0, 0, dim[2], 0, /*|*/ -0.5, -0.5, -0.5, 1);
    volume2Model = volume2Model.Inverse();
    float spacingInModel[3] = {
        volume2Model.Transform(Vector3D(1.0, 0.0, 0.0)).Magnitude(), 
        volume2Model.Transform(Vector3D(0.0, 1.0, 0.0)).Magnitude(),
        volume2Model.Transform(Vector3D(0.0, 0.0, 1.0)).Magnitude()};
    mDrrGenerator->SetSpacing(spacingInModel);
    float downSampleRate = mStateControlGO->GetDownSampleRate();
    mDrrGenerator->SetSampleRate(downSampleRate);
    //printf("\nwidth: %d, height: %d, sample rate: %f\n", mWidth, mHeight, downSampleRate);

    float intercept = mVolumeGO->GetIntercept();
    float slope = mVolumeGO->GetSlope();
    int dataType = mVolumeGO->GetDataType();
    double minValue = mVolumeGO->GetMinScalar();
    switch(dataType)
    {
    case MEDVIEWER3D_SHORT:
        intercept += (slope * TPS_MIN(0, minValue));
    case MEDVIEWER3D_USHORT:
        slope *= 65535;
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unsupported data type!";
        return false;
    }
    mDrrGenerator->SetRescalePara(slope, intercept);
  
    //float energy = 6;
    //if (mNormalGroupCollectionGO != nullptr){
    //    auto normalGroup = mNormalGroupCollectionGO->GetActiveNormalGroup();
    //    if (normalGroup != nullptr){
    //        auto beamGo = normalGroup->GetActiveBeamGo();
    //        if (beamGo != nullptr){
    //            energy = beamGo->GetEnergy();
    //        }
    //    }
    //}

    float energy;
    drrsettingGo->GetElectronEnergy(energy);
    mDrrGenerator->SetEnergy(energy);
  
    int count = mWidth * mHeight;
    //float* drrBuffer = new float[count];
    memset(mDrrImage, 0, count * sizeof(float));
    mDrrGenerator->GenerateDrrImage(mDrrImage);
  
    double wl, ww;
    mDrrGO->GetWindowing(&ww, &wl);
    if (ww <= 0){
        TPS_LOG_DEV_ERROR<<"Wrong window level!";
        return false;
    }
    //0~255 -> 0.f~1.f
    ww /= 256;
    wl = (wl + 1) / 256;
    float temp;
    for (int i = 0; i < count; ++i){
        temp = (mDrrImage[i] - wl) / ww + 0.5f;
        mDrrImage[i] = TPS_CLAMP(temp, 0.f, 1.f);
    }

    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, mWidth, mHeight, 0, 
        GL_LUMINANCE, GL_FLOAT, mDrrImage);
    glBindTexture(GL_TEXTURE_2D, 0);
    //delete[] drrBuffer; drrBuffer = nullptr;
    return true;
}

int DrrRenderStrategy::CacheChanges(){
    if (nullptr == mDrrGO || nullptr == mVolumeGO || 
        nullptr == mCt2DensityGOCollection  || nullptr == mStateControlGO || mCameraGO == nullptr
        || mDrrSettingGoCollection == nullptr) {
        return -1;
    }

    if (mDrrGO->IsDirty() || mVolumeGO->IsDirty() || mCt2DensityGOCollection->IsDirty() || 
        mStateControlGO->IsDirty() || mCameraGO->IsDirty() || mDrrSettingGoCollection->IsDirty()) {
        mDirty = true;
    }
    return 0;
}

void DrrRenderStrategy::Resize(int iWidth, int iHeight){
    if (mWidth * mHeight != iWidth * iHeight || mDrrImage == nullptr){
        if (mDrrImage) delete[] mDrrImage;
        mDrrImage = new float[iWidth * iHeight];
    }
    mWidth = iWidth;
    mHeight = iHeight;

    if (mDrrGenerator){
        mDrrGenerator->Resize(iWidth, iHeight);
    }
}

void DrrRenderStrategy::SetVolumeGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volumeGo){
    if (mVolumeGO != volumeGo){
        mVolumeGO = volumeGo;
        mDirty = true;
    }
}

void DrrRenderStrategy::SetDrrGO(std::shared_ptr<DrrGraphicObejct> drrGO){
    if (mDrrGO != drrGO){
        mDrrGO = drrGO;
        mDirty = true;
    }
}

void DrrRenderStrategy::SetCt2DensityGOColletion(std::shared_ptr<Ct2DensityGraphicObejctCollection> ctGOCollection)
{
    if(mCt2DensityGOCollection != ctGOCollection)
    {
        mCt2DensityGOCollection = ctGOCollection;
        mDirty = true;
    }
}

void DrrRenderStrategy::SetDrrSettingGoColletion(std::shared_ptr<DRRSettingGraphicObjectCollection> drrSettingGOColletion)
{
    if(mDrrSettingGoCollection != drrSettingGOColletion)
    {
        mDrrSettingGoCollection = drrSettingGOColletion;
        mDirty = true;
    }
}

void DrrRenderStrategy::SetStateControlGO(std::shared_ptr<StateControlGraphicObject> stateGo){
    if (mStateControlGO != stateGo){
        mStateControlGO = stateGo;
        mDirty = true;
    }
}

void DrrRenderStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject> cameraGo){
    if (mCameraGO != cameraGo){
        mCameraGO = cameraGo;
        mDirty = true;
    }
}

void DrrRenderStrategy::SetNormalGroupCollectionGO(
    std::shared_ptr<NormalGroupCollection> ngCollectionGO){
    if (mNormalGroupCollectionGO != ngCollectionGO){
        mNormalGroupCollectionGO = ngCollectionGO;
        mDirty = true;
    }
}
TPS_END_NAMESPACE  // end namespace tps