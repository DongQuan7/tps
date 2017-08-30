////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatepoioperation.h
/// 
///  \brief class TpsUpdatePoiOperation
/// 
///  \version 1.0
/// 
///  \date    2014/3/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEPOIOPERATION_H_
#define TPS_RL_UPDATEPOIOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE
class TPS_RL_DECLSPEC TpsUpdatePoiOperation
    : public OperationBase {
public:
    TpsUpdatePoiOperation(
        const std::string &strSeriesUid,
        const std::string &strPoiUid);
    ~TpsUpdatePoiOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string             m_strSeriesUid;
    std::string             m_strPoiUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdatePoiOperation);
};

class TPS_RL_DECLSPEC TpsUpdateMaxPointOperation
    : public OperationBase {
public:
    TpsUpdateMaxPointOperation(
        const std::string& strPlanUid,
        const std::string& strPoiUid);
    ~TpsUpdateMaxPointOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string             m_strPlanUid;
    std::string             m_strPoiUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateMaxPointOperation);
};

TPS_END_NAMESPACE
#endif