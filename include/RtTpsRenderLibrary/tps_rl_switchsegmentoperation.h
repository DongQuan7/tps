////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_switchsegmentoperation.h
/// 
///  \brief class TpsSwithSegmentOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/15
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SWITCHSEGMENTOPERATION_H_
#define TPS_RL_SWITCHSEGMENTOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsSwithSegmentOperation
    : public OperationBase {
public:
    TpsSwithSegmentOperation(const std::string& beamUid,int page);

    ~TpsSwithSegmentOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string               mBeamUid;
    int                       mPage;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSwithSegmentOperation);



};

TPS_END_NAMESPACE

#endif