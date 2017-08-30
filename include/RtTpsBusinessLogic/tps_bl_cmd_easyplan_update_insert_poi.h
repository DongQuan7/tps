//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \brief   delete image
///
///  \version 1.0
///  \date  2015/10/30  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_UPDATE_INSERT_POI_H
#define TPS_BL_CMD_EASYPLAN_UPDATE_INSERT_POI_H

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certified_poi.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsSendPoiInfoEasyPlanCmd : public TpsCommand {
public:
    TpsSendPoiInfoEasyPlanCmd(const CertifiedPoi &poi);
    virtual ~TpsSendPoiInfoEasyPlanCmd();

protected:
    virtual int Execute();

private:
    CertifiedPoi mCertPoi;
};

TPS_END_NAMESPACE

#endif