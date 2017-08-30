////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  RONG Chengcheng  chengcheng.rong@united-imaging.com
/// 
///  \file tps_da_tpsmask.cpp
/// 
///  \brief class TPSMask (2D) 
/// 
///  \version 1.0
/// 
///  \date    2013/12/07
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "RtTpsFramework/tps_fw_mask.h"

TPS_BEGIN_NAMESPACE

TPSMask::TPSMask()
: mBuffer(nullptr) {
    mSize[0] = mSize[1] = 0;
}

TPSMask::TPSMask(size_t xdim, size_t ydim) {
    mSize[0] = xdim;
    mSize[1] = ydim;

    // alloca memory here
    mBuffer = new unsigned char[xdim*ydim];
    memset(mBuffer, 0, xdim*ydim);

    mMaskID = 1;
}

TPSMask::~TPSMask() {
    Clear();
}

TPSMask::TPSMask(const TPSMask& mask){

    mSize[0] = mask.mSize[0];
    mSize[1] = mask.mSize[1];

    // alloca memory here
    mBuffer = new unsigned char[mSize[0]*mSize[1]];
    memcpy(mBuffer, mask.mBuffer, sizeof(unsigned char)*mSize[0]*mSize[1]);
}

void TPSMask::operator = (const TPSMask& mask){

    mSize[0] = mask.mSize[0];
    mSize[1] = mask.mSize[1];

    // alloca memory here
    mBuffer = new unsigned char[mSize[0]*mSize[1]];
    memcpy(mBuffer, mask.mBuffer, sizeof(unsigned char)*mSize[0]*mSize[1]);
}

void TPSMask::GetMaskSize(size_t& xdim, size_t& ydim) const {
    xdim = mSize[0];
    ydim = mSize[1];
}

unsigned char* TPSMask::GetMaskBuffer() const {
    return mBuffer;
}

void TPSMask::SetMaskData(unsigned char* pData, size_t xdim, size_t ydim) {

    if (mBuffer == nullptr || xdim * ydim != mSize[0] * mSize[1]){
        delete[] mBuffer;
        mBuffer = new unsigned char[xdim * ydim];
    }

    mSize[0] = xdim;
    mSize[1] = ydim;
    memcpy(mBuffer, pData, sizeof(unsigned char) * xdim * ydim);
}

void TPSMask::Clear() {
    if (nullptr != mBuffer) {
        delete[] mBuffer;
        mBuffer = nullptr;
    }

    mSize[0] = mSize[1] = 0;
}

void TPSMask::SetMaskID(unsigned char maskID){
    mMaskID = maskID;
}

unsigned char TPSMask::GetMaskID() const{
    return mMaskID;
}

bool TPSMask::SaveToTrueColorBitmap(const std::string& sPath) {
    // prepare the true color buffer from mBuffer
    unsigned char* pTrueColorBuffer = new unsigned char[mSize[0]*mSize[1]*4];
    for (int m=0; m<mSize[1];++m){
        for (int n=0; n<mSize[0];++n){
            unsigned char val = mBuffer[m*mSize[0]+n];
            pTrueColorBuffer[(m*mSize[0]+n)*4] = val*100;
            pTrueColorBuffer[(m*mSize[0]+n)*4+1] = val*100;
            pTrueColorBuffer[(m*mSize[0]+n)*4+2] = val*100;
            pTrueColorBuffer[(m*mSize[0]+n)*4+3] = 0;
        }
    }

    if (!SaveToTrueColorBitmap_i(sPath, pTrueColorBuffer, mSize[1], mSize[0])) {
        delete[] pTrueColorBuffer;
        pTrueColorBuffer = nullptr;
        return false;
    }

    delete[] pTrueColorBuffer;
    pTrueColorBuffer = nullptr;

    return true;
}

bool TPSMask::SaveToTrueColorBitmap_i(const std::string& sPath, 
    unsigned char* pPixelBuffer,
    const int& nImageHeight,
    const int& nImageWidth) {
#ifdef WIN32
    //todo: check whether the file is existed

    // calculate the pad size of whole image
    int nStride = (nImageWidth*32+7)/8;
    int paddedsize = nStride*nImageHeight;

    // declare bmp structures 
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER info;

    // andinitialize them to zero
    memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
    memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );

    // fill the fileheader with data
    bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
    bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

    // fill the infoheader

    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biWidth = nImageWidth;
    info.biHeight = nImageHeight;
    info.biPlanes = 1;			// we only have one bitplane
    info.biBitCount = 32;		// RGB mode is 24 bits
    info.biCompression = BI_RGB;	
    info.biSizeImage = 0;		// can be 0 for 24 bit images
    info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
    info.biYPelsPerMeter = 0x0ec4;     
    info.biClrUsed = 0;			// we are in RGB mode and have no palette
    info.biClrImportant = 0;    // all colors are important

    // now we open the file to write to
    //#ifdef _DEBUG
    //    HANDLE file = CreateFileW(sPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
    //        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    //#else
    std::wstring wsPath(sPath.begin(), sPath.end());
    HANDLE file = CreateFile(wsPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    //#endif

    if ( file == NULL )
    {
        CloseHandle ( file );
        return false;
    }

    // write file header
    unsigned long bwritten;
    if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
    {	
        CloseHandle ( file );
        return false;
    }
    // write infoheader
    if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
    {	
        CloseHandle ( file );
        return false;
    }
    // write image data
    if ( WriteFile ( file, pPixelBuffer, paddedsize, &bwritten, NULL ) == false )
    {	
        CloseHandle ( file );
        return false;
    }

    // and close file
    CloseHandle ( file );
#else
    sPath;
    pPixelBuffer;
    nImageWidth;
    nImageHeight;
#endif // _WINDOWS

    return true;
}

TPS_END_NAMESPACE
