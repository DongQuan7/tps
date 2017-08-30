////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_displaydelegate.cpp
/// 
///  \brief class TpsDisplayDelegate implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/11
////////////////////////////////////////////////////////////////
#include"StdAfx.h"

#include "RtTpsRenderController/tps_rc_displaydelegate.h"

#include "RtTpsRenderController/tps_rc_layoutmanager.h"
#include "RtTpsRenderController/tps_rc_displaycache.h"
#include "RtTpsRenderController/tps_rc_safelock.h"

#include "tps_logger.h"
#include "RtTpsFramework/tps_fw_commproxy.h"
#include "Common\McsfSystemEnvironmentConfig\mcsf_systemenvironment_factory.h"
#include "McsfMedViewer/mcsf_med_viewer_controller_interface.h"
#include "McsfMedViewer/mcsf_med_viewer_controller_factory.h"
#include "McsfMedViewer/mcsf_med_viewer_image_data.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsDisplayDelegate::TpsDisplayDelegate() : mInitialized(false) {

}

TpsDisplayDelegate::~TpsDisplayDelegate(){

}

bool TpsDisplayDelegate::Initialize(TpsLayoutManager* layoutManager){
    if (mInitialized){
        TPS_LOG_DEV_INFO<<"[INFO][TpsDisplayDelegate][Initialize]: "
            <<"TpsDisplayDelegate object has already been initialized.";
        return true;
    }

    try{
        if (layoutManager == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Initialize]: "
                <<"The input layout manager is empty.";
            return false;
        }
        mLayoutManager = layoutManager;

        //todo, how to check communication proxy
        mContaineeName = TpsCommProxy::GetInstance()->GetName();

        Mcsf::ISystemEnvironmentConfig* systemEnviromentConfig = 
            Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
        if (systemEnviromentConfig == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Initialize]: "
                <<"Failed to get system environment.";
            return false;
        }
        std::string cfgPath = 
            systemEnviromentConfig->GetApplicationPath("RtTpsConfigPath");
        mViewControlConfigPath.assign(cfgPath.begin(), cfgPath.end());

        mViewerControllerFatory = Mcsf::MedViewerControllerFactory::Instance();
        if (mViewerControllerFatory == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Initialize]: "
                <<"Failed to get MedViewerControllerFactory instance.";
            return false;
        }

        mInitialized = true;
        return true;
    }
    catch (const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsDisplayDelegate][Initialize]: "<<ex.what();
        return false;
    }
}

bool TpsDisplayDelegate::RegisterDisplayCache(
    WINDOW_TYPE type, TpsDisplayCache* cache){
    try
    {
        if (!mInitialized){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][RegisterDisplayCache]: "
                <<"TpsDisplayDelegate has not been initialized.";
            return false;
        }

        if (cache == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][RegisterDisplayCashe]: "
                <<"TpsDisplayCashe to be registered is empty.";
            return false;
        }

        DISPLAYCACHE_UID uid = cache->CasheID();
        mDisplayCacheMap[uid] = cache;
        auto thisPtr = this->shared_from_this();
        cache->AttachObserver(thisPtr);

        mLayoutManager->RegisterView(uid, type);
        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<"[EEXCEPTION][TpsDisplayDelegate][RegisterDisplayCashe]: "
            <<ex.what();
        return false;
    }
}

void TpsDisplayDelegate::Update(int iID /*= -1*/){
    try{
        if (!mInitialized){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Update]: "
                <<"TpsDisplayDelegate has not been initialized.";
            return;
        }
        //get cache according ID
        auto it = mDisplayCacheMap.find(iID);
        if (it == mDisplayCacheMap.end()){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Update]: "
                <<"Failed to find TpsDisplayCache object with ID: "
                <<iID<<" in cache map.";
            return;
        }
        //get size & render result buffer
        int width, height;
        unsigned char* buffer;
        auto cache = it->second;
         if (!cache->GetBuffer(width, height, buffer)){
             TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Update]: "
                 <<"Failed to get buffer from TpsDisplayCache with ID: "
                 <<iID;
             return;
         }

        //get layout information
        LAYOUT_UNIT layout;
        {
            TpsSafeLock<TpsLayoutManager> lock(mLayoutManager);
            if (!mLayoutManager->GetLayoutUnit(iID, layout)){
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Update]: "
                    <<"Failed to get layout unit with ID: "<<iID;
                return;
            }
        }
        //get viewer control
        auto viewController = mViewerControllerFatory->GetController(
            mContaineeName, mViewControlConfigPath, layout.viewerControlID);
        if (viewController == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Update]: "
                <<"Failed to get MedViewerController instance with ID: "
                <<layout.viewerControlID;
            return;
        }
        //get cell image data
        auto imageData = viewController->GetImageData(layout.cellID, 0);
        bool isResized = cache->IsResized();
        if (imageData == nullptr || isResized){
            TPS_LOG_DEV_INFO<<"[INFO][TpsDisplayDelegate][Update]: "
                <<"Create new CellImageBuffer of ViewControl ID: "
                <<layout.viewerControlID << ", Cell ID: "<<layout.cellID;
            void* sharedMemory;
            viewController->CreateCellImageBuffer(width, height, RGBA32, 
                layout.cellID, nullptr, sharedMemory);
            imageData = viewController->GetImageData(layout.cellID, 0);
        }

        // Get share memory length of ViewerControl
        unsigned int shareMemLen = imageData->GetPixelDataLen();
        // Actual image size
        unsigned int imageSize = width * height * 4;
        // check if we need to truncate the image ( defensive programming)
        if (imageSize > shareMemLen){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Update]: "
                <<"The rendered image extends the screen size of ViewControl ID: "
                <<layout.viewerControlID << ", Cell ID: "<<layout.cellID;
            return;
        }
        imageData->WritePixel(buffer, 0, imageSize);
        viewController->UpdatePixelData(layout.cellID, width, height);

#ifdef _DEBUG
        char fileName[256];
        sprintf_s(fileName, "d:\\temp\\fbo_result%d_in_delegate.bmp", iID);
        SaveToTrueColorBitmap_i(fileName, buffer, height, width);
#endif

    }
    catch (const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsDisplayDelegate][Update]: "<<ex.what();
    }
}

bool TpsDisplayDelegate::SaveToTrueColorBitmap_i(const std::string& sPath, 
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