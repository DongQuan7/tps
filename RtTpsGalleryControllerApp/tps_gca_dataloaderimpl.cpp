////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_dataloaderimpl.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_gca_dataloaderimpl.h"

// c++ header
#include <cctype>                                                   // for string toupper 

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

//TMS
#include "tms/rt_fwk_dl_access.h"
#include "tms/rt_fwk_dl_object_series.h"
#include "tms/rt_fwk_dl_object_patient.h"
#include "tms/rt_fwk_dl_object_image3d.h"
#include "tms/rt_fwk_dl_object_course.h"

#include "tps_logger.h"    

#include "RtTpsDataAccess/tps_da_image3dentity.h"
#include "RtTpsDataAccess/tps_da_image2dentity.h"
#include "RtTpsDataAccess/tps_da_mask.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsProtoLib/rt_tps_certifiedunit.h"
#include "RtTpsFramework/tps_rc_safelock.h"  
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "tps_gca_datamodel_series.h"

#include "boost/exception/all.hpp"  
#include "boost/algorithm/string.hpp"

TPS_BEGIN_NAMESPACE
    bool SaveToTrueColorBitmap( const std::string& path, 
    unsigned char* buffer,
    const int& height,
    const int& width );

TpsGcaDataLoaderImpl::TpsGcaDataLoaderImpl(void){

}


TpsGcaDataLoaderImpl::~TpsGcaDataLoaderImpl(void){

}

void TpsGcaDataLoaderImpl::ClearGalleryManager(){
   TpsGalleryPatientManager::GetInstance()->ClearManager();
   TpsGalleryImageManager::GetInstance()->ClearManager();     
}

void TpsGcaDataLoaderImpl::ClearGalleryPatientManager(){
    TpsGalleryPatientManager::GetInstance()->ClearManager();
} 

bool TpsGcaDataLoaderImpl::Initialize(){ 
    return true;
}

bool TpsGcaDataLoaderImpl::Finalize(){
    return true;
}

bool TpsGcaDataLoaderImpl::LoadPatient(const std::string& patientUID) {
    RTFWK::RTFWKDLDOPatient aPatient;
    aPatient.SetUID(patientUID);

    if (!RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&aPatient)) {
        TPS_LOG_DEV_ERROR<<"No patient in TMS with PatientUID:"<<patientUID;
        return false;
    }

    if (!TpsGalleryPatientManager::GetInstance()->AddPatient(aPatient)) {
        TPS_LOG_DEV_WARNING<<"The patient already exist:"
            <<patientUID;
    }

    TpsGalleryPatientManager::GetInstance()->SetCurrentPatient(patientUID);

    return true;
}

bool TpsGcaDataLoaderImpl::LoadSeries(const std::string& patientUID, std::vector<std::string>& loadedSeriesUIDList) {

    RTFWK::RTFWKDLDOPatient* currentPatient = nullptr;
    TpsGalleryPatientManager::GetInstance()->GetPatient(patientUID, currentPatient);
    if (currentPatient == nullptr) {
        TPS_LOG_DEV_ERROR<<"No Current Patient!";
        return false;
    }

    std::vector<std::string> vSeriesUID;
    if (!currentPatient->GetSeriesUIDList(vSeriesUID) || vSeriesUID.size() < 1) {
        TPS_LOG_DEV_WARNING<<"No Series belong to the patient:"<<patientUID;
        //return false;
    }

    std::vector<RTFWK::RTFWKDLDOSeries> vecSeriesLoaded;
    for(int i = 0; i < vSeriesUID.size(); ++i){

        RTFWK::RTFWKDLDOSeries series;
        series.SetUID(vSeriesUID[i]);
        if(!RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&series)){
            TPS_LOG_DEV_ERROR<<"Failed to load RTFWKDLDOSeries which uid: "<<vSeriesUID[i];
            return false;
        }

        if(!TpsGalleryImageManager::GetInstance()->AddImageSeries(vSeriesUID[i], series)){
            TPS_LOG_DEV_ERROR<<"Failed to add imageSeries to image3dmanager!";
            return false;
        }
        // load image of this series
        if(!LoadImage3D_i(&series)){
            TPS_LOG_DEV_ERROR<<"Failed to load image 3d which uid: "<<vSeriesUID[i];
            return false;
        }

        // prepare the image text buffer
        if(!PrepareImageTextData_i(&series)){
            TPS_LOG_DEV_ERROR<<"Failed to prepare image text for series which uid: "<<vSeriesUID[i];
            return false;
        }
        loadedSeriesUIDList.push_back(vSeriesUID[i]);

        vecSeriesLoaded.push_back(series);
    }

    return true;
}

