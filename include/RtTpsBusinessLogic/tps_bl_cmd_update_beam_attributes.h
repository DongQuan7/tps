////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_beam_attributes.h
/// 
///  \brief class for UpdateBeamAttributesCmd without beam's angles
///         e.g. isvisible, name, color
/// 
///  \version 1.0
/// 
///  \date    Oct.19,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_UPDATE_BEAM_ATTRIBUTES_H_
#define TPS_BL_CMD_UPDATE_BEAM_ATTRIBUTES_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

#include "RtTpsProtoLib/rt_tps_proto_beam_update.pb.h"

TPS_BEGIN_NAMESPACE;

class RtBeam;

class RtBeamsegment;

class TPS_BL_DECLSPEC UpdateBeamAttributesCmd : public TpsCommand
{
public:
    UpdateBeamAttributesCmd(const proto::RtTpsProtoBeamUpdate &protoBeamUpdate, int editType);

    virtual ~UpdateBeamAttributesCmd(void);

    std::vector<RtBeamsegment*> GetSegmentList();

protected:
    virtual int Execute();

private:
    proto::RtTpsProtoBeamUpdate    mProtoBeamUpdate;
    int                            mEditType;
};

TPS_END_NAMESPACE

#endif //TPS_PCA_ADDTRYCMD_H
