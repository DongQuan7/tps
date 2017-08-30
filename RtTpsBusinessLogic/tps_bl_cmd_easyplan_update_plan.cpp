//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_update_plan.cpp 
///  \brief  
///
///  \version 1.0
///  \date  2015/11/02  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_update_plan.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"


TPS_BEGIN_NAMESPACE
    UpdateEasyPlanCmd::UpdateEasyPlanCmd( tps::RtPlan* rtPlan, tps::RtNormgroup* rtNormgroup, std::vector<tps::RtPoi*>& rtPoiList )
    : mRtPlan(rtPlan), mRtNormgroup(rtNormgroup), mRtPoiList(rtPoiList){

}

UpdateEasyPlanCmd::~UpdateEasyPlanCmd()
{

}

int UpdateEasyPlanCmd::Execute()
{
    std::string planUID = mRtPlan->get_uid();//mCertEasyPlan.mPlanUID;
    // 1. update plan
    RtPlan *plan = nullptr;
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
    if (!patientManager->GetPlan(planUID, &plan)
        || plan == nullptr){
            TPS_LOG_DEV_ERROR<<"The try with UID: "<<planUID<<" does not exist.";
            return TPS_ER_FAILURE;
    }
    //*plan = *mRtPlan;
    plan->set_planner(mRtPlan->get_planner());
    plan->set_bodythickness(mRtPlan->get_bodythickness());
    plan->set_patientposition(mRtPlan->get_patientposition());

    // 2. update normgroup
    TpsNormGroupManager *normGroupManager = mDataRepository->GetNormgroupManager();
    std::vector<std::string> normGroupList = normGroupManager->GetNormGroupUIDListByPlan(planUID);

    if (normGroupList.size() != 1) {
        TPS_LOG_DEV_WARNING<<"The amount of normal group of easy plan: " << planUID
            << " is wrong.";
        //return TPS_ER_FAILURE;
    }

    for (auto itNormGroupUID = normGroupList.begin(); itNormGroupUID != normGroupList.end(); ++itNormGroupUID) {
        RtNormgroup *normGroup = nullptr;
        if(!normGroupManager->GetNormGroup(*itNormGroupUID, &normGroup)
            || normGroup == nullptr) {
                TPS_LOG_DEV_ERROR<<"Failed to get normal group of easy plan: " << planUID
                    << " is wrong.";
                return TPS_ER_FAILURE;
        }
        bool isMachineChanged = false;
        if(normGroup->get_machineuid() != mRtNormgroup->get_machineuid()) {
            //normGroup->set_machineuid(mRtNormgroup->get_machineuid());
            isMachineChanged = true;
        }
        //*normGroup = *mRtNormgroup;
        normGroup->set_machineuid(mRtNormgroup->get_machineuid());
        normGroup->set_radiationtype((RtDbDef::RADIATION_TYPE)mRtNormgroup->get_radiationtype());
        normGroup->set_plantype((RtDbDef::PLAN_TYPE)mRtNormgroup->get_plantype());
        normGroup->set_prescriptionuid(mRtNormgroup->get_prescriptionuid());
        normGroup->set_doseperfraction(mRtNormgroup->get_doseperfraction());
        normGroup->set_fraction(mRtNormgroup->get_fraction());
        normGroup->set_name(mRtNormgroup->get_name());
        normGroup->set_normalpointpoiuid(mRtNormgroup->get_normalpointpoiuid());
        if(isMachineChanged){
            //// update beam info(mlc numbers ...etc.)
            TpsBeamManager *beamManager = mDataRepository->GetBeamManager();
            std::vector<RtBeam*> beamList = beamManager->GetBeamsByNormGroup(normGroup->get_uid());
            for(auto it = beamList.begin(); it != beamList.end(); it++) {
                UpdateBeamInfoAfterMachineChanged( normGroup->get_uid(), (*it)->get_uid());
            }
        }
    }

    //3. update poi collection of the plan in poi manager
    TpsPOIManager *poiManager = mDataRepository->GetPoiManager();

    for(int i=0; i<mRtPoiList.size(); i++){
        RtPoi* oldPOI = nullptr;
        oldPOI = poiManager->GetPOI(mRtPoiList[i]->get_uid());

        if (oldPOI == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to update poi into poi manager of plan: " << planUID;
            return TPS_ER_FAILURE;
        }
        else {
            // modify old poi existing in poi manager 
            //*oldPOI = *(mRtPoiList[i]);
            oldPOI->set_type(mRtPoiList[i]->get_type());
            oldPOI->set_locktype(mRtPoiList[i]->get_locktype());
            oldPOI->set_plandoseinfo(planUID, mRtPoiList[i]->get_plandose(planUID));
            oldPOI->set_coordinate3dx(mRtPoiList[i]->get_coordinate3dx());
            oldPOI->set_coordinate3dy(mRtPoiList[i]->get_coordinate3dy());
            oldPOI->set_coordinate3dz(mRtPoiList[i]->get_coordinate3dz());
            oldPOI->set_isvisible(mRtPoiList[i]->get_isvisible());
        }
    }
    return TPS_ER_SUCCESS;
}

bool UpdateEasyPlanCmd::UpdateBeamInfoAfterMachineChanged(const std::string &normgroupUID, std::string beamUID) {
    RtBeam *tmsBeam = nullptr;
    tmsBeam = mDataRepository->GetBeamManager()->GetBeam(beamUID);
    if (tmsBeam == nullptr || tmsBeam->get_uid() != beamUID) {
        return false;
    }

    // update mlc info
    std::vector<RtBeamsegment*> segments = tmsBeam->get_beamsegments();
    if(segments.size() != 0 && segments[0] != nullptr) {

        //create default MLC shape
        RtNormgroup *normGroup = nullptr;
        if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normgroupUID,&normGroup) 
            || nullptr ==normGroup) {
                TPS_LOG_DEV_ERROR<<"Failed to get normgroup:"<<normgroupUID;
                return false;
        }
        RtMachine* machine = nullptr;
        if (!mDataRepository->GetMachineManager()->GetMachine(normGroup->get_machineuid(), machine) 
            || machine == nullptr){
                TPS_LOG_DEV_ERROR<<"Failed to get machine.";
                return false;
        }
        //RTFWK::RTFWKDLDOMLCSetting *mlcSetting = machine->GetMLCSetting();
        //int leafNum = mlcSetting->GetLeaivesNum();
        int leafNum = machine->get_leafnum();
        std::vector<db_Point2d> vecLeafPos;
        db_Point2d leafPos;
        for (int i = 0; i < leafNum; i++) {
            leafPos.x = -10; //-200;
            leafPos.y = 10; //200;
            vecLeafPos.push_back(leafPos);
        }
        // TEST END //
        RtMlcshape *startMlcShape = segments[0]->get_startmlcshape();
        RtMlcshape *endMlcShape = segments[0]->get_endmlcshape();
        startMlcShape->set_leafpositions(vecLeafPos);
        endMlcShape->set_leafpositions(vecLeafPos);
        //SetLeafPositions(std::vector<RTFWKDLDOLeafPosition> vLeafPositions); //RTFWKDLDOLeafPosition represents a pair of leaves, upper one is in right side

        // HERE SHOULD MAKE SURE MLCSHAPE HAVE DIFERENT UID.
        *(segments[0]->get_startmlcshape()) = *startMlcShape;
        *(segments[0]->get_endmlcshape()) = *endMlcShape;
        tmsBeam->set_beamsegments(segments);
    }
    return true;
}

TPS_END_NAMESPACE