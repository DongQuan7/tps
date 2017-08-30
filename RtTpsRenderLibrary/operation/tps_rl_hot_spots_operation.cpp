////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author LI Song  song.li@united-imaging.com
///
///  \file tps_rl_hot_spots_operation.cpp
/// 
///  \brief class TpsHotSpotsOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/10/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_operation.h"

#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"  // for TpsHotColdSpotsManager
#include "RtTpsDataAccess/tps_da_voimanager.h"              // for TpsVOIManager
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
//#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"      // for HotSpotsGraphicObject
//#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
//#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsHotSpotsOperation::TpsHotSpotsOperation(
    const std::string& planUID,
    const std::string& voiUID,
    float fDoseLimit, 
    int iHotFlag) : 
    m_strPlanUID(planUID),
    m_strVoiUID(voiUID),
    m_iHotFlag(iHotFlag),
    m_fDoseLimit(fDoseLimit)
{

}

TpsHotSpotsOperation::~TpsHotSpotsOperation()
{

}

bool TpsHotSpotsOperation::ModifyGraphicObject() const
{
    if (mDataRepository == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get data repository.";
        return false;
    }

    TpsVOIManager *pVOIManager = mDataRepository->GetVoiManager();
    TpsHotColdSpotsManager *pHotColdSpotsManager = mDataRepository->GetHotColdSpotsManager();
    if (pVOIManager == nullptr || pHotColdSpotsManager == nullptr) 
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get manager instance.";
        return false;
    }

    RtPlan* plan = nullptr;
    mDataRepository->GetPatientManager()->GetPlan(m_strPlanUID, &plan);
    if (plan == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get plan.";
        return false;
    }
    std::string imageUID = plan->get_imageseriesuid();

    // Get the VOI
    std::vector<ROIEntity*> vVoiList;
    ROIEntity *pVoi = nullptr;
    if (m_strVoiUID == "ALL")
    {
        //选择All 应该用全部VOI集合算热区冷区 wzd
        //pVOIManager->GetSkinVOI(imageUID, &pVoi);
        vVoiList = pVOIManager->GetROIBySeries(imageUID);
    }
    else
    {
        pVoi = pVOIManager->GetROI(m_strVoiUID);
    }

    if (pVoi == nullptr && vVoiList.size() == 0)
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to find the VOI.";
        return false;
    }

    if (m_iHotFlag == 0)
    {
        if (m_strVoiUID == "ALL")
        {
            pHotColdSpotsManager->AddColdSpotsForAllVoi(m_strPlanUID, vVoiList, m_fDoseLimit);
        }
        else
        {
            pHotColdSpotsManager->AddColdSpots(m_strPlanUID, pVoi, m_fDoseLimit);
        }
    }
    else
    {
        if (m_strVoiUID == "ALL")
        {
            pHotColdSpotsManager->AddHotSpotsForAllVoi(m_strPlanUID, vVoiList, m_fDoseLimit);
        }
        else
        {
            pHotColdSpotsManager->AddHotSpots(m_strPlanUID, pVoi, m_fDoseLimit);
        }
    }

    if (m_iHotFlag == 0)
    {
        auto go = mModelWarehouse->GetModelObject(COMMON_BASE +"|"+ GOTypeHelper::ToString(GO_TYPE_COLD_SPOTS));
        if (nullptr == go)
        {
            TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get ColdSpots go!";
            return false;
        }
        auto spColdSpotsGO = std::dynamic_pointer_cast<HotSpotsGraphicObject>(go);
        if (nullptr == spColdSpotsGO)
        {
            TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to cast ColdSpots go!";
            return false;
        }
        spColdSpotsGO->m_spVecPoints_Axial_Map[m_strPlanUID] = pHotColdSpotsManager->m_spVecColdPoints_Axial;
        spColdSpotsGO->m_spVecPoints_Coronal_Map[m_strPlanUID] = pHotColdSpotsManager->m_spVecColdPoints_Coronal;
        spColdSpotsGO->m_spVecPoints_Sagittal_Map[m_strPlanUID] = pHotColdSpotsManager->m_spVecColdPoints_Sagittal;
        spColdSpotsGO->SetDirty(true);
    }
    else
    {
        auto go = mModelWarehouse->GetModelObject(COMMON_BASE +"|"+ GOTypeHelper::ToString(GO_TYPE_HOT_SPOTS));
        if (nullptr == go) 
        {
            TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get HotSpots go!";
            return false;
        }
        auto spHotSpotsGO = std::dynamic_pointer_cast<HotSpotsGraphicObject>(go);
        if (nullptr == spHotSpotsGO)
        {
            TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to cast HotSpots go!";
            return false;
        }
        spHotSpotsGO->m_spVecPoints_Axial_Map[m_strPlanUID] = pHotColdSpotsManager->m_spVecHotPoints_Axial;
        spHotSpotsGO->m_spVecPoints_Coronal_Map[m_strPlanUID] = pHotColdSpotsManager->m_spVecHotPoints_Coronal;
        spHotSpotsGO->m_spVecPoints_Sagittal_Map[m_strPlanUID] = pHotColdSpotsManager->m_spVecHotPoints_Sagittal;
        spHotSpotsGO->SetDirty(true);
    }
    return true;
}

TPS_END_NAMESPACE