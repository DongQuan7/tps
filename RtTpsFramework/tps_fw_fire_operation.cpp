////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_fire_operation.cpp
/// 
///  \brief class TpsFireOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsFireOperation::TpsFireOperation(bool bForce)
:mbForce(bForce)
{

}

TpsFireOperation::~TpsFireOperation(){

}

bool TpsFireOperation::ModifyGraphicObject() const{
    if (mbForce) {
        mModelWarehouse->SetAllGODirty();
    }

    TPS_LOG_DEV_INFO<<"New rendering request commited.";

    mModelWarehouse->Notify();
    mModelWarehouse->ClearDirtyFlag();
    return true;
}
TPS_END_NAMESPACE  // end namespace tps