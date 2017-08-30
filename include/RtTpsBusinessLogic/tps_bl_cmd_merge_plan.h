//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_merge_plan.h
///  \brief   
///
///  \version 1.0
///  \date  2015/10/26  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_MERGE_PLAN_H_
#define TPS_BL_CMD_MERGE_PLAN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"

namespace Mcsf {
	class Matrix4f;
}

TPS_BEGIN_NAMESPACE

class RtPlan;
class RtDosegrid;

class TPS_BL_DECLSPEC MergePlanCmd : public TpsCommand 
{
public:
	MergePlanCmd(CertifiedPlan &primaryPlan, CertifiedPlan &secondaryPlan);
	~MergePlanCmd();

    std::string GetMergedPlanUID();
protected:
	virtual int Execute();

private:
	bool ValidateImageData_i();
	std::string CreatePlan_i();
	//\brief     generate dose grid, currently generate the outer cube including dg1 & dg2
	//param[out]  new dose grid for evaluation(merge || compare)
	//return     true if succeed, otherwise false
	bool GenerateDoseGrid_i(RtPlan& aPlan);
	RtDosegrid* GetNormgroupDosegrid_i(const std::string& strNgUID);
	RtDosegrid* GetPlanDosegrid_i(const std::string& planUID);
	bool CreateDoseLineSetting_i(RtPlan* plan);
	bool CreateNormalGroup_i(const std::string &originalPlan,
		const std::string &mergePlan/*, std::vector<std::string> *mergePlanList*/);
private:
	CertifiedPlan		mPrimaryPlan;
	CertifiedPlan		mSencondaryPlan;
	Mcsf::Matrix4f		*mRegistrationMatrix;
    std::string         mResultPlanUID;
private:
	TPS_DISALLOW_COPY_AND_ASSIGN(MergePlanCmd);

};

TPS_END_NAMESPACE

#endif