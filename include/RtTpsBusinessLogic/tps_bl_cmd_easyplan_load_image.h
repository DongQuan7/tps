//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_load_image.h 
///  \brief   load image
///
///  \version 1.0
///  \date  2015/10/30  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_LOAD_IMAGE_H_
#define TPS_BL_CMD_EASYPLAN_LOAD_IMAGE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "McsfDataHeader/mcsf_data_header_defs.h"

namespace tps{
    class RtRtimage;
    class RtBeam;
}

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsLoadEasyPlanImageDataCmd : public TpsCommand {
public:
    TpsLoadEasyPlanImageDataCmd(const std::string& sBeamUID, 
        const std::string& sImageFilePath);
    virtual ~TpsLoadEasyPlanImageDataCmd();

    std::vector<RtRtimage*> GetRtImageList();

protected:
    virtual int Execute();

private:
    std::string mBeamUid;
    std::string mImageFilePath;
    std::vector<RtRtimage*> mVRtImageList;

private:
    int LoadEasyPlanDICOMImageData();
    int LoadEasyPlanJPGImageData();
    int LoadEasyPlanBMPImageData();

    bool CreatOpposedRtImage(const Mcsf::ConstUInt16Array puint16Array, char *bufferUnit, const int iSize, RtRtimage* rtImage);
};

TPS_END_NAMESPACE

#endif