////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_remove_registration_relationship_operation.cpp
/// 
///  \brief class RemoveRegistrationRelationship implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/03/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_remove_registration_relationship_operation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

RemoveRegistrationRelationshipOperation::RemoveRegistrationRelationshipOperation(
    const std::string& fixedUid, const std::string& floatUid) : 
    mFixedUid(fixedUid), mFloatUid(floatUid)
{

}

RemoveRegistrationRelationshipOperation::~RemoveRegistrationRelationshipOperation()
{

}

bool RemoveRegistrationRelationshipOperation::ModifyGraphicObject() const
{
    if (mFixedUid.empty() || mFloatUid.empty()){
        TPS_LOG_DEV_ERROR<<"The Image UID or VOI UID is empty.";
        return false;
    }

    GO_TYPE goType;
    std::string keyPrefix = mFixedUid + "|" + mFloatUid + "|";
    FIRST_POSTFIX_COMPONENT sections[3] = { AXIAL, CORONAL, SAGITTAL };
    for (int i = 0; i < 3; ++i){
        goType = GOTypeHelper::ComposeAsGOType(FUSION_CHESS, sections[i]);
        mModelWarehouse->RemoveModelObject(keyPrefix + GOTypeHelper::ToString(goType));
    }
    mModelWarehouse->RemoveModelObject(keyPrefix + GOTypeHelper::ToString(GO_TYPE_FUSION));
    return true;
}

TPS_END_NAMESPACE  // end namespace tps