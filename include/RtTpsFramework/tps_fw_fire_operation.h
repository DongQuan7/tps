////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_fire_operation.h
/// 
///  \brief class TpsFireOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/20
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_FIRE_OPERATION_H_
#define TPS_FW_FIRE_OPERATION_H_

#include "RtTpsFramework/tps_fw_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsFireOperation
/// 
/// \brief this operation calls ModelWarehouse notify to fire the rendering
class TPS_FW_DECLSPEC TpsFireOperation : public OperationBase{
public:
    //the constructor and destructor
    TpsFireOperation(bool bForce=false);
    ~TpsFireOperation();

    virtual bool ModifyGraphicObject() const;

private:
    bool mbForce;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsFireOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif