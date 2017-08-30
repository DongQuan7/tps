////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_pageturnformovieoperation.h
/// 
///  \brief class movie page turn op
/// 
///  \version 1.0
/// 
///  \date    2014/6/23
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_PAGETURNFORMOVIEOPERATION_H_
#define TPS_RL_PAGETURNFORMOVIEOPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC MoviePageTurnOperation :
    public OperationBase {

public:
    MoviePageTurnOperation(const std::string &imageUid, int sliceNum);
    virtual ~MoviePageTurnOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   mImageUID;
    int           mSliceNum;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MoviePageTurnOperation);

};

TPS_END_NAMESPACE
#endif
