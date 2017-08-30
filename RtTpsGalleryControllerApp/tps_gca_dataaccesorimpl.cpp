////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_dataaccesorimpl.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_gca_dataaccessorimpl.h"

//TMS
#include "tms/rt_fwk_dl_access.h"
#include "tms/rt_fwk_dl_object_image3d.h"
#include "tms/rt_fwk_dl_object_course.h"
#include "tms/rt_fwk_dl_object_image3d.h"
#include "tms/rt_fwk_dl_object_series.h"

// TPS
#include "tps_logger.h"
#include "RtTpsProtoLib/rt_tps_certifiedimage3d.h"
#include "RtTpsProtoLib/rt_tps_gallery_course.pb.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_image3dentity.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"

// dataModel
#include "tps_gca_datamodel_editcourse.h"
#include "tps_gca_datamodel_deleteseries.h"
#include "tps_gca_datamodel_expandseries.h"
#include "tps_gca_datamodel_deleteslices.h"

TPS_BEGIN_NAMESPACE

    bool TpsGcaDataAccessorImpl::Initialize(){
        return true;
}

bool TpsGcaDataAccessorImpl::Finalize(){
    return true;
}

void TpsGcaDataAccessorImpl::GetCourse(const std::string& courseUID, 
    RTFWK::RTFWKDLDOCourse ** pCourse) const{  
        TpsGalleryPatientManager::GetInstance()->GetCourse(courseUID , pCourse);
}

void TpsGcaDataAccessorImpl::GetCurrentSeriesUID(std::string& sCurSerUID) const{
    sCurSerUID = TpsGalleryImageManager::GetInstance()->GetCurrentImage3DUID();
}

bool TpsGcaDataAccessorImpl::GetSeriesUIDList(const std::string& sPatientUID,
    std::vector<std::string>* pSerUIDList) const{
        return TpsGalleryImageManager::GetInstance()->GetSeriesUIDList(sPatientUID, pSerUIDList);
}

bool TpsGcaDataAccessorImpl::GetSeriesByUID(const std::string& serUID, 
    RTFWK::RTFWKDLDOSeries** pDLDOSeris) const{
        return TpsGalleryImageManager::GetInstance()->GetImageSeries(serUID, pDLDOSeris);
}

bool TpsGcaDataAccessorImpl::GetImage3DHeader(const std::string& sSeriesUID,
    RTFWK::RTFWKDLDOImage3DHeader** pImage3DHeader) const{
        TpsImage3DEntity* pImage3DEntity = nullptr;
        if (TpsGalleryImageManager::GetInstance()->GetImage3DData(sSeriesUID, &pImage3DEntity) &&
            pImage3DEntity != nullptr){
                if (pImage3DEntity->GetImage3DHeader(*pImage3DHeader) && 
                    *pImage3DHeader!=nullptr){
                        return true;
                }
                else{
                    TPS_LOG_DEV_ERROR<<"pImage3DHeader is null pointer or failed to GetImage3DHeader \
                                       from TpsImage3DEntity! seriesUID["<<sSeriesUID<<"]";
                }
        }
        else{
            TPS_LOG_DEV_ERROR<<"pImage3DData is null or Failed to GetImage3DData \
                               from TpsGalleryImageManager. seriesUID["<<sSeriesUID<<"]";
        }
        *pImage3DHeader = nullptr;
        return false;
}

