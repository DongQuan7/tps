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
#ifndef TPS_BL_CMD_EASYPLAN_DELETE_IMAGE_H_
#define TPS_BL_CMD_EASYPLAN_DELETE_IMAGE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsDeleteEasyPlanImageDataCmd : public TpsCommand {
public:
    TpsDeleteEasyPlanImageDataCmd(const std::string& sEzPlanUID);
    virtual ~TpsDeleteEasyPlanImageDataCmd();

protected:
    virtual int Execute();

private:
    int SetEmpty2DImageForEasyPlan();

private:
    std::string mPlanUid;
};

TPS_END_NAMESPACE

#endif