////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \Author Cheng Yilong yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplan_modifybeam_operation.h
/// 
///  \brief class TpsEasyPlanModifyBeamOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/06
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMODIFY_BEAM_OPERATION_H_
#define TPS_RL_EASYPLANMODIFY_BEAM_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "McsfMedViewer3DArithmetic/point3d.h"
#include "Mcsf3DArithmetic\mcsf_3d_point2f.h"
TPS_BEGIN_NAMESPACE
struct TpsCertifiedBeam;
/// \class TpsEasyPlanModifyBeamOperation
/// 
/// \brief this operation modify beam
class TPS_RL_DECLSPEC TpsEasyPlanModifyBeamOperation : public OperationBase {
public:
    //the constructor and destructor
    TpsEasyPlanModifyBeamOperation(const std::string &beamUID);
    ~TpsEasyPlanModifyBeamOperation();
    virtual bool ModifyGraphicObject() const;
    void SetDrrImageUID(const std::string &drrImageUID);
    std::string GetDrrImageUID() const;
private:
    std::string                         mBeamUID;
    std::string                         mDrrImageUID;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanModifyBeamOperation);
};

TPS_END_NAMESPACE

#endif
