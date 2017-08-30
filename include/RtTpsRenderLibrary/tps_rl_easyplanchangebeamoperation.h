////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \Author Cheng Yilong yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplanchangebeamoperation.h
/// 
///  \brief class TpsEasyPlanChangeBeamOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLAN_CHANGE_BEAM_OPERATION_H_
#define TPS_RL_EASYPLAN_CHANGE_BEAM_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

//class TpsSegmentGraphicObj;
/// \class TpsEasyPlanChangeBeamOperation
/// 
/// \brief this operation reset mlc shape
class TPS_RL_DECLSPEC TpsEasyPlanChangeBeamOperation : public OperationBase{
public:
    //the constructor and destructor
    TpsEasyPlanChangeBeamOperation(const std::string &planUid,const std::string &normGroupUid,const std::string &beamUid);
    ~TpsEasyPlanChangeBeamOperation();

    virtual bool ModifyGraphicObject() const;
private:
    std::string                  m_BeamUid;
	std::string                  m_NormgroupUid;
	std::string                  m_PlanUid;
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanChangeBeamOperation);
};

TPS_END_NAMESPACE

#endif