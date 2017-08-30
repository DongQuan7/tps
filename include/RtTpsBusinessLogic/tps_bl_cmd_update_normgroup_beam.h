////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Zhednong zhendong.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_update_normgroup_beam.h
/// 
///  \brief class for update normgroup beam
/// 
///  \version 1.0
/// 
///  \date    June.12,2017
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_UPDATE_NORMGROUP_BEAM_H_
#define TPS_BL_CMD_UPDATE_NORMGROUP_BEAM_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"
#include "RtTpsProtoLib/rt_tps_proto_beam_operate.pb.h"



TPS_BEGIN_NAMESPACE;

class RtNormgroup;
class RtBeamsegment;
class RtMachine;
class RtCommissionedunit;
struct TpsCertifiedBeam;

class TPS_BL_DECLSPEC UpdateNormgroupBeamCmd : public TpsCommand
{
public:
    UpdateNormgroupBeamCmd(std::vector<TpsCertifiedBeam*> certifyBeamList, proto::RtTpsProtoRepeatedBeamOperate beamOperateList);
    virtual ~UpdateNormgroupBeamCmd(void);

protected:
    virtual int Execute();

private:

    bool UpdateBeamCommissionedUnit_i(RtMachine* machine, int radiationType, int techniqueType, RtBeam* beam);

    void ClearBeamAttachment_i(RtBeam* beam);

    int DeleteBeam_i(const std::string& beamUid);

    std::string GetImageUidViaBeam_i(RtBeam* beam);

    void ClearCertifyBeamAttachment_i(TpsCertifiedBeam* certifyBeam);

private:


    std::vector<TpsCertifiedBeam*>    mCertifiedBeamList;
    proto::RtTpsProtoRepeatedBeamOperate mBeamOperateList;

    CreateBeamCmd createBeamCmd;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateNormgroupBeamCmd);
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_UPDATE_NORMGROUP_BEAM_H_