////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  xi.chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_highlightbeamoperation.h
/// 
///  \brief class HighlightBeamOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_HIGHLIGHTBEAMOPERATION_H_
#define TPS_RL_HIGHLIGHTBEAMOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsAddBeamOperation
/// 
/// \brief this operation convert tps beam to beam graphic object and add in Model
class TPS_RL_DECLSPEC HighlightBeamOperation
    : public OperationBase{
public:
    //the constructor and destructor
    HighlightBeamOperation(
        const std::string &beamUID, 
        const std::string &normGroupUID,
        const std::string &planUID,
        const std::string &imageUID);
    ~HighlightBeamOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     mBeamUID;
    std::string     mNormGroupUID;
    std::string     mPlanUID;
    std::string     mImageUID;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(HighlightBeamOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_HIGHLIGHTBEAMOPERATION_H_
