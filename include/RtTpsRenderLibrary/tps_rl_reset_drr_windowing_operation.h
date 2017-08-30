////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_reset_drr_windowing.h
/// 
///  \brief class ResetDrrWindowing declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_RESET_DRR_WINDOWING_H_
#define TPS_RL_RESET_DRR_WINDOWING_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class ResetDrrWindowingOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC ResetDrrWindowingOperation
 : public OperationBase{
public:
    //the constructor and destructor
    ResetDrrWindowingOperation(const std::string& imageUID);
    virtual ~ResetDrrWindowingOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     mImageUID;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ResetDrrWindowingOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif