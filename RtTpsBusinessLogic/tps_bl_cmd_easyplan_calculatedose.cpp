//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_calculatedose.cpp 
///  \brief  calculate dose in easy plan
///
///  \version 1.0
///  \date  2015/11/03  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_calculatedose.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_poi.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_beamcontribution.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

TPS_BEGIN_NAMESPACE

    EzDoseCalculateCmd::EzDoseCalculateCmd( std::string &planUID, std::string &currentBeamUID )
    : mPlanUID(planUID), mCurrentBeamUID(currentBeamUID){

}

EzDoseCalculateCmd::~EzDoseCalculateCmd()
{
    for (auto it = mPoints.begin(); it != mPoints.end(); ++it)
    {
        DEL_PTR(*it);
    }
}

void EzDoseCalculateCmd::GetBeams( std::vector<RtBeam*> &beams )
{
    beams = mBeams;
}

void EzDoseCalculateCmd::GetPoints( std::vector<RtPoi*> &points )
{
    points = mPoints;
}

int EzDoseCalculateCmd::Execute()
{
    typedef std::map<std::string, float> CalculateResult; //<poiUID, tempDose>

    //1. check Patient
    RtPlan* pPlan = nullptr;

    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

    if (!patientManager->GetPlan(mPlanUID, &pPlan)
        || pPlan == nullptr){
            TPS_LOG_DEV_ERROR<<"The try with UID: "<<mPlanUID<<" does not exist.";
            return TPS_ER_FAILURE;
    }

    //2. check NormGroup
    std::vector<std::string> normGroupUIDList;
    TpsNormGroupManager *normgroupManager = mDataRepository->GetNormgroupManager();
    normGroupUIDList = normgroupManager->GetNormGroupUIDListByPlan(mPlanUID);
    // easy plan should only have one normal group
    if (normGroupUIDList.size() != 1) {
        TPS_LOG_DEV_ERROR<<"Normgroup count is abnormal. plan uid: " << mPlanUID;
        return TPS_ER_FAILURE;
    }

    RtNormgroup *pNormGroup = nullptr;
    if (!normgroupManager->GetNormGroup(normGroupUIDList[0],&pNormGroup)
        || pNormGroup == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get the first normal group of plan uid: " << mPlanUID;
            return TPS_ER_FAILURE;
    }
    //get NG data
    float prescribeDose = pNormGroup->get_doseperfraction();
    int fractionCount = pNormGroup->get_fraction();

    //3. gather all beams of the plan( one or two beams)
    std::vector<std::string> beamUIDList;
    TpsBeamManager *beamManager = mDataRepository->GetBeamManager();
    beamUIDList = beamManager->GetBeamUIDListByNormgroupUID(normGroupUIDList[0]);
    if (beamUIDList.empty()) {
        TPS_LOG_DEV_ERROR<<"Failed to get beam uid list of normal group uid: " << normGroupUIDList[0];
        return TPS_ER_FAILURE;
    }

    //4 Get all points belong to current plan
    TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
    std::vector<RtPoi*> vPOIListOld = poiManager->GetAllPoiInSeries(pPlan->get_imageseriesuid());
    if (vPOIListOld.empty())
    {
        TPS_LOG_DEV_ERROR<<"Failed to get Poi of series uid: " << pPlan->get_imageseriesuid();
        return TPS_ER_FAILURE;
    }
    std::vector<RtPoi*> vPOIListNew;
    for (auto itr=vPOIListOld.cbegin(); itr!=vPOIListOld.cend(); ++itr)
    {
        RtPoi* pPoiNew = new RtPoi(*(*itr));
        vPOIListNew.push_back(pPoiNew);
    }
    RtBeam *easyBeam = nullptr;
    CalculateResult poiUID2TotalDose; //poi total dose

    /*
    目前计算接口传入的POI点坐标的定义：
    POI的X,Y坐标表示视线按照传入beam的角度看下去，此时ISO平面上该点对ISOCenter点的偏移量（上边，右边分别对应ISO平面的正方向），
    Z坐标表示沿此方向看下去，相对ISO平面的深度，顺着视线，点落在ISO平面前方则为负，落在ISO平面后方则为正，此坐标范围是[-20,20]
    */
    int count = 0; //beam counter
    for (auto iterBeam = beamUIDList.begin(); iterBeam != beamUIDList.end(); ++iterBeam) 
    {
        easyBeam = beamManager->GetBeam(*iterBeam);

        //Check Beam Segments
        std::vector<RtBeamsegment*> segments = easyBeam->get_beamsegments();
        if (segments.size() < 1)
        {
            for(auto itr=vPOIListNew.begin(); itr!=vPOIListNew.end(); ++itr)
            {
                DEL_PTR(*itr);
            }
            return TPS_ER_FAILURE;
        }

        if(1 == count) { // for opposite beam 180, the POI coordinate change accordingly.
            for(int i=0; i<vPOIListNew.size(); ++i)
            {
                vPOIListNew[i]->set_coordinate3dx(-1 * vPOIListNew[i]->get_coordinate3dx());
                vPOIListNew[i]->set_coordinate3dz(-1 * vPOIListNew[i]->get_coordinate3dz());
            }
        }

        float currentBeamDose = prescribeDose * easyBeam->get_beamweight() / 100; // percentage

        //2. invoke alg once or twice to calculate dose at each beams of the plan, in the first invoke, save intermediate result to a map
        if (!mDoseProxy->EasyPlanCalculateDose(easyBeam, vPOIListNew, currentBeamDose, fractionCount, poiUID2TotalDose))
        {
            TPS_LOG_DEV_ERROR<<"Failed to calculate dose and mu when invoke algorithm.";
            for(auto itr=vPOIListNew.begin(); itr!=vPOIListNew.end(); ++itr)
            {
                DEL_PTR(*itr);
            }
            return TPS_ER_FAILURE;
        }
        easyBeam->get_dosegrid()->set_isdosevalid(true);
        mBeams.push_back(easyBeam);
        count++;
    }

    //3. output dose @ specific beam and total dose of point and uid of point
    for (auto itNew = vPOIListNew.begin(); itNew != vPOIListNew.end(); ++itNew) {
        std::string poiUID = (*itNew)->get_uid();
        RtPoi *outputPOI = new RtPoi(**itNew);
        for (auto itManagedPOI = vPOIListOld.begin(); itManagedPOI != vPOIListOld.end(); ++itManagedPOI)
        {
            // find the poi in manager and update it
            if ((*itManagedPOI)->get_uid() == poiUID)
            {
                // update total dose of this poi
                (*itManagedPOI)->set_plandoseinfo(mPlanUID, poiUID2TotalDose[poiUID]);
                outputPOI->set_plandoseinfo(mPlanUID, poiUID2TotalDose[poiUID]);
                //update poidose beam contribution
                std::map<std::string, float> poi_dosemap = (*itNew)->get_poidosemap();
                for (std::map<std::string, float>::iterator itDoseMap = poi_dosemap.begin(); itDoseMap != poi_dosemap.end(); ++itDoseMap)
                {
                    (*itManagedPOI)->set_poidose(itDoseMap->first, itDoseMap->second);
                    outputPOI->set_poidose(itDoseMap->first, itDoseMap->second);
                }
            }
        }
        mPoints.push_back(outputPOI);
    }

    // free the memory
    for(auto itr=vPOIListNew.begin(); itr!=vPOIListNew.end(); ++itr)
    {
        DEL_PTR(*itr);
    }

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE