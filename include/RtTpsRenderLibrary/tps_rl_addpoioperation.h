////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_addpoioperation.h
/// 
///  \brief class TpsAddPoiOperation
/// 
///  \version 1.0
/// 
///  \date    2014/2/14
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ADDPOIOPERATION_H_
#define TPS_RL_ADDPOIOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsAddPoiOperation
    : public OperationBase 
{
public:
    TpsAddPoiOperation(const std::string &seriesUid, const std::string &poiUid);
    ~TpsAddPoiOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                  mSeriesUid;
    std::string                  mPoiUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsAddPoiOperation);
};
TPS_END_NAMESPACE

#endif