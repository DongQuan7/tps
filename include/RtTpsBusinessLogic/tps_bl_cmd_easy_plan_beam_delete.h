//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \file    tps_bl_cmd_easy_plan_beam_delete.h 
///  \brief   delete easy plan beam
///
///  \version 1.0
///  \date  2015/10/30
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_EASY_PLAN_BEAM_DELETE_H_
#define TPS_BL_CMD_EASY_PLAN_BEAM_DELETE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"


//namespace Mcsf{
//    class Matrix4f;
//    class Point3f;
//}

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

class TPS_BL_DECLSPEC EasyPlanBeamDeleteCmd : public TpsCommand
{
public:
	EasyPlanBeamDeleteCmd(const std::string &beamUid);
    virtual ~EasyPlanBeamDeleteCmd(void);

protected:
    virtual int Execute();
private:
	std::string     mUid;

};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_EASY_PLAN_BEAM_CREATE_H_
