////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  XU QAING (mailto:qiang.xu@united-imaging.com)
/// 
///  \file tps_bl_cmd_duplicate_beam.h
/// 
///  \brief class  Create duplicate beam with segment and block
/// 
///  \version 1.0
/// 
///  \date    Dec.01,2016
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_DUPLICATE_BEAM_H_
#define TPS_BL_CMD_DUPLICATE_BEAM_H_

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

class TPS_BL_DECLSPEC DuplicateBeamCmd : public CreateBeamCmd
{
public:
    DuplicateBeamCmd(const RtBeam& rtBeam);
    DuplicateBeamCmd();
    virtual ~DuplicateBeamCmd(void);
    void GetDuplicateBeam(const RtBeam*& pBeam);

protected:
    virtual int Execute();

protected:
    RtBeam mRtBeam;
    std::string newBeamUid;
};

TPS_END_NAMESPACE

#endif //TPS_PA_PLANNING_CREATEBEAM_HANDLER_H_
