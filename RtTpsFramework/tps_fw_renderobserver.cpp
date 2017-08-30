////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file 
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2015/07/08
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_renderobserver.h"

#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

RenderObserver::RenderObserver() : mResponser(nullptr)
{

}

RenderObserver::~RenderObserver()
{

}

void RenderObserver::Update(int iID /* = -1 */)
{
    iID;
    //get cache according ID
    if (mDisplayCache == nullptr) {
        printf("\ndisplay cache is empty");
        return;
    }

    //get size & render result buffer
    int width = 0;
    int height = 0;
    unsigned char* buffer;
    if (!mDisplayCache->GetBuffer(width, height, buffer)){
        printf("\nFailed to get buffer from display cache");
        return;
    }

//#ifdef _DEBUG
//    char fileName[256];
//    sprintf_s(fileName, "d:\\temp\\RenderObserver_%d.bmp", iID);
//    SaveToTrueColorBitmap(fileName, buffer, height, width);
//#endif

    if (mResponser != nullptr) {
        mResponser->Response(width, height, buffer);
    }
}

void RenderObserver::AttachResponser(const WindowResponserPtr& responser)
{
    mResponser = responser;
}

bool RenderObserver::SaveToTrueColorBitmap(const std::string& path, 
    unsigned char* buffer,
    const int& height,
    const int& width)
{
#ifdef WIN32
        //todo: check whether the file is existed

        // calculate the pad size of whole image
        int nStride = (width*32+7)/8;
        int paddedsize = nStride*height;

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
        info.biWidth = width;
        info.biHeight = height;
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
        std::wstring wsPath(path.begin(), path.end());
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
        if ( WriteFile ( file, buffer, paddedsize, &bwritten, NULL ) == false )
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