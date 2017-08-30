////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatevolumeoperation.h
/// 
///  \brief class UpdateVolumeOperation
/// 
///  \version 1.0
/// 
///  \date    2014/7/04
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEVOLUMEOPERATION_H_
#define TPS_RL_UPDATEVOLUMEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC UpdateVolumeOperation
    : public OperationBase {
public:
    UpdateVolumeOperation(const std::string &imageUid);
    ~UpdateVolumeOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                  mImageUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateVolumeOperation);
};
TPS_END_NAMESPACE

#endif