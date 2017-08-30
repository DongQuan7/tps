////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Zuo Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_dosegridboundarygraphicobject.cpp
/// 
///  \brief class TpsDoseGridBoundaryGraphicObject definition 
/// 
///  \version 1.0
/// 
///  \date    2014/4/11
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"

TPS_BEGIN_NAMESPACE
TpsDoseGridBoundaryGraphicObject::TpsDoseGridBoundaryGraphicObject() :
mIsVisible(true), mType("") {

    mColor[0] = 1.0f, mColor[1] = 0.0f, mColor[2] = 0.0f;
}

TpsDoseGridBoundaryGraphicObject::~TpsDoseGridBoundaryGraphicObject() {

}

void TpsDoseGridBoundaryGraphicObject::SetLeftBottomPoint(const Mcsf::MedViewer3D::Point3D& point) {
    mLeftBottomPoint = point;
}

Mcsf::MedViewer3D::Point3D TpsDoseGridBoundaryGraphicObject::GetLeftBottomPoint() {
    return mLeftBottomPoint;
}

void TpsDoseGridBoundaryGraphicObject::SetRightTopPoint(const Mcsf::MedViewer3D::Point3D& point) {
    mRightTopPoint = point;
}

Mcsf::MedViewer3D::Point3D TpsDoseGridBoundaryGraphicObject::GetRightTopPoint() {
    return mRightTopPoint;
}

void TpsDoseGridBoundaryGraphicObject::SetVisible(bool isVisible) {
    mIsVisible = isVisible;
}

bool TpsDoseGridBoundaryGraphicObject::GetVisible() {
    return mIsVisible;
}

void TpsDoseGridBoundaryGraphicObject::SetColor(float* color){
    memcpy(mColor, color, 3 * sizeof(float));
}

void TpsDoseGridBoundaryGraphicObject::GetColor(const float*& color) const{
    color = mColor;
}

void TpsDoseGridBoundaryGraphicObject::SetSpacing(const Mcsf::MedViewer3D::Point3D& spacing) {
    mSpacing = spacing;
}

Mcsf::MedViewer3D::Point3D TpsDoseGridBoundaryGraphicObject::GetSpacing() {
    return mSpacing;
}

void TpsDoseGridBoundaryGraphicObject::SetType(std::string type) {
    mType = type;
}
std::string TpsDoseGridBoundaryGraphicObject::GeType() {
    return mType;
}
TPS_END_NAMESPACE