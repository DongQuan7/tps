////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_switch_bev_dispay_mode_operation.h
/// 
///  \brief class SwitchBevDisplayMode declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SWITCH_BEV_DISPAY_MODE_OPERATION_H_
#define TPS_RL_SWITCH_BEV_DISPAY_MODE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class SwitchBevDisplayModeOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC SwitchBevDisplayModeOperation
 : public OperationBase{
public:
    //the constructor and destructor
    SwitchBevDisplayModeOperation(int bevDisplayMode);
    virtual ~SwitchBevDisplayModeOperation();

    virtual bool ModifyGraphicObject() const;

private:
    int mBevDisplayMode;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SwitchBevDisplayModeOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif