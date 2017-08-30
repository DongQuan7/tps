////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \Author Cheng Yilong yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplandeletebeamoperation.h
/// 
///  \brief class TpsEasyPlanDeleteBeamOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLAN_DELETE_BEAM_OPERATION_H_
#define TPS_RL_EASYPLAN_DELETE_BEAM_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

//class TpsSegmentGraphicObj;
/// \class TpsEasyPlanDeleteBeamOperation
/// 
/// \brief this operation reset mlc shape
class TPS_RL_DECLSPEC TpsEasyPlanDeleteBeamOperation : public OperationBase{
public:
    //the constructor and destructor
    TpsEasyPlanDeleteBeamOperation(std::string beamUid);
    ~TpsEasyPlanDeleteBeamOperation();

    virtual bool ModifyGraphicObject() const;
private:
    std::string                  m_BeamUid;
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanDeleteBeamOperation);
};

TPS_END_NAMESPACE

#endif