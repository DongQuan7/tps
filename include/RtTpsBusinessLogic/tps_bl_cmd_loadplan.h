//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_bl_cmd_loadplan.h 
///  \brief   load plan
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_LOADPLAN_H_
#define TPS_BL_CMD_LOADPLAN_H_

#include "tps_bl_defs.h"

#include "RtTpsFramework/tps_fw_command.h"

namespace Mcsf {
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE;

class RtPlan;
class RtNormgroup;
class RtBeam;
class RtDoseline;
class RtDoseoptgoalbase;
class RtDvh;
class RtRtimage;
class RtPoi;

class TPS_BL_DECLSPEC LoadPlanCommand : public TpsCommand
{
public:
    LoadPlanCommand(const std::string sPlanUID);
    virtual ~LoadPlanCommand();

    virtual void GetResult(RtPlan*& pRtPlan, std::vector<RtNormgroup*>& vRtNormgroupList,
        std::vector<RtBeam*>& vRtBeamList,
        std::vector<RtDoseline*>& vRtDoselineList,
        std::vector<RtDoseoptgoalbase*>& vRtDoseoptgoalbaseList,
        std::vector<RtDvh*>& vRtDvhList,
        std::vector<RtPoi*>& vRtPoiList,
        std::vector<RtPoi*>& vRtPoiMaxList,
        std::vector<RtRtimage*>& vRtImageList);

protected:
    virtual int Execute();

private:
    int InitializePlan();
    int InitializeNormgroup();
    int InitializeBeam();
    int InitializeDoseline();
    int InitializeDoseoptgoal();
    int InitializeDvh();
    int InitializePoi();
    int InitializeMaxPoint();
    int InitializeDrrImage();

private:
    std::string msPlanUID;
    RtPlan* mpRtPlan;
    std::vector<RtNormgroup*> mvRtNormgroupList;
    std::vector<RtBeam*> mvRtBeamList;
    std::vector<RtDoseline*> mvRtDoselineList;
    std::vector<RtDoseoptgoalbase*> mvRtDoseoptgoalbaseList;
    std::vector<RtDvh*> mvRtDvhList;
    std::vector<RtPoi*> mvRtPoiList;
    std::vector<RtPoi*> mvRtPoiMaxList;
    std::vector<RtRtimage*> mvRtImageList;
};

TPS_END_NAMESPACE

#endif
