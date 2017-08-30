////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_volume4d_bev_renderstrategy.cpp
/// 
///  \brief class TpsVolume4dBevRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_bev_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
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
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

static double MPR_TOLERANCE = 0.01;

Volume4dBevRenderStrategy::Volume4dBevRenderStrategy()
    : m_bInitialized(false), m_spVolume4dGO(nullptr)
    , m_spDrrGO(nullptr), m_spCt2DensityGO(nullptr), m_spStateControlGO(nullptr)
    , m_spCameraGO(nullptr), m_spNormalGroupCollectionGO(nullptr), mTextureID(0), m_pDrrImage(nullptr)
{
    m_pDrrGenerator = new DrrGLGenerator();
}

Volume4dBevRenderStrategy::~Volume4dBevRenderStrategy(){

    if (m_pDrrGenerator){
        delete m_pDrrGenerator;
        m_pDrrGenerator = nullptr;
    }

    if (m_pDrrImage){
        delete[] m_pDrrImage;
        m_pDrrImage = nullptr;
    }
}

void Volume4dBevRenderStrategy::Render_i(){
    if (m_spVolume4dGO == nullptr){
        return;
    }
    //check again
    if (nullptr == m_spDrrGO || nullptr == m_spCt2DensityGO  || nullptr == m_spStateControlGO || m_spCameraGO == nullptr) {
            return;
    }

    if (!m_spStateControlGO->GetDrrVisibility()) return;
    if(FLUENCE_MAP == m_spStateControlGO->GetType()) return;

    if (!GenerateDrrImage_i()){
        TPS_LOG_DEV_ERROR<<"Failed to generate drr image.";
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
   
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, mWidth, mHeight, 0, GL_LUMINANCE, GL_FLOAT, m_pDrrImage);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f); glVertex2f(-1.f, -1.f);
    glTexCoord2f(1.f, 0.f); glVertex2f(1.f, -1.f);
    glTexCoord2f(1.f, 1.f); glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f); glVertex2f(-1.f, 1.f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    /*#ifdef _DEBUG
        unsigned char* bmpBuffer = new unsigned char[mWidth * mHeight * 4];
        for (int j=0; j < mWidth * mHeight; ++j){
            bmpBuffer[4 * j] = drrBuffer[j] * 255;
            bmpBuffer[4 * j + 1] = drrBuffer[j] * 255;
            bmpBuffer[4 * j + 2] = drrBuffer[j] * 255;
            bmpBuffer[4 * j + 3] = 255;
        }
        char fileName[256];
        sprintf_s(fileName, "d:\\temp\\drr_result.bmp");
        SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, mHeight, mWidth);
        delete []bmpBuffer;
    #endif*/

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

bool Volume4dBevRenderStrategy::GenerateDrrImage_i(){
    m_pDrrGenerator->SetViewportRegion(0, 0, mWidth, mHeight);
    
    m_spCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
    Matrix4x4 mvp = m_spCameraGO->GetViewProjectionMatrix();
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spCurrentVolume = m_spVolume4dGO->GetCurrentVolume();
    if (spCurrentVolume == nullptr)
        return false;
    mvp.Append(spCurrentVolume->GetModel2World());
    float matrix[16];
    for (int i = 0; i < 16; ++i){
        matrix[i] = mvp._m[i];
    }
    m_pDrrGenerator->SetMVP(matrix);
    m_pDrrGenerator->SetVolumeTexture(spCurrentVolume->GetTexture3D()->GetID());
  
    Matrix4x4 model2Patient = spCurrentVolume->GetModel2PatientMat();
    for (int i = 0; i < 16; ++i){
        matrix[i] = model2Patient._m[i];
    }
    m_pDrrGenerator->SetModel2Patient(matrix);
  
    int ct2densitySize = 0;
    float* table;
    int startCt = -1024;
    m_spCt2DensityGO->GetCt2Density(&table, &ct2densitySize);
    std::shared_ptr<Texture1D> ct2Density = m_spCt2DensityGO->GetTexture();
    if (ct2Density == nullptr){
        TPS_LOG_DEV_ERROR<<"The ct 2 density texture is null.";
        return false;
    }
    m_pDrrGenerator->SetCt2Density(startCt, ct2densitySize, ct2Density->GetID());
  
    size_t dim[3];
    spCurrentVolume->GetDimension(dim);
    Matrix4x4 volume2Model(dim[0], 0, 0, 0, /*|*/ 0, dim[1], 0, 0, 
        0, 0, dim[2], 0, /*|*/ -0.5, -0.5, -0.5, 1);
    volume2Model = volume2Model.Inverse();
    float spacingInModel[3] = {
        volume2Model.Transform(Vector3D(1.0, 0.0, 0.0)).Magnitude(), 
        volume2Model.Transform(Vector3D(0.0, 1.0, 0.0)).Magnitude(),
        volume2Model.Transform(Vector3D(0.0, 0.0, 1.0)).Magnitude()};
    m_pDrrGenerator->SetSpacing(spacingInModel);
    m_pDrrGenerator->SetSampleRate(1.f);
  
    float intercept = spCurrentVolume->GetIntercept();
    float slope = spCurrentVolume->GetSlope();
    int dataType = spCurrentVolume->GetDataType();
    double minValue = spCurrentVolume->GetMinScalar();
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
    m_pDrrGenerator->SetRescalePara(slope, intercept);
  
    float energy = 6;
    if (m_spNormalGroupCollectionGO != nullptr){
        auto normalGroup = m_spNormalGroupCollectionGO->GetActiveNormalGroup();
        if (normalGroup != nullptr){
            auto beamGo = normalGroup->GetActiveBeamGo();
            if (beamGo != nullptr){
                energy = beamGo->GetEnergy();
            }
        }
    }
    m_pDrrGenerator->SetEnergy(energy);
  
    int count = mWidth * mHeight;
    //float* drrBuffer = new float[count];
    memset(m_pDrrImage, 0, count * sizeof(float));
    m_pDrrGenerator->GenerateDrrImage(m_pDrrImage);
  
    double wl, ww;
    m_spDrrGO->GetWindowing(&ww, &wl);
    if (ww <= 0){
        TPS_LOG_DEV_ERROR<<"Wrong window level!";
        return false;
    }
    //0~255 -> 0.f~1.f
    ww /= 256;
    wl = (wl + 1) / 256;
    float temp;
    for (int i = 0; i < count; ++i){
        temp = (m_pDrrImage[i] - wl) / ww + 0.5f;
        m_pDrrImage[i] = TPS_CLAMP(temp, 0.f, 1.f);
    }

    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, mWidth, mHeight, 0, GL_LUMINANCE, GL_FLOAT, m_pDrrImage);
    glBindTexture(GL_TEXTURE_2D, 0);
    //delete[] drrBuffer; drrBuffer = nullptr;
    return true;
}

