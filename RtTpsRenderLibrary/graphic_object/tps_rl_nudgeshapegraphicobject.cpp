////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_nudgecirclegraphicobject.cpp
/// 
///  \brief class TpsNudgeShapeGraphicObject implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/03/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_nudgeshapegraphicobject.h"



TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsNudgeShapeGraphicObject::TpsNudgeShapeGraphicObject() : 
    mActive(false), mRadius(0.0), mOrigin(0.0, 0.0, 0.0){

    memset(mColor, 0, sizeof(float));
}

TpsNudgeShapeGraphicObject::~TpsNudgeShapeGraphicObject(){

}

void TpsNudgeShapeGraphicObject::SetRadius(double radius){

    mRadius = radius;
}

double TpsNudgeShapeGraphicObject::GetRadius() const{

    return mRadius;
}

Mcsf::MedViewer3D::Point3D TpsNudgeShapeGraphicObject::GetOrigin() const{

    return mOrigin;
}

void TpsNudgeShapeGraphicObject::SetOrigin(const Mcsf::MedViewer3D::Point3D& origin){

    mOrigin = origin;
}

void TpsNudgeShapeGraphicObject::SetShape(int shape) {
    mShape = shape;
}
int TpsNudgeShapeGraphicObject::GetShape() {
    return mShape;
}

void TpsNudgeShapeGraphicObject::SetColor(float* color){

    if (color != nullptr) memcpy(mColor, color, 4 * sizeof(float));
}

void TpsNudgeShapeGraphicObject::GetColor(float* color) const{

    memcpy(color, mColor, 4 * sizeof(float));
}

void TpsNudgeShapeGraphicObject::SetActive(bool bActive){

    mActive = bActive;
}

bool TpsNudgeShapeGraphicObject::IsActive() const{

    return mActive;
}
TPS_END_NAMESPACE  // end namespace tps