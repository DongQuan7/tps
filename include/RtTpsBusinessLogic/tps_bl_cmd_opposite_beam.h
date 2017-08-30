////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Zhendong (mailto:zhendong.wang@united-imaging.com)
/// 
///  \file tps_bl_cmd_opposite_beam.h
/// 
///  \brief class  Create Opposite beam
/// 
///  \version 1.0
/// 
///  \date    June.16,2017
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_OPPOSITE_BEAM_H_
#define TPS_BL_CMD_OPPOSITE_BEAM_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"

namespace Mcsf
{
    class Matrix4f;
    class Point3f;
}

TPS_BEGIN_NAMESPACE;

class RtBeam;
class RtBeamsegment;
class TpsIECCoordinateSystem;

class TPS_BL_DECLSPEC OppositeBeamCmd : public CreateBeamCmd
{
public:
    OppositeBeamCmd(const RtBeam& srcBeam);
    OppositeBeamCmd();
    virtual ~OppositeBeamCmd(void);
    void GetOppositeBeam(const RtBeam*& pOppositeBeam);

protected:
    virtual int Execute();

private:

protected:
    RtBeam mSrcBeam;
    RtBeam* mOppositeBeam;


};

TPS_END_NAMESPACE

#endif //TPS_PA_PLANNING_CREATEBEAM_HANDLER_H_
