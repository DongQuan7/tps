////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_remove_registration_relationship_operation.h
/// 
///  \brief class RemoveRegistrationRelationship declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/03/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_REMOVE_REGISTRATION_RELATIONSHIP_OPERATION_H_
#define TPS_RL_REMOVE_REGISTRATION_RELATIONSHIP_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class RemoveRegistrationRelationshipOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC RemoveRegistrationRelationshipOperation
 : public OperationBase{
public:
    //the constructor and destructor
    RemoveRegistrationRelationshipOperation(const std::string& fixedUid, 
        const std::string& floatUid);
    virtual ~RemoveRegistrationRelationshipOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mFixedUid;
    std::string mFloatUid;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RemoveRegistrationRelationshipOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif