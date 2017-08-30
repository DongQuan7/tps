////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_displaycache.h
/// 
///  \brief class TpsDisplayCashe implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/03
////////////////////////////////////////////////////////////////
#include"StdAfx.h"

#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_cpurenderstrategy.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "tps_logger.h"

#include "McsfMedViewer3D/McsfMedViewer3DScene/mcsf_viewer3d_view.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

//int TpsDisplayCache::cacheID = -1;

TpsDisplayCache::TpsDisplayCache()
    :/*mID(++cacheID), */mID(-1), mBuffer(nullptr), mIsResized(false){
    mView.reset();
}

TpsDisplayCache::~TpsDisplayCache(){
    if (mBuffer){
        delete []mBuffer;
        mBuffer = nullptr;
    }
}

void TpsDisplayCache::SetRenderUnit(std::shared_ptr<Mcsf::MedViewer3D::ClsView> view){
    if (view == nullptr){
        TPS_LOG_DEV_WARNING<<"[WARNING][TpsDisplayCache][SetRenderUnit]: "
                           <<"The input ClsView is empty.";
    }

    mView = view;
}

void TpsDisplayCache::SetCacheID(WINDOW_TYPE window_type){
    mID = window_type;
}

DISPLAY_CACHE_UID TpsDisplayCache::GetCacheID() const{
    return mID;
}

bool TpsDisplayCache::GetBuffer(
    int& width, int& height, unsigned char*& buffer) const{

    if (mView.expired()){
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
        if (mView.expired()){
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
        auto view = mView.lock();
        int width, height;
        view->GetDisplaySize(width, height);
        mIsResized = (mWidth != width || mHeight != height);

        if (mIsResized || mBuffer == nullptr){
            if (mBuffer) delete []mBuffer;
            mBuffer = new unsigned char[width * height * 4];
        }

        view->GetRGBResult(mWidth, mHeight, mBuffer);

//#ifdef _DEBUG
//        char fileName[256];
//        sprintf_s(fileName, "d:\\temp\\fbo_result%d_in_cache.bmp", mID);
//        SaveToTrueColorBitmap(fileName, mBuffer, mHeight, mWidth);
//#endif

        //TODO: maybe need to revise this design in the future
        DoCPURendering_i(mBuffer, width, height);

        //reverse buffer
        int* pLine = new int[mWidth];
        for (int iLine = 0; iLine < mHeight / 2; ++iLine) {
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

bool TpsDisplayCache::RegisterCPURenderStrategy(const std::string& strCPURenderName,
    CPURenderStrategy* cpuRender) {
    if (nullptr == cpuRender) {
        TPS_LOG_DEV_ERROR<<"cpu render is nullptr.";
        return false;
    }

    CPURenderMap::iterator it = mmapCPURender.find(strCPURenderName);
    if (it != mmapCPURender.end()) {
        TPS_LOG_DEV_ERROR<<"cpu render["<<strCPURenderName<<"] already exist!";
        return false;
    }

    mmapCPURender[strCPURenderName] = cpuRender;

    return true;
}

void TpsDisplayCache::DoCPURendering_i(unsigned char*& buffer, int bufWidth, 
    int bufHeight) {
    if (nullptr == buffer || bufWidth<=0 || bufHeight<=0) {
        TPS_LOG_DEV_ERROR<<"buffer is null or bufWidth<=0 or bufHeight<=0.";
        return;
    }

    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    CPURenderMap::iterator it = mmapCPURender.begin();
    for (; it!=mmapCPURender.end(); ++it) {
        if (!it->second->Render(buffer, bufWidth, bufHeight)) {
            TPS_LOG_DEV_ERROR<<"CPURender ["<<it->first<<"] failed.";
        }
    }
}

bool TpsDisplayCache::IsResized(){
    return mIsResized;
}

// bool TpsDisplayCache::SaveToTrueColorBitmap(const std::string& path, 
//     unsigned char* buffer,
//     const int& height,
//     const int& width){
// 
// #ifdef WIN32
//         //todo: check whether the file is existed
// 
//         // calculate the pad size of whole image
//         int nStride = (width*32+7)/8;
//         int paddedsize = nStride*height;
// 
//         // declare bmp structures 
//         BITMAPFILEHEADER bmfh;
//         BITMAPINFOHEADER info;
// 
//         // andinitialize them to zero
//         memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
//         memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );
// 
//         // fill the fileheader with data
//         bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
//         bmfh.bfReserved1 = 0;
//         bmfh.bfReserved2 = 0;
//         bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
//         bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
// 
//         // fill the infoheader
// 
//         info.biSize = sizeof(BITMAPINFOHEADER);
//         info.biWidth = width;
//         info.biHeight = height;
//         info.biPlanes = 1;			// we only have one bitplane
//         info.biBitCount = 32;		// RGB mode is 24 bits
//         info.biCompression = BI_RGB;	
//         info.biSizeImage = 0;		// can be 0 for 24 bit images
//         info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
//         info.biYPelsPerMeter = 0x0ec4;     
//         info.biClrUsed = 0;			// we are in RGB mode and have no palette
//         info.biClrImportant = 0;    // all colors are important
// 
//         // now we open the file to write to
//         //#ifdef _DEBUG
//         //    HANDLE file = CreateFileW(sPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
//         //        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
//         //#else
//         std::wstring wsPath(path.begin(), path.end());
//         HANDLE file = CreateFile(wsPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
//             NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
//         //#endif
// 
//         if ( file == NULL )
//         {
//             CloseHandle ( file );
//             return false;
//         }
// 
//         // write file header
//         unsigned long bwritten;
//         if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
//         {	
//             CloseHandle ( file );
//             return false;
//         }
//         // write infoheader
//         if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
//         {	
//             CloseHandle ( file );
//             return false;
//         }
//         // write image data
//         if ( WriteFile ( file, buffer, paddedsize, &bwritten, NULL ) == false )
//         {	
//             CloseHandle ( file );
//             return false;
//         }
// 
//         // and close file
//         CloseHandle ( file );
// #else
//         sPath;
//         pPixelBuffer;
//         nImageWidth;
//         nImageHeight;
// #endif // _WINDOWS
// 
//         return true;
// }

void TpsDisplayCache::DetachAllObserver()
{
    auto itObserver = m_vecObserver.begin();
    for (; itObserver != m_vecObserver.end(); ++itObserver) {
        DetachObserver(*itObserver);
    }
}

void TpsDisplayCache::SetModelWarehouse(ModelWarehouse* model){
    mModelWarehouse = model;
}
TPS_END_NAMESPACE  // end namespace tps
