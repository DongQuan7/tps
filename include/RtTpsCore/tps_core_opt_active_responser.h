////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_core_opt_active_responser.h
/// 
///  \brief class OptActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/17
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_OPT_ACTIVE_RESPONSER_H_
#define TPS_CORE_OPT_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class OptActiveResponserImpl
/// 
/// \brief 
class OptActiveResponserImpl : public OptActiveResponser
{
public:
    //the constructor and destructor
    OptActiveResponserImpl();
    ~OptActiveResponserImpl();

    virtual void SetHandle(void* pHandle);
    virtual void Response(const proto::RT_TPS_OptOutput& optOutput);

protected:
    virtual void FreeHandle_i();
};

TPS_END_NAMESPACE  // end namespace tps

#endif