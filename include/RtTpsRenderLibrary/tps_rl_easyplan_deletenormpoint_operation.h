////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_deletenormpoint_operation.h 
/// 
///  \brief delete 
/// 
///  \version 1.0
/// 
///  \date    2015/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANDELETENORMPOINTOPERATION_H_
#define TPS_RL_EASYPLANDELETENORMPOINTOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanDeleteNormPointOperation
    : public OperationBase {
public:
    TpsEasyPlanDeleteNormPointOperation(const std::string &strImageUid, const std::string& strPlanUid, const std::string &strPoiUid);

    ~TpsEasyPlanDeleteNormPointOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   m_strPoiUid;
    std::string   m_strPlanUid;
    std::string   m_strImageUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanDeleteNormPointOperation);
};


TPS_END_NAMESPACE

#endif