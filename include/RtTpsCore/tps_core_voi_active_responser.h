////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_core_voi_active_responser.h
/// 
///  \brief class VOIActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/12
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_VOI_ACTIVE_RESPONSER_H_
#define TPS_CORE_VOI_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

namespace tps
{
    /// \class VOIActiveResponserImpl
    /// 
    /// \brief *****
    class VOIActiveResponserImpl : public VOIActiveResponser
    {
    public:
        //the constructor and destructor
        VOIActiveResponserImpl();
        virtual ~VOIActiveResponserImpl();

        virtual void SetHandle(void* pHandle);
        virtual void Response(const CertifiedVOI& certVoi);

    protected:
        virtual void FreeHandle_i();
    };
}

#endif