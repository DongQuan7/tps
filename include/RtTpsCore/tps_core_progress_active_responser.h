////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_core_progress_active_responser.h
/// 
///  \brief class BeamActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/04/19
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_PROGRESS_ACTIVE_RESPONSER_H_
#define TPS_CORE_PROGRESS_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE;  // begin namespace tps

/// \class ProgressActiveResponserImpl
/// 
/// \brief 
class ProgressActiveResponserImpl : public ProgressActiveResponser
{
public:
    //the constructor and destructor
    ProgressActiveResponserImpl();
    ~ProgressActiveResponserImpl();

    virtual void SetHandle(void* pHandle);
    virtual void Response(double value, bool isToCancel, const std::string& message);

protected:
    virtual void FreeHandle_i();
};

TPS_END_NAMESPACE  // end namespace tps

#endif