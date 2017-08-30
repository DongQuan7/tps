//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_create_merge_plan.h 
///  \brief  save merge result from evaluation into db
///
///  \version 1.0
///  \date  2015/10/31  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_CREATE_MERGE_PLAN_H_
#define TPS_BL_CMD_CREATE_MERGE_PLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include <map>

namespace Mcsf {
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE;

enum MultiPlanOperation
{
    Compare = 0,
    Merge,
    CreateMerge,
};

class RtPlan;
class RtDosegrid;

class TPS_BL_DECLSPEC CreateMergePlanCmd : public TpsCommand
{
public:
    CreateMergePlanCmd(MultiPlanOperation OperateType = CreateMerge);
	~CreateMergePlanCmd();

public:
    void SetNormgroupUids(std::vector<std::string> &normgroupUids);
    void SetDoseLineTemplate(DoseLineTemplate* doseLineTemplate);
    inline void SetMergedPlanName(const std::string &mergedPlanName){ mMergedPlanName = mergedPlanName; }
protected:
	virtual int Execute();
private:
    std::string CreatePlan_i(const std::string &primaryPlanUid, const std::string &secondaryPlanUid);
    bool ValidateImageData_i(const std::string &primaryPlanUid, const std::string &secondaryPlanUid);
    bool GenerateDoseGrid_i(RtPlan &primaryPlan, RtPlan &secondaryPlan, RtPlan& aPlan);
    bool CreateDoseLineSetting_i(RtPlan* plan);
    RtDosegrid* GetPlanDosegrid_i(const std::string& planUID);
    bool CreateNormalGroup_i(const std::string &mergedPlanUid);

private:
    std::vector<std::string> mNormgroupUids;
    std::string              mMergedPlanName;
    Mcsf::Matrix4f	    *mRegistrationMatrix;
    MultiPlanOperation   mOperateType;
    DoseLineTemplate*   mDoseLineTemplate;


};

TPS_END_NAMESPACE
#endif