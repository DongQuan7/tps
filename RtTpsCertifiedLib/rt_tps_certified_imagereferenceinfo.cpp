//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenlong.zhao (mailto:wenlong.zhao@united-imaging.com)
///
///  \file   rt_tps_certified_imagereferenceinfo.cpp 
///  \brief   
///
///  \version 1.0
///  \date  2015/05/04  
///  \
//////////////////////////////////////////////////////////////////////////

#include "RtTpsCertifiedLib/rt_tps_certified_imagereferenceinfo.h"
#include "RtTpsProtoLib/tda_imagereferenceinfo.pb.h"//new

TPS_BEGIN_NAMESPACE  // begin namespace tps

//initialization is necessary in MLC Auto-shape
CertifiedImageReferenceInfo::CertifiedImageReferenceInfo()
{
    mImageUID = "";
    mBeamUID = "";
    mImageDate = "";
    mImageTime = "";
    mImageAngle = 0.0f;

    mImageType = -1;
    mImageHeight = 0;
    mImageWidth = 0;
    mPixelSpacing = 0.0f;
    mCenterX = 0;
    mCenterY = 0;
    mBitsAllocated = 0;
    mBufferLength = 0;

    mImageBuffer = nullptr;
    mCpNumber = 0;

    mReceptOrTranslationX = 0.0f;
    mReceptOrTranslationY = 0.0f;
    mReceptOrTranslationZ = 0.0f;
    mReceptOrAngle = 0.0f;
    mReceptOrPosX = 0.0f;
    mReceptOrPosY = 0.0f;
}

void CertifiedImageReferenceInfo::Convert(const tda::ReferenceImage& referenceImage) 
{
    if(referenceImage.has_imageuid()) {
        mImageUID = referenceImage.imageuid();
    }
    if(referenceImage.has_beamuid()) {
        mBeamUID = referenceImage.beamuid();
    }
    if(referenceImage.has_imagedate()) {
        mImageDate = referenceImage.imagedate();
    }
    if(referenceImage.has_imagetime()) {
        mImageTime = referenceImage.imagetime();
    }
    if(referenceImage.has_imageangle()) {
        mImageAngle = referenceImage.imageangle();
    }
    if(referenceImage.has_imagetype()) {
        mImageType = (int)referenceImage.imagetype();
    }
    if(referenceImage.has_imageheight()) {
        mImageHeight = referenceImage.imageheight();
    }
    if(referenceImage.has_imagewidth()) {
        mImageWidth = referenceImage.imagewidth();
    }
    if(referenceImage.has_pixelspacing()) {
        mPixelSpacing = referenceImage.pixelspacing();
    }
    if(referenceImage.has_centerx()) {
        mCenterX = referenceImage.centerx();
    }
    if(referenceImage.has_centery()) {
        mCenterY = referenceImage.centery();
    }
    if(referenceImage.has_bitsallocated()) {
        mBitsAllocated = referenceImage.bitsallocated();
    }
    if(referenceImage.has_bufferlength()) {
        mBufferLength = referenceImage.bufferlength();
    }
    if(referenceImage.has_imagebuffer()) {
        mImageBuffer = const_cast<char *>(referenceImage.imagebuffer().data());
    }
    if(referenceImage.has_cpnumber()) {
        mCpNumber = referenceImage.cpnumber();
    }
    if(referenceImage.has_receptortranslationx()) {
        mReceptOrTranslationX = referenceImage.receptortranslationx();
    }
    if(referenceImage.has_receptortranslationy()) {
        mReceptOrTranslationY = referenceImage.receptortranslationy();
    }
    if(referenceImage.has_receptortranslationz()) {
        mReceptOrTranslationZ = referenceImage.receptortranslationz();
    }
    if(referenceImage.has_receptorangle()) {
        mReceptOrAngle = referenceImage.receptorangle();
    }
    if(referenceImage.has_receptorposx()) {
        mReceptOrPosX = referenceImage.receptorposx();
    }
    if(referenceImage.has_receptorposy()) {
        mReceptOrPosY = referenceImage.receptorposy();
    }
}

void CertifiedImageReferenceInfo::ConvertBack(tda::ReferenceImage* referenceImage) const
{
   referenceImage->set_imageuid(mImageUID);
   referenceImage->set_beamuid(mBeamUID);
   referenceImage->set_imagedate(mImageDate);
   referenceImage->set_imagetime(mImageTime);
   referenceImage->set_imageangle(mImageAngle);

   referenceImage->set_imagetype((tda::ReferenceImage_IMAGE_TYPE)mImageType);
   referenceImage->set_imageheight(mImageHeight);
   referenceImage->set_imagewidth(mImageWidth);
   referenceImage->set_pixelspacing(mPixelSpacing);
   referenceImage->set_centerx(mCenterX);
   referenceImage->set_centery(mCenterY);
   referenceImage->set_bitsallocated(mBitsAllocated);
   referenceImage->set_bufferlength(mBufferLength);

   referenceImage->set_imagebuffer(mImageBuffer);
   referenceImage->set_cpnumber(mCpNumber);

   referenceImage->set_receptortranslationx(mReceptOrTranslationX);
   referenceImage->set_receptortranslationy(mReceptOrTranslationY);
   referenceImage->set_receptortranslationz(mReceptOrTranslationZ);
   referenceImage->set_receptorangle(mReceptOrAngle);
   referenceImage->set_receptorposx(mReceptOrPosX);
   referenceImage->set_receptorposy(mReceptOrPosY);
}
TPS_END_NAMESPACE  // end namespace tps
