//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_change_selected_beam.cpp 
///  \brief  change selected beam
///
///  \version 1.0
///  \date  2015/11/03  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_change_selected_beam.h"
#include "RtTpsRenderController/tps_rc_defs.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

TPS_BEGIN_NAMESPACE
    TpsChangeSelectedBeamEasyPlanCmd::TpsChangeSelectedBeamEasyPlanCmd( int iWndType, 
    const std::string& sWndUid, TpsCertifiedBeam certBeam )
    :  m_iWndType(iWndType), m_sWndUid(sWndUid), mCertBeam(certBeam){

}
TpsChangeSelectedBeamEasyPlanCmd::~TpsChangeSelectedBeamEasyPlanCmd()
{

}

int TpsChangeSelectedBeamEasyPlanCmd::Execute()
{
    TpsNormGroupManager *normgroupManager = mDataRepository->GetNormgroupManager();
    std::string planUID = normgroupManager->GetPlanUIDByNormGroupUID(mCertBeam.mNormGroupUID);
    mRenderProxy->EasyPlanChangeSelectedBeam(planUID,mCertBeam.mNormGroupUID,mCertBeam.mBeamUID);
    std::string imageUID("");
    TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
    RtBeam* beam = nullptr;
    beam = beamManager->GetBeam(mCertBeam.mBeamUID);
    if(beam != nullptr){
        imageUID = beam->get_relatedrtimageuid();
    }
    if(imageUID.empty()) {
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_EZPLAN, IMAGE);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }

    mRenderProxy->SetDataSource(SOURCE_GROUP_EZPLAN, IMAGE, imageUID);
    mRenderProxy->SetEasyPlanPixelSpace();


    // DIMFIX 48429: we need to fire an operation so that when image loaded, it will simulate a set isocenter operation based on the
    //  stored Isocenter position in plan image(not opposed image) model.
    std::string strPlanUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(mCertBeam.mNormGroupUID);
    /*std::vector<RtPoi*> vPois = mDataRepository->GetPoiManager()->GetAllPoiInPlan(strPlanUid);*/
    std::string isocenterUID = "";
    float isoPosXInImageModel = 0.0f;
    float isoPosYInImageModel = 0.0f;
    int imageWidth = 0;
    int imageHeight = 0;
    float firstImagePlanePixelSpacing = 0.0f;
    float secondImagePlanePixelSpacing = 0.0f;
    DISPLAY_SIZE displaySize;
    if(!mRenderProxy->GetDisplaySize((WINDOW_TYPE)m_iWndType, m_sWndUid, displaySize)) {
        TPS_LOG_DEV_ERROR<<"Failed to get easy plan display size!";
        return TPS_ER_FAILURE;
    }

    //for (std::vector<RtPoi*>::iterator itPoi = vPois.begin(); itPoi != vPois.end(); ++itPoi){
    //    //neglect max point
    //    if ((*itPoi)->get_type() == RtDbDef::IsoCenter) {
    //        isocenterUID = (*itPoi)->get_uid();
    //        isoPosXInImageModel = (*itPoi)->get_coordinate3dx();
    //        isoPosYInImageModel = (*itPoi)->get_coordinate3dy();
    //        break;
    //    }
    //}

    TpsImage2DEntity *rtImageEntity = nullptr;
    if(!mDataRepository->GetImageDataManager()->GetImage2DData(imageUID, &rtImageEntity)
        || rtImageEntity == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get rt image with UID:"
                << imageUID;
            return TPS_ER_FAILURE;
    }

    if(!rtImageEntity->GetImageSize(imageWidth, imageHeight)) {
        TPS_LOG_DEV_ERROR<<"Failed to get rt image size with UID:"
            << imageUID;
        return TPS_ER_FAILURE;
    }
    firstImagePlanePixelSpacing = rtImageEntity->get()->get_firstimageplanepixelspacing();
    secondImagePlanePixelSpacing = rtImageEntity->get()->get_secondimageplanepixelspacing();

    // simulate a setisocenter operation so that the isocenter can be targeted at the correct place on image.
    mRenderProxy->EasyPlanSetISOCenter(strPlanUid, isocenterUID, imageUID, isoPosXInImageModel, isoPosYInImageModel, 
        0.5, 0.5,
        displaySize.width, displaySize.height,imageWidth,imageHeight,firstImagePlanePixelSpacing,secondImagePlanePixelSpacing);


    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
