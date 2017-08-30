////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WENLONG ZHAO wenlong.zhao@united-imaging.com
/// 
///  \file tps_bl_cmd_planqa_createnormalgroup.h
/// 
///  \brief class Create a new Norm Group(including beams) declaration for PLAN QA 
/// 
///  \version 1.0
/// 
///  \date    JUL 21,2016
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_PLANQA_CREATE_NORMGROUP_H_
#define TPS_BL_CMD_PLANQA_CREATE_NORMGROUP_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

// proto
#include "RtTpsProtoLib/rt_tps_planqaaddnormalgroupconfigtable.pb.h"


namespace Mcsf{
    class Point3f;
}

namespace tps{
    class RtBeam;
    class TpsIECCoordinateSystem;
}

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC CreateQANormGroupCmd : public TpsCommand
{
public:
    CreateQANormGroupCmd(const std::string planuid, const proto::RtTpsProtoPlanQAAddNormalGroupConfigTable& protoAddNGConfigTable);
    virtual ~CreateQANormGroupCmd(void);
    std::string CreateQANormGroupCmd::GetCreatedNormGroupUID();
protected:
    virtual int Execute();

private:
    int CreateQANormalgroup();
    int CreateQABeams() const;
    TpsIECCoordinateSystem* CreateQANormGroupCmd::CreateIECCS_i(
        const std::string& imageUID, float sad, 
        const std::string& beamUID, float gantryAngle, float tableAngle, 
        float collimatorAngle, const Mcsf::Point3f& isoCenter) const;
    int UpdateIECCSForBeam(RtBeam* beam, float gantryAngle, float collimatorAngle, float couchAngle) const;


private:
    std::string                                             mPlanUID;
    proto::RtTpsProtoPlanQAAddNormalGroupConfigTable        mProtoAddNGConfigTable;
    std::string                                             mCreatedNormalizationGroupUID;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_PLANQA_CREATE_NORMGROUP_H_