int Volume4dBevRenderStrategy::CacheChanges(){

    if (m_spVolume4dGO == nullptr || nullptr == m_spDrrGO || 
        nullptr == m_spCt2DensityGO  || nullptr == m_spStateControlGO || m_spCameraGO == nullptr) {
        return -1;
    }

    if (m_spDrrGO->IsDirty() || m_spVolume4dGO->IsDirty() || m_spCt2DensityGO->IsDirty() || 
        m_spStateControlGO->IsDirty() || m_spCameraGO->IsDirty()) {
        mDirty = true;
    }
    return 0;
}

void Volume4dBevRenderStrategy::Initialize(){

    m_pDrrGenerator->Initialize();

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Volume4dBevRenderStrategy::Finalize(){

    if (mTextureID){
        glDeleteTextures(1, &mTextureID);
    }

    m_pDrrGenerator->Finialize();
}

void Volume4dBevRenderStrategy::Resize(int iWidth, int iHeight){
    if (mWidth * mHeight != iWidth * iHeight || m_pDrrImage == nullptr){
        if (m_pDrrImage) delete[] m_pDrrImage;
        m_pDrrImage = new float[iWidth * iHeight];
    }

    mWidth = iWidth;
    mHeight = iHeight;

    if (m_pDrrGenerator){
        m_pDrrGenerator->Resize(iWidth, iHeight);
    }

    mDirty = true;
}


//void checkGLStatus() 
//{
//    GLenum err =  glGetError(); 
//    switch (err) 
//    { 
//    case GL_NO_ERROR: 
//        printf("No error.\n"); 
//        break; 
//    case GL_INVALID_ENUM: 
//        printf("Invalid enum.\n"); 
//        break; 
//    case GL_INVALID_VALUE: 
//        printf("Invalid value.\n"); 
//        break; 
//
//    case GL_INVALID_OPERATION: 
//        printf("Invalid operation.\n"); 
//        break; 
//    case GL_INVALID_FRAMEBUFFER_OPERATION: 
//        printf("Invalid frame operation.\n"); 
//        break; 
//    case GL_OUT_OF_MEMORY: 
//        printf("Out of memory.\n"); 
//        break; 
//    case GL_STACK_UNDERFLOW: 
//        printf("Stack underflow.\n"); 
//        break; 
//    case GL_STACK_OVERFLOW: 
//        printf("Stack overflow.\n"); 
//        break; 
//    default: 
//        printf("Unknown error.\n"); 
//        break;
//    }
//}
TPS_END_NAMESPACE  // end namespace tps
