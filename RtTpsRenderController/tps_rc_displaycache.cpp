////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_displaycashe.h
/// 
///  \brief class TpsDisplayCashe implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/03
////////////////////////////////////////////////////////////////
#include"StdAfx.h"

#include "RtTpsRenderController/tps_rc_displaycache.h"
#include "tps_logger.h"

#include "McsfMedViewer3D/McsfMedViewer3DScene/mcsf_viewer3d_view.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

int TpsDisplayCache::cacheID = -1;

TpsDisplayCache::TpsDisplayCache()
    :mID(++cacheID), mBuffer(nullptr), mView(nullptr), mIsResized(false){
    
}

TpsDisplayCache::~TpsDisplayCache(){
    if (mBuffer){
        delete []mBuffer;
        mBuffer = nullptr;
    }
}

void TpsDisplayCache::SetRenderUnit(Mcsf::MedViewer3D::ClsView* view){
    if (view == nullptr){
        TPS_LOG_DEV_WARNING<<"[WARNING][TpsDisplayCache][SetRenderUnit]: "
                           <<"The input ClsView is empty.";
    }

    mView = view;
}

DISPLAYCACHE_UID TpsDisplayCache::CasheID() const{
    return mID;
}

bool TpsDisplayCache::GetBuffer(
    int& width, int& height, unsigned char*& buffer) const{
    if (mView == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayCache][GetBuffer]: "
                         <<"No render unit is being observed.";
        return false;
    }

    if (mBuffer == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayCache][GetBuffer]: "
            <<"The render result buffer is empty.";
        return false;
    }

    if (mWidth <= 0 || mHeight <= 0){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayCache][GetBuffer]: "
            <<"The render result buffer size is not positive.";
        return false;
    }

    width = mWidth;
    height = mHeight;
    buffer = mBuffer;
    return true;
}

void TpsDisplayCache::Update(int iID /* = -1 */){
    try{
        if (mView == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayCache][Update]: "
                             <<"No render unit is being observed.";
            return;
        }

        if (iID != 3){
            TPS_LOG_DEV_INFO<<"[INFO][TpsDisplayCache][Update]: "
                <<"Non-image render, return.";
            return;
        }

        //get buffer from ClsView
        int width, height;
        mView->GetDisplaySize(width, height);
        if (mWidth != width || mHeight != height || mBuffer == nullptr){
            if (mBuffer) delete []mBuffer;
            mBuffer = new unsigned char[width * height * 4];
        }

        mIsResized = (mWidth != width || mHeight != height);
        mView->GetRGBResult(mWidth, mHeight, mBuffer);

#ifdef _DEBUG
        char fileName[256];
        sprintf_s(fileName, "d:\\temp\\fbo_result%d_in_cache.bmp", mID);
        SaveToTrueColorBitmap_i(fileName, mBuffer, mHeight, mWidth);
#endif

        //reverse buffer
        int* pLine = new int[mWidth];
        for (int iLine = 0; iLine < mHeight / 2; ++iLine)
        {
            int* pSrc = (int*)mBuffer + mWidth * iLine;
            int* pDst = (int*)mBuffer + mWidth * (mHeight - 1 - iLine);

            memcpy(pLine, pSrc, mWidth * 4);
            memcpy(pSrc, pDst, mWidth * 4);
            memcpy(pDst, pLine, mWidth * 4);
        }
        delete []pLine; pLine = nullptr;

        Notify(mID);
    }
    catch (std::bad_alloc& badAllocEx){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsDisplayCache][Update]: "
                         <<"Bad memory allocation occurs."<<badAllocEx.what();
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsDisplayCache][Update]: "
                         <<ex.what();
    }
}

bool TpsDisplayCache::IsResized(){
    return mIsResized;
}

bool TpsDisplayCache::SaveToTrueColorBitmap_i(const std::string& sPath, 
    unsigned char* pPixelBuffer,
    const int& nImageHeight,
    const int& nImageWidth)
{
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

TPS_END_NAMESPACE  // end namespace tps