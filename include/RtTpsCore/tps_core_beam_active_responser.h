////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_core_beam_active_responser.h
/// 
///  \brief class BeamActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/26
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_BEAM_ACTIVE_RESPONSER_H_
#define TPS_CORE_BEAM_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE;  // begin namespace tps

/// \class BeamActiveResponserImpl
/// 
/// \brief 
class BeamActiveResponserImpl : public BeamActiveResponser
{
public:
    //the constructor and destructor
    BeamActiveResponserImpl();
    ~BeamActiveResponserImpl();

    virtual void SetHandle(void* pHandle);
    virtual void Response(RtBeam* beam);

protected:
    virtual void FreeHandle_i();
};

TPS_END_NAMESPACE  // end namespace tps

#endif