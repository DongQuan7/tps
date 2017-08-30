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
#ifndef TPS_FW_RENDEROBSERVER_H_
#define TPS_FW_RENDEROBSERVER_H_

#include "RtTpsFramework/tps_fw_defs.h"

#include "McsfMedViewer3D/McsfMedViewer3DCommon/mcsf_viewer3d_observer_interface.h"
#include "McsfMedViewer3D/McsfMedViewer3DCommon/mcsf_viewer3d_subject.h"

TPS_BEGIN_NAMESPACE
class TpsDisplayCache;
class TpsRenderControllerBase;
class WindowActiveResponser;
typedef std::shared_ptr<WindowActiveResponser> WindowResponserPtr;

//定义一个函数指针，返回的是24位的位图
//typedef void(__stdcall *RenderBufferReady)(int bufferWidth, int bufferHeight,
//    void* pixelBuffer);

class TPS_FW_DECLSPEC RenderObserver : public Mcsf::MedViewer3D::IObserver
{
public:
    RenderObserver();
    virtual ~RenderObserver();

    // observer interface
    // \brief  clear old buffer & copy new buffer from render unit
    virtual void Update(int iID = -1);

    void AttachResponser(const WindowResponserPtr& responser);

    void SetDisplayCache(tps::TpsDisplayCache* pCache)
    {
        mDisplayCache = pCache;
    }

private:
    bool SaveToTrueColorBitmap(const std::string& path, unsigned char* buffer,
        const int& height, const int& width);

private:
    WindowResponserPtr    mResponser;
    tps::TpsDisplayCache* mDisplayCache;
};

TPS_END_NAMESPACE  // end namespace tps

#endif