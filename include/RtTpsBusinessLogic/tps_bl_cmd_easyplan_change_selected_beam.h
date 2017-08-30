//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_change_selected_beam.h 
///  \brief  change select beam in easy plan
///
///  \version 1.0
///  \date  2015/11/03  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_CHANGE_SELECTED_BEAM_H_
#define TPS_BL_CMD_EASYPLAN_CHANGE_SELECTED_BEAM_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"

TPS_BEGIN_NAMESPACE
class TPS_BL_DECLSPEC TpsChangeSelectedBeamEasyPlanCmd : public TpsCommand
{
public:
    TpsChangeSelectedBeamEasyPlanCmd(int iWndType, 
        const std::string& sWndUid, TpsCertifiedBeam certBeam);
    ~TpsChangeSelectedBeamEasyPlanCmd();

protected:
    virtual int Execute();
private:
    TpsCertifiedBeam            mCertBeam;
    std::string                 m_sWndUid;
    int                         m_iWndType;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsChangeSelectedBeamEasyPlanCmd);
};
TPS_END_NAMESPACE

#endif