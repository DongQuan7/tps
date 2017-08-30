//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Miao Chenfeng Chenfeng.Miao@united-imaging.com
//
/// \file    tps_rl_doselinegraphicobject.cpp
///
/// \brief   DoseLineGraphicObject
/// 
/// \version 1.0
/// 
/// \date    2013/12/01
/// 
///  \modified by maoliang.gong(maoliang.gong@united-imaging.com)
///  \brief  refactor dose graphic object
///  \date    Jan. 13, 2014
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"


TPS_BEGIN_NAMESPACE
TpsMPRDoseGraphicObject::TpsMPRDoseGraphicObject():
    mDosePlaneBuffer(nullptr),mHeight(0),mWidth(0){
}

TpsMPRDoseGraphicObject::~TpsMPRDoseGraphicObject() {
    if (mDosePlaneBuffer != nullptr){
        delete[] mDosePlaneBuffer;
    }
}

bool TpsMPRDoseGraphicObject::SetDoseGrid(float* buffer,int height, int width, 
                    const Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane){

    if (buffer == nullptr || width <= 0 || height <= 0){
        TPS_LOG_DEV_ERROR<<"Invalid buffer pointer or memory size: "
            <<"buffer pointer: "<<buffer<<"|width: "<<width<<"|height: "<<height;
        return false;
    }
    if (mDosePlaneBuffer == nullptr || mWidth * mHeight != width * height){
        if (mDosePlaneBuffer) delete[] mDosePlaneBuffer;
        mDosePlaneBuffer = new float[width * height];
    }
    
    mWidth = width;
    mHeight = height;
    memcpy(mDosePlaneBuffer, buffer, width * height * sizeof(float));
    mWorldToNormalPlaneTM = matWorldToNormalPlane;
    return true;
}

void TpsMPRDoseGraphicObject::GetDoseGrid(float*& buffer,int& height, int& width, 
                      Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const{
    buffer = mDosePlaneBuffer;
    width = mWidth;
    height = mHeight;
    matWorldToNormalPlane = mWorldToNormalPlaneTM;
}

TPS_END_NAMESPACE