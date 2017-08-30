////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poigraphicobject.cpp
/// 
///  \brief class TpsPoiGraphicObject definition 
/// 
///  \version 1.0
/// 
///  \date    2013/2/12
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"

TPS_BEGIN_NAMESPACE
TpsPoiGraphicObject::TpsPoiGraphicObject() : mIsActive(false),
    mIsCrossHairVisible(false),mIsVisible(false){
    mColor[0] = 0.0;
    mColor[1] = 0.0;
    mColor[2] = 0.0;
    mColor[3] = 1.0;

}

TpsPoiGraphicObject::~TpsPoiGraphicObject() {

}

void TpsPoiGraphicObject::SetColor(float color[4]) {
    memcpy(mColor, color, 4*sizeof(float));
}

void TpsPoiGraphicObject::GetColor(float color[4]) const {
    memcpy(color, mColor, 4*sizeof(float));
}

void TpsPoiGraphicObject::SetActiveFlag(bool flag) {
    mIsActive = flag;
}

bool TpsPoiGraphicObject::IsActive() {
    return mIsActive;
}

void TpsPoiGraphicObject::SetPosition(const Mcsf::MedViewer3D::Point3D &pt) {
    mPosition = pt;
}

Mcsf::MedViewer3D::Point3D TpsPoiGraphicObject::GetPosition() {
    return mPosition;
}

void TpsPoiGraphicObject::SetCrossHairVisible(bool flag) {
    mIsCrossHairVisible = flag;
}

void TpsPoiGraphicObject::SetPoiType(RtDbDef::POI_TYPE type) {
    mPoiType = type;
}

RtDbDef::POI_TYPE TpsPoiGraphicObject::GetPoiType() const {
    return mPoiType;
}

void TpsPoiGraphicObject::SetPoiName(const std::string &name) {
    mPoiName = name;
}

std::string TpsPoiGraphicObject::GetPoiName() const {
    return mPoiName;
}

bool TpsPoiGraphicObject::IsCrossHairVisible() {
    return mIsCrossHairVisible;
}

void TpsPoiGraphicObject::SetVisible(bool flag) {
    mIsVisible = flag;
}

bool TpsPoiGraphicObject::IsVisible() {
    return mIsVisible;
}


TPS_END_NAMESPACE