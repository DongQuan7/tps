//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_bl_cmd_loadpatientpartial.h 
///  \brief   load patient partial
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_LOADPATIENTPARTIAL_H_
#define TPS_BL_CMD_LOADPATIENTPARTIAL_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE;

class RtPatient;
class RtCourse;
class RtSite;
class RtPrescription;
class RtSeries;
class RtPlan;

class TPS_BL_DECLSPEC LoadPatientPartialCommand : public TpsCommand
{
public:
    LoadPatientPartialCommand(const std::string patientUID);
    virtual ~LoadPatientPartialCommand();

    virtual void GetResult(RtPatient*& patient,
        std::vector<RtCourse*>& vCourseList, std::vector<RtSite*>& vSiteList, std::vector<RtPrescription*>& vPrescriptionList,
        std::vector<RtSeries*>& vSeriesList, std::vector<RtPlan*>& vPlanList);

protected:
    virtual int Execute();
private:
    int LoadPatient();
    int LoadCourse();
    int LoadSite();
    int LoadPrescription();

    int LoadSeries();
    int LoadPlan();

private:
    std::string msPatientUID;
    RtPatient* mPatient;
    std::vector<RtCourse*> mvCourseList;
    std::vector<RtSite*> mvSiteList;
    std::vector<RtPrescription*> mvPrescriptionList;

    std::vector<RtSeries*> mvSeriesList;
    std::vector<RtPlan*> mvPlanList;

};

TPS_END_NAMESPACE

#endif