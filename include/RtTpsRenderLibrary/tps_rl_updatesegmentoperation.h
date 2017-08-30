////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Chen Xi  xi.chen@united-imaging.com
/// 
///  \file tps_rl_updatesegmentoperation.h
/// 
///  \brief class TpsUpdateSegmentOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/10/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATESEGMENTOPERATION_H_
#define TPS_RL_UPDATESEGMENTOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

/// \class TpsUpdateBeamOperation
/// 
/// \brief this operation update a existing according to beam in beam manager.
class TPS_RL_DECLSPEC TpsUpdateSegmentOperation
	: public OperationBase{
public:
	TpsUpdateSegmentOperation(const std::string& normGroupUID, const std::string& beamUID);
	~TpsUpdateSegmentOperation();

	virtual bool ModifyGraphicObject() const;

private:
	std::string         mNormGroupUID;
	std::string         mBeamUID;
private:
	TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateSegmentOperation);
};

TPS_END_NAMESPACE

#endif // TPS_RL_UPDATESEGMENTOPERATION_H_