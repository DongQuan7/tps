////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_removetableoperation.h
/// 
///  \brief class RemoveTableOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/04
////////////////////////////////////////////////////////////////

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC RemoveTableOperation :
    public OperationBase {
public:
    RemoveTableOperation(bool isVisible , float height, std::string seriesUid);
    ~RemoveTableOperation();

    virtual bool ModifyGraphicObject() const;

private:
    bool        mIsVisible;
    float       mHeight;
    std::string mSeriesUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(RemoveTableOperation);
};


TPS_END_NAMESPACE