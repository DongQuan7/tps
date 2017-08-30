//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_test_environment.cpp
///  \brief     rt tps test environment
///
///  \version 1.0
///  \date    April 28,2015
///  \{
//////////////////////////////////////////////////////////////////////////
//#include "Stdafx.h"

#include "RtTpsTestEnvironment/rt_tps_test_environment.h"
#include <vector>
//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

//////////////////////////////////////////////////////////////////////////
TPS_BEGIN_NAMESPACE;

TpsTestEnvironment::TpsTestEnvironment()
    :mFirstName(""),
    mLastName("Zhou Yingen"),
    mDatabase(nullptr),
    mRepository(nullptr),
    mDefaultMachine(nullptr),
    mDefaultUnit(nullptr),
    mDefaultAcc(nullptr),
    mDefaultSeries(nullptr),
    mDefaultCourse(nullptr),
    mDefaultPlan(nullptr),
    mDefaultPoi(nullptr),
    mDefaultNg(nullptr),
    mDefaultBeam(nullptr),
    mDefaultVoi(nullptr),
    mDefaultPatient(nullptr),
    mDefaultPrescriptionUid(""),
    mInitialized(false)
{

}

TpsTestEnvironment::~TpsTestEnvironment()
{
    //clear data
    if (nullptr != mDefaultPlan)
    {
        int iRet = mDatabase->EraseObject(mDefaultPlan->get_uid(), RtDbDef::TYPE_SERIES);
        if(DB_CODES_SUCCESS != iRet)
        {
            printf("EraseObject plan failed:%s\n", mDefaultPlan->get_uid().c_str());
        }
    }

    if (nullptr != mDatabase)
    {
        mDatabase->Finalize();
    }
    DEL_PTR(mDatabase);
    DEL_PTR(mRepository);
    //DEL_PTR(mDefaultBeam);
}

void TpsTestEnvironment::SetPatientName(const std::string firstName, const std::string lastName)
{
   mFirstName = firstName;
   mLastName = lastName;
}

IRtDatabaseWrapper*     TpsTestEnvironment::GetDatabase() { return mDatabase;}
ITpsDataRepository*     TpsTestEnvironment::GetRepository() { return mRepository;}
RtMachine*              TpsTestEnvironment::GetDefaultMachine(){ return mDefaultMachine;}
RtCommissionedunit*     TpsTestEnvironment::GetDefaultUnit(){ return mDefaultUnit;}
RtAccessory*            TpsTestEnvironment::GetDefaultAccessory(){ return mDefaultAcc;}
RtCourse*               TpsTestEnvironment::GetDefaultCourse(){ return mDefaultCourse;}
RtPlan*                 TpsTestEnvironment::GetDefaultPlan(){ return mDefaultPlan;}
RtPoi*                TpsTestEnvironment::GetDefaultPoi(){ return mDefaultPoi;}
RtNormgroup*            TpsTestEnvironment::GetDefaultNormgroup(){ return mDefaultNg;}
RtBeam*                 TpsTestEnvironment::GetDefaultBeam(){ return mDefaultBeam;}
//VOIEntity*              TpsTestEnvironment::GetDefaultVoi(){ return mDefaultVoi;}
RtPatient*              TpsTestEnvironment::GetDefaultPatient() { return mDefaultPatient;}
RtSeries*               TpsTestEnvironment::GetDefaultSeries() { return mDefaultSeries;}

