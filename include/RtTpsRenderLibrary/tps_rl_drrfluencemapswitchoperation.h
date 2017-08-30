////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_drrfluencemapswitchoperation.h
/// 
///  \brief class DRRFluenceMapSwitch 
/// 
///  \version 1.0
/// 
///  \date    2014/07/21
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DRRFLUENCEMAPSWITCH_H_
#define TPS_RL_DRRFLUENCEMAPSWITCH_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE
class ITpsDataRepository;


class TPS_RL_DECLSPEC DRRFluenceMapSwitch 
    : public OperationBase {
public:
    DRRFluenceMapSwitch(const std::string& imageUID, bool flag);

    ~DRRFluenceMapSwitch();

    virtual bool ModifyGraphicObject() const;

	//void SetDataRepository(ITpsDataRepository* dataRepository);

private:
    bool                mFlag;
    std::string         mImageUID;
private:
	ITpsDataRepository*         mDataRepository;


private:
    TPS_DISALLOW_COPY_AND_ASSIGN(DRRFluenceMapSwitch);
};

TPS_END_NAMESPACE
#endif