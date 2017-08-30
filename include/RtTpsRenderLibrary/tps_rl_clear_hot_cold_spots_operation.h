////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file tps_rl_clear_hot_cold_spots_operation.h
/// 
///  \brief class TpsHotSpotsOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/05/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CLEAR_HOT_COLD_SPOTS_OPERATION_H_
#define TPS_RL_CLEAR_HOT_COLD_SPOTS_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsClearHotColdSpotsOperation
/// 
/// \brief this operation clear hot/cold spots
class TPS_RL_DECLSPEC TpsClearHotColdSpotsOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsClearHotColdSpotsOperation(const std::string& planUID);
    ~TpsClearHotColdSpotsOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string                   m_strPlanUID;
   

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsClearHotColdSpotsOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_CLEAR_HOT_COLD_SPOTS_OPERATION_H_
