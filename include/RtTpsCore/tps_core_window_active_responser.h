////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_core_window_active_responser.h
/// 
///  \brief class WindowActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/17
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_WINDOW_ACTIVE_RESPONSER_H_
#define TPS_CORE_WINDOW_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class WindowActiveResponser
/// 
/// \brief active update rendering buffer
class WindowActiveResponserImpl : public WindowActiveResponser
{
public:
    //the constructor and destructor
    WindowActiveResponserImpl();
    ~WindowActiveResponserImpl();

    virtual void SetHandle(void* pHandle);
    virtual void Response(int width, int height, void* buffer);

protected:
    virtual void FreeHandle_i();
};

TPS_END_NAMESPACE  // end namespace tps

#endif