bool TpsTestEnvironment::LoadSystemData(void)
{
    if (mInitialized){
        printf("TpsTestEnvironment initialized.");
        return true;
    }

    //import data
    std::string import_bat("D:\\UIH\\appdata\\rt\\tps\\ut_data\\import_test_data.bat");
    system( import_bat.c_str());

    //Initialize
    TpsDataRepositoryFactory DataRepFactory;
    mRepository = DataRepFactory.CreateDataRepository();
    if (!mRepository->Initialize()) {
        printf("Failed to initialize the data repository.");
        return false;
    }

    IDatabaseFactory  dbFactory;
    mDatabase = dbFactory.CreateDBWrapper();
    if (!mDatabase->Initialize()){
        printf("Can't connect to database! \n");
        return false;
    }

    std::vector<RtMachine*> vMachineList;
    std::vector<std::string> vMachine;
    int iRet = mDatabase->GetAllMachines(vMachineList);

    if(DB_CODES_SUCCESS != iRet || vMachineList.empty())
    {
        printf("Failed to get all machines from database.");
        return false;
    }

    std::string MachineUid("uMLA506C0001");
    std::string UnitUid("UIH-6MV-FF");
    for (auto itr=vMachineList.begin(); itr!=vMachineList.end(); ++itr){
        printf("MachineUid:%s\n", (*itr)->get_uid().c_str());
        if(MachineUid == (*itr)->get_uid())
        {
            mDefaultMachine = (*itr);
            vMachine.push_back(MachineUid);//will be delete by Machine
        }
        mRepository->GetMachineManager()->AddMachine((*itr)->get_uid() , (*itr));
    }

    std::vector<RtCommissionedunit*> vCommissionedunitList;
    iRet = mDatabase->GetCommissionedunitListByMachineUidList(vMachine, vCommissionedunitList);
    if (DB_CODES_SUCCESS != iRet) {
        printf("Failed to get all commissioned units from database.");
        return false;
    }
    if (vCommissionedunitList.empty()){
        printf("There is no commissioned unit in the machines.");
        return false;
    }

    std::vector<std::string> vApprovedCommissionedunitUidList;
    std::string sUnitUid("");
    std::for_each(vCommissionedunitList.begin(), vCommissionedunitList.end(), [&](RtCommissionedunit* item) {
        const std::string uid = item->get_uid();
        vApprovedCommissionedunitUidList.push_back(uid);
        mRepository->GetMachineManager()->AddCommissonedUnit(uid, item);

        printf("UnitUid:%s\n", uid.c_str());
        if(UnitUid == uid) {
            mDefaultUnit = item;
        }
    });

    //////////////////////////////////////////////////////////////////////////
    //etc
    {
        // for physical wedge
        std::vector<RtAccessorychunk*> vRtAccessorychunkList;
        iRet = mDatabase->GetAccessorychunkListByCommissionedunitUidList(
            vApprovedCommissionedunitUidList, vRtAccessorychunkList);
        if (DB_CODES_SUCCESS != iRet) {
            printf("Failed to get all accessory chunk from database.");
            return false;
        }
        for (auto itAccChunk = vRtAccessorychunkList.begin(); itAccChunk != vRtAccessorychunkList.end(); ++itAccChunk) {
            mRepository->GetMachineManager()->AddAccessoryChunk(*itAccChunk);//added physical wedge
            // this part is hard coded contour for accessory chunk profile.
            if ((*itAccChunk)->get_uid() == "ideal_comp_chunk"){
                continue;
            }
            std::string profileUid = (*itAccChunk)->get_uid() + "_profile";
            RtContour* profile = nullptr;
            mDatabase->GetContourByUid(profileUid, profile);
            if (nullptr != profile) {
                mRepository->GetMachineManager()->AddAccessoryChunkProfile(profile);
            }
        }

        //for virtual wedge
        std::vector<RtGoldenstt*> vVirtualWedge;
        iRet = mDatabase->GetGoldensttListByCommissionedunitUidList(vApprovedCommissionedunitUidList, vVirtualWedge);
        if (DB_CODES_SUCCESS != iRet) {
            printf("Failed to get all golden stt from database.");
            return false;
        }

        // 取出所有相关的Accessorychunk中AccessoryUid。
        std::vector<std::string> vApprovedAccessoryUidList;
        std::for_each(vRtAccessorychunkList.begin(), vRtAccessorychunkList.end(),[&](RtAccessorychunk* item) {
                vApprovedAccessoryUidList.push_back(item->get_accessoryuid());
        });

        //added virtual wedge
        for (auto itrVW=vVirtualWedge.begin(); itrVW!=vVirtualWedge.end(); ++itrVW)
        {
            vApprovedAccessoryUidList.push_back((*itrVW)->get_accessoryuid());
            mRepository->GetMachineManager()->AddGoldenstt(*itrVW);
        }

        // 取出所有相关的Accessory。
        std::vector<RtAccessory*> vRtAccessoryList;
        iRet = mDatabase->GetAccessoryListByAccessoryUidList(vApprovedAccessoryUidList, vRtAccessoryList);
        if (DB_CODES_SUCCESS != iRet) {
            printf("Failed to get all accessory from database.");
            return false;
        }

        std::for_each(vRtAccessoryList.begin(), vRtAccessoryList.end(),[&](RtAccessory* item) {
            mRepository->GetMachineManager()->AddAccessory(item);
            if (nullptr == mDefaultAcc){
                mDefaultAcc = item;
            }
        });
    }

    //////////////////////////////////////////////////////////////////////////
    //get patient list
    std::vector<RtPatient*> vRtPatientList;
    iRet = mDatabase->GetAllPatientObjects(vRtPatientList);
    if(DB_CODES_SUCCESS != iRet || vRtPatientList.empty())
    {
        printf("Failed to get patient from database.");
        return false;
    }

    for (auto itr = vRtPatientList.begin(); itr != vRtPatientList.end(); ++itr)
    {
        printf(" GetPatientUID:\t%s \n", (*itr)->get_uid().c_str());
        printf(" GetPatientID:\t%s \n", (*itr)->get_patientid().c_str());
        printf(" GetPatientName:\t%s \n", (*itr)->get_firstname().c_str());
        //       printf(" get_updatetime:\t%s \n",  (boost::posix_time::to_simple_string((*itr)->get_updatetime())).c_str());

        mRepository->GetPatientManager()->SetPatient(*itr);
        if (mLastName == (*itr)->get_lastname()
            && nullptr == mDefaultPatient)
        {
            mDefaultPatient = (*itr);
        }
        else
        {
            //DEL_PTR(*itr);
        }
    }

    //delete vRtPatientList
    //for (auto itr = vRtPatientList.begin(); itr != vRtPatientList.end(); ++itr) 
    //    DEL_PTR(*itr);
    //vRtPatientList.clear();//clear

    //////////////////////////////////////////////////////////////////////////
    if (nullptr == mDefaultPatient){
         printf("No default patient! \n");
        return false;
    }
    //get series 
    std::vector<RtSeries*> vRtSeriesList;
    iRet = mDatabase->GetSeriesListByPatientUid(mDefaultPatient->get_uid(), vRtSeriesList);
    if(DB_CODES_SUCCESS != iRet || vRtSeriesList.empty()){
        printf("Failed to get series from database.");
        return false;
    }

    for (auto itr = vRtSeriesList.begin(); itr != vRtSeriesList.end(); ++itr){
        printf("\n get series uid :\t%s \n", (*itr)->get_uid().c_str());
        if (nullptr == mDefaultSeries)
        {
            mDefaultSeries = *itr;

            TpsImageDataManager* pTmp = mRepository->GetImageDataManager();
            if (nullptr == pTmp){
                printf("Failed to get image data manager.");
                return TPS_ER_FAILURE;
            }
            TpsImage3DEntity* pTpsImage3D = pTmp->CreateImage3D();
            if (pTpsImage3D == nullptr) {
                printf("pTpsImage3D is nullptr");
                return TPS_ER_FAILURE;
            }
            if (!pTpsImage3D->SetupImage3D(mDefaultSeries)){ // get Image3D from series
                delete pTpsImage3D;
                pTpsImage3D = nullptr;
                printf("Failed to set up image 3d object.");
                return TPS_ER_FAILURE;
            }
            // set up a default slice buffer for each Image3d Entity
            if (!pTpsImage3D->InitSliceBuffer()){
                printf("Failed to Initialize slice buffer.");
                return TPS_ER_FAILURE;
            }

            std::string seriesUid = mDefaultSeries->get_uid();
            mRepository->GetImageDataManager()->AddImage3D(seriesUid, pTpsImage3D);

            //get voi
            std::vector<RtVoi*> vVoiList;
            mDatabase->GetVoiListBySeriesUid(seriesUid, vVoiList);
            if (vVoiList.empty()){
                printf("Failed to voi from series.");
                return TPS_ER_FAILURE;
            }

            for (auto itrVoi=vVoiList.begin(); itrVoi!=vVoiList.end(); ++itrVoi)
            {
                //create object
                ROIEntity* pTpsVoi = mRepository->GetVoiManager()->CreateROIEntity(*itrVoi);
                (*itrVoi)->set_seriesuid(seriesUid);
                pTpsVoi->SetIsQA(false);

                std::string newVOIName = (*itrVoi)->get_name();

                std::vector<RtContour*> vecContours = (*itrVoi)->get_contours();
                if (vecContours.empty()){
                    printf("No contour in the voi.");
                }

                //load ROI of this VOI
                if (!LoadROIs_i(pTpsVoi, seriesUid)){
                    printf("LoadROIs for VOI(uid:%s) failed!",(*itrVoi)->get_uid().c_str());
                    return TPS_ER_FAILURE;
                }

                //add VOI into manager
                //todo: judge ret value
                mRepository->GetVoiManager()->AddROI(pTpsVoi);
                if (nullptr == mDefaultVoi && pTpsVoi->IsSkin())
                {
                    mDefaultVoi = pTpsVoi;
                }
            }
        }
        else
        {
            DEL_PTR(*itr);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //get course 
    std::vector<RtCourse*> vRtCourseList;
    iRet = mDatabase->GetCourseListByPatientUid(mDefaultPatient->get_uid(), vRtCourseList);
    if(DB_CODES_SUCCESS != iRet || vRtCourseList.empty())
    {
        printf("Failed to get course from database.");
        return false;
    }

    for (auto itr = vRtCourseList.begin(); itr != vRtCourseList.end(); ++itr)
    {
        mRepository->GetPatientManager()->AddCourse(*itr);
        printf("\n get_courseuid :\t%s \n", (*itr)->get_uid().c_str());
        if (nullptr == mDefaultCourse){
            mDefaultCourse = *itr;
        }
        //else
        //{
        //    DEL_PTR(*itr);
        //}
    }

    //////////////////////////////////////////////////////////////////////////
    //get prescription
    if (nullptr != mDefaultCourse)
    {
        std::vector<RtPrescription*> vPreList;
        iRet = mDatabase->GetPrescriptionListByCourseUid(mDefaultCourse->get_uid(), vPreList);
        if(iRet == DB_CODES_SUCCESS)
        {         
            for (auto itr= vPreList.begin(); itr != vPreList.end(); ++itr)
            {
                mRepository->GetPatientManager()->AddPrescription(*itr);

                printf(" get_uid:\t%s \n", (*itr)->get_uid().c_str());
                printf(" get_doseperfraction:\t%f \n", (*itr)->get_doseperfraction());
                printf(" get_fractionsnumber:\t%d \n", (*itr)->get_fractionsnumber());

                if (mDefaultPrescriptionUid.empty())
                    mDefaultPrescriptionUid = (*itr)->get_uid();
            }
        }
    }
    mInitialized = true;
    return true;
}

// created a Plan 
bool TpsTestEnvironment::CreateTestData(void)
{
    //////////////////////////////////////////////////////////////////////////
    // insert plan
    if(nullptr == mDefaultCourse) return false;

    if (nullptr != mDefaultCourse && nullptr != mDefaultSeries){
        RtPlan* onePlan= new RtPlan(true);
        onePlan->set_courseuid(mDefaultCourse->get_uid());
        onePlan->set_name("plan_one");
        onePlan->set_imageseriesuid(mDefaultSeries->get_uid());
        //g_sPlanUid = onePlan.get_uid();

        // Add a default dose grid to the plan

        RtDosegrid* pDosegrid = onePlan->get_dosegrid();
        pDosegrid->set_isdosevalid(false);
        pDosegrid->set_bitsallocated(TPS_BITSALLOCATED);
        pDosegrid->set_bitsstored(TPS_BITSALLOCATED);
        pDosegrid->set_highbits(TPS_BITSALLOCATED -1);
        DosegridInfo info;
        info.spacingx = 3;
        info.spacingy = 3;
        info.spacingz = 3;

        TpsImage3DEntity* image3DEntity = nullptr;
        if (!mRepository->GetImageDataManager()->GetImage3DData(mDefaultCourse->get_uid(), &image3DEntity)){
            printf("Failed to get the image data.");
            return TPS_ER_FAILURE;
        }

        ROIEntity* skinVoi = mRepository->GetVoiManager()->GetExternal(mDefaultCourse->get_uid());

        if (!mRepository->GetDoseManager()->InitializeDosegrid(info, image3DEntity, skinVoi, *pDosegrid)) {
            printf("Failed to initialize default empty plan dosegrid.");
            delete onePlan;
            return false;
        }
        int iRet = mDatabase->InsertPlanObject(onePlan);
        if(DB_CODES_SUCCESS != iRet) return false;

        mDefaultPlan = onePlan;
        mRepository->GetPatientManager()->AddPlan(onePlan);
    }

    //////////////////////////////////////////////////////////////////////////
    //insert poi
    if(nullptr == mDefaultPlan) return false;
    if (nullptr != mDefaultPlan)
    {
        RtPoi* onePoi = new RtPoi(true);
        onePoi->set_seriesuid(mDefaultPlan->get_imageseriesuid());
        onePoi->set_name("one poi");
        onePoi->set_coordinate3dx(10.f);
        onePoi->set_coordinate3dy(10.f);
        onePoi->set_coordinate3dy(10.f);
        //g_sPoiUid = onePoi->get_uid();
        int iRet = mDatabase->InsertPoiObject(*onePoi);
        if(DB_CODES_SUCCESS != iRet) return false;
        mDefaultPoi = onePoi;
        mRepository->GetPoiManager()->AddPOI(onePoi);
    }

    //////////////////////////////////////////////////////////////////////////
    //insert normgroup
    if (nullptr != mDefaultPlan && nullptr != mDefaultPoi)
    {
        RtNormgroup* pNg = new RtNormgroup(true);

        tps::RtNormgroup oneNormgroup(true);
        pNg->set_planuid(mDefaultPlan->get_uid());
        pNg->set_name("normgroup_one of plan_one");
        pNg->set_normalpointpoiuid(mDefaultPoi->get_uid());
        pNg->set_prescriptionuid(mDefaultPrescriptionUid);
        pNg->set_machineuid(mDefaultMachine->get_uid());

        tps::RtDosegrid* pDosegrid = pNg->get_dosegrid();
        if (nullptr != pDosegrid){
            pDosegrid->set_xcount(10);
            pDosegrid->set_ycount(10);
            pDosegrid->set_zcount(20);

            const int size = 10 * 10 * 20;
            float* pBuffer = pDosegrid->create_dosegrid_buffer();
            for (int i = 0; i < size; ++i){
                pBuffer[i] = (float)i;
            }
        }

        int iRet = mDatabase->InsertNormgroupObject(pNg);
        if(DB_CODES_SUCCESS != iRet)
        {
            printf("Failed to InsertNormgroupObject.");
            DEL_PTR(pNg);
        }
        else
        {
            mDefaultNg = pNg;
            mRepository->GetNormgroupManager()->AddNormGroup(pNg);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //insert beam
    if (nullptr != mDefaultNg)
    {
        RtBeam* oneBeam = new RtBeam(true);
        oneBeam->set_normgroupuid(mDefaultNg->get_uid());
        oneBeam->set_name("beam_90");
        oneBeam->set_commissionedunituid(mDefaultUnit->get_uid());
        oneBeam->set_isocenterpoiuid(mDefaultPoi->get_uid());

        const std::string sBeamUid = oneBeam->get_uid();
        RtDosegrid* pDosegrid = oneBeam->get_dosegrid();
        if (nullptr != pDosegrid){
            pDosegrid->set_uid(sBeamUid);
            pDosegrid->set_beamuid(sBeamUid);
            //        pDosegrid->set_filepath("1");
            pDosegrid->set_xcount(10);
            pDosegrid->set_ycount(10);
            pDosegrid->set_zcount(20);

            const int size = 10 * 10 * 20;
            float* pBuffer = pDosegrid->create_dosegrid_buffer();
            for (int i = 0; i < size; ++i){
                pBuffer[i] = (float)(i + 0.000001*i);
            }
        }

        //TODO add mclshape
        RtBeamsegment* oneBeamsegment = new RtBeamsegment(true);
        oneBeamsegment->set_beamuid(sBeamUid);
        oneBeamsegment->set_startgantryangle(90.);
        oneBeamsegment->set_arclength(90.);

        RtMlcshape* pStartMlcshape = oneBeamsegment->get_startmlcshape();
        std::vector<db_Point2d> vLeaf;
        for (int i(0); i < 40; ++i)
        {
            float fv = (float)(i + 0.00001*i);
            db_Point2d pt(fv, fv);
            vLeaf.push_back(pt);
        }
        pStartMlcshape->set_leafpositions(vLeaf);
        pStartMlcshape->set_jawxissymmetry(true);
        pStartMlcshape->set_jawyissymmetry(false);
        pStartMlcshape->set_xupperjawpos(3.093);
        pStartMlcshape->set_accuracy(5);

        RtMlcshape* pEndMlcshape = oneBeamsegment->get_endmlcshape();
        pEndMlcshape->set_leafpositions(vLeaf);
        pEndMlcshape->set_jawxissymmetry(false);
        pEndMlcshape->set_jawyissymmetry(true);

        RtBeamsegment* secondBeamsegment = new RtBeamsegment(*oneBeamsegment);
        secondBeamsegment->set_uid(mDatabase->GeneraterUid());
        RtMlcshape* pStartMlcshapeSecond = secondBeamsegment->get_startmlcshape();
        pStartMlcshapeSecond->set_uid(mDatabase->GeneraterUid());
        RtMlcshape* pEndMlcshapeSecond = secondBeamsegment->get_endmlcshape();
        pEndMlcshapeSecond->set_uid(mDatabase->GeneraterUid());

        std::vector<RtBeamsegment*> vBeamsegment;
        vBeamsegment.push_back(oneBeamsegment);
        vBeamsegment.push_back(secondBeamsegment);
        oneBeam->set_beamsegments(vBeamsegment);
        for (auto itr=vBeamsegment.begin(); itr!=vBeamsegment.end(); ++itr) 
        {
            DEL_PTR(*itr);
        }

        int iRet = mDatabase->InsertBeamObject(*oneBeam);
        if(DB_CODES_SUCCESS != iRet)
        {
            printf("Failed to insert beam.");
            DEL_PTR(oneBeam);
        }
        else
        {
            mDefaultBeam = oneBeam;
            mRepository->GetBeamManager()->AddBeam2Map(mDefaultBeam);
        }
    }
    return true;
}

//bool SetupAnastruct(const std::vector<RtContour*>& vecContours, 
//    VOIEntity* pVoiEntity)
//{
//    if (vecContours.empty() || pVoiEntity == nullptr){
//        printf("Invalid parameters.");
//        return false;
//    }
//
//    //TpsSafeLock<VOIEntity> lockAna(pVoiEntity);
//    TPS_ANASTRUCT* ana = pVoiEntity->GetAnastruct();
//    if (ana == nullptr){
//        printf("The ana-struct of voi entity is empty.");
//        return false;
//    }
//
//    //clear old data
//    memset(ana->is_dirty, 0, ana->slice_count* sizeof(char));
//    memset(ana->contour_count, 0, ana->slice_count * sizeof(int));
//    for (int i=0; i<ana->slice_count; ++i){
//        if (ana->contours[i]) delete[] ana->contours[i];
//    }
//    memset(ana->contours, 0, ana->slice_count * sizeof(TPS_CONTOUR_STC*));
//
//    ana->is_visible = pVoiEntity->GetVoiVisibilty();
//    pVoiEntity->GetColor(ana->color);
//
//    int sliceNum;
//    for (int i=0; i<vecContours.size(); ++i){
//        sliceNum = vecContours[i]->get_sliceindex() - 1;
//        if (sliceNum < 0 || sliceNum > ana->slice_count - 1) continue;
//        ana->contour_count[sliceNum]++;
//    }
//
//    for (int i=0; i<ana->slice_count; ++i){
//        if (!ana->contour_count[i]) continue;
//        ana->contours[i] = new TPS_CONTOUR_STC[ana->contour_count[i]];
//    }
//
//    size_t vertexCount;
//    TPS_CONTOUR_STC* contour = nullptr;
//    int* currentConcourIndex = new int[ana->slice_count];
//    memset(currentConcourIndex, 0, ana->slice_count * sizeof(int));
//    std::vector<db_Point3f> vecPts;
//    for (int i=0; i<vecContours.size(); ++i){
//        sliceNum = vecContours[i]->get_sliceindex() - 1;
//        if (sliceNum < 0 || sliceNum > ana->slice_count - 1) continue;
//        contour = ana->contours[sliceNum] + (currentConcourIndex[sliceNum]++);
//
//        //rcc: 这里茂亮建议改成直接从db_Point3f拷贝成Mcsf::Point3f
//        //vecPoints = std::move(vecContours[i]->GetPoints());
//
//        //std::string ptFilePath = "";//vecContours[i]->get_pointsfilepath();
//        //vecContours[i]->GetPoints();
//        //        int accuracy = vecContours[i]->get_accuracy();
//        //if (!ReadContourPointFromFile(ptFilePath, accuracy, vecPoints)) {
//        //    TPS_LOG_DEV_ERROR<<"Failed to read contour points from local file["<<ptFilePath<<"].";
//        //    continue;
//        //}
//        vecPts = std::move(vecContours[i]->get_contour_points());
//        vertexCount = vecPts.size();
//        if (vertexCount == 0){
//            printf("Contour size is zero, ignore it.");
//            continue;
//        }
//        contour->pt = new Mcsf::Point3f[vertexCount];
//        for (int i = 0; i < vertexCount; i++){
//            contour->pt[i].SetX(vecPts[i].x);
//            contour->pt[i].SetY(vecPts[i].y);
//            contour->pt[i].SetZ(vecPts[i].z);
//        }
//        contour->vertex_count = vertexCount;
//        contour->slice_number = sliceNum;
//    }
//    delete[] currentConcourIndex;
//    return true;
//}

//int GetBoundingBox(const VOIEntity& voiEntity,
//    const TpsImage3DEntity& image3D, float* boundaryBox)
//{
//    if (nullptr == boundaryBox){
//        printf("boundaryBox is null.");
//        return TPS_ER_FAILURE;
//    }
//
//    RtImage3DHeader* header;
//    if (!image3D.GetImage3DHeader(header)){
//        printf("Failed to get current image 3d header.");
//        return TPS_ER_FAILURE;
//    }
//
//    const std::vector<RtContour*>& vecContour = voiEntity.GetContours();
//    if (vecContour.empty()){
//        boundaryBox[0] = 0.f;
//        boundaryBox[1] = 0.f;
//        boundaryBox[2] = 0.f;
//        boundaryBox[3] = 0.f;
//        boundaryBox[4] = 0.f;
//        boundaryBox[5] = 0.f;
//        return TPS_ER_SUCCESS;
//    }
//
//    int iDimX = header->m_iXDim;
//    int iDimY = header->m_iYDim;
//    boundaryBox[0] = iDimX - 1.f;
//    boundaryBox[1] = 0.f;
//    boundaryBox[2] = iDimY - 1.f;
//    boundaryBox[3] = 0.f;
//    boundaryBox[4] = header->m_iSliceCount -1.f;
//    boundaryBox[5] = 0.f;
//
//    Mcsf::Matrix4f matPatientToVolume;
//    if (!image3D.GetVolumeToPatientMatrix(matPatientToVolume)){
//        printf("Failed to get volume to patient matrix.");
//        return TPS_ER_FAILURE;
//    }
//    matPatientToVolume = matPatientToVolume.Inverse();
//
//    Mcsf::Point3f point;
//    TPS_ANASTRUCT* ana = voiEntity.GetAnastruct();
//    TPS_CONTOUR_STC** allContours = ana->contours;
//    for (int iSlice=0; iSlice<ana->slice_count; iSlice++) {
//        // 该层面上有多少条contour
//        int contoursOnSlice = ana->contour_count[iSlice];
//        // 遍历该层面上所有contour
//        for (int iContour=0; iContour<contoursOnSlice; iContour++) {
//            TPS_CONTOUR_STC* contour = &allContours[iSlice][iContour];
//            // 遍历这条contour上的所有点
//            for (int iPt=0; iPt<contour->vertex_count; iPt++) {
//                Mcsf::Point3f& contourPt = contour->pt[iPt];
//                point = matPatientToVolume.Transform(contourPt);
//                boundaryBox[0] = TPS_MIN(point.GetX(), boundaryBox[0]);
//                boundaryBox[1] = TPS_MAX(point.GetX(), boundaryBox[1]);
//                boundaryBox[2] = TPS_MIN(point.GetY(), boundaryBox[2]);
//                boundaryBox[3] = TPS_MAX(point.GetY(), boundaryBox[3]);
//                boundaryBox[4] = TPS_MIN(point.GetZ(), boundaryBox[4]);
//                boundaryBox[5] = TPS_MAX(point.GetZ(), boundaryBox[5]);
//            }
//        }
//    }
//
//    return TPS_ER_SUCCESS;
//}

int GetMaskFromContours(
    const std::vector<std::vector<Mcsf::Point2f>>& vvContour, 
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned char* buffer, bool isSpline = false)
{
    if (uiWidth <= 0 || uiHeight <= 0 || buffer == nullptr){
        printf("Invalid input parameters.");
        return TPS_ER_FAILURE;
    }

    std::vector<Mcsf::Vector2f> vContour;
    std::vector<std::vector<Mcsf::Vector2f>> vvContourVec(vvContour.size(), vContour);
    for (int i=0; i<vvContour.size(); ++i){
        vvContourVec[i].reserve(vvContour[i].size());
        for (int j=0; j<vvContour[i].size(); ++j){
            vvContourVec[i].push_back(vvContour[i][j]);
        }
    }

    std::vector<Mcsf::Contour> vvMcsfContour;
    Mcsf::Contour contour;
    for (int i = 0; i < vvContourVec.size(); ++i){
        contour.outContour = vvContourVec[i];
        vvMcsfContour.push_back(contour);
    }

    bool bRet = Mcsf::McsfAlgoAutoContour::GetMaskFromContours(vvMcsfContour,
        uiWidth, uiHeight, 1, buffer, isSpline);
    if (!bRet){
        printf("GetMaskFromContours error.");
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

//int UpdateSubVolumeFromContour(std::vector<std::vector<Mcsf::Point2f>>& vvContour,
//    unsigned int uiWidth,
//    unsigned int uiHeight,
//    const Mcsf::Point3f *pVertices,
//    SubVolume* pSubVolume,
//    int iCount)
//{
//    if (nullptr == pSubVolume){
//        printf("pSubVolume is null.");
//        return TPS_ER_FAILURE;
//    }
//
//    iCount;
//
//    const unsigned int size = uiWidth * uiHeight;
//    unsigned char* buffer = new unsigned char[size];
//    memset(buffer, 0, size);
//
//    if(TPS_ER_SUCCESS != GetMaskFromContours(vvContour, uiWidth, uiHeight, buffer)){
//         printf("GetMaskFromContours failed!");
//        return TPS_ER_FAILURE;
//    }
//
//    // bool bRet = UpdateSubVolume(pVertices, aTpsMask, pSubVolume);    
//    int offsetX = 0;
//    int offsetY = 0;
//    int offsetZ = 0;
//    pSubVolume->GetOffSet(offsetX,offsetY,offsetZ);
//    unsigned char * pData = (unsigned char*)pSubVolume->GetData();
//    const unsigned int z = (unsigned int)(pVertices[0].GetZ()-offsetZ + 0.5);
//    //memcpy(pData + size* z, buffer,size);//TODO use DB to save a flag for muli-contour on one slice 
//    for(unsigned int i =0; i < size; ++i){
//        pData[size*z + i] = (0 == pData[size*z + i]) ? buffer[i] : 1;// 0 means backgroud
//    }
//
//    delete[] buffer; buffer = nullptr;
//    return TPS_ER_SUCCESS;
//}

bool TpsTestEnvironment::LoadROIs_i(ROIEntity* /*pVoiEntity*/, const std::string& /*sImageDataUID*/)
{
//     printf("[INFO][TpsLoadObjects][LoadROIs_i]: Load ROI execute.");
//     if (nullptr == pVoiEntity){
//         printf("pVoiEntity is null.");
//         return false;
//     }
//     std::string voiUid = pVoiEntity->GetUid();
// 
//     TpsImage3DEntity* pImage3D = nullptr;
//     mRepository->GetImageDataManager()->GetImage3DData(sImageDataUID, &pImage3D);
//     if (nullptr == pImage3D){
//         printf("pImage3D is null with ImageDataUID:%s",sImageDataUID.c_str());
//         return false;
//     }
// 
//     RtImage3DHeader* header = nullptr;
//     if (!pImage3D->GetImage3DHeader(header) || header == nullptr){
//         printf("Failed to get image header of image:%s", sImageDataUID.c_str());
//         return false;
//     }
//     int dim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
// 
//     //  SubVolumeHelperAPI::BuildSubVolumeFromContour(*pImage3D, pVoiEntity);  
// 
//     Mcsf::Matrix4f matPatientToVolume;
//     if (!pImage3D->GetVolumeToPatientMatrix(matPatientToVolume)){
//         printf("Failed to get volume to patient matrix.");
//         return false;
//     }
//     matPatientToVolume = matPatientToVolume.Inverse();
//     float matP2V[16];
//     matPatientToVolume.GetElements(matP2V);
//     pVoiEntity->SetPat2VolumeMatrix(matP2V);
//     //set relation ship between VOI and Image3D data
//     //pImage3D->AddVOI(pVoiEntity->GetVoiUid());
//     std::vector<RtContour*> vecContour = pVoiEntity->GetContours();
//     auto pSubVolume = pVoiEntity->GetSubVolume();
//     {
//         //TpsSafeLock<VOIEntity> lockAna(pVoiEntity);
//         pVoiEntity->InitializeAna(header->m_iSliceCount);
//     }
// 
//     if (vecContour.empty()){
//         //如果voi为空，将subvolume设置成volume大小
//         int ix = 0, iy = 0, iz = 0;
//         pSubVolume->GetSize(ix, iy, iz);
//         if(ix == 0 || iy == 0 || iz == 0)
//         {
//             pSubVolume->SetSize(dim[0], dim[1], dim[2]);
//             pSubVolume->SetOffSet(0,0,0);
//             char *buffer = pSubVolume->GetData();
//             memset(buffer, 0, dim[0] * dim[1] * dim[2] * sizeof(char));
//         }
//         printf("No contours in this voi");
//         return true;
//     }
// 
//     if (!SetupAnastruct(vecContour, pVoiEntity)){
//         printf("Failed to setup ana-struct.");
//         return false;
//     }
// 
//     Mcsf::Point3f point;
//     float boundary[6];
//     if (TPS_ER_SUCCESS != GetBoundingBox(*pVoiEntity, *pImage3D, boundary)){
//         printf("GetBoundingBox failed");
//         return false;
//     }
// 
//     //for sub-volume definition, we need leave one slice of "0" around the sub-volume 
//     //at each direction if not out of original range
//     float newBoundary[6] = 
//     {
//         TPS_CLAMP(boundary[0] - 1, 0, dim[0] - 1),
//         TPS_CLAMP(boundary[1] + 1, 0, dim[0] - 1),
//         TPS_CLAMP(boundary[2] - 1, 0, dim[1] - 1),
//         TPS_CLAMP(boundary[3] + 1, 0, dim[1] - 1),
//         TPS_CLAMP(boundary[4] - 1, 0, dim[2] - 1),
//         TPS_CLAMP(boundary[5] + 1, 0, dim[2] - 1)
//     };
// 
//     Mcsf::Point3f vertices[4] = 
//     {
//         Mcsf::Point3f(newBoundary[0], newBoundary[2], newBoundary[4]),
//         Mcsf::Point3f(newBoundary[1], newBoundary[2], newBoundary[4]),
//         Mcsf::Point3f(newBoundary[1], newBoundary[3], newBoundary[4]),
//         Mcsf::Point3f(newBoundary[0], newBoundary[3], newBoundary[4])
//     };
//     int xdim = (int)(newBoundary[1] - newBoundary[0] + 1.0 + 0.5);
//     int ydim = (int)(newBoundary[3] - newBoundary[2] + 1.0 + 0.5);
//     int zdim = (int)(newBoundary[5] - newBoundary[4] + 1.0 + 0.5);
//     if (xdim < 1 || ydim < 1 || zdim < 1){
//         printf("LoadROIs_i failed with wrong dim size.");
//         return false;
//     }
//     //todo to be removed, work around for sub-volume algorithm bug when only one slice has contours
//     if (zdim == 1){
//         zdim += 2;
//         newBoundary[4] = TPS_MAX(newBoundary[4] - 1, 0);
//     }
//     int offset[3] = 
//     {
//         (int)(newBoundary[0] + 0.5), 
//         (int)(newBoundary[2] + 0.5), 
//         (int)(newBoundary[4] + 0.5)
//     };
// 
//     //build sub-volume
//     pSubVolume->SetSize(xdim, ydim, zdim);
//     pSubVolume->SetOffSet(offset[0], offset[1], offset[2]);
//     memset(pSubVolume->GetData(), 0, sizeof(char)*xdim*ydim*zdim);
// 
//     //sort by index
//     //现在改为使用voientity里面的anastruct结构，就不需要再排序了
//     //std::sort(vecContour.begin(), vecContour.end(), LessFunc);
//     std::vector<std::vector<Mcsf::Point2f>> vvContour;
//     TPS_ANASTRUCT* ana = pVoiEntity->GetAnastruct();
//     TPS_CONTOUR_STC** allcontours = ana->contours;
//     TPS_CONTOUR_STC* contoursInSlice;
//     Mcsf::Point3f* pointList;
//     std::vector<Mcsf::Point2f> vContour;
//     float roiMin[2];
//     float roiMax[2];
//     float minValue = std::numeric_limits<float>::lowest();
//     float maxValue = std::numeric_limits<float>::max();
//     for (int iSlice = 0; iSlice < ana->slice_count; iSlice++) {
//         // 遍历一层上所有的contour
//         contoursInSlice = allcontours[iSlice];
//         for (int iContour = 0; iContour < ana->contour_count[iSlice]; iContour++) {
//             //获取一条contour
//             TPS_CONTOUR_STC& contour = contoursInSlice[iContour];
//             pointList = contour.pt;
//             if (pointList == nullptr || contour.vertex_count <= 0) {
//                 printf("point list is null or empty in slice[%d], contour[%d]",iSlice, iContour);
//                 continue;
//             }
//             //point = matPatientToVolume.Transform(pointList[0]);
//             for (int j = 0; j < 4; ++j){
//                 vertices[j].SetZ(iSlice);
//             }
// 
//             vContour.clear();
//             vContour.reserve(contour.vertex_count);
//             //check ROI
//             roiMin[0] = maxValue; roiMin[1] = maxValue;
//             roiMax[0] = minValue; roiMax[1] = minValue;
//             for (int iPoint = 0; iPoint < contour.vertex_count; iPoint++) {
//                 point = matPatientToVolume.Transform(pointList[iPoint]);
//                 point.SetX(point.GetX() - offset[0]);
//                 point.SetY(point.GetY() - offset[1]);
//                 point.SetZ(point.GetZ() - offset[2]);
//                 vContour.push_back(std::move(Mcsf::Point2f(point.GetX(), point.GetY())));
//                 roiMin[0] = TPS_MIN(roiMin[0], point.GetX());
//                 roiMin[1] = TPS_MIN(roiMin[1], point.GetY());
//                 roiMax[0] = TPS_MAX(roiMax[0], point.GetX());
//                 roiMax[1] = TPS_MAX(roiMax[1], point.GetY());
//             }
// 
//             //need more then 2 pixes
//             if (roiMax[1] - roiMin[1] >= 2 && roiMax[0] - roiMin[0] >= 2){
//                 vvContour.push_back(vContour);
//             }
//         }
// 
//         //UpdateSubVolumeFromContour
//         if (!vvContour.empty()) {
//             if (TPS_ER_SUCCESS != UpdateSubVolumeFromContour(vvContour,
//                 xdim, ydim, vertices, pSubVolume, ana->contour_count[iSlice])) {
//                     printf("UpdateSubVolumeFromContour failed!");
//                     return false;
//             }
//             vvContour.clear();
//         }
// 
//     }
// 
//     pVoiEntity->SetIsSubVolumeMinimumBounded(true);
//     //TEST_PERFORMANCE_END("Build up Subvolume: " + pTmsVoi->get_name());
// 
//     printf("[INFO][TpsLoadObjects][LoadROIs_i]: Load ROI leave!");
    return true;
}

TPS_END_NAMESPACE;

