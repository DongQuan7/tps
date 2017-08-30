//////////////////////////////////////////////////////////////////
/////  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
/////  All rights reserved.
///// 
/////  \author  Zuo Kai  kai.zuo@united-imaging.com
///// 
/////  \file tps_rl_contoursgraphicobject.cpp
///// 
/////  \brief class ContoursGo declaration 
///// 
/////  \version 1.0
///// 
/////  \date    2016/01/11
//////////////////////////////////////////////////////////////////
//
#include "StdAfx.h"
//#include "RtTpsRenderLibrary/tps_rl_contoursgraphicobject.h"
//
//#include "tps_logger.h"
//
//TPS_BEGIN_NAMESPACE   // begin namespace tps
//
//ContoursGraphicObject::ContoursGraphicObject(){
//    for (int i = 0; i < 3; i++)
//    {
//        mAngles[i] = 0.0;
//        mOffset[i] = 0.0;
//        mOffsetInVolume[i] = 0;
//    }
//    mIsinitialized = false;
//    mIsVisible = false;
//}
//
//ContoursGraphicObject::~ContoursGraphicObject(){
//
//}
//
//void ContoursGraphicObject::SetUid(const std::string& uid) {
//    mUid = uid;
//}
//std::string ContoursGraphicObject::GetUid() {
//    return mUid;
//}
//
//void ContoursGraphicObject::SetOffset(float* offset) {
//    mOffset[0] = offset[0];
//    mOffset[1] = offset[1];
//    mOffset[2] = offset[2];
//}
//void ContoursGraphicObject::GetOffset(float*& offset) {
//    offset = mOffset;
//}
//
//void ContoursGraphicObject::SetAngles(float* angles) {
//    mAngles[0] = angles[0];
//    mAngles[1] = angles[1];
//    mAngles[2] = angles[2];
//}
//void ContoursGraphicObject::GetAngles(float*& angles) {
//    angles = mAngles;
//}
//
//void ContoursGraphicObject::SetOffsetInVolume(int* offsetInVolume) {
//    mOffsetInVolume[0] = offsetInVolume[0];
//    mOffsetInVolume[1] = offsetInVolume[1];
//    mOffsetInVolume[2] = offsetInVolume[2];
//}
//void ContoursGraphicObject::GetOffsetInVolume(int*& offsetInVolume){
//    offsetInVolume = mOffsetInVolume;
//}
//
//void ContoursGraphicObject::SetColor(float* color) {
//    for (int i = 0; i < 4; ++i)
//    {
//        mColor[i] = color[i];
//    }
//}
//void ContoursGraphicObject::GetColor(float*& color) {
//    color = mColor;
//}
//
//void ContoursGraphicObject::AddContour(const std::vector<Mcsf::MedViewer3D::Point3D>& contour, int slice) {
//    mContoursList[slice].push_back(contour);
//}
//void ContoursGraphicObject::AddContours(const std::vector<std::vector<Mcsf::MedViewer3D::Point3D> >& contours, int slice) {
//    mContoursList[slice] = contours;
//}
//void ContoursGraphicObject::GetContours(std::vector<std::vector<Mcsf::MedViewer3D::Point3D> >& contours, int slice) {
//    if(mContoursList.size() > slice)
//    {
//        contours =  mContoursList[slice];
//    }
//}
//
//void ContoursGraphicObject::AddSagittalContour(const std::vector<Mcsf::MedViewer3D::Point3D>& contour, int slice) {
//    mSagittalContoursList[slice].push_back(contour);
//}
//void ContoursGraphicObject::AddSagittalContours(const std::vector<std::vector<Mcsf::MedViewer3D::Point3D> >& contours, int slice){
//    mSagittalContoursList[slice] = contours;
//}
//void ContoursGraphicObject::GetSagittalContours(std::vector<std::vector<Mcsf::MedViewer3D::Point3D> >& contours, int slice) {
//    if(mSagittalContoursList.size() > slice)
//    {
//        contours =  mSagittalContoursList[slice];
//    }
//}
//
//void ContoursGraphicObject::AddCoronalContour(const std::vector<Mcsf::MedViewer3D::Point3D>& contour, int slice) {
//    mCoronalContoursList[slice].push_back(contour);
//}
//void ContoursGraphicObject::AddCoronalContours(const std::vector<std::vector<Mcsf::MedViewer3D::Point3D> >& contours, int slice) {
//    mCoronalContoursList[slice] = contours;
//}
//void ContoursGraphicObject::GetCoronalContours(std::vector<std::vector<Mcsf::MedViewer3D::Point3D> >& contours, int slice) {
//    if(mCoronalContoursList.size() > slice)
//    {
//        contours =  mCoronalContoursList[slice];
//    }
//}
//
//void ContoursGraphicObject::GetContoursList(std::vector<std::vector<std::vector<Mcsf::MedViewer3D::Point3D> > >& contoursList) {
//    contoursList = mContoursList;
//}
//void ContoursGraphicObject::GetCoronalContoursList(std::vector<std::vector<std::vector<Mcsf::MedViewer3D::Point3D> > >& contoursList) {
//    contoursList = mCoronalContoursList;
//}
//void ContoursGraphicObject::GetSagittalContoursList(std::vector<std::vector<std::vector<Mcsf::MedViewer3D::Point3D> > >& contoursList) {
//    contoursList = mSagittalContoursList;
//}
//
//void ContoursGraphicObject::InitializeContoursGo(int sagittalSlice, int coronalSlice, int totalSlice) {
//    if(mContoursList.size() != totalSlice)
//    {
//        mContoursList.clear();
//        mContoursList.resize(totalSlice);
//    }
//    if(mSagittalContoursList.size() != sagittalSlice)
//    {
//        mSagittalContoursList.clear();
//        mSagittalContoursList.resize(sagittalSlice);
//    }
//    if(mCoronalContoursList.size() != coronalSlice)
//    {
//        mCoronalContoursList.clear();
//        mCoronalContoursList.resize(coronalSlice);
//    }
//}
//
//void ContoursGraphicObject::SetIsinitialized(const bool& isinitialized) {
//    mIsinitialized = isinitialized;
//}
//bool ContoursGraphicObject::GetIsinitialized() {
//    return mIsinitialized;
//}
//
//void ContoursGraphicObject::SetIsVisible(const bool& isVisible) {
//    mIsVisible = isVisible;
//}
//bool ContoursGraphicObject::GetIsVisible() {
//    return mIsVisible;
//}
//
//TPS_END_NAMESPACE  // end namespace tps