////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobject.cpp
/// 
///  \brief class TpsVOIGraphicObject implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsFramework/tps_fw_contour_set.h"

#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_vertex_array_buffer.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIGraphicObject::TpsVOIGraphicObject()
    : mMode(TpsVOIGraphicObject::LINE), 
      mVisible(true), 
      mHasInterpolate(false), 
      mIsInterpolate(nullptr), 
      mSliceCount(0), 
      mIsMeshDataDirty(true),
      mIsShaded(true),
      mSDF(new SignedDistanceField()),
      mCS(new ContourSet())
{
    mColor[0] = 1.0;
    mColor[1] = 0.0;
    mColor[2] = 0.0;
    mColor[3] = 1.0;

    mMeshVBO = Mcsf::MedViewer3D::VertexArrayBuffer::Create();
    mMeshNBO = Mcsf::MedViewer3D::VertexArrayBuffer::Create();
}

TpsVOIGraphicObject::~TpsVOIGraphicObject()
{
    if(mIsInterpolate != nullptr) {
        delete []mIsInterpolate;
        mIsInterpolate = nullptr;
    }

    TPS_DEL_PTR(mSDF);
    TPS_DEL_PTR(mCS);
}

void TpsVOIGraphicObject::SetVisionMode(TpsVOIGraphicObject::VISION_MODE mode){
    mMode = mode;
}

TpsVOIGraphicObject::VISION_MODE TpsVOIGraphicObject::GetVisionMode() const{
    return mMode;
}

void TpsVOIGraphicObject::SetVisibility(bool visibility){
    
    mVisible = visibility;
}

bool TpsVOIGraphicObject::GetVisibility() const{
    return mVisible;
}

void TpsVOIGraphicObject::SetColor(const float* color) {

    memcpy(mColor, color ,4 * sizeof(float));
}

void TpsVOIGraphicObject::GetColor(float* color) const{
    
    memcpy(color, mColor, 4 * sizeof(float));
}
void TpsVOIGraphicObject::SetSlices(int* slices, bool isCut) {
    mSlices[0] = slices[0];
    mSlices[1] = slices[1];
    mIsCutContour = isCut;
}
void TpsVOIGraphicObject::GetSlices(int* slices) const {
    slices[0] = mSlices[0];
    slices[1] = mSlices[1];
}

bool TpsVOIGraphicObject::IsCutContour() {
    return mIsCutContour;
}

void TpsVOIGraphicObject::SetIsInterpolate(int slice, bool isInterpolate) {
    mIsInterpolate[slice] = isInterpolate;
}

bool TpsVOIGraphicObject::GetIsInterpolate(int slice) {
    return mIsInterpolate[slice];
}

void TpsVOIGraphicObject::SetHasInterpolate(bool hasInterpolate) {
    mHasInterpolate = hasInterpolate;
}

bool TpsVOIGraphicObject::GetHasInterpolate() {
    return mHasInterpolate;
}

void TpsVOIGraphicObject::SetInterpolateFlags(bool *flags) {
    if (flags == nullptr || mIsInterpolate == nullptr || mSliceCount == 0) return;
    memcpy(mIsInterpolate, flags, sizeof(bool) * mSliceCount);
}

void TpsVOIGraphicObject::GetInterpolateFlags(bool* &flags) {
    flags = mIsInterpolate;
}

void TpsVOIGraphicObject::SetSliceCount(int sliceCount) {
    if(sliceCount != mSliceCount) {
        if(mIsInterpolate != nullptr) {
            delete []mIsInterpolate;
            mIsInterpolate = nullptr;
        } 
        mIsInterpolate = new bool[sliceCount];
        memset(mIsInterpolate, 1, sliceCount * sizeof(bool));
        mSliceCount = sliceCount;
    }
}

int TpsVOIGraphicObject::GetSliceCount() {
    return mSliceCount;
}

bool TpsVOIGraphicObject::IsShaded() const
{
    return mIsShaded;
}

void TpsVOIGraphicObject::SetIsShaded(bool isShaded)
{
    mIsShaded = isShaded;
}

float TpsVOIGraphicObject::GetAlpha3D() const
{
    return mAlpha3D;
}

void TpsVOIGraphicObject::SetAlpha3D(float alpha)
{
    mAlpha3D = alpha;
}

bool TpsVOIGraphicObject::IsMeshDataDirty() const
{
    return mIsMeshDataDirty;
}

void TpsVOIGraphicObject::SetMeshDataDirty(bool isMeshDataDirty)
{
    mIsMeshDataDirty = isMeshDataDirty;
}

void TpsVOIGraphicObject::GetMeshBuffer(unsigned int& vboID, unsigned int& nboID) const
{
    vboID = mMeshVBO->GetID();
    nboID = mMeshNBO->GetID();
}

void TpsVOIGraphicObject::GetMeshVertexCount(unsigned int& vertexCount) const
{
    vertexCount = mMeshVertexCount;
}

void TpsVOIGraphicObject::SetMeshVertexCount(unsigned int vertexCount)
{
    mMeshVertexCount = vertexCount;
}

SignedDistanceField* TpsVOIGraphicObject::GetSDF() const
{
    return mSDF;
}

ContourSet* TpsVOIGraphicObject::GetContourSet() const
{
    return mCS;
}
TPS_END_NAMESPACE  // end namespace tps