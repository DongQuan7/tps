////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_landmarkgraphicobject.cpp
/// 
///  \brief class LandmarkGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_landmarkgraphicobject.h"



TPS_BEGIN_NAMESPACE   // begin namespace tps

LandmarkGraphicObject::LandmarkGraphicObject()
: mIsEnabled(false), mMarkFlag(0), mFixedImageUID("")
, mFloatImageUID(""), mPointSize(3), mSelectedIndex(-1){

}

LandmarkGraphicObject::~LandmarkGraphicObject(){
    
}

void LandmarkGraphicObject::SetImageUID(const std::string& fst, const std::string& snd){
    mFixedImageUID = fst;
    mFloatImageUID = snd;
}

void LandmarkGraphicObject::GetImageUID(std::string& fst, std::string& snd) const{
    fst = mFixedImageUID;
    snd = mFloatImageUID;
}

void LandmarkGraphicObject::SetMarkFlag(short flag){
    mMarkFlag = flag;
}

short LandmarkGraphicObject::GetMarkFlag() const{
    return mMarkFlag;
}

void LandmarkGraphicObject::Enable(bool isEnabled){
    mIsEnabled = isEnabled;
}

bool LandmarkGraphicObject::IsEnabled() const{
    return mIsEnabled;
}

bool LandmarkGraphicObject::SetLandmarkPoint(int index, const Mcsf::MedViewer3D::Point3D& point){
    if (index < 1 || index > LANDMARK_POINT_PAIR_COUNT * 2){
        TPS_LOG_DEV_ERROR<<"The index is out of range.";
        return false;
    }

    mLandmarkPoints[index - 1] = point;
    return true;
}

bool LandmarkGraphicObject::GetLandmarkPoint(int index, Mcsf::MedViewer3D::Point3D& point) const{
    if (index < 1 || index > LANDMARK_POINT_PAIR_COUNT * 2){
        TPS_LOG_DEV_ERROR<<"The index is out of range.";
        return false;
    }

    point = mLandmarkPoints[index - 1];
    return true;
}

void LandmarkGraphicObject::SetPointSize(int size){
    mPointSize = size;
}

int LandmarkGraphicObject::GetPointSize() const{
    return mPointSize;
}

void LandmarkGraphicObject::SetSelectedIndex(int index){
    mSelectedIndex = index;
}

int LandmarkGraphicObject::GetSelectedIndex() const{
    return mSelectedIndex;
}
TPS_END_NAMESPACE  // end namespace tps