//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenlong.zhao (mailto:wenlong.zhao@united-imaging.com)
///
///  \file   rt_tps_certified_imagereferenceinfo.h 
///  \brief   
///
///  \version 1.0
///  \date  2015/04/29  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef RT_TPS_CERTIFIEDIMAGEREFERENCEINFO_H_
#define RT_TPS_CERTIFIEDIMAGEREFERENCEINFO_H_

#include "tps_defs.h"

namespace tda {
    class ReferenceImage;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct CertifiedImageReferenceInfo {

    CertifiedImageReferenceInfo();

    std::string mImageUID;
    std::string mBeamUID;
    std::string mImageDate;
    std::string mImageTime;
    float mImageAngle;

    int mImageType;
    int mImageHeight;
    int mImageWidth;
    float mPixelSpacing;
    int mCenterX;
    int mCenterY;
    int mBitsAllocated;
    unsigned int mBufferLength;

    char* mImageBuffer;
    int mCpNumber;

    float mReceptOrTranslationX;
    float mReceptOrTranslationY;
    float mReceptOrTranslationZ;
    float mReceptOrAngle;
    float mReceptOrPosX;
    float mReceptOrPosY;

    void Convert(const tda::ReferenceImage& referenceImage);

    void ConvertBack(tda::ReferenceImage* referenceImage) const;
};
TPS_END_NAMESPACE  // end namespace tps
#endif