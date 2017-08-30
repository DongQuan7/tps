////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Qiangqiang ZHOU qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_ct2density_graphicobject.h
/// 
///  \brief class Ct2DensityGraphicObejct implementation 
/// 
///  \version 1.0
/// 
///  \date    July 24, 2014
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobject.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_1d.h"

TPS_BEGIN_NAMESPACE
Ct2DensityGraphicObejct::Ct2DensityGraphicObejct() : 
    mCt2DensitySize(0), mCt2Density(nullptr), mTex1D(nullptr), mElectronEnergy(0.2f){
}

Ct2DensityGraphicObejct::~Ct2DensityGraphicObejct() {
    if (nullptr != mCt2Density){
        delete mCt2Density; 
        mCt2Density = nullptr;
    }
}

void Ct2DensityGraphicObejct::SetCt2Density(float* pCt2Density, int ct2DensitySize){
    if (ct2DensitySize < 1){
        return;
    }

    if (nullptr != mCt2Density){
        delete mCt2Density;
        mCt2Density = nullptr;
    }
    
    mCt2DensitySize = ct2DensitySize;
    mCt2Density = new float[ct2DensitySize];
    memcpy(mCt2Density, pCt2Density, sizeof(float) * ct2DensitySize);

    //if (!LoadTexture_i()){
    //    TPS_LOG_DEV_ERROR<<"Failed to invoke LoadTexture_i.";
    //}
}

void Ct2DensityGraphicObejct::GetCt2Density(float** pCt2Denstiy, int* ct2DensitySize){
    *pCt2Denstiy = mCt2Density;
    *ct2DensitySize = mCt2DensitySize;
}

std::shared_ptr<Mcsf::MedViewer3D::Texture1D> Ct2DensityGraphicObejct::GetTexture(){
    return mTex1D;
}

bool Ct2DensityGraphicObejct::LoadTexture(float thresholdMin, float thresholdMax){
    if (mCt2Density == nullptr || mCt2DensitySize <= 0) return false;

    if (mTex1D == nullptr){
        mTex1D = Mcsf::MedViewer3D::Texture1D::Create();
    }
    if (mTex1D == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to create Mcsf::MedViewer3D::Texture1D.";
        return false;
    }

    float *ct2Density = new float[mCt2DensitySize];
    memcpy(ct2Density, mCt2Density, sizeof(float) * mCt2DensitySize);

    for (int i = 0; i < mCt2DensitySize; ++i)
    {
        if(ct2Density[i] < thresholdMin || ct2Density[i] > thresholdMax)
        {
            ct2Density[i] = 0.0;
        }
    }

    if (!mTex1D->Load(0, GL_R16F, mCt2DensitySize, 0, GL_RED, GL_FLOAT, ct2Density)){
        TPS_LOG_DEV_ERROR<<"Failed to load texture 1d.";
        return false;
    }
    delete []ct2Density; ct2Density = nullptr;
    return true;
}

void Ct2DensityGraphicObejct::SetElectronEnergy(float electronEnergy)
{
    mElectronEnergy = electronEnergy;
}
float Ct2DensityGraphicObejct::GetElectronEnergy()
{
    return mElectronEnergy;
}

void Ct2DensityGraphicObejct::SetCT2DensityTableUid(const std::string& uid)
{
    mCt2DensityUid = uid;
}
std::string Ct2DensityGraphicObejct::GetCT2DensityTableUid()
{
    return mCt2DensityUid;
}
TPS_END_NAMESPACE