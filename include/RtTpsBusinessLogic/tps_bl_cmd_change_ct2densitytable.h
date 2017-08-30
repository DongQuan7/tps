//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
///
///  \file    tps_bl_cmd_change_ct2densitytable.h
///  \brief   
///
///  \version 1.0
///  \date    Mar. 25, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_CCA_CHANGECT2DENSITYTABLE_CMD_H_
#define TPS_CCA_CHANGECT2DENSITYTABLE_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC ChangeCT2DensityTableCmd : public TpsCommand {

public:
    ChangeCT2DensityTableCmd(const std::string& imageUid, const std::string& ct2DensityUid);
    virtual ~ChangeCT2DensityTableCmd();

protected:
    virtual int Execute();

private:
   
    std::string             mImageUid;
    std::string             mCt2DensityUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(ChangeCT2DensityTableCmd);
};

TPS_END_NAMESPACE
#endif