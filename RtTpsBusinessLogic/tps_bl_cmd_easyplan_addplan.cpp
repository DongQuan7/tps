//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_cmd_addeasyplan.cpp 
///  \brief   add easy plan
///
///  \version 1.0
///  \date  2015/10/08  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_addplan.h"

#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE

TpsAddEasyPlanCmd::TpsAddEasyPlanCmd(int winType, std::string winUid, tps::RtPlan* rtPlan, tps::RtNormgroup* rtNormgroup, std::vector<tps::RtPoi*>& rtPoiList )
: mWinType(winType), mWinUid(winUid), mRtPlan(rtPlan), mRtNormgroup(rtNormgroup), mRtPoiList(rtPoiList){

}

TpsAddEasyPlanCmd::~TpsAddEasyPlanCmd()
{

}

int TpsAddEasyPlanCmd::Execute()
{
    RtPlan* aPlan = new RtPlan(true);

    // Initialize doseGrid object

    RtDosegrid* planDosegrid = aPlan->get_dosegrid();
    planDosegrid->set_isdosevalid(false);
    planDosegrid->set_bitsallocated(TPS_BITSALLOCATED);
    planDosegrid->set_bitsstored(TPS_BITSALLOCATED);
    planDosegrid->set_highbits(TPS_BITSALLOCATED -1);
    DosegridInfo info;
    info.spacingx = 1;
    info.spacingy = 1;
    info.spacingz = 1;

    RtImage3DHeader* pImgHdr = new RtImage3DHeader();
    pImgHdr->m_iXDim = 10;
    pImgHdr->m_iYDim = 10;
    pImgHdr->m_iSliceCount = 2;
    pImgHdr->m_dXSize = 10;
    pImgHdr->m_dYSize = 10;
    pImgHdr->m_ImageXOrientation = db_Vector3f(0,1,0);
    pImgHdr->m_ImageYOrientation = db_Vector3f(1,0,0);
    db_Point3f positionPoint1(1.0,0.0,0.0);
    db_Point3f positionPoint2(0.0,0.0,0.0);
    RtImage3DPerScanInfo scanInfo1;
    RtImage3DPerScanInfo scanInfo2;
    scanInfo1.m_ImagePosition = positionPoint1;
    scanInfo2.m_ImagePosition = positionPoint2;
    pImgHdr->m_vScanInfoList.push_back(scanInfo1);
    pImgHdr->m_vScanInfoList.push_back(scanInfo2);
    //pImgHdr->m_vScanInfoList[1].m_ImagePosition = Mcsf::Point3f(1.0,0.0,0.0);

    TpsImage3DEntity* image3DEntity = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(mRtPlan->get_imageseriesuid(), &image3DEntity)){
        TPS_LOG_DEV_ERROR<<"Failed to get the image data.";
        return TPS_ER_FAILURE;
    }

    ROIEntity* skinVoi = mDataRepository->GetVoiManager()->GetExternal(mRtPlan->get_imageseriesuid());

    if (!mDataRepository->GetDoseManager()->InitializeDosegrid(info, image3DEntity, skinVoi, *planDosegrid)) {
        TPS_LOG_DEV_ERROR<<"Failed to initialize default empty plan dosegrid.";
        delete pImgHdr; pImgHdr = nullptr; 
        return TPS_ER_FAILURE;
    }
    aPlan->set_uid(mRtPlan->get_uid());
    aPlan->set_name(mRtPlan->get_name());
    aPlan->set_courseuid(mRtPlan->get_courseuid());
    aPlan->set_plansourcetype(mRtPlan->get_plansourcetype());

    aPlan->set_planner(mRtPlan->get_planner());
    aPlan->set_planid(mRtPlan->get_planid());
    if (!mDataRepository->GetPatientManager()->AddPlan(aPlan)) {
        TPS_LOG_DEV_ERROR<<"Failed to add try in TpsPatientManager.";
        delete aPlan; aPlan = nullptr;
        delete pImgHdr; pImgHdr = nullptr; 
        return TPS_ER_FAILURE;
    }
    

    RtNormgroup *normgroup = new RtNormgroup(true);

    // set default DoseGrid
    RtDosegrid* ngDoseGrid = normgroup->get_dosegrid();
    ngDoseGrid->set_xcoordinate3dstart(planDosegrid->get_xcoordinate3dstart());
    ngDoseGrid->set_ycoordinate3dstart(planDosegrid->get_ycoordinate3dstart());
    ngDoseGrid->set_zcoordinate3dstart(planDosegrid->get_zcoordinate3dstart());
    ngDoseGrid->set_xcoordinate3dinc(planDosegrid->get_xcoordinate3dinc());
    ngDoseGrid->set_ycoordinate3dinc(planDosegrid->get_ycoordinate3dinc());
    ngDoseGrid->set_zcoordinate3dinc(planDosegrid->get_zcoordinate3dinc());
    ngDoseGrid->set_grid_to_pat_t(planDosegrid->get_grid_to_pat_t());
    ngDoseGrid->set_xcount(planDosegrid->get_xcount());
    ngDoseGrid->set_ycount(planDosegrid->get_ycount());
    ngDoseGrid->set_zcount(planDosegrid->get_zcount());
    ngDoseGrid->set_accuracy(planDosegrid->get_accuracy());
    ngDoseGrid->create_dosegrid_buffer();
    float* buffer = ngDoseGrid->get_dosegrid_buffer();
    size_t bufferSize = ngDoseGrid->get_xcount() * 
        ngDoseGrid->get_ycount() * ngDoseGrid->get_zcount();
    memset(buffer, 0, bufferSize * sizeof(float));
    ngDoseGrid->set_isdosevalid(false);
    //should be 16 or 32
    ngDoseGrid->set_bitsallocated(TPS_BITSALLOCATED);
    ngDoseGrid->set_bitsstored(TPS_BITSALLOCATED);
    ngDoseGrid->set_highbits(TPS_BITSALLOCATED -1);
    //TODO: set dose algorithm type of normgroup

    mRenderProxy->SetDataSource(SOURCE_GROUP_EZPLAN,PLAN, mRtPlan->get_uid());

    // 1. set empty 2D image as default plan image
    RtRtimage* planRtImage = nullptr;
    planRtImage = SetEmpty2DImageForEasyPlan();

    // 2. set the middle of image as the IsoCenter for the plan
    // the default offset value is (0,0,0) and the default IsoCenter value is (0,0,0) by Design.
    RtPoi *isoCenter = nullptr;
    isoCenter = new RtPoi(true);
    isoCenter->set_type(RtDbDef::IsoCenter);
    isoCenter->set_name("ISO Center");
    isoCenter->set_seriesuid(mRtPlan->get_imageseriesuid());
    // Set the (0.5,0.5,0)(in imageModel coordinates) as default IsoCenter position in current table setup
    isoCenter->set_coordinate3dx(0.5f);
    isoCenter->set_coordinate3dy(0.5f);
    isoCenter->set_coordinate3dz(0.0f);
    isoCenter->set_isvisible(true);
    TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
    if (!poiManager->AddPOI(isoCenter)){
        TPS_LOG_DEV_ERROR<<"Failed to add POI with type: Iso center.";
        return TPS_ER_FAILURE;
    }

    // 3. add normal point.
    for(int i=0; i<mRtPoiList.size(); i++){
        if(mRtPoiList[i]->get_type() == RtDbDef::NormalPoint
            || mRtPoiList[i]->get_type() == RtDbDef::Calculation){

            if(mRtPoiList[i]->get_type() == RtDbDef::NormalPoint){
                mRtNormgroup->set_normalpointpoiuid(mRtPoiList[i]->get_uid());
            }
            RtPoi* newPoi = new RtPoi(*(mRtPoiList[i]));
            poiManager->AddPOI(newPoi);
            mRenderProxy->EasyPlanSetNormPoint(mRtPlan->get_uid(), mRtPoiList[i]->get_uid(), "");
            mRenderProxy->Render();
        }
    }

    // 4. render isocenter point and other POI
    DISPLAY_SIZE displaySize;
    if(!mRenderProxy->GetDisplaySize((WINDOW_TYPE)mWinType, mWinUid, displaySize)) {
        TPS_LOG_DEV_ERROR<<"Failed to get easy plan display size!";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->EasyPlanSetISOCenter(mRtPlan->get_uid(), isoCenter->get_uid(), planRtImage->get_uid(), 
        isoCenter->get_coordinate3dx(), isoCenter->get_coordinate3dy(), 
        isoCenter->get_coordinate3dx(), isoCenter->get_coordinate3dy(), displaySize.width, displaySize.height, planRtImage->get_columns(), planRtImage->get_rows(),
        planRtImage->get_firstimageplanepixelspacing(), planRtImage->get_secondimageplanepixelspacing());

    // add normal group
    normgroup->set_uid(mRtNormgroup->get_uid());
    normgroup->set_normalpointpoiuid(mRtNormgroup->get_normalpointpoiuid());
    normgroup->set_planuid(mRtNormgroup->get_planuid());
    normgroup->set_machineuid(mRtNormgroup->get_machineuid());
    normgroup->set_radiationtype(mRtNormgroup->get_radiationtype());
    normgroup->set_plantype(mRtNormgroup->get_plantype());
    normgroup->set_name(mRtNormgroup->get_name());
    normgroup->set_prescriptionuid(mRtNormgroup->get_prescriptionuid());
    if (!mDataRepository->GetNormgroupManager()->AddNormGroup(normgroup)) {
        TPS_LOG_DEV_ERROR<<"Failed to add plan in TpsPlanManager.";
        delete normgroup; normgroup = nullptr;
        delete pImgHdr; pImgHdr = nullptr; 
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetNormgroupManager()->AddNgDoseCalculateMode(normgroup->get_uid(), RtDbDef::BASE_ON_WEIGHT))
    {
        TPS_LOG_DEV_ERROR << "Failed to add dose calculate mode.";
        delete normgroup;
        normgroup = nullptr;
        delete pImgHdr;
        pImgHdr = nullptr;
        return TPS_ER_FAILURE;
    }

    
    delete pImgHdr; pImgHdr = nullptr; 

	mRenderProxy->AddNormalGroup(mRtNormgroup->get_planuid(), mRtNormgroup->get_uid());
    mRenderProxy->RefreshDoseLine(mRtNormgroup->get_planuid());
	mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

RtRtimage* TpsAddEasyPlanCmd::SetEmpty2DImageForEasyPlan()
{
    RtRtimage *rtImage = new RtRtimage(true);
    RtDbDef::RTIMAGE_TYPE imageTypess;
    imageTypess = RtDbDef::DRR;
    // TODO: Need to modify these params in order to make a 40cm *40cm black image as default plan image.
    rtImage->set_rows(401);
    rtImage->set_columns(512);
    rtImage->set_bitsstored(12);
    rtImage->set_highbit(11);
    rtImage->set_imagetype(1);
    rtImage->set_name("EmptyRtImage");
    rtImage->set_pixelrepresentation("0");
    rtImage->set_bitsallocated(16);
    rtImage->set_samplesperpixel(1);
    rtImage->set_firstimageplanepixelspacing(1.209f);
    rtImage->set_secondimageplanepixelspacing(1.209f);
    rtImage->set_windowcenter(127);
    rtImage->set_windowwidth(256);
    rtImage->set_frame(1);
    rtImage->set_planuid(mRtPlan->get_uid());
    rtImage->set_courseuid(mRtPlan->get_courseuid());

    // the new RtImage will just be used to create a new rtimage uid.
    RtRtimage newRtImage(true);
    RtRtimage *opposedRtImage = new RtRtimage(*rtImage);
    opposedRtImage->set_uid(newRtImage.get_uid());

    // set pixel data
    int iSize = 205312;
    char *bufferUnit = nullptr;
    char *opposedBufferUnit;

    bufferUnit = new char[iSize * 2];
    memset(bufferUnit, 0, iSize * 2);
    rtImage->set_pixel_data_buffer(bufferUnit, iSize*sizeof(unsigned short)); // deep copy inside

    opposedBufferUnit = new char[iSize * 2];
    memset(opposedBufferUnit, 0, iSize * 2);
    opposedRtImage->set_pixel_data_buffer(opposedBufferUnit, iSize*sizeof(unsigned short)); // deep copy inside

    if(bufferUnit != nullptr) {
        DEL_ARRAY(bufferUnit);
    }
    if(opposedBufferUnit != nullptr){
        DEL_ARRAY(opposedBufferUnit);
    }

    // 2: Save this RTImage (type of Image2DEntity) to ImageDataManager
    std::string imageUID = rtImage->get_uid();
    std::string opposedImageUID = opposedRtImage->get_uid();

    // set current easy plan image uid to image data manager
    TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
    imageDataManager->SetEasyPlanImageUID(mRtPlan->get_uid(), imageUID);
    imageDataManager->SetEasyPlanOpposedImageUID(mRtPlan->get_uid(), opposedImageUID);

    TpsImage2DEntity *pTpsImage_DRR = nullptr;
    TpsImage2DEntity *pTpsOpposedImage_DRR = nullptr;
    if (false == imageDataManager->GetImage2DData(imageUID, &pTpsImage_DRR)) {
        pTpsImage_DRR = new TpsImage2DEntity();
        pTpsImage_DRR->SetRTImage(rtImage); //not deep copy
        imageDataManager->AddImage2D(imageUID, pTpsImage_DRR);
    }
    if (false == imageDataManager->GetImage2DData(opposedImageUID, &pTpsOpposedImage_DRR)) {
        pTpsOpposedImage_DRR = new TpsImage2DEntity();
        pTpsOpposedImage_DRR->SetRTImage(opposedRtImage); //not deep copy
        imageDataManager->AddImage2D(opposedImageUID, pTpsOpposedImage_DRR);
    }

    // 3: CALL mRenderProxy->SetDataSource() and it will call the render controller -> render converter-> checkGO..
    mRenderProxy->SetDataSource(SOURCE_GROUP_EZPLAN, IMAGE, imageUID);
    mRenderProxy->SetEasyPlanPixelSpace();
    return rtImage;
}

TPS_END_NAMESPACE