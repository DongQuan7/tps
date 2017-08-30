////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenqiao.Jiang  Wenqiao.Jiang@united-imaging.com
/// 
///  \file tps_bl_cmd_poi_update_attribute.cpp
/// 
///  \brief class PoiDeleteCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/12/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_poi_update_attribute.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"


//#include "RtTpsCommonControllerApp/tps_comca_icommoncontroller.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"


TPS_BEGIN_NAMESPACE
PoiUpdateAttributeCmd::PoiUpdateAttributeCmd(const CertifiedPoi &poi,bool isUpdatePoiDose) :
    mCertifiedPoi(poi),
    m_isUpdatePoiDose(isUpdatePoiDose)
    {
        m_strCommandName = "PoiUpdateAttributeCmd";
    }

PoiUpdateAttributeCmd::~PoiUpdateAttributeCmd() {
}

void PoiUpdateAttributeCmd::GetEditPoi(const RtPoi*& pPoi)
{
    RtPoi* currentPoi = mDataRepository->GetPoiManager()->GetPOI(mCertifiedPoi.m_strPoiUID);
    pPoi = currentPoi;
}

int PoiUpdateAttributeCmd::Execute() 
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

    std::string sPoiUid = mCertifiedPoi.m_strPoiUID;
    RtPoi *pPoi = mDataRepository->GetPoiManager()->GetPOI(sPoiUid);
    if(nullptr == pPoi) {
        TPS_LOG_DEV_ERROR<<"PoiUpdateAttributeCmd: Failed to get poi from poi manager!";
        return TPS_ER_FAILURE;
    }

    Mcsf::Point3f ptInPat(mCertifiedPoi.m_fXCoordInPat, mCertifiedPoi.m_fYCoordInPat, mCertifiedPoi.m_fZCoordInPat);

    pPoi->set_name(mCertifiedPoi.m_strPoiName);     // Name, Visible, LockType, Color, Type, Coordinate
    pPoi->set_isvisible(mCertifiedPoi.m_bIsVisible);
    pPoi->set_locktype(mCertifiedPoi.m_nLockType);
    pPoi->set_red(mCertifiedPoi.m_fPoiColor[0]);
    pPoi->set_green(mCertifiedPoi.m_fPoiColor[1]);
    pPoi->set_blue(mCertifiedPoi.m_fPoiColor[2]);
    pPoi->set_alpha(mCertifiedPoi.m_fPoiColor[3]);
    pPoi->set_type((RtDbDef::POI_TYPE)(mCertifiedPoi.m_nPoiType));
    pPoi->set_coordinate3dx(ptInPat.GetX());
    pPoi->set_coordinate3dy(ptInPat.GetY());
    pPoi->set_coordinate3dz(ptInPat.GetZ());

    if (m_isUpdatePoiDose)
    {
        float dose = INVALID_DOSE_VALUE;
        std::vector<std::string> relatedPlans = mDataRepository->GetPatientManager()->GetPlanUidsBySeriesUid(mCertifiedPoi.m_strSeriesUid);
        for(auto itr = relatedPlans.begin(); itr != relatedPlans.end(); ++itr)
        {
            dose = INVALID_DOSE_VALUE;
            mDataRepository->GetDoseManager()->GetPointDoseValueofPlan(*itr, ptInPat.GetX(), ptInPat.GetY(), ptInPat.GetZ(), dose);
            pPoi->set_plandoseinfo(*itr, dose, RtDbDef::Interpolated);
        }
    }


    if (mSaveObjects->UpdatePoi(pPoi) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "PoiUpdateAttributeCmd: Failed to save poi to database.";
        return TPS_ER_DB_ERROR;
    }

    mRenderProxy->UpdatePOI(sImageUid, sPoiUid);
    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE
