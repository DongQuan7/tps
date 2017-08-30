////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author Xiaoqing.Shangguan  Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file tps_cmd_poi_create_3d.cpp
/// 
///  \brief
/// 
///  \version 1.0
/// 
///  \date    2015/06/01
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_poi_create_3d.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "tps_logger.h" 

TPS_BEGIN_NAMESPACE
Poi3dCreateCmd::Poi3dCreateCmd(const CertifiedPoi &poi) : mCertifiedPoi(poi) {
}

Poi3dCreateCmd::~Poi3dCreateCmd() {
}

void Poi3dCreateCmd::GetPoiCreated(const RtPoi*& pPoi)
{
    RtPoi* currentPoi = mDataRepository->GetPoiManager()->GetPOI(mCertifiedPoi.m_strPoiUID);
    pPoi = currentPoi;
}

int Poi3dCreateCmd::Execute()
{
    std::string sImageUid = mCertifiedPoi.m_strSeriesUid;
    TpsImage3DEntity* pImage3DData = nullptr;
    if(!mDataRepository->GetImageDataManager()->GetImage3DData(sImageUid, &pImage3DData)
        || pImage3DData == nullptr)
    {
        TPS_LOG_DEV_ERROR << ": Failed to get the image3D, uid: "<<sImageUid;
        return TPS_ER_FAILURE;
    }

    if(!pImage3DData->InImage(mCertifiedPoi.m_fXCoordInPat, mCertifiedPoi.m_fYCoordInPat, mCertifiedPoi.m_fZCoordInPat))
    {
        return TPS_ER_PLANNING_ADDPOI_OUTOFIMAGE;
    }

    RtPoi* poi = new RtPoi();
    poi->set_isvisible(mCertifiedPoi.m_bIsVisible);
    poi->set_name(mCertifiedPoi.m_strPoiName);
    poi->set_uid(mCertifiedPoi.m_strPoiUID);
    poi->set_seriesuid(mCertifiedPoi.m_strSeriesUid);
    poi->set_type(mCertifiedPoi.m_nPoiType);
    poi->set_red(mCertifiedPoi.m_fPoiColor[0]);
    poi->set_green(mCertifiedPoi.m_fPoiColor[1]);
    poi->set_blue(mCertifiedPoi.m_fPoiColor[2]);
    poi->set_alpha(mCertifiedPoi.m_fPoiColor[3]);
    poi->set_coordinate3dx(mCertifiedPoi.m_fXCoordInPat);
    poi->set_coordinate3dy(mCertifiedPoi.m_fYCoordInPat);
    poi->set_coordinate3dz(mCertifiedPoi.m_fZCoordInPat);
    poi->set_locktype(mCertifiedPoi.m_nLockType);    // FE don't know if locked, just lock it

    float dose = INVALID_DOSE_VALUE;
    std::vector<std::string> relatedPlans = mDataRepository->GetPatientManager()->GetPlanUidsBySeriesUid(mCertifiedPoi.m_strSeriesUid);
    //interpolation in related plans
    for (auto itr = relatedPlans.begin(); itr != relatedPlans.end(); ++itr)
    {
        dose = INVALID_DOSE_VALUE;
        mDataRepository->GetDoseManager()->GetPointDoseValueofPlan(*itr, 
            mCertifiedPoi.m_fXCoordInPat, mCertifiedPoi.m_fYCoordInPat, mCertifiedPoi.m_fZCoordInPat, dose);
        poi->set_plandoseinfo(*itr, dose, RtDbDef::Interpolated);
    }

    if (TPS_ER_SUCCESS != mSaveObjects->SavePoi(*poi))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save poi to database.";
        delete poi;
        poi = nullptr;
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetPoiManager()->AddPOI(poi))
    {
        TPS_LOG_DEV_ERROR<<"Failed to add poi into poi manager.";
        delete poi;
        poi = nullptr;
        return TPS_ER_FAILURE;
    }

    mRenderProxy->AddPoi(sImageUid, mCertifiedPoi.m_strPoiUID);
    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
