//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_update_beam.h 
///  \brief  update beam info in easy plan
///
///  \version 1.0
///  \date  2015/11/03  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_UPDATE_BEAM_H_
#define TPS_BL_CMD_EASYPLAN_UPDATE_BEAM_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"

TPS_BEGIN_NAMESPACE

class RtBeamsegment;
class RtBlock;

class TPS_BL_DECLSPEC UpdateEasyPlanBeamCmd : public TpsCommand
{
public:
	UpdateEasyPlanBeamCmd(std::string &easyPlanUID, TpsCertifiedBeam easyBeam);
	~UpdateEasyPlanBeamCmd();

protected:
	virtual int Execute();

private:
	bool UpdateSegment(TpsCertifiedBeam &beam, RtBeamsegment* segment);
    bool UpdateBlock( TpsCertifiedBeam &certBeam, RtBlock* block, int blockType);
private:
	TpsCertifiedBeam	mCertBeam;
	std::string			mEasyPlanUID;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(UpdateEasyPlanBeamCmd);
};

TPS_END_NAMESPACE


#endif