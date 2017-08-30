////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_updatefusionregionoperation.h
/// 
///  \brief class UpdateFusionRegion declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEFUSIONREGIONOPERATION_H_
#define TPS_RL_UPDATEFUSIONREGIONOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class UpdateFusionRegionOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC UpdateFusionRegionOperation
 : public OperationBase{
public:
    //the constructor and destructor
    UpdateFusionRegionOperation(FusionRegionUpdateType refreshType = AllRegion);
    virtual ~UpdateFusionRegionOperation();

    virtual bool ModifyGraphicObject() const;

private:
    FusionRegionUpdateType mRefreshType;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateFusionRegionOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif