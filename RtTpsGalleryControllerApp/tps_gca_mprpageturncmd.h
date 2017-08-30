//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mprpageturncmd.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_MPRPAGETURNCMD_H_
#define TPS_GCA_MPRPAGETURNCMD_H_

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsRenderController/tps_rc_renderdefines.h"

TPS_BEGIN_NAMESPACE

class TpsRenderProxyBase;

class TpsGcaMPRPageturnCmd : public TpsCommand
{
public:
    TpsGcaMPRPageturnCmd(LAYOUT_UNIT unit, int iPageturn);

    virtual ~TpsGcaMPRPageturnCmd(void);

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRPageturnCmd * Clone();

private:
    LAYOUT_UNIT                 mUnit;
    int                         mPageturn;
};



TPS_END_NAMESPACE

#endif //TPS_GCA_MPRPAGETURNCMD_H_
