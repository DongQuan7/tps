////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_editsetuppoigraphicobject.h
/// 
///  \brief class EditSetupPtGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/03/31
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"

TPS_BEGIN_NAMESPACE

EditSetupPtGraphicObject::EditSetupPtGraphicObject() :mPosition(0.0,0.0),mVisible(false){

}

EditSetupPtGraphicObject::~EditSetupPtGraphicObject() {

}

bool EditSetupPtGraphicObject::Initialize() {
    return true;
}

bool EditSetupPtGraphicObject::Finalize() {
    return true;
}

void EditSetupPtGraphicObject::SetPosition(const Mcsf::MedViewer3D::Point2D &pt) {
    mPosition = pt;
}

const Mcsf::MedViewer3D::Point2D &EditSetupPtGraphicObject::GetPosition() const {
    return mPosition;
}


void EditSetupPtGraphicObject::SetVisible(bool flag) {
    mVisible = flag;
}

bool EditSetupPtGraphicObject::GetVisible() const {
    return mVisible;
}
TPS_END_NAMESPACE