//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_shaded_surface_graphicobject.cpp
///  \brief   shaded surface grophic object
///
///  \version 1.0
///  \date    Dec. 22, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"


TPS_BEGIN_NAMESPACE

ShadedSurfaceGraphicObject::ShadedSurfaceGraphicObject(): mIsVisible(true),
mExtent(50.0f / 65536),mTransparency(0.5f)/*, mZoomFactor(1.0f)*/{
    mLightColor[0] = 0.8f;
    mLightColor[1] = 0.8f;
    mLightColor[2] = 0.8f;

    mSampleColor[0] = 0.1f;
    mSampleColor[1] = 0.1f;
    mSampleColor[2] = 0.1f;

    mLightFactor[0] = 0.3f;
    mLightFactor[1] = 0.7f;
    mLightFactor[2] = 0.4f;
    mLightFactor[3] = 25.0f;

    mMinDisplayValue = 0.0f;
    mMaxDisplayValue = 300.0f;

}

ShadedSurfaceGraphicObject::~ShadedSurfaceGraphicObject() {

}

void ShadedSurfaceGraphicObject::SetTransParency(const float &tran) {
    mTransparency = tran;
}

float ShadedSurfaceGraphicObject::GetTransparency() const {
    return mTransparency;
}

void ShadedSurfaceGraphicObject::SetLightPosition(const Mcsf::MedViewer3D::Point3D &pt) {
    mLightPosition = pt;
}

Mcsf::MedViewer3D::Point3D ShadedSurfaceGraphicObject::GetLightPosition() const {
    return mLightPosition;
}

void ShadedSurfaceGraphicObject::SetLightColor(float color[3]) {
    mLightColor[0] = color[0];
    mLightColor[1] = color[1];
    mLightColor[2] = color[2];
}

float *ShadedSurfaceGraphicObject::GetLightColor() const {
    float *color = new float[3];
    memcpy(color,mLightColor,3*sizeof(float));
    return color;
}

void ShadedSurfaceGraphicObject::SetColor(float color[3] ) {
    mSampleColor[0] = color[0];
    mSampleColor[1] = color[1];
    mSampleColor[2] = color[2];

}

float *ShadedSurfaceGraphicObject::GetColor() const {
    float *color = new float[3];
    memcpy(color,mSampleColor,3*sizeof(float));
    return color;
}

void ShadedSurfaceGraphicObject::SetVisible(bool flag) {
    mIsVisible = flag;
}

bool ShadedSurfaceGraphicObject::GetVisible() const {
    return mIsVisible;
}

void ShadedSurfaceGraphicObject::SetExtent(const float &ex) {
    mExtent = ex;
}

float ShadedSurfaceGraphicObject::GetExtent() const {
    return mExtent;
}

void ShadedSurfaceGraphicObject::SetLightFactor(float factor[4]) {
    mLightFactor[0] = factor[0];
    mLightFactor[1] = factor[1];
    mLightFactor[2] = factor[2];
    mLightFactor[3] = factor[3];
 
}

void ShadedSurfaceGraphicObject::GetLightFactor(float factor[4]) const {
    factor[0] = mLightFactor[0];
    factor[1] = mLightFactor[1];
    factor[2] = mLightFactor[2];
    factor[3] = mLightFactor[3];
}


void ShadedSurfaceGraphicObject::SetDisplayValue(float minCt, float maxCt) {
    if(minCt > maxCt) {
        TPS_LOG_DEV_ERROR<<"set display value failed  minCt > maxCt";
        mMinDisplayValue = 0;
        mMaxDisplayValue = 0;
        return;
    }
    mMinDisplayValue = minCt;
    mMaxDisplayValue = maxCt;
}

void ShadedSurfaceGraphicObject::GetDisplayValue(float *minCt, float *maxCt) {
    *minCt = mMinDisplayValue;
    *maxCt = mMaxDisplayValue;
}

// void ShadedSurfaceGraphicObject::SetZoomFactor(float fZoomFator){
//     mZoomFactor = fZoomFator;
// }
// 
// float ShadedSurfaceGraphicObject::GetZoomFactor() const{
//     return mZoomFactor;
// }

TPS_END_NAMESPACE