bool TpsGcaDataAccessorImpl::GetCertifiedImage3DEntity(const std::string& image3DUID, 
    CertifiedImage3D& ceritifiedImage3DEntity){

        TpsImage3DEntity* image3DEntity = nullptr;
        if(!TpsGalleryImageManager::GetInstance()->GetImage3DData(image3DUID, &image3DEntity)){
            return false;
        }

        ceritifiedImage3DEntity.mUID = image3DEntity->get()->GetSeriesUID();
        ceritifiedImage3DEntity.mModality = image3DEntity->get()->m_Header.m_cModality;
        ceritifiedImage3DEntity.mPatientPosition = image3DEntity->get()->m_Header.m_cPatientPosition;

        std::stringstream stream;

        if(!image3DEntity->get()->m_Header.m_dSeriesDate.is_not_a_date()){
            stream<<image3DEntity->get()->m_Header.m_dSeriesDate.month()<<"/"
                <<image3DEntity->get()->m_Header.m_dSeriesDate.day()<<"/"
                <<image3DEntity->get()->m_Header.m_dSeriesDate.year();
            ceritifiedImage3DEntity.mSeriesDate = stream.str();
        }

        if(!image3DEntity->get()->m_Header.m_tSeriesTime.is_not_a_date_time()){
            stream.str("");
            stream<<image3DEntity->get()->m_Header.m_tSeriesTime.hours()<<":"
                <<image3DEntity->get()->m_Header.m_tSeriesTime.minutes();
            ceritifiedImage3DEntity.mSeriesTime = stream.str();
        }

        ceritifiedImage3DEntity.mDescription = image3DEntity->GetSeriesDescription();
        ceritifiedImage3DEntity.mSliceCount = image3DEntity->get()->m_Header.m_iSliceCount;
        ceritifiedImage3DEntity.mSliceSpacing = image3DEntity->get()->m_Header.m_fSliceSpacing;
        ceritifiedImage3DEntity.mWindowCenter = image3DEntity->get()->m_Header.m_iWindowCenter;
        ceritifiedImage3DEntity.mWindowWidth = image3DEntity->get()->m_Header.m_iWindowWidth;
        ceritifiedImage3DEntity.mXDim = image3DEntity->get()->m_Header.m_iXDim;
        ceritifiedImage3DEntity.mYDim = image3DEntity->get()->m_Header.m_iYDim;
        ceritifiedImage3DEntity.mXSize = image3DEntity->get()->m_Header.m_dXSize;
        ceritifiedImage3DEntity.mYSize = image3DEntity->get()->m_Header.m_dYSize;
        ceritifiedImage3DEntity.mSliceBuffer = image3DEntity->GetSliceBuffer();

        return true;
}

void TpsGcaDataAccessorImpl::GetCourseListByPatientUID(const std::string& sPatientUID, proto::RT_TPS_Gallery_Courses &courses)
{
    std::vector<std::string> courseUIDList;
    TpsGalleryPatientManager::GetInstance()->GetCourseListByPatientUID(sPatientUID, &courseUIDList);

    RTFWK::RTFWKDLDOCourse* pCourse = nullptr;

    proto::RT_TPS_Gallery_Course* protoCourse = nullptr;
    proto::RT_TPS_Proto* proto = nullptr;
    proto::RT_TPS_Gallery_Series* protoSeries = nullptr;
    TpsImage3DEntity* pImage3DEntity = nullptr;
    proto = courses.mutable_protoinfo();
    proto->set_parent_uid(sPatientUID);

    for (auto it = courseUIDList.begin(); it != courseUIDList.end(); ++it){
        this->GetCourse(*it , &pCourse);
        if (pCourse == nullptr){
            TPS_LOG_DEV_INFO<<"The Course["<<*it<<"] does not exist.";
            return;
        }

        protoCourse = courses.add_courses();
        protoCourse->set_name(pCourse->GetCourseName());
        protoCourse->set_uid(pCourse->GetUID());
        protoCourse->set_hasbeamgroup(true); // default value;
        std::vector<std::string> vBeamGroupUIDs;
        if (TpsGalleryPatientManager::GetInstance()->GetBeamGroupUIDsByCourseUID(pCourse->GetUID(), vBeamGroupUIDs)){
            if (vBeamGroupUIDs.size()==0){
                protoCourse->set_hasbeamgroup(false);
            }
        } 

        std::vector<std::string> vSeriesUIDList;
        if (TpsGalleryPatientManager::GetInstance()->GetSeriesListByCourseUID(pCourse->GetUID(), &vSeriesUIDList))
        {
            for(auto seriesIt = vSeriesUIDList.begin(); seriesIt != vSeriesUIDList.end(); ++seriesIt){
                protoSeries = protoCourse->add_serieslist();

                TpsGalleryImageManager::GetInstance()->GetImage3DData(*seriesIt, &pImage3DEntity);
                protoSeries->set_seriesuid(pImage3DEntity->GetUID());
                protoSeries->set_modality(pImage3DEntity->get()->m_Header.m_cModality);
                protoSeries->set_patientposition(pImage3DEntity->get()->m_Header.m_cPatientPosition);

                std::stringstream stream;
                if(!pImage3DEntity->get()->m_Header.m_dSeriesDate.is_not_a_date()){
                    stream<<pImage3DEntity->get()->m_Header.m_dSeriesDate.month()<<"/"
                        <<pImage3DEntity->get()->m_Header.m_dSeriesDate.day()<<"/"
                        <<pImage3DEntity->get()->m_Header.m_dSeriesDate.year();
                    protoSeries->set_seriesdate(stream.str());
                }

                if(!pImage3DEntity->get()->m_Header.m_tSeriesTime.is_not_a_date_time()){
                    stream.str("");
                    stream<<pImage3DEntity->get()->m_Header.m_tSeriesTime.hours()<<":"
                        <<pImage3DEntity->get()->m_Header.m_tSeriesTime.minutes();
                    protoSeries->set_seriestime(stream.str());
                }

                protoSeries->set_description(pImage3DEntity->GetSeriesDescription());
                protoSeries->set_slicecount(pImage3DEntity->get()->m_Header.m_iSliceCount);
                protoSeries->set_slicespacing(pImage3DEntity->get()->m_Header.m_fSliceSpacing);
                protoSeries->set_windowingcenter(pImage3DEntity->get()->m_Header.m_iWindowCenter);
                protoSeries->set_windowingwidth(pImage3DEntity->get()->m_Header.m_iWindowWidth);
                protoSeries->set_xdimension(pImage3DEntity->get()->m_Header.m_iXDim);
                protoSeries->set_ydimension(pImage3DEntity->get()->m_Header.m_iYDim);

                protoSeries->set_xpixelspacing(pImage3DEntity->get()->m_Header.m_dXSize);
                protoSeries->set_ypixelspacing(pImage3DEntity->get()->m_Header.m_dYSize);
                protoSeries->set_imageslicebuffer((void*)pImage3DEntity->GetSliceBuffer(), pImage3DEntity->get()->m_Header.m_iXDim * pImage3DEntity->get()->m_Header.m_iYDim);
                protoSeries->set_iscopied(pImage3DEntity->GetIsQACopied());
                RTFWK::RTFWKDLDOSeries* pSeries = nullptr;
                TpsGalleryImageManager::GetInstance()->GetImageSeries(pImage3DEntity->get()->GetSeriesUID(), &pSeries);
                if (nullptr != pSeries)
                {    
                    protoSeries->set_seriesnumber(pSeries->GetSeriesNumber());
                    protoSeries->set_frameofreferenceuid(pSeries->GetFrameOfReferenceUID());
                    protoSeries->set_manufacturermodelname(pSeries->GetManufacturerModelName());
                }
                bool hasVOI = true;
                std::vector<std::string> vVOIUIDs;
                if (pSeries->GetVOIUIDList(vVOIUIDs))  // 如果调用Get方法失败，默认有voi
                {
                    if (vVOIUIDs.size()==0)
                    {
                        hasVOI = false;
                    }
                }
                protoSeries->set_hasvoi(hasVOI);
            }
        }
    }
}
   
bool TpsGcaDataAccessorImpl::EditCourse(proto::RT_TPS_Gallery_EditCourse protoEditCourse)
{
    TpsGcaDmEditCourse editCourse;
    editCourse.m_sPatientUID = protoEditCourse.patientuid();
    for (int i=0; i<protoEditCourse.addedcourses_size(); ++i)
    {
        auto protoItem = protoEditCourse.addedcourses(i);
        TpsGcaDmEditCourse::Course course;
        course.m_sName = protoItem.name();
        course.m_sUID = protoItem.uid();
        for (int j=0; j<protoItem.addedseriesuids_size(); ++j)
        {
            course.m_vAddedSeriesUIDs.push_back(protoItem.addedseriesuids(j));
        }
        for (int j=0; j<protoItem.removedseriesuids_size(); ++j)
        {
            course.m_vRemovedSeriesUIDs.push_back(protoItem.removedseriesuids(j));
        }
        editCourse.m_vAddedCourses.push_back(course);
    }

    for (int i=0; i<protoEditCourse.deletedcourses_size(); ++i)
    {
        auto protoItem = protoEditCourse.deletedcourses(i);
        TpsGcaDmEditCourse::Course course;
        course.m_sName = protoItem.name();
        course.m_sUID = protoItem.uid();
        for (int j=0; j<protoItem.addedseriesuids_size(); ++j)
        {
            course.m_vAddedSeriesUIDs.push_back(protoItem.addedseriesuids(j));
        }
        for (int j=0; j<protoItem.removedseriesuids_size(); ++j)
        {
            course.m_vRemovedSeriesUIDs.push_back(protoItem.removedseriesuids(j));
        }
        editCourse.m_vDeletedCourses.push_back(course);
    }

    for (int i=0; i<protoEditCourse.updatedcourses_size(); ++i)
    {
        auto protoItem = protoEditCourse.updatedcourses(i);
        TpsGcaDmEditCourse::Course course;
        course.m_sName = protoItem.name();
        course.m_sUID = protoItem.uid();
        for (int j=0; j<protoItem.addedseriesuids_size(); ++j)
        {
            course.m_vAddedSeriesUIDs.push_back(protoItem.addedseriesuids(j));
        }
        for (int j=0; j<protoItem.removedseriesuids_size(); ++j)
        {
            course.m_vRemovedSeriesUIDs.push_back(protoItem.removedseriesuids(j));
        }
        editCourse.m_vUpdatedCourses.push_back(course);
    }

    bool bRet = RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(&editCourse); 

    return bRet;

}   

bool TpsGcaDataAccessorImpl::DeleteSeries(const std::string &sSeriesUID){

    if (sSeriesUID.empty()){
        TPS_LOG_DEV_ERROR<<"[DeleteSeries] : seriesUID is empty.";
        return false;
    }
    TpsGcaDmDeleteSeries dmDeleteSeries(sSeriesUID);
    if (!RTFWK::RTFWKDLDataAccess::GetInstance()->DeleteObjectInDB(&dmDeleteSeries)){
        TPS_LOG_DEV_ERROR<<"[DeleteSeries] : Failed to Delete DmDeleteSeries.";
        return false;
    }
    return true;
}

bool TpsGcaDataAccessorImpl::ExpandSeries(proto::RT_TPS_Gallery_ExpandSeries protoExpandSeries){

    TpsGcaDmExpandSeries expandSeries(protoExpandSeries.seriesuid(), protoExpandSeries.slicenumber(),
        protoExpandSeries.isexpandbottom(), protoExpandSeries.issaveasnewseries()); 

    if (!RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(&expandSeries)){
        TPS_LOG_DEV_ERROR<<"[ExpandSeries] : Failed to Save DmExpandSeries.";
        return false;
    }  
    return true;
}

bool TpsGcaDataAccessorImpl::DeleteSlices(const std::string &sSeriesUID, const bool& isSaveAsNewSeries) {

    if (sSeriesUID.empty()){
        TPS_LOG_DEV_ERROR<<"[DeleteSlices] : seriesUID is empty.";
        return false;
    }          
    // Prepare Data
    std::vector<std::string> vImageUIDs;
    if (!TpsGalleryImageManager::GetInstance()->GetDeletedImageUIDs(sSeriesUID, vImageUIDs)){
        TPS_LOG_DEV_ERROR<<"[DeleteSlices] : Failed to Get DeletedImageUIDs.";
        return false;
    }
    if (vImageUIDs.size() == 0) {
        TPS_LOG_DEV_ERROR<<"[DeleteSlices] : DeletedImageUIDs is empty.";
        return false;
    }              
    TpsGcaDmDeleteSlices dmDeleteSlices(sSeriesUID, vImageUIDs, isSaveAsNewSeries);
    if (!RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(&dmDeleteSlices)){
        TPS_LOG_DEV_ERROR<<"[DeleteSlices] : Failed to Save DmDeleteSlices.";
        return false;
    }
    return true;
}

TPS_END_NAMESPACE

