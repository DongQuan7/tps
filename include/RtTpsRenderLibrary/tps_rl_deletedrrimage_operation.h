////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xiang.Liang xiang.liang@united-imaging.com
/// 
///  \file tps_rl_easyplan_seleteimage_operation.h
/// 
///  \brief class TpsDeleteDrrImageOperation
/// 
///  \version 1.0
/// 
///  \date    2017/7/30
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DELETEDRRIMAGEOPERATION_H_
#define TPS_RL_DELETEDRRIMAGEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsDeleteDrrImageOperation
    : public OperationBase {
public:
    TpsDeleteDrrImageOperation(const std::string &beamUID, const std::string &imageUID);
    ~TpsDeleteDrrImageOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                    mBeamUID;
    std::string                    mImageUID;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDeleteDrrImageOperation);
};
TPS_END_NAMESPACE

#endif