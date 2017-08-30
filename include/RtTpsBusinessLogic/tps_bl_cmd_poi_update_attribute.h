////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author Wenqiao.Jiang  Wenqiao.Jiang@united-imaging.com
/// 
///  \file tps_cmd_poi_update.h
/// 
///  \brief
/// 
///  \version 1.0
/// 
///  \date    2015/10/21
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_POI_UPDATE_ATTRIBUTE_H_
#define TPS_BL_CMD_POI_UPDATE_ATTRIBUTE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certified_poi.h"

TPS_BEGIN_NAMESPACE

class RtPoi;

class TPS_BL_DECLSPEC PoiUpdateAttributeCmd : public TpsCommand {

public:
	PoiUpdateAttributeCmd(const CertifiedPoi& poi,bool isUpdatePoiDose);

	virtual ~PoiUpdateAttributeCmd();

    void GetEditPoi(const RtPoi*& pPoi);
protected:
	virtual int Execute();

private:
	CertifiedPoi               mCertifiedPoi;
	std::string                m_strCommandName;
    bool                       m_isUpdatePoiDose;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(PoiUpdateAttributeCmd);
};

TPS_END_NAMESPACE
#endif