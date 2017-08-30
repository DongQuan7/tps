////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Song LI  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_easy_plan_beam_create.h
/// 
///  \brief class  EasyPlanBeamCreateCmd 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_EASY_PLAN_BEAM_CREATE_H_
#define TPS_BL_CMD_EASY_PLAN_BEAM_CREATE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"


namespace Mcsf{
    class Matrix4f;
    class Point3f;
}

TPS_BEGIN_NAMESPACE;

class RtBeam;
class RtBeamsegment;
class RtBlock;
class RtPlan;
class RtNormgroup;
class RtPoi;
class RtVoi;
class RtMachine;
class TpsIECCoordinateSystem;

namespace proto {
    class RtTpsProtoBeam;
}
struct TpsCertifiedBeam;
struct db_Point2d;

class TPS_BL_DECLSPEC EasyPlanBeamCreateCmd : public TpsCommand
{
public:
    EasyPlanBeamCreateCmd(TpsCertifiedBeam& certifiedBeam, /*proto::RtTpsProtoBeam** protoBeam,*/ int dataSourceId = SOURCE_GROUP_EZPLAN);
    
    virtual ~EasyPlanBeamCreateCmd(void);

    void GetBeamCreated(const RtBeam*& pBeam);

protected:
    virtual int Execute();

    bool CreateDefaultSingleSegment(RtBeamsegment* segment, RtMachine* const machine, RtBlock* block);

    bool CreateEmptyBlock_i(RtBeamsegment* beam, RtBlock* block);

    bool CreateDosegridForBeam(RtPlan* plan, RtNormgroup* normGroup, RtBeam* beam);

    void ConvertBeamToProto(const RtBeam* beam, proto::RtTpsProtoBeam* protoBeam);

private:
    enum RectShapeMode
    {
        Jaw_Followed = 0x01,
        Leaf_Followed = 0x02
    };

    bool CreateRectShape_i(RtBeamsegment* segment, 
        RtMachine* const machine,
        double* rectSize = nullptr, int mode = 0);

    void CreateMlcShapes(RtBeamsegment* segment,
        const std::vector<db_Point2d>& leafPositions,
        double jawPos[4],
        RtMachine* machine);


    //proto::RtTpsProtoBeam**         mProtoBeam;
    TpsCertifiedBeam                mCertifiedBeam;
    int                             mDataSourceId;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_EASY_PLAN_BEAM_CREATE_H_
