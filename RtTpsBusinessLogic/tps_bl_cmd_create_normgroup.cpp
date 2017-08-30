////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_pca_create_normgroupcmd.cpp
/// 
///  \brief class Create a new Norm Group 
/// 
///  \version 1.0
/// 
///  \date    Dec.11,2013
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_normgroup.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_drrsetting.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"


#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE

CreateNormGroupCmd::CreateNormGroupCmd(RtNormgroup* pNormgroup, RtDrrsetting* pDrrsetting, int dataSourceId)
    : mNormgroup(pNormgroup), mDrrsetting(pDrrsetting), mNormGroupUID(""), mDataSourceId(dataSourceId){
}

CreateNormGroupCmd::~CreateNormGroupCmd(void) 
{
}

bool CreateNormGroupCmd::CheckInputData()
{
    TpsError err(TPS_ER_INPUT_INVALID);
    mLastError = err;
    if (mNormgroup->get_uid().empty()){
        TPS_LOG_DEV_ERROR<<"mNormgroup->mNormGroupUID is empty.";
        return false;
    }
    if (mNormgroup->get_fraction() < 1e-6){
        TPS_LOG_DEV_ERROR<<"mNormgroup->mFractionCount < 0.";
        return false;
    }
    if (mNormgroup->get_doseperfraction() < 1e-6){
        TPS_LOG_DEV_ERROR<<"mNormgroup->mDosePerFraction < 0.";
        return false;
    }

    if (mNormgroup->get_isnorm2voi() && (mNormgroup->get_normvoipercentagevolume() < 1e-6)){
        TPS_LOG_DEV_ERROR<<"mNormgroup->mVOIDosePercent < 0.";
        return false;
    }
    mLastError.SetError(TPS_ER_SUCCESS);
    return true;
}


/* <key> \n
* PRA: Yes \n
* Name: int CreateNormGroupCmd::Execute() \n
* Traced from: 7625 SSFS_PRA_TPS_SavePlan_Identification \n
* Description: Creating a new normgroup(RtPlan in DICOM) \n
               and saving it into the database. \n
* Short Description: create a new normgroup \n
* Component: TPS \n
* </key> \n
*/
int CreateNormGroupCmd::Execute()
{
    TPS_LOG_DEV_INFO<<"[INFO][CreateNormGroupCmd][Execute]: "<<"CreateNormGroupCmd execute!";
    // check if the POI is a Normalization Point. If not, copy one and save as NP
    std::string planUID = mNormgroup->get_planuid();
    //std::string ct2densityUid = mDataRepository->GetImageDataManager()->GetDefaultCt2DensityTableUid();
    //if(mNormgroup->mTargetIsPOI)//Is Poi
    /*Don't need the following codes
    {
        const std::string strPoiUid = mNormgroup->get_normalpointpoiuid();
        if (strPoiUid.empty()){
            printf("PlanningCreateNormGroupHandler: mNormalizationPoiUID is empty.");
            return TPS_ER_FAILURE;
        }

        TpsPOIManager *pPoiManager = mDataRepository->GetPoiManager();
        RtPoi* pPoi = pPoiManager->GetPOI(strPoiUid);
        if (pPoi->get_type() != RtDbDef::NormalPoint) {
            RtPoi *normalizationPoint = new RtPoi(*pPoi);   // true - generate a POI uid
            normalizationPoint->set_uid(mDatabaseWrapper->GeneraterUid());
            normalizationPoint->set_type(RtDbDef::NormalPoint);
            std::string strPoiName = "NP_of_" + mNormgroup->get_name();
            normalizationPoint->set_name(strPoiName);
            normalizationPoint->set_planuid(planUID);
            normalizationPoint->set_coordinate3dx(pPoi->get_coordinate3dx());
            normalizationPoint->set_coordinate3dy(pPoi->get_coordinate3dy());
            normalizationPoint->set_coordinate3dz(pPoi->get_coordinate3dz());

            if (!pPoiManager->AddPOI(normalizationPoint))
            {
                TPS_LOG_DEV_ERROR<<"Failed to auto add a Normalization point when adding a norm group.";
                return TPS_ER_FAILURE;
            }

            if(TPS_ER_SUCCESS != mSaveObjects->SavePoi(*normalizationPoint)){
                return TPS_ER_DB_ERROR;
            }
            mRenderProxy->AddPoi(planUID, normalizationPoint->get_uid());
            mRenderProxy->Render();

            // The norm group will use the new POI instead
            mNormgroup->set_normalpointpoiuid(normalizationPoint->get_uid());

            // rcc: 这里需要后续补上相应功能
            // Send new POI info to FE
            //CertifiedPoiInfo tpsPoi;
            //tpsPoi.mPoiUid = normalizationPoint->get_uid();
            //tpsPoi.mPlanUid = normalizationPoint->get_planuid();
            //tpsPoi.mName = strPoiName;
            //normalizationPoint->GetColor(tpsPoi.mColor);
            //tpsPoi.mActive = true;
            //tpsPoi.mIsLocked = true;
            //tpsPoi.mRole = RtDbDef::NormalPoint;
            //tpsPoi.mXCoordInPat = normalizationPoint->get_coordinate3dx();
            //tpsPoi.mYCoordInPat = normalizationPoint->get_coordinate3dy();
            //tpsPoi.mZCoordInPat = normalizationPoint->get_coordinate3dz();
            //std::vector<CertifiedPoiInfo> vecTpsPoi;
            //vecTpsPoi.push_back(tpsPoi);
            //RepositoryDispatcher::GetInstance()->AddPoiToFE(vecTpsPoi);
        }
    }
    //else{//Is Voi
    //    if (mNormgroup->mNormalizationVoiUID.empty()){
    //        TPS_LOG_DEV_ERROR<<"PlanningCreateNormGroupHandler: mNormalizationVoiUID is empty.";
    //        return TPS_ER_FAILURE;
    //    }
    //}
    */
    //////////////////////////////////////////////////////////////////////////
    //check input
    if (!CheckInputData()){
        TPS_LOG_DEV_ERROR<<"CheckInputData failed!";
        return TPS_ER_FAILURE;
    } 


    //check plan
    RtPlan* pPlan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &pPlan)){
        TPS_LOG_DEV_ERROR<<"The plan with UID: "<<planUID<<" does not exist.";
        return TPS_ER_FAILURE;
    }

    const std::string imageUID = pPlan->get_imageseriesuid();
    //check image data
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &image)){
        TPS_LOG_DEV_ERROR<<"The image entity with UID: "<<imageUID<<" does not exist.";
        return TPS_ER_FAILURE;
    }

    //RtNormgroup* pNormGroup = new RtNormgroup();//UID from FE

    //pNormGroup->set_uid(mNormgroup->mNormGroupUID);
    //pNormGroup->set_name(mNormgroup->mNormGroupName);
    //pNormGroup->set_planuid(planUID);
    //pNormGroup->set_machineuid(mNormgroup->mMachineUID);
    //pNormGroup->set_isautocalculate(mNormgroup->mIsAutoCalculate);
    //pNormGroup->set_plantype((RtDbDef::PLAN_TYPE)mNormgroup->mTechniqueType);
    //pNormGroup->set_doseperfraction(mNormgroup->mDosePerFraction);
    //pNormGroup->set_fraction(mNormgroup->mFractionCount);

    //bool defaultJoined = true;
    //mNormgroup->set_isjoined(defaultJoined);
    //mNormgroup->set_radiationtype((RtDbDef::RADIATION_TYPE)mNormgroup->mRadiationType);
    //mNormgroup->set_prescriptionuid(mNormgroup->mPrescriptionUID);

    //pNormGroup->set_isgating(mNormgroup->mIsGating);
    //pNormGroup->set_binmode((RtDbDef::BINMODE_TYPE)mNormgroup->mBinMode);
    //pNormGroup->set_beamonrangestart(mNormgroup->mBeamOnRangeStart);
    //pNormGroup->set_beamonrangeend(mNormgroup->mBeamOnRangeEnd);

    //set default dose calculation algorithm type
    //DOSEALGORITHM_TYPE
    //PENCILBEAM = 1,
    //    CONVOLUTION,
    //    MONTECARLO,
    //    ELECTRON
    switch((RtDbDef::RADIATION_TYPE)mNormgroup->get_radiationtype()){
    case RtDbDef::RADIATION_ELETRON:
//        mNormgroup->set_dosealgorithmtype(RtDbDef::MONTECARLO);
        break;
    case RtDbDef::RADIATION_PHOTON:
//        mNormgroup->set_dosealgorithmtype((RtDbDef::DOSEALGORITHM_TYPE)mNormgroup->get_dosealgorithmtype());
        break;
    case RtDbDef::RADIATION_PROTON:
    case RtDbDef::RADIATION_NEUTRON:
        TPS_LOG_DEV_ERROR<<"unsupported radiation type:"<<mNormgroup->get_radiationtype();
        return TPS_ER_FAILURE;
    }

    //pNormGroup->set_normalpointpoiuid(mNormgroup->mNormalizationPoiUID);
    //mNormgroup->set_percentage(mNormgroup->get() / 100);
    //pNormGroup->set_voiuid(mNormgroup->mNormalizationVoiUID);
    //pNormGroup->set_isnorm2voi(!mNormgroup->mTargetIsPOI);
    //pNormGroup->set_isnorm2voimeandose(mNormgroup->mIsAverageVOI);
    //pNormGroup->set_normvoipercentagevolume(mNormgroup->mVOIDosePercent);

    mNormGroupUID = mNormgroup->get_uid();

    RtDosegrid* planDosegrid = pPlan->get_dosegrid();
    if (planDosegrid->get_dosegrid_buffer()==nullptr) {
        TPS_LOG_DEV_ERROR<<"Invalid dosegrid buffer under plan["<<pPlan->get_uid()<<"].";
        return TPS_ER_FAILURE;
    }

    RtDosegrid* ngDoseGrid = mNormgroup->get_dosegrid();
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
    ngDoseGrid->set_bitsallocated(TPS_BITSALLOCATED);
    ngDoseGrid->set_bitsstored(TPS_BITSALLOCATED);
    ngDoseGrid->set_highbits(TPS_BITSALLOCATED -1);

    // save normgroup to database
    //mNormgroup->set_softwareversionno(mDatabaseWrapper->GetTpsVersion());
    mNormgroup->set_tpsequipmentuid(mDatabaseWrapper->GetDefaultEquipmentUid());
    if (nullptr == mSaveObjects)
    {
        TPS_LOG_DEV_ERROR<<"mSaveObjects is null with uid:"<<mNormGroupUID;
        return TPS_ER_FAILURE;
    }

    if (TPS_ER_SUCCESS != mSaveObjects->SaveNormgroup(*mNormgroup)) {
        TPS_LOG_DEV_ERROR<<"InsertNormgroupObject failed! uid:"<<mNormGroupUID;
        return TPS_ER_DB_ERROR;
    }

    // 等成员都完整后， 将Normgroup加入内存管理
    if (!mDataRepository->GetNormgroupManager()->AddNormGroup(mNormgroup)) {
        TPS_LOG_DEV_ERROR<<"Failed to add plan in TpsPlanManager.";
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetNormgroupManager()->AddNgDoseCalculateMode(mNormgroup->get_uid(), RtDbDef::BASE_ON_WEIGHT))
    {
        TPS_LOG_DEV_ERROR << "Failed to add dose calculate mode.";
        return TPS_ER_FAILURE;
    }


    if(mDrrsetting != nullptr)
    {
        //mDrrsetting->set_uid(mDatabaseWrapper->GeneraterUid());
        mDrrsetting->set_normalgroupuid(mNormgroup->get_uid());
        if (nullptr != mDatabaseWrapper){
            if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDrrsetting(*mDrrsetting)){
                TPS_LOG_DEV_ERROR<<"Insertdrrsetting failed! uid:"<<mNormGroupUID;
                return TPS_ER_DB_ERROR;
            }
        }
        if(!mDataRepository->GetNormgroupManager()->AddDrrSetting(mDrrsetting)) {
            TPS_LOG_DEV_ERROR<<"Failed to add drrsetting in NormgroupManager.";
            return TPS_ER_FAILURE;
        }
    }

    // rcc: 接下来的逻辑如果再失败，这个normgroup对象是否回滚，需要后续考虑！
    auto vVois = mDataRepository->GetVoiManager()->GetROIUidListBySeries(imageUID);

    mRenderProxy->AddNormalGroup(planUID, mNormGroupUID);
    mRenderProxy->RefreshDoseLine(planUID);
    if(mDrrsetting != nullptr)
    {
        mRenderProxy->UpdateDrrSetting(imageUID, mNormGroupUID, mDrrsetting->get_ctthresholdminvalue(), 
            mDrrsetting->get_ctthresholdmaxvalue(), mDrrsetting->get_ct2densityuid(), mDrrsetting->get_electronenergy());
    }
    mRenderProxy->Render();

    //set the reply information
    //only return NormGroupUID
    //mNormgroup->mNormGroupUID = mNormGroupUID;
    //mNormgroup->mIsJoined = defaultJoined;

    TPS_LOG_DEV_INFO<<"[INFO][CreateNormGroupCmd][Execute]: "<<"CreateNormGroupCmd leave!";
    return TPS_ER_SUCCESS;
}

std::string CreateNormGroupCmd::GetNormGroupUID() const
{
    return mNormGroupUID;
}

TPS_END_NAMESPACE
