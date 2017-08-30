////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Cheng yilong  yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplan_graphicobject.cpp
/// 
///  \brief class TpsTpsEasyPlanDrrRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/10/29
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"


TPS_BEGIN_NAMESPACE
    EasyPlanDrrGraphicObejct::EasyPlanDrrGraphicObejct(void){
        mWidth =0;
        mHeight = 0;
        mBufferLen = 0;
        mWinWidth = 100;
        mWinCenter = 800;
        mImage = nullptr;
        mZoomFactor = 1.0f;
        mIsInversed = false;
        mIsOpposedImage = false;
        mImageCaptureTableAngle = 0.0;
        mCurrentImageTableAngle = 0.0;
        mIsocenterPosInImageModel.SetX(0.5f);
        mIsocenterPosInImageModel.SetY(0.5f);
        mIsocenterPosInImageModel.SetZ(0.0f);
}

EasyPlanDrrGraphicObejct::~EasyPlanDrrGraphicObejct(void){
    if (nullptr != mImage) {
        delete mImage;
        mImage = nullptr;
    }
}

bool EasyPlanDrrGraphicObejct::Initialize() {
    Mcsf::Vector3f col0(1.0f,0.0f,0.0f);
    Mcsf::Vector3f col1(0.0f,1.0f,0.0f);
    Mcsf::Vector3f col2(0.0f,0.0f,1.0f);
    mDrrModel2ImageModelMatrix3f.SetCol0(col0);
    mDrrModel2ImageModelMatrix3f.SetCol1(col1);
    mDrrModel2ImageModelMatrix3f.SetCol2(col2);
    mInitialDrrModel2ImageModelMatrix3f.SetCol0(col0);
    mInitialDrrModel2ImageModelMatrix3f.SetCol0(col1);
    mInitialDrrModel2ImageModelMatrix3f.SetCol0(col2);
    mTransformMatrix3f.SetCol0(col0);
    mTransformMatrix3f.SetCol1(col1);
    mTransformMatrix3f.SetCol2(col2);

    mWinCenter = 100;
    mWinWidth = 199;
    return  true;
}

bool EasyPlanDrrGraphicObejct::Finalize() {
    return true;
} 

void EasyPlanDrrGraphicObejct::SetImageBuffer(char* buffer,int width,
    int height,unsigned long bufferLen,DATATYPE dataType) {
        mWidth = width;
        mHeight = height;
        mBufferLen = bufferLen;
        mDataType = dataType;
        if (nullptr != mImage) {
            delete mImage;
            mImage = nullptr;
        }

        mImage = new char[mBufferLen];
        memcpy(mImage,buffer,mBufferLen*sizeof(char));
}

bool EasyPlanDrrGraphicObejct::SetDrrModel2ImageModel(const Mcsf::Matrix3f & drrModel2ImageModel ) {
    mDrrModel2ImageModelMatrix3f = drrModel2ImageModel;
    return true;
}

bool EasyPlanDrrGraphicObejct::GetDrrModel2ImageModel( Mcsf::Matrix3f* drrModel2ImageModel ) {
    *drrModel2ImageModel = mDrrModel2ImageModelMatrix3f;
    return true;
}

bool EasyPlanDrrGraphicObejct::GetWindowingWinWidthAndWinCenter( int* winWidth,int* winCenter ) {
    *winCenter = mWinCenter;
    *winWidth = mWinWidth;
    return true;
}

bool EasyPlanDrrGraphicObejct::SetWindowingWinWidthAndWinCenter( int winWidth, int winCenter ) {
    mWinWidth = winWidth;
    mWinCenter = winCenter;
    return true;
}

void EasyPlanDrrGraphicObejct::SetZoomFactor( float zoomFactor ) {
    mZoomFactor = zoomFactor;
}

float EasyPlanDrrGraphicObejct::GetZoomFactor() {
    return mZoomFactor;
}

bool EasyPlanDrrGraphicObejct::GetIsInversed() {
    return mIsInversed;
}

void EasyPlanDrrGraphicObejct::SetIsInversed(bool isInversed) {
    mIsInversed = isInversed;
}

bool EasyPlanDrrGraphicObejct::GetIsOpposedImage() {
    return mIsOpposedImage;
}

void EasyPlanDrrGraphicObejct::SetIsOpposedImage(bool isOpposedImage) {
    mIsOpposedImage = isOpposedImage;
}

bool EasyPlanDrrGraphicObejct::SetInitialDrrModel2ImageModel( const Mcsf::Matrix3f & initialDrrMode2ImageModel ) {
    mInitialDrrModel2ImageModelMatrix3f = initialDrrMode2ImageModel;
    return true;
}

bool EasyPlanDrrGraphicObejct::GetInitialDrrModel2ImageModel( Mcsf::Matrix3f* initialDrrMode2ImageModel ) {
    *initialDrrMode2ImageModel = mInitialDrrModel2ImageModelMatrix3f;
    return true;
}

bool EasyPlanDrrGraphicObejct::ResetDrrModel2ImageModel() {
    mDrrModel2ImageModelMatrix3f = mInitialDrrModel2ImageModelMatrix3f;
    return true;
}

bool EasyPlanDrrGraphicObejct::SetMlcTransformMatrix( const Mcsf::Matrix3f & transformMatrix ) {
    mTransformMatrix3f = transformMatrix;
    return true;
}

bool EasyPlanDrrGraphicObejct::GetMlcTransformMatrix( Mcsf::Matrix3f* transformMatrix ) {
    *transformMatrix = mTransformMatrix3f;
    return true;
}

void EasyPlanDrrGraphicObejct::ResetMlcTransformMatrix() {
    mTransformMatrix3f.SetIdentity();
}

void EasyPlanDrrGraphicObejct::SetImageCaptureTableAngle(double imageCaptureTableAngle){
    mImageCaptureTableAngle = imageCaptureTableAngle;
}

double EasyPlanDrrGraphicObejct::GetImageCaptureTableAngle() {
    return mImageCaptureTableAngle;
}

void EasyPlanDrrGraphicObejct::SetImageTableAngle(double tableAngle) { 
    mCurrentImageTableAngle = tableAngle;
}

double EasyPlanDrrGraphicObejct::GetImageTableAngle() {
    return mCurrentImageTableAngle;
}

void EasyPlanDrrGraphicObejct::SetIsocenterPosInImageModel(const Mcsf::Point3f &isocenterPosInImageModel) {
    mIsocenterPosInImageModel = isocenterPosInImageModel;
}

void EasyPlanDrrGraphicObejct::SetBeamUID(const std::string beamUID)
{
    mBeamUID = beamUID;
}
const std::string EasyPlanDrrGraphicObejct::GetBeamUID()
{
    return mBeamUID;
}

Mcsf::Point3f EasyPlanDrrGraphicObejct::GetIsocenterPosInImageModel() {
    return mIsocenterPosInImageModel;
}

TPS_END_NAMESPACE
