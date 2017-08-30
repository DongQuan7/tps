//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_take_igrt.h 
///  \brief  receive igrt image from tda
///
///  \version 1.0
///  \date  2015/12/01  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_TAKE_IGRT_H_
#define TPS_BL_CMD_EASYPLAN_TAKE_IGRT_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class RtRtimage;

class TPS_BL_DECLSPEC TpsTakeIgrtCmd : public TpsCommand {
public:
    TpsTakeIgrtCmd(std::string planUID);
    ~TpsTakeIgrtCmd();
    void SetRtImage(RtRtimage *rtImage);
protected:
    virtual int Execute();

private:
    RtRtimage       *m_pRtImage;
    std::string     mPlanUID;
};

TPS_END_NAMESPACE

#endif