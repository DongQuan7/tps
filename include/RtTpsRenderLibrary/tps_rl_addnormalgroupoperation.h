////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_addnormalgroupoperation.h
/// 
///  \brief class AddNormalGroupOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ADDNORMALGROUPOPERATION_H_
#define TPS_RL_ADDNORMALGROUPOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class AddNormalGroupOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC AddNormalGroupOperation
    : public OperationBase{
public:
    //the constructor and destructor
    AddNormalGroupOperation(const std::string& planUID, const std::string& normGroupUID);
    virtual ~AddNormalGroupOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     mPlanUID;
    std::string     mNormGroupUID;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(AddNormalGroupOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
