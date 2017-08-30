////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_core_prepare_active_responser.h
/// 
///  \brief class PrepareActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/04/19
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_PROGRESS_ACTIVE_RESPONSER_H_
#define TPS_CORE_PROGRESS_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE;  // begin namespace tps

/// \class PrepareActiveResponserImpl
/// 
/// \brief 
class PrepareActiveResponserImpl : public PrepareActiveResponser
{
public:
    //the constructor and destructor
    PrepareActiveResponserImpl();
    ~PrepareActiveResponserImpl();

    virtual void SetHandle(void* pHandle);
    virtual void Response(int totalFileNumber,int errortype);

protected:
    virtual void FreeHandle_i();
};

TPS_END_NAMESPACE  // end namespace tps

#endif