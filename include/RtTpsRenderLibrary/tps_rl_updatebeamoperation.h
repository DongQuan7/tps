////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_updatebeamoperation.h
/// 
///  \brief class TpsUpdateBeamOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/02
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEBEAMOPERATION_H_
#define TPS_RL_UPDATEBEAMOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

/// \class TpsUpdateBeamOperation
/// 
/// \brief this operation update a existing according to beam in beam manager.
class TPS_RL_DECLSPEC TpsUpdateBeamOperation
    : public OperationBase{
public:
    TpsUpdateBeamOperation(const std::string& ngUID, const std::string& beamUID);
    ~TpsUpdateBeamOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string         mNgUid;
    std::string         mBeamUID;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateBeamOperation);
};

TPS_END_NAMESPACE

#endif