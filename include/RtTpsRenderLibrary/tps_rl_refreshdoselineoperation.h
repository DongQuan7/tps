////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_rl_refreshdoselineoperation.h
/// 
///  \brief class TpsRereshDoseLineOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/05
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATETPSDOSELINEOPERATION_H_
#define TPS_RL_UPDATETPSDOSELINEOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"


TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsRefreshDoseLineOperation
        : public OperationBase{
public:
    //the constructor and destructor
    TpsRefreshDoseLineOperation(const std::string& planUid);
    ~TpsRefreshDoseLineOperation();

    virtual bool ModifyGraphicObject() const;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRefreshDoseLineOperation);

    std::string     mPlanUID;
};

TPS_END_NAMESPACE  // end namespace tps

#endif