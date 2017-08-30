//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_printerbase.cpp
///
/// \brief   PrinterBase
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#include"stdafx.h"

#include "McsfMedViewer3D/McsfMedViewer3DScene/mcsf_viewer3d_view.h"

#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_rl_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_rc_cpurenderstrategy.h"
#include "RtTpsFramework/tps_rl_fire_operation.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"

#include "tps_prca_printerbase.h"

TPS_BEGIN_NAMESPACE

PrinterBase::PrinterBase(int width, int height)
    : mClsView(nullptr),
    mBuffer(nullptr),
    mIsInitialized(false),
    mWidth(width),
    mHeight(height) {

}

PrinterBase::~PrinterBase() {

    if (mBuffer != nullptr) {
        delete []mBuffer;
        mBuffer = nullptr;
    }
}

bool PrinterBase::Print() {

    if (!mIsInitialized) {
        if (!OnPreparePrinting()) {
            return false;
        }
    }

    if (!OnBeginPrinting()) {
        return false;
    }

    while (true)
    {
        PRINT_STATE state = OnPrinting();
        ModelWarehouse::GetInstance()->Synchronize();
        mClsView->Render(true);

        if (state == FINISH) {
            break;
        }
    }

    if (!OnEndPrinting()) {
        return false;
    }

    return true;
}

bool PrinterBase::AttachListener(boost::function<void (unsigned char*, int, int)> listener) {
    if (mListener.empty()) {
        mListener = listener;
        return true;
    }

    return false;
}

bool PrinterBase::OnPreparePrinting() {

    if (mWidth == 0 || mHeight == 0) {
        return false;
    }

    mClsView.reset(new Mcsf::MedViewer3D::ClsView(mWidth, mHeight, ModelWarehouse::GetInstance()->GetModelEntity()));
    if (!mClsView->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize ClsView.";
        return false;
    }

    ModelWarehouse::GetInstance()->AttachObserver(mClsView);

    mClsView->AttachObserver((Mcsf::MedViewer3D::IObserverPtr)this);

    if (!PreparePrinting()) {
        return false;
    }

    mIsInitialized = true;

    return true;
}

bool PrinterBase::PreparePrinting() {
    return true;
}

bool PrinterBase::OnBeginPrinting() {
    return true;
}

PRINT_STATE PrinterBase::OnPrinting() {
    return FINISH;
}

bool PrinterBase::OnEndPrinting() {
    return true;
}

void PrinterBase::Update(int iID/* = -1*/) {

    if (iID != 3){
        TPS_LOG_DEV_INFO<<"[INFO][TpsDisplayCache][Update]: "
            <<"Non-image render, return.";
        return;
    }

    int width, height;
    mClsView->GetDisplaySize(width, height);

    if (mBuffer != nullptr) {
        delete []mBuffer;
    }

    mBuffer = new unsigned char[width * height * 4];

    mClsView->GetRGBResult(width, height, mBuffer);

    DoCPURendering(mBuffer, width, height);

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

    if (!mListener.empty()) {
        mListener(mBuffer, width, height);
    }

#ifdef _DEBUG
    char fileName[256];
    sprintf_s(fileName, "d:\\temp\\fbo_result%d_in_cache.bmp", iID);
    SaveToTrueColorBitmap(fileName, mBuffer, mHeight, mWidth);
#endif

}

bool PrinterBase::CheckGraphicObject(const std::string& baseUID,
    GO_TYPE goType, 
    IGraphicObjectPtr& go,
    bool forceToUpdate/* = false*/) {

        std::string goTypeStr = GOTypeHelper::ToString(goType);
        std::string key = baseUID + "|" + goTypeStr;
        go = ModelWarehouse::GetInstance()->GetModelObject(key);
        if (go == nullptr || forceToUpdate) {
            TPS_LOG_DEV_INFO<<"[INFO][TpsMPRRenderController][CheckGraphicObject_i]: "
                <<"Graphic object with key: "<<key<<" does not exist, create new.";
            //get convert via graphic object type
            auto converter = TpsGOConverterFactory::GetInstance()->GetConverter(goType);
            if (converter == nullptr) {
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRRenderController][CheckGraphicObject_i]: "
                    <<"Failed to get graphic object converter which type is: "<<goType;
                return false;
            }
            //convert/update data
            if (go == nullptr) {
                go = converter->Convert(baseUID);
                if (go == nullptr) {
                    TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRRenderController][CheckGraphicObject_i]: "
                        <<"Failed to convert RT data to graphic object which key is: "<<baseUID;
                    return false;
                }

                ModelWarehouse::GetInstance()->AddModelObject(key, go);
            }
            else if (!converter->Update(baseUID, go)) {
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRRenderController][CheckGraphicObject_i]: "
                    <<"Failed to update RT data to graphic object which key is: "<<baseUID;
                return false;
            }
        }

        return true;
}

bool PrinterBase::RegisterCPURenderStrategy(const std::string& strCPURenderName,
    CPURenderStrategy* cpuRender) {
        if (nullptr == cpuRender) {
            TPS_LOG_DEV_ERROR<<"cpu render is nullptr.";
            return false;
        }

        CPURenderMap::iterator it = mCPURender.find(strCPURenderName);
        if (it != mCPURender.end()) {
            TPS_LOG_DEV_ERROR<<"cpu render["<<strCPURenderName<<"] already exist!";
            return false;
        }

        mCPURender[strCPURenderName] = cpuRender;

        return true;
}

void PrinterBase::DoCPURendering(unsigned char*& buffer, int bufWidth, 
    int bufHeight) {
        if (nullptr == buffer || bufWidth<=0 || bufHeight<=0) {
            TPS_LOG_DEV_ERROR<<"buffer is null or bufWidth<=0 or bufHeight<=0.";
            return;
        }

        CPURenderMap::iterator it = mCPURender.begin();
        for (; it!=mCPURender.end(); ++it) {
            if (!it->second->Render(buffer, bufWidth, bufHeight)) {
                TPS_LOG_DEV_ERROR<<"CPURender ["<<it->first<<"] failed.";
            }
        }
}

bool PrinterBase::SaveToTrueColorBitmap(const std::string& path, 
    unsigned char* buffer,
    const int& height,
    const int& width){

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
