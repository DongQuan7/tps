//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_ezplan_set_isocenter.h
///  \brief   
///
///  \version 1.0
///  \date  2015/10/29  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_CMD_EZPLAN_SETISOCENTER_H_
#define TPS_CMD_EZPLAN_SETISOCENTER_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certified_poi.h"

TPS_BEGIN_NAMESPACE

struct CertifiedPoi;

class TPS_BL_DECLSPEC EzPlanSetIsoCenterCmd : public TpsCommand
{
public:
	EzPlanSetIsoCenterCmd(const std::string& sWndUid, int iWndType, 
		int nAction, float fX, float fY, const CertifiedPoi& tpsPoi);
	~EzPlanSetIsoCenterCmd();

	CertifiedPoi GetCertifiedPoi();

protected:
	virtual int Execute();

private:
	std::string                 m_sWndUid;
	int                         m_iWndType;
	int                         m_nAction;
	float                       m_fX;
	float                       m_fY;
	CertifiedPoi                m_stcCertifiedPoi;
private:
	TPS_DISALLOW_COPY_AND_ASSIGN(EzPlanSetIsoCenterCmd);
};

TPS_END_NAMESPACE

#endif