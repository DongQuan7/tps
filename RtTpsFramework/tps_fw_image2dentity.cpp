//////////////////////////////////////////////////////////////////////////
/// \defgroup  TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yilong.cheng mailto:yilong.cheng@united-imaging.com
///
///  \file    tda_da_TpsImage2DEntity.cpp
///  \brief   TpsImage2DEntity interface
///
///  \version 1.0
///  \date    2014-10-29
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_image2dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsImage2DEntity::TpsImage2DEntity() : mImage2D(nullptr) {
        mBufferLen = 0;
        mImage2D = nullptr;
}

TpsImage2DEntity::~TpsImage2DEntity() {
    if (mImage2D){
        delete mImage2D;
        mImage2D = nullptr;
    }
}

void TpsImage2DEntity::SetRTImage(RtRtimage* pRtImage) {
    mImage2D = pRtImage;
}

RtRtimage* TpsImage2DEntity::get() const {
    return mImage2D;
}

bool TpsImage2DEntity::GetImage2DBuffer(char*& buffer){
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return false;
    } 
    // 临时LEN， 暂时 只做参数
    unsigned long len =0;
    if (nullptr != mImage2D->get_pixel_data_buffer(&len)) {
        buffer = mImage2D->get_pixel_data_buffer(&len);
    }
    else {
        TPS_LOG_DEV_ERROR << "The image 2d buffer is empty.";
        return false;
    }
    return true;
}

bool TpsImage2DEntity::GetUID(std::string& uid) const {
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return false;
    }
    uid = mImage2D->get_uid();
    return true;
}

bool TpsImage2DEntity::ReadImage2DFromFile(const std::string& /*sFilePath*/) {
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return false;
    }

    //return mImage2D->LoadPixelDataFromFile(sFilePath);
    return true;
}

bool TpsImage2DEntity::WriteImage2DToFile(const std::string& /*sFilePath*/) const {
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return false;
    }

    return true;
    //return mImage2D->SavePixelDataToFile(sFilePath);
}

bool TpsImage2DEntity::GetImageSize( int &width,int &height ) {
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return false;
    }
    height = mImage2D->get_rows();
    width = mImage2D->get_columns();

    return true;
}

unsigned long TpsImage2DEntity::GetBufferLen() {
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return 0;
    }
    int width;
    int height;
    int frame;
    int samplePerPixel;
    //long byteCount = 2  // test
    long byteCount= mImage2D->get_bitsallocated()/8;
    height = mImage2D->get_rows();
    width = mImage2D->get_columns();
    frame = mImage2D->get_frame();
    samplePerPixel = mImage2D->get_samplesperpixel();
    return width*height*byteCount*frame*samplePerPixel;
}

bool TpsImage2DEntity::SetRegistrationMatrix(const std::string &sDrrFloatUID,
    Mcsf::Matrix3f &matrix ) {
        if (mImage2D == nullptr){
            TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
            return false;
        }
        std::string sUID = sDrrFloatUID;
        auto it = mRegistrationMatrixMap.find(sUID);
        if (it!=mRegistrationMatrixMap.end()) {
            it->second = matrix;
            return true;
        }
        mRegistrationMatrixMap[sUID] = matrix;
        return true;
}

bool TpsImage2DEntity::GetRegistrationMatrix(const std::string &sDrrFloatUID,
    Mcsf::Matrix3f &matrix ) {
        if (mImage2D == nullptr){
            TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
            return false;
        }
        std::string sUID = sDrrFloatUID;
        auto it = mRegistrationMatrixMap.find(sUID);
        if (it == mRegistrationMatrixMap.end()) {
            TPS_LOG_DEV_ERROR<<"Registration matrix does not exist to image data: "<<sUID;
            return false;
        }

        matrix = it->second;
        return true;
}

bool TpsImage2DEntity::GetImageWindowingInfo( int &winCenter, int &winWidth )
{
    if (mImage2D == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 2d is empty.";
        return false;
    }
    winCenter = mImage2D->get_windowcenter();
    winWidth = mImage2D->get_windowwidth();

    return true;
}

TPS_END_NAMESPACE  // end namespace tps
