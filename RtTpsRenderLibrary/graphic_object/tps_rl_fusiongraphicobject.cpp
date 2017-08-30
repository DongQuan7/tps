////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprfusiongraphicobject.cpp
/// 
///  \brief class TpsMprFusionGraphicObject implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

FusionGraphicObject::FusionGraphicObject()
: mFixedImageUID(""), mFloatImageUID(""), mFixedAlpha(0.5f)
, mFloatAlpha(0.5f), mIsFusionEnabled(true), mFusionType(FUSION_TYPE_BLEND)
, mCanEdit(true), mRegAlg(Rigid), mTexFusionVolume(nullptr){

    mRegistrationMatrix.SetIdintity();
}

FusionGraphicObject::~FusionGraphicObject(){

}

void FusionGraphicObject::SetFixedAlpha(float alpha){
    mFixedAlpha = alpha;
}

float FusionGraphicObject::GetFixedAlpha() const{
    return mFixedAlpha;
}

void FusionGraphicObject::SetFloatAlpha(float alpha){
    mFloatAlpha = alpha;
}

float FusionGraphicObject::GetFloatAlpha() const{
    return mFloatAlpha;
}

void FusionGraphicObject::SetRegistrationMatrix(const Mcsf::MedViewer3D::Matrix4x4& matrix){
    mRegistrationMatrix = matrix;
}

Mcsf::MedViewer3D::Matrix4x4 FusionGraphicObject::GetRegistrationMatrix() const{
    return mRegistrationMatrix;
}

void FusionGraphicObject::SetIsFusionEnabled(bool bIsFusion) {
    mIsFusionEnabled = bIsFusion;
}

bool FusionGraphicObject::IsFusionEnabled() const {
    return mIsFusionEnabled;
}
TPS_END_NAMESPACE  // end namespace tps
