////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_addbeamoperation.h
/// 
///  \brief class TpsAddBeamOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/28
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ADDBEAMOPERATION_H_
#define TPS_RL_ADDBEAMOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsAddBeamOperation
/// 
/// \brief this operation convert tps beam to beam graphic object and add in Model
class TPS_RL_DECLSPEC TpsAddBeamOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsAddBeamOperation(const std::string& planUID, const std::string& beamUID);
    ~TpsAddBeamOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string         mNormGroupUID;
    std::string         mBeamUID;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsAddBeamOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif