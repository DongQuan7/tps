////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_manualremovetablegraphicobject.cpp
/// 
///  \brief class ManualRemoveTableGraphicObject 
/// 
///  \version 1.0
/// 
///  \date    2014/07/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_manualremovetablegraphicobject.h"

TPS_BEGIN_NAMESPACE
ManualRemoveTableGraphicObject::ManualRemoveTableGraphicObject(): mLineHeight(0.0f),
mIsVisible(false) {

}

ManualRemoveTableGraphicObject::~ManualRemoveTableGraphicObject() {

}

void ManualRemoveTableGraphicObject::SetHeight(float height) {
    mLineHeight = height;
}

float ManualRemoveTableGraphicObject::GetHeight() const {
    return mLineHeight;
}

void ManualRemoveTableGraphicObject::SetVisibilty(bool bVisible) {
    mIsVisible = bVisible;
}

bool ManualRemoveTableGraphicObject::GetVisibilty() const {
    return mIsVisible;
}


TPS_END_NAMESPACE