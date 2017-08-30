////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_voi_update_attribute.h
/// 
///  \brief class for VoiUpdateAttributeCmd
/// 
///  \version 1.0
/// 
///  \date    Oct.19,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_VOI_UPDATE_ATTRIBUTE_H_
#define TPS_BL_CMD_VOI_UPDATE_ATTRIBUTE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE;

class ROIEntity;

class TPS_BL_DECLSPEC VoiUpdateAttributeCmd : public TpsCommand{
public:
    VoiUpdateAttributeCmd(ROIEntity *roiEntity);
    virtual ~VoiUpdateAttributeCmd(void);

protected:
    virtual int Execute();

private:
    ROIEntity*      m_pSrcVoi;
};

TPS_END_NAMESPACE

#endif //TPS_PCA_ADDTRYCMD_H
