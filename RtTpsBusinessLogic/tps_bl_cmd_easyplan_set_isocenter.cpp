//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_ezplan_set_isocenter.cpp
///  \brief   
///
///  \version 1.0
///  \date  2015/10/29  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_set_isocenter.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess\tps_da_normgroupmanager.h"
#include "RtTpsDataAccess\tps_da_beammanager.h"

TPS_BEGIN_NAMESPACE

EzPlanSetIsoCenterCmd::EzPlanSetIsoCenterCmd( const std::string& sWndUid,
int iWndType, int nAction, float fX, float fY, const CertifiedPoi& tpsPoi )
: m_sWndUid(sWndUid), m_iWndType(iWndType), m_nAction(nAction), 
m_fX(fX), m_fY(fY), m_stcCertifiedPoi(tpsPoi) {

}

EzPlanSetIsoCenterCmd::~EzPlanSetIsoCenterCmd()
{

}

int EzPlanSetIsoCenterCmd::Execute()
{
	// Check 
	TpsPatientManager* pPatientManager = mDataRepository->GetPatientManager();
	if (pPatientManager == nullptr) {
		TPS_LOG_DEV_ERROR << " Failed to find patient manager";
		return TPS_ER_FAILURE;
	}

	if(mRenderProxy == nullptr) {
		TPS_LOG_DEV_ERROR << " Failed to get the render proxy.";
		return TPS_ER_FAILURE;
	}

    // Processing 
    if (m_nAction == 2) // Mouse Move
    {
        mRenderProxy->EasyPlanUpdateSetupCrossHair(
            m_sWndUid, (WINDOW_TYPE)m_iWndType, Mcsf::Point2f(m_fX, m_fY), true);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }

    if (m_nAction == 0) // Mouse Down
    {
        mRenderProxy->EasyPlanUpdateSetupCrossHair(m_sWndUid, (WINDOW_TYPE)m_iWndType, Mcsf::Point2f(m_fX, m_fY), false);

        // if the mouse is clicked, return failure if the plan uid is empty
        //comment by xiaowei.li , temp solution for easy plan module
        std::string strPlanUid = m_stcCertifiedPoi.m_strSeriesUid;
        if (strPlanUid.empty()){
            TPS_LOG_DEV_ERROR << "Failed to get current plan ID";
            return TPS_ER_FAILURE;
        }

        // the coordinate of this poi will be the position in plan image coordinate
        RtPoi *poi = new RtPoi;
        poi->set_uid(m_stcCertifiedPoi.m_strPoiUID);
        poi->set_type(RtDbDef::IsoCenter);
        poi->set_name(m_stcCertifiedPoi.m_strPoiName);
        poi->set_red(m_stcCertifiedPoi.m_fPoiColor[0]);
        poi->set_green(m_stcCertifiedPoi.m_fPoiColor[1]);
        poi->set_blue(m_stcCertifiedPoi.m_fPoiColor[2]);
        poi->set_alpha(m_stcCertifiedPoi.m_fPoiColor[3]);
        poi->set_seriesuid(m_stcCertifiedPoi.m_strSeriesUid);
        poi->set_isvisible(true);

        std::string currentRTImage="";
        mRenderProxy->GetDataSource((WINDOW_TYPE)m_iWndType, IMAGE, currentRTImage);

        TpsImage2DEntity *rtImageEntity = nullptr;
        if(!mDataRepository->GetImageDataManager()->GetImage2DData(currentRTImage, &rtImageEntity)
            || rtImageEntity == nullptr) {
                TPS_LOG_DEV_ERROR<<"Failed to get rt image with UID:"
                    << currentRTImage;
                return TPS_ER_FAILURE;
        }

        int imageWidth, imageHeight;
        if(!rtImageEntity->GetImageSize(imageWidth, imageHeight)) {
            TPS_LOG_DEV_ERROR<<"Failed to get rt image size with UID:"
                << currentRTImage;
            return TPS_ER_FAILURE;
        }
        float firstImagePlanePixelSpacing = rtImageEntity->get()->get_firstimageplanepixelspacing();
        float secondImagePlanePixelSpacing = rtImageEntity->get()->get_secondimageplanepixelspacing();

        Mcsf::Point2f ptIn;
        Mcsf::Point3f ptOut;
        ptIn.SetX(m_fX);
        ptIn.SetY(m_fY);
        // transform points coordinate from screen to image coordinate.
        mRenderProxy->TransformPoint((WINDOW_TYPE)m_iWndType,m_sWndUid,ptIn,ptOut);

        bool isOpposedImage = false;

        //std::string planImageUid = "";
        std::string planOpposedImageUid = "";
        mDataRepository->GetImageDataManager()->GetEasyPlanOpposedImageUID(strPlanUid, &planOpposedImageUid);
        if(planOpposedImageUid == currentRTImage) {
            isOpposedImage = true;
            poi->set_coordinate3dx(1.0f - ptOut.GetX() /** imageWidth * firstImagePlanePixelSpacing*/);
        }
        else {
            isOpposedImage = false;
            poi->set_coordinate3dx(ptOut.GetX() /** imageWidth * firstImagePlanePixelSpacing*/);
        }
        poi->set_coordinate3dy(ptOut.GetY() /** imageHeight * secondImagePlanePixelSpacing*/);
        poi->set_coordinate3dz(0.0);

        // this point is just used for moving image for one time.
        float previousIsoInPlanImgCoorX = 0.0f;
        float previousIsoInPlanImgCoorY = 0.0f;

        std::vector<RtPoi*> poiList;
        TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
        poiList = poiManager->GetAllPoiInSeries(m_stcCertifiedPoi.m_strSeriesUid);

        for (auto iterPOI = poiList.begin(); iterPOI != poiList.end(); ++iterPOI) {
            if ((*iterPOI)->get_type() == RtDbDef::IsoCenter) {
                std::string poiUid = (*iterPOI)->get_uid(); // fix bug of reference

                previousIsoInPlanImgCoorX = (*iterPOI)->get_coordinate3dx();
                previousIsoInPlanImgCoorY = (*iterPOI)->get_coordinate3dy();

                if (!poiManager->DeletePOI(poiUid)) {
                    TPS_LOG_DEV_ERROR << "Failed to delete old isocenter in plan: " << strPlanUid;
                    return TPS_ER_FAILURE;
                }
            }
        }

        //if current plan have no isocenter, then add it. otherwise replace it
        if (!poiManager->AddPOI(poi)){
            TPS_LOG_DEV_ERROR<<"Failed to add POI with type: Iso center.";
            return TPS_ER_FAILURE;
        }

        // everytime when isocenter changed, then modify those other pois in current plan 
        //      according to the new Isocenter and render all poi.
        poiList = poiManager->GetAllPoiInSeries(m_stcCertifiedPoi.m_strSeriesUid);
        for (auto iterPOI = poiList.begin(); iterPOI != poiList.end(); ++iterPOI) {
            if ((*iterPOI)->get_type() == RtDbDef::NormalPoint
                || (*iterPOI)->get_type() == RtDbDef::Calculation) {
                    mRenderProxy->EasyPlanSetNormPoint(strPlanUid, (*iterPOI)->get_uid(), currentRTImage);
            }
        }
        DISPLAY_SIZE displaySize;
        if(!mRenderProxy->GetDisplaySize((WINDOW_TYPE)m_iWndType, m_sWndUid, displaySize)) {
            TPS_LOG_DEV_ERROR<<"Failed to get easy plan display size!";
            return TPS_ER_FAILURE;
        }
        mRenderProxy->EasyPlanSetISOCenter(strPlanUid, poi->get_uid(), currentRTImage, poi->get_coordinate3dx(), poi->get_coordinate3dy(), 
            previousIsoInPlanImgCoorX, previousIsoInPlanImgCoorY,
            displaySize.width, displaySize.height,imageWidth,imageHeight,firstImagePlanePixelSpacing,secondImagePlanePixelSpacing);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    TPS_LOG_DEV_INFO << "[INFO][SetISOCenterCmd][Execute]: " << "SetISOCenterCmd leave!";
	return TPS_ER_FAILURE;
}

tps::CertifiedPoi EzPlanSetIsoCenterCmd::GetCertifiedPoi()
{
	return m_stcCertifiedPoi;
}

TPS_END_NAMESPACE