bool TpsGcaDataLoaderImpl::ReloadSeries(const std::string& patientUID, std::vector<std::string>& loadedSeriesUIDList) {

    RTFWK::RTFWKDLDOPatient* currentPatient = nullptr;
    TpsGalleryPatientManager::GetInstance()->GetPatient(patientUID, currentPatient);
    if (currentPatient == nullptr) {
        TPS_LOG_DEV_ERROR<<"No Current Patient!";
        return false;
    }

    std::vector<std::string> vSeriesUID;
    if (!currentPatient->GetSeriesUIDList(vSeriesUID) || vSeriesUID.size() < 1) {
        TPS_LOG_DEV_WARNING<<"No Series belong to the patient:"<<patientUID;
        //return false;
    }

    std::vector<RTFWK::RTFWKDLDOSeries> vecSeriesLoaded;
    std::vector<RTFWK::RTFWKDLDOSeries> vecQASeriesLoaded;
    for(int i = 0; i < vSeriesUID.size(); ++i){

        RTFWK::RTFWKDLDOSeries* pSeries = nullptr;

        if (TpsGalleryImageManager::GetInstance()->GetImageSeries(vSeriesUID[i], &pSeries)){
           // not load again
           continue;
        }
        
        RTFWK::RTFWKDLDOSeries series;
        series.SetUID(vSeriesUID[i]);
        if(!RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&series)){
            TPS_LOG_DEV_ERROR<<"Failed to load RTFWKDLDOSeries which uid: "<<vSeriesUID[i];
            return false;
        }

        if(!TpsGalleryImageManager::GetInstance()->AddImageSeries(vSeriesUID[i], series)){
            TPS_LOG_DEV_ERROR<<"Failed to add imageSeries to image3dmanager!";
            return false;
        }
        // load image of this series
        if(!LoadImage3D_i(&series)){
            TPS_LOG_DEV_ERROR<<"Failed to load image 3d which uid: "<<vSeriesUID[i];
            return false;
        }

        // prepare the image text buffer
        if(!PrepareImageTextData_i(&series)){
            TPS_LOG_DEV_ERROR<<"Failed to prepare image text for series which uid: "<<vSeriesUID[i];
            return false;
        }
        loadedSeriesUIDList.push_back(vSeriesUID[i]);
        if(!series.GetIsCopy()) {
            vecSeriesLoaded.push_back(series);
        }
        else {
            vecQASeriesLoaded.push_back(series);
        }
    }

    SetDefaultImage_i(vecSeriesLoaded);

    return true;
}

bool TpsGcaDataLoaderImpl::LoadImage3D(const std::string& sImage3DUID){
    TpsImage3DEntity* image3DEntity = nullptr;
    if(TpsGalleryImageManager::GetInstance()->GetImage3DData(sImage3DUID, &image3DEntity)){
        TPS_LOG_DEV_INFO<<"Image3D which UID: "<<sImage3DUID<<" has already been loaded.";
        return true;
    }

    RTFWK::RTFWKDLDOSeries series;
    series.SetUID(sImage3DUID);
    if(!RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&series)){
        TPS_LOG_DEV_ERROR<<"Failed to load RTFWKDLDOSeries whose uid is: "<<sImage3DUID;
        return false;
    }

    if(!TpsGalleryImageManager::GetInstance()->AddImageSeries(sImage3DUID, series)){
        TPS_LOG_DEV_ERROR<<"Failed to add imageSeries to image3dmanager!";
        return false;
    }

    if(!LoadImage3D_i(&series)){
        TPS_LOG_DEV_ERROR<<"Failed to load Image 3D object(uid"<<sImage3DUID<<")";
        return false;
    }

    return true;
}

bool TpsGcaDataLoaderImpl::LoadImage3D_i(RTFWK::RTFWKDLDOSeries* const pSeries){
    TpsGalleryImageManager* pTmp = TpsGalleryImageManager::GetInstance();
    if (nullptr == pTmp)
        return false;
    TpsImage3DEntity* pTpsImage3D = pTmp->CreateImage3D();
    if(pTpsImage3D == nullptr){
        TPS_LOG_DEV_ERROR<<"nullptr";
        return false;
    }
    if(!pTpsImage3D->SetupImage3D(pSeries)){
        delete pTpsImage3D;
        pTpsImage3D = nullptr;
        TPS_LOG_DEV_ERROR<<"Failed to set up image 3d object.";
        return false;
    }

    // set up a default slice buffer for each Image3d Entity
    if(!pTpsImage3D->InitSliceBuffer()){
        TPS_LOG_DEV_ERROR<<"Failed to Initialize slice buffer.";
        return false;
    }

    std::string sImage3DUid = pSeries->GetUID();
    if(!TpsGalleryImageManager::GetInstance()->AddImage3D(sImage3DUid, pTpsImage3D)){
        delete pTpsImage3D;
        pTpsImage3D = nullptr;
        TPS_LOG_DEV_ERROR<<"Failed to add image 3d object(uid: "<<sImage3DUid<<") into ImageDataManager.";
        return false;
    } 

    //////////////////////////////////////////////////////////////////////////
    // TODO  Use DL Interface
    TpsGcaDmSeries dmSeries(pSeries->GetUID());
    if (RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&dmSeries)){
        auto imageLists = dmSeries.GetImageLists();
        std::map<int, std::string> imageMap;
        bool bIsValid = true;
        for (auto iter = imageLists.cbegin(); iter!= imageLists.cend(); ++iter)
        {
            // Check 
            if (imageMap.find(iter->m_iInstanceNumber) != imageMap.end()){
                TPS_LOG_DEV_ERROR<<"InstanceNumber is duplicate for Series "<<sImage3DUid;
                bIsValid = false;
                break;
            }
            imageMap[iter->m_iInstanceNumber] = iter->m_sImageUID;
        }
        if (bIsValid) {
            if (TpsGalleryImageManager::GetInstance()->AddImageInstanceUIDMap(pSeries->GetUID(), imageMap)){
                // log Warning
            } 
        }            
    }

    if(pSeries->GetModality() == "PT"){
        if(!TpsGalleryImageManager::GetInstance()->AddDataHeader(pSeries)){
            TPS_LOG_DEV_ERROR<<"Failed to add series to DataHeaderElement Map";
            return false;
        }
    }

    return true;
}

bool TpsGcaDataLoaderImpl::PrepareImageTextData_i(RTFWK::RTFWKDLDOSeries* const pSeries){

    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;

    if(nullptr == pSeries){
        TPS_LOG_DEV_ERROR<<"pSeries is null.";
        return false;
    }

    // init Freetype string builder
    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();

    std::string strbufText = "";
    //pan
    std::stringstream ssPan;
    int XOffset = 0;
    int YOffset = 0;
    ssPan<<"Pan: "<<"XOffset:"<<XOffset<<"|"<<"YOffset:"<<YOffset;
    if(!PrepareOneImageTextString_i(*pBufGen,ssPan.str(),"BevPan")){
        TPS_LOG_DEV_ERROR<<"Failed to PrepareOneImageTextString_i??"<<ssPan.str();
    }

    //Zoom
    std::stringstream ssZoom;
    float scale = 1.0f;
    ssZoom<<"Zoom: "<<scale;
    if(!PrepareOneImageTextString_i(*pBufGen,ssZoom.str(),"BevZoom")){
        TPS_LOG_DEV_ERROR<<"Failed to PrepareOneImageTextString_i??"<<ssZoom.str();
    }

    //windowing BEV
    std::stringstream ssWindowing;
    float wl = 800.0f;
    float ww = 1000.0f;
    ssWindowing<<"Windowing: "<<"WL"<<wl<<"|"<<"WW"<<ww;
    if(!PrepareOneImageTextString_i(*pBufGen,ssWindowing.str(),"BevWindowing")){
        TPS_LOG_DEV_ERROR<<"Failed to PrepareOneImageTextString_i??"<<ssWindowing.str();
    }

    // ruler text
    if(!PrepareOneImageTextString_i(*pBufGen, "0", "RulerText0")){
        TPS_LOG_DEV_WARNING<<"Failed to PrepareOneImageTextString_i:["<<strbufText<<"].";
    }

    if(!PrepareOneImageTextString_i(*pBufGen, "2", "RulerText1")){
        TPS_LOG_DEV_WARNING<<"Failed to PrepareOneImageTextString_i:["<<strbufText<<"].";
    }

    if(!PrepareOneImageTextString_i(*pBufGen, "4", "RulerText2")){
        TPS_LOG_DEV_WARNING<<"Failed to PrepareOneImageTextString_i:["<<strbufText<<"].";
    }

    if(!PrepareOneImageTextString_i(*pBufGen, "6", "RulerText3")){
        TPS_LOG_DEV_WARNING<<"Failed to PrepareOneImageTextString_i:["<<strbufText<<"].";
    }

    if(!PrepareOneImageTextString_i(*pBufGen, "8", "RulerText4")){
        TPS_LOG_DEV_WARNING<<"Failed to PrepareOneImageTextString_i:["<<strbufText<<"].";
    }

    if(!PrepareOneImageTextString_i(*pBufGen, "10", "RulerText5")){
        TPS_LOG_DEV_WARNING<<"Failed to PrepareOneImageTextString_i:["<<strbufText<<"].";
    }

    //TEST_PERFORMANCE_END("PrepareImageTextData_i")
    return true;
}

bool TpsGcaDataLoaderImpl::PrepareOneImageTextString_i(FTStringBufferGenerator& bufGen,
    const std::string& strText, const std::string& strKey){

        // create string text buffer
        //TEST_PERFORMANCE_INIT;
        //TEST_PERFORMANCE_BEGIN;
        Pixel32* pStringBuf = nullptr;
        int iStringBufWidth = 0;
        int iStringBufHeight = 0;

        if(strText.empty()){
            TPS_LOG_DEV_WARNING<<"Empty string. Skip creating the buffer for display.";
            return false;
        }

        // Convert string to wstring
        std::wstring wstrText(strText.begin(), strText.end());

        if(!bufGen.GenerateRasterBuffer(wstrText, &pStringBuf, 
            iStringBufWidth, iStringBufHeight)){
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
                return false;
        }

        // store the buffer into data manager
        if(!TpsGalleryImageManager::GetInstance()->AddImageTextData(strKey, 
            (unsigned char*)pStringBuf, iStringBufWidth, iStringBufHeight)){
                TPS_LOG_DEV_ERROR<<"Failed to add image text data into data manager";
                return false;
        }

        //TEST_PERFORMANCE_END("End of composing string buffer.");
        return true;
}

void TpsGcaDataLoaderImpl::SetDefaultImage_i(const std::vector<RTFWK::RTFWKDLDOSeries>& vecSeries){
    using namespace RTFWK;

    std::string sDefaultSeriesUID("");
    DATE_BOOST dateLatest(boost::date_time::min_date_time);
    TIME_BOOST timeLatest(boost::date_time::min_date_time);
    for(int i = 0; i < vecSeries.size(); ++i){
        DATE_BOOST date = vecSeries[i].GetSeriesDate();
        if(!date.is_not_a_date()){
            TIME_BOOST time = vecSeries[i].GetSeriesTime();
            if(date > dateLatest){
                dateLatest = date;
                if(!time.is_not_a_date_time()){
                    timeLatest = time;
                }
                sDefaultSeriesUID = vecSeries[i].GetUID();
            }
            else if(date == dateLatest){
                if(time >= timeLatest){
                    timeLatest = time;
                    sDefaultSeriesUID = vecSeries[i].GetUID();
                }
            }
            //else{
            //nothing
            //}
        }
        else{
            // Since 4D CT does not have a correct Series Date/Time, 
            // we need to set the default seriesUid the first one
            sDefaultSeriesUID = vecSeries[0].GetUID();
        }
    }

    if(!sDefaultSeriesUID.empty()){
        TpsGalleryImageManager::GetInstance()->SetDefaultImage3D(sDefaultSeriesUID);
    }
}

bool TpsGcaDataLoaderImpl::LoadCoursesOfPatient(const std::string& patientUID, std::vector<std::string>& vCourseUIDs)
{
    vCourseUIDs.clear();

    RTFWK::RTFWKDLDOPatient* currentPatient = nullptr;
    TpsGalleryPatientManager::GetInstance()->GetPatient(patientUID, currentPatient);
    if (currentPatient == nullptr) {
        TPS_LOG_DEV_ERROR<<"No Current Patient!";
        return false;
    }

    std::vector<RTFWK::RTFWKDLDOCourse*> vCourse;
    if (!currentPatient->GetCourseObjectList(vCourse)) {
        TPS_LOG_DEV_ERROR<<"Failed to Get Course for Patient :"<<patientUID;
        return false;
    }

    for(std::size_t i =0; i<vCourse.size(); ++i)
    {
        std::vector<std::string> vBeamGroupUIDs;
        if (!LoadBeamGroupUIDs(vCourse[i], vBeamGroupUIDs))
        {
            // log
            continue;
        }
        TpsGalleryPatientManager::GetInstance()->AddCourse(*vCourse[i]);
        vCourseUIDs.push_back(vCourse[i]->GetUID());
        TpsGalleryPatientManager::GetInstance()->SetBeamGroupUIDsByCourseUID(vCourse[i]->GetUID(), vBeamGroupUIDs);
    }      

    return true;
}

bool TpsGcaDataLoaderImpl::LoadImage2DData( const std::string& sImage2DUid ) {
    bool ret = true;

    TpsImage2DEntity *image2D = nullptr;
    ret = TpsGalleryImageManager::GetInstance()->GetImage2DData(sImage2DUid, &image2D);
    if (true == ret){
        TPS_LOG_DEV_INFO<<"Image2D which UID: "<<sImage2DUid
            <<" has already been loaded.";
        return ret;
    }

    return ret;
}   

bool TpsGcaDataLoaderImpl::LoadBeamGroupUIDs(const RTFWK::RTFWKDLDOCourse* pCourse, std::vector<std::string> &vBeamGroupUIDs){

    vBeamGroupUIDs.clear();
    std::vector<std::string> vPlanUIDs;
    if (!const_cast<RTFWK::RTFWKDLDOCourse*>(pCourse)->GetPlanUIDList(vPlanUIDs))
    {
        TPS_LOG_DEV_ERROR<<"Failed to GetPlanUIDList of Course: "<<pCourse->GetUID();
        return false;
    }
    vBeamGroupUIDs.insert(vBeamGroupUIDs.end(), vPlanUIDs.begin(), vPlanUIDs.end());
    std::vector<std::string> vDeliveryBeamGroupUIDs;
    if (!const_cast<RTFWK::RTFWKDLDOCourse*>(pCourse)->GetDeliverybeamgroupUIDList(vDeliveryBeamGroupUIDs))
    {
        TPS_LOG_DEV_ERROR<<"Failed to GetDeliverybeamgroupUIDList of Course: "<<pCourse->GetUID();
        return false;
    }
    vBeamGroupUIDs.insert(vBeamGroupUIDs.end(), vDeliveryBeamGroupUIDs.begin(), vDeliveryBeamGroupUIDs.end());
    return true;
}

TPS_END_NAMESPACE

