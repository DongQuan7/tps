////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author Wenqiao.Jiang  Wenqiao.Jiang@united-imaging.com
/// 
///  \file tps_cmd_poi_jump.h
/// 
///  \brief
/// 
///  \version 1.0
/// 
///  \date    2015/10/21
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_POI_JUMP_H_
#define TPS_BL_CMD_POI_JUMP_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certified_poi.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC PoiJumpCmd : public TpsCommand {

public:
	PoiJumpCmd(const CertifiedPoi& poi);

	virtual ~PoiJumpCmd();
protected:
	virtual int Execute();

private:
	CertifiedPoi            mCertifiedPoi;
	std::string                 m_strCommandName;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(PoiJumpCmd);
};

TPS_END_NAMESPACE
#endif