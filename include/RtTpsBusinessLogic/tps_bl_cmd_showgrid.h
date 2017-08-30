//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  xiaowei.li mailto:xiaowei.li@united-imaging.com
///
///  \file    tps_bl_cmd_showgrid.h
///  \brief   show dose grid command
///
///  \version 1.0
///  \date    Dec. 14, 2016
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_SHOWGRIDCMD_H
#define TPS_BL_SHOWGRIDCMD_H

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedgridsize.h"

TPS_BEGIN_NAMESPACE


class TPS_BL_DECLSPEC TpsShowGridCMD : public TpsCommand{
public:
    TpsShowGridCMD(CertifiedGridSize *pocoGridSize);
    virtual ~TpsShowGridCMD(void);
protected:
    virtual int Execute();
private:
    CertifiedGridSize *mPocoGridSize;
};

TPS_END_NAMESPACE

#endif //TPS_BL_SHOWGRIDCMD_H