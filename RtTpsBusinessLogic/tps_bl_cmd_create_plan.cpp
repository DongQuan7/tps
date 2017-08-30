////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_cmd_create_plan.cpp
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Dec.15,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_plan.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

//#include "RtTpsCommonControllerApp/tps_comca_icommoncontroller.h"
//#include "RtTpsCommonControllerApp/tps_comca_idatasaver.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsAddPlanCMD::TpsAddPlanCMD(CertifiedPlan *certPlan) 
    : mCertPlan(*certPlan) {

}

TpsAddPlanCMD::~TpsAddPlanCMD(void) {

}

/*<key> \n
*Name: bool DataSaverImpl::SavePlan(const std::string& sPlanUID) {}; \n
*PRA: Yes \n
*Traced from: SSFS_PRA_SavePlan_Identification;SSFS_PRA_CheckSumFunctionForPlan \n
*Description: save plan info \n
*Short Description: SavePlanInfo \n
*Component: DataSaverImpl \n
*</key> \n
*/
int TpsAddPlanCMD::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][TpsAddPlanCMD][Execute]: "<<"TpsAddPlanCMD execute!";

    RtPlan* aPlan = new RtPlan();
    aPlan->set_uid(mCertPlan.mPlanUID);
    aPlan->set_name(mCertPlan.mPlanName);
    aPlan->set_courseuid(mCertPlan.mCourseUID);
    aPlan->set_imageseriesuid(mCertPlan.mImageUID);
    aPlan->set_planner(mCertPlan.mPlanner);
    aPlan->set_plansourcetype(mCertPlan.mPlanSourceType);
    //dose line setting part
    aPlan->set_displaymode(mCertPlan.mDisplayMode);
    aPlan->set_hundredpercentdefinition(mCertPlan.mHundredPercentDefinition);
    aPlan->set_specifieddose(mCertPlan.mSpecifiedDose);
    aPlan->set_rendermode(mCertPlan.mRenderMode);
    aPlan->set_transparency(mCertPlan.mTransParency);
    aPlan->set_enableskindoseview(mCertPlan.mEnableSkinDoseView);
    aPlan->set_maxskindose(mCertPlan.mMaxSkinDose);
    aPlan->set_patientposition(mCertPlan.mpatientPosition);
    aPlan->set_description(mCertPlan.mPlanDescription);
    aPlan->set_bodythickness(mCertPlan.mBodyThickness);
    aPlan->set_description(mCertPlan.mPlanDescription);
    if (mCertPlan.mSetupPoiUid.empty())
    {
        aPlan->disable_field(PLAN_SETUPPOIUID);
    }
    else
    {
        RtPoi* pSetupPoiu = nullptr;
        pSetupPoiu = mDataRepository->GetPoiManager()->GetPOI(mCertPlan.mSetupPoiUid);
        if (nullptr == pSetupPoiu){
            TPS_LOG_DEV_ERROR<<"setup poi is null.";
            return TPS_ER_FAILURE;
        }
        aPlan->set_setuppoiuid(mCertPlan.mSetupPoiUid);
    }

    // Initialize doseGrid object
    RtDosegrid* pDosegrid = aPlan->get_dosegrid();
    pDosegrid->set_isdosevalid(false);
    pDosegrid->set_bitsallocated(TPS_BITSALLOCATED);
    pDosegrid->set_bitsstored(TPS_BITSALLOCATED);
    pDosegrid->set_highbits(TPS_BITSALLOCATED -1);
    DosegridInfo info;
    info.spacingx = mCertPlan.mdosegridspacingx;
    info.spacingy = mCertPlan.mdosegridspacingy;
    info.spacingz = mCertPlan.mdosegridspacingz;

    TpsImage3DEntity* image3DEntity = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(mCertPlan.mImageUID, &image3DEntity)){
        TPS_LOG_DEV_ERROR<<"Failed to get the image data.";
        delete aPlan;
        return TPS_ER_FAILURE;
    }

    ROIEntity* skinVoi = mDataRepository->GetVoiManager()->GetExternal(mCertPlan.mImageUID);

    if (!mDataRepository->GetDoseManager()->InitializeDosegrid(info, image3DEntity, skinVoi, *pDosegrid)) {
        TPS_LOG_DEV_ERROR<<"Failed to initialize default empty plan dosegrid.";
        delete aPlan;
        return TPS_ER_FAILURE;
    }

    if (nullptr != mDatabaseWrapper){
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPlanObject(aPlan)){
            TPS_LOG_DEV_ERROR<<"InsertPlanObject failed! uid:"<<aPlan->get_uid();
            return TPS_ER_DB_ERROR;
        }
    }

    // 添加到内存管理，这步需要早于调用DataSaver，
    // 因为DataSaver是基于内存中存在的Plan进行DB存储
    if (!mDataRepository->GetPatientManager()->AddPlan(aPlan)) {
        TPS_LOG_DEV_ERROR<<"Failed to add plan in TpsPatientManager.";
        delete aPlan;
        return TPS_ER_FAILURE;
    }

    std::string planUid = aPlan->get_uid();
    auto vVois = mDataRepository->GetVoiManager()->GetROIUidListBySeries(mCertPlan.mImageUID);
 
    //mDataRepository->GetPatientManager()->SetCurrentPlanUID(planUid);

    std::vector<RtDoseline*> doseLineList;
    RtDoseline* doseLine = nullptr;
    doseLineList.reserve(mCertPlan.mDoseLineList.size());
    for (int i = 0 ; i < mCertPlan.mDoseLineList.size(); ++i){
        doseLine = new RtDoseline(true);
        CertifiedDoseLine& certDoseLine = mCertPlan.mDoseLineList[i];
        doseLine->set_uid(certDoseLine.mUid);
        doseLine->set_doseratio(certDoseLine.mDoseRatio);
        doseLine->set_red(certDoseLine.mColor[0]);
        doseLine->set_green(certDoseLine.mColor[1]);
        doseLine->set_blue(certDoseLine.mColor[2]);
        doseLine->set_alpha(certDoseLine.mColor[3]);
        doseLine->set_isvisible(certDoseLine.mIsVisible);
        doseLine->set_planuid(certDoseLine.mPlanUid);
        doseLineList.push_back(doseLine);
    }

    if(TPS_ER_SUCCESS != mSaveObjects->SaveDoselines(mCertPlan.mPlanUID, doseLineList)){
        TPS_LOG_DEV_ERROR<<"Failed to Save DoseLineList or doseLine list to TMS";
        for_each(doseLineList.begin(), doseLineList.end(), [](RtDoseline* dl){
            if(dl) delete dl; });
            doseLineList.clear();
            return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetDoseManager()->AddDoseLineList(mCertPlan.mPlanUID, doseLineList)) {
        TPS_LOG_DEV_ERROR<<"Failed to add doselineList in TpsDoseManager.";
        for_each(doseLineList.begin(), doseLineList.end(), [](RtDoseline* dl){
            if(dl) delete dl; });
        doseLineList.clear();
        return TPS_ER_FAILURE;
    }

    //update cert-plan
    float xinc = pDosegrid->get_xcoordinate3dinc();
    float yinc = pDosegrid->get_ycoordinate3dinc();
    float zinc = pDosegrid->get_zcoordinate3dinc();
    float xstart = pDosegrid->get_xcoordinate3dstart();
    float ystart = pDosegrid->get_ycoordinate3dstart();
    float zstart = pDosegrid->get_zcoordinate3dstart();
    float xcount = pDosegrid->get_xcount();
    float ycount = pDosegrid->get_ycount();
    float zcount = pDosegrid->get_zcount();
    mCertPlan.mdosegridsizex = xinc * xcount;
    mCertPlan.mdosegridsizey = yinc * ycount;
    mCertPlan.mdosegridsizez = zinc * zcount;
    mCertPlan.mdosegridstartx = xstart;
    mCertPlan.mdosegridstarty = ystart;
    mCertPlan.mdosegridstartz = zstart;
    mCertPlan.mdosegridspacingx = xinc;
    mCertPlan.mdosegridspacingy = yinc;
    mCertPlan.mdosegridspacingz = zinc;

    TPS_LOG_DEV_INFO<<"[INFO][TpsAddPlanCMD][Execute]: "<<"TpsAddPlanCMD leave!";
    return TPS_ER_SUCCESS;
}

CertifiedPlan TpsAddPlanCMD::GetCertfiedPlan() const{
    return mCertPlan;
}
TPS_END_NAMESPACE
