////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_hotspots_operation.h
/// 
///  \brief class TpsHotSpotsOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/10/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_HOT_SPOTS_OPERATION_H_
#define TPS_RL_HOT_SPOTS_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsHotSpotsOperation
/// 
/// \brief this operation draws beam outline
class TPS_RL_DECLSPEC TpsHotSpotsOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsHotSpotsOperation(const std::string& planUID, const std::string& beamUID, float fDoseLimit, int iHotFlag);
    ~TpsHotSpotsOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string m_strPlanUID;
    std::string m_strVoiUID;
    int         m_iHotFlag;
    float       m_fDoseLimit;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsHotSpotsOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_HOT_SPOTS_OPERATION_H_
