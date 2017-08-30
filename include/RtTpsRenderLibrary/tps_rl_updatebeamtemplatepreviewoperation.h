////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \file tps_rl_updatebeamtemplatepreviewoperation.h
/// 
///  \brief class UpdateBeamTemplatePreviewOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/12/22
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEBEAMTEMPLATEPREVIEW_OPERATION_H_
#define TPS_RL_UPDATEBEAMTEMPLATEPREVIEW_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsFramework/tps_fw_beamstructure.h"

TPS_BEGIN_NAMESPACE

/// \class UpdateBeamTemplatePreviewOperation
/// 
/// \brief this operation update each beam's preview from beamtemplate.
class TPS_RL_DECLSPEC UpdateBeamTemplatePreviewOperation
    : public OperationBase{
public:
    UpdateBeamTemplatePreviewOperation(std::string beamTemplateUid, std::string normGroupUid, std::string isocenterUid);
    ~UpdateBeamTemplatePreviewOperation();

    virtual bool ModifyGraphicObject() const;

private:

    std::string mBeamTemplateUid;
    std::string mNormGroupUid;
    std::string mIsocenterUid;

    int GetPatientPosition_i(std::string sPP) const;

    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateBeamTemplatePreviewOperation);
};

TPS_END_NAMESPACE

#endif