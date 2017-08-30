//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author   wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \brief   update or insert poi
///
///  \version 1.0
///  \date  2015/11/6
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic\tps_bl_cmd_easyplan_update_insert_poi.h"

#include "RtTpsFramework\tps_fw_command.h"
#include "RtTpsRenderController\tps_rc_renderproxy.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsCertifiedLib\rt_tps_certified_poi.h"
#include "RtTpsDataAccess\tps_da_idatarepository.h"
#include "RtTpsDataAccess\tps_da_normgroupmanager.h"
#include "RtTpsDataAccess\tps_da_beammanager.h"
#include "RtTpsDataAccess\tps_da_idatarepository.h"
#include "RtTpsDataAccess\tps_da_patientmanager.h"
#include "RtTpsDataAccess\tps_da_poimanager.h"
//#include "RtTpsDataAccess\tps_da_imagedatamanager.h"

#include "RtTpsDatabaseWrapper\rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_beam.h"

TPS_BEGIN_NAMESPACE
TpsSendPoiInfoEasyPlanCmd::TpsSendPoiInfoEasyPlanCmd(const CertifiedPoi &poi) :mCertPoi(poi){

}

TpsSendPoiInfoEasyPlanCmd::~TpsSendPoiInfoEasyPlanCmd()
{

}

int TpsSendPoiInfoEasyPlanCmd::Execute()
{
    std::string seriesUID = mCertPoi.m_strSeriesUid;
    // 1. find the series the poi belongs to.
    if (seriesUID.empty())
    {
        TPS_LOG_DEV_ERROR << "The series with UID: " << seriesUID << " does not exist.";
        return false;
    }

    //2. update poi of the plan in poi manager
    TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
    RtPoi* oldPOI = nullptr;
    oldPOI = poiManager->GetPOI(mCertPoi.m_strPoiUID);
    if (oldPOI == nullptr) {
        // add this poi from fe into manager
        RtPoi* tmsPOI = new RtPoi();
        tmsPOI->set_uid(mCertPoi.m_strPoiUID);
        tmsPOI->set_name(mCertPoi.m_strPoiName);
        tmsPOI->set_seriesuid(seriesUID);
        tmsPOI->set_type((RtDbDef::POI_TYPE)(mCertPoi.m_nPoiType));
        tmsPOI->set_coordinate3dx(mCertPoi.m_fXCoordInCol);
        tmsPOI->set_coordinate3dy(mCertPoi.m_fYCoordInCol);
        tmsPOI->set_coordinate3dz(mCertPoi.m_fPointDepth);
        tmsPOI->set_isvisible(mCertPoi.m_bIsVisible);
        //TODO:Beam,VOI,POI,Dose
        //tmsPOI.SetDose(mCertPoi.mDose);

        if (!poiManager->AddPOI(tmsPOI)) {
            TPS_LOG_DEV_ERROR << "Failed to add poi into poi manager of series: " << seriesUID;
            return false;
        }
    }
    else {
        // modify old poi existing in poi manager 
        oldPOI->set_name(mCertPoi.m_strPoiName);
        oldPOI->set_type((RtDbDef::POI_TYPE)(mCertPoi.m_nPoiType));
        oldPOI->set_coordinate3dx(mCertPoi.m_fXCoordInCol);
        oldPOI->set_coordinate3dy(mCertPoi.m_fYCoordInCol);
        oldPOI->set_coordinate3dz(mCertPoi.m_fPointDepth);
        oldPOI->set_isvisible(mCertPoi.m_bIsVisible);
        //TODO:Beam,VOI,POI,Dose
        //oldPOI->SetDose(mCertPoi.mDose);
    }

    //2. save points 
    RtPoi* newPoi = nullptr;
    newPoi = poiManager->GetPOI(mCertPoi.m_strPoiUID);
    if (nullptr == newPoi)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get poi with UID:"
            << mCertPoi.m_strPoiUID;
    }

    int iRet = mSaveObjects->SavePoi(*newPoi);
    if (TPS_ER_SUCCESS != iRet){
        TPS_LOG_DEV_ERROR<<"Failed to save poi with UID:"
            << mCertPoi.m_strPoiUID;
        return iRet;
        }

    //3. update GO and render
    //get imageUid by planUid

    //comment by xiaowei.li, temp solution for easy plan module
    //std::string imageUid;
    //std::vector<std::string> normalUids = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(mCertPoi.m_strCurrentPlanUid);
    //if(normalUids.size() == 1)
    //{
    //    TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
    //    std::vector<std::string> beamUids = beamManager->GetBeamUIDListByNormgroupUID(normalUids[0]);
    //    if(beamUids.size() >= 1)
    //    {
    //        //mDataRepository->GetImageDataManager()->GetEasyPlanBeamImageUID(beamUids[0], &imageUid);
    //        RtBeam* beam = nullptr;
    //        beam = beamManager->GetBeam(beamUids[0]);
    //        if(beam != nullptr){
    //            imageUid = beam->get_relatedrtimageuid();
    //        }
    //    }
    //}
    //if ((RtDbDef::POI_TYPE)mCertPoi.m_nPoiType == RtDbDef::NormalPoint || (RtDbDef::POI_TYPE)mCertPoi.m_nPoiType == RtDbDef::Calculation) {
    //   // mRenderProxy->SetDataSource()
    //    mRenderProxy->EasyPlanSetNormPoint(mCertPoi.m_strCurrentPlanUid, mCertPoi.m_strPoiUID, imageUid);
    //    mRenderProxy->Render();
    //}

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE