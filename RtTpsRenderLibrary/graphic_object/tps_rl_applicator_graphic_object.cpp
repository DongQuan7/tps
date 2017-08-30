////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_applicator_graphic_object.cpp
/// 
///  \brief class ApplicatorGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

ApplicatorGraphicObject::ApplicatorGraphicObject() : mName(""), 
    mShapeType(RtDbDef::SYM_SQUARE), mOpening(0.0), mOpeningX(0.0), mOpeningY(0.0){
    
}

ApplicatorGraphicObject::~ApplicatorGraphicObject(){
    
}

std::string ApplicatorGraphicObject::GetName() const{
    return mName;
}

void ApplicatorGraphicObject::SetName(const std::string& name){
    mName = name;
}

RtDbDef::APPLICATOR_APERTURE_SHAPE ApplicatorGraphicObject::GetShapeType() const{
    return mShapeType;
}

void ApplicatorGraphicObject::SetShapeType(RtDbDef::APPLICATOR_APERTURE_SHAPE shapeType){
    mShapeType = shapeType;
}

double ApplicatorGraphicObject::GetOpening() const{
    return mOpening;
}

void ApplicatorGraphicObject::SetOpening(double val){
    mOpening = val;
}

double ApplicatorGraphicObject::GetOpeningX() const{
    return mOpeningX;
}

void ApplicatorGraphicObject::SetOpeningX(double val){
    mOpeningX = val;
}

double ApplicatorGraphicObject::GetOpeningY() const{
    return mOpeningY;
}

void ApplicatorGraphicObject::SetOpeningY(double val){
    mOpeningY = val;
}
TPS_END_NAMESPACE  // end namespace tps