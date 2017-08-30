//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:xiang.liang@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_loadrtimageinfo.h 
///  \brief   load partial image
///
///  \version 1.0
///  \date  2017/8/4  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_LOADRTIMAGEINFO_H_
#define TPS_BL_CMD_EASYPLAN_LOADRTIMAGEINFO_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace tps{
    class RtRtimage;
}

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsEasyPlanLoadRtImageInfoCmd : public TpsCommand {
public:
    TpsEasyPlanLoadRtImageInfoCmd(const std::string& sImageFilePath);
    virtual ~TpsEasyPlanLoadRtImageInfoCmd();

    RtRtimage* GetRtImage();

protected:
    virtual int Execute();

private:
    std::string mImageFilePath;
    RtRtimage* mVRtImage;
};

TPS_END_NAMESPACE

#endif