//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_calculatedose.h 
///  \brief  calculate dose in easy plan
///
///  \version 1.0
///  \date  2015/11/03  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_CALCULATEDOSE_H_
#define TPS_BL_CMD_EASYPLAN_CALCULATEDOSE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE
class RtBeam;
class RtPoi;
class TPS_BL_DECLSPEC EzDoseCalculateCmd : public TpsCommand
{
public:
	EzDoseCalculateCmd(std::string &planUID, std::string &currentBeamUID);
	~EzDoseCalculateCmd();
	void GetBeams(std::vector<RtBeam*> &beams);
	void GetPoints(std::vector<RtPoi*> &points);

protected:
	virtual int Execute();

private:
	std::string							mPlanUID;
	std::string							mCurrentBeamUID;
	std::vector<RtBeam*>				mBeams;
	std::vector<RtPoi*>					mPoints;
private:
	TPS_DISALLOW_COPY_AND_ASSIGN(EzDoseCalculateCmd);
};

TPS_END_NAMESPACE
#endif