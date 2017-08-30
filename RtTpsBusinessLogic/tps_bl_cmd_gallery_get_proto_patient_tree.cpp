//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  yuxuan.duan
//
/// \file    tps_bl_cmd_gallery_get_proto_patient_tree.cpp
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/11/17
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_gallery_get_proto_patient_tree.h"

#include "boost/date_time/posix_time/posix_time.hpp"

//TPS
#include "RtTpsFramework/tps_fw_commandids.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"

//#include "RtTpsDataAccess/tps_da_dosemanager.h"
//#include "RtTpsDataAccess/tps_da_patientmanager.h"
//#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
//#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"
//#include "RtTpsFramework/tps_fw_voientity.h"
//#include "RtTpsDataAccess/tps_da_beammanager.h"
//#include "RtTpsDataAccess/tps_da_poimanager.h"
//#include "RtTpsDataAccess/tps_da_voimanager.h"
//#include "RtTpsDataAccess/tps_da_dataconvertor.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
//#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
//#include "RtTpsDataAccess/tps_da_optgoalmanager.h"

#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"


#include "RtTpsProtoLib/rt_tps_gallery.pb.h"

#include "tps_logger.h"

#pragma warning(disable:4702)

TPS_BEGIN_NAMESPACE // begin namespace tps

GetProtoPatientTreeForGalleryCmd::GetProtoPatientTreeForGalleryCmd(std::string patientUid) 
    : mPatientUid(patientUid)
{
    m_patientDataProto = new proto::RT_TPS_Gallery_PatientTreeData();
}

GetProtoPatientTreeForGalleryCmd::~GetProtoPatientTreeForGalleryCmd()
{
    if (m_patientDataProto!=nullptr) {
        delete m_patientDataProto;
    }
}

//void GetProtoPatientTreeForGalleryCmd::GetCourse(const std::string& courseUID, 
//    RtCourse** pCourse) const
//{  
//    mDataRepository->GetGalleryPatientManager()->GetCourse(courseUID , pCourse);
//}

int GetProtoPatientTreeForGalleryCmd::Execute()
{
    //proto::RT_TPS_Proto* proto = nullptr;
    //proto::RT_TPS_Gallery_Course* protoCourse = nullptr;    
    //proto::RT_TPS_Gallery_Series* protoSeries = nullptr;
    //TpsImage3DEntity* pImage3DEntity = nullptr;

    proto::RT_TPS_Gallery_Patient* protoPatient = nullptr;
    proto::RT_TPS_Gallery_Course* protoCourse = nullptr;
    proto::RT_TPS_Gallery_Series* protoSeries = nullptr;
    proto::RT_TPS_Gallery_SeriesGroup* protoSeriesGroup = nullptr;

    std::vector<RtCourse*> courseList;
    std::vector<TpsImage3DEntity*> image3DEntityList;

    //proto = m_coursesDataProto->mutable_protoinfo();
    //proto->set_parent_uid(mPatientUid);

    //GetPatient
    protoPatient = m_patientDataProto->mutable_patient();
    protoPatient->set_patientuid(mPatientUid);
    RtPatient* currentPatient = nullptr;
    currentPatient = mDataRepository->GetGalleryPatientManager()->GetPatient();
    if(currentPatient != nullptr){
        protoPatient->set_patientnameprefix(currentPatient->get_nameprefix());
        protoPatient->set_patientfirstname(currentPatient->get_firstname());
        protoPatient->set_patientmiddlename(currentPatient->get_middlename());
        protoPatient->set_patientlastname(currentPatient->get_lastname());
        protoPatient->set_patientnamesuffix(currentPatient->get_namesuffix());
        protoPatient->set_patientid(currentPatient->get_patientid());
        protoPatient->set_patientsex(currentPatient->get_patientsex());
        if(!currentPatient->get_patientbirthdate().is_not_a_date()){
            std::string dateOfBirth = to_iso_string(currentPatient->get_patientbirthdate());
            protoPatient->set_patientdateofbirth(dateOfBirth);
        }
    }

    // GetSeries
    std::vector<std::string> seriesUIDList = std::move(mDataRepository->GetGalleryImageManager()->GetSeriesUidListByPatientUid(mPatientUid));
    TpsImage3DEntity* image3DEntity = nullptr;
    for (auto itSeriesUID = seriesUIDList.cbegin(); itSeriesUID != seriesUIDList.cend(); ++itSeriesUID) 
    {
        if (!mDataRepository->GetGalleryImageManager()->GetImage3DData(*itSeriesUID, &image3DEntity) || image3DEntity == nullptr) 
        {
            TPS_LOG_DEV_ERROR<<"Failed to get image3DData["<<*itSeriesUID<<"] from imageDataManager";
            return false;
        }

        image3DEntityList.push_back(image3DEntity);
    }

    for (auto itImage3DEntity = image3DEntityList.cbegin(); itImage3DEntity != image3DEntityList.cend(); ++itImage3DEntity) {
        protoSeries = m_patientDataProto->add_serieslist();
        ConvertImage3DEntityToProto(*itImage3DEntity, protoSeries);
    }

    //GetCourseList
    mDataRepository->GetGalleryPatientManager()->GetCourseListByPatientUID(mPatientUid, courseList);
    for (auto itCourse = courseList.cbegin(); itCourse != courseList.cend(); ++itCourse)
    {
        protoCourse = m_patientDataProto->add_courselist();
        ConvertCourseToProto(*itCourse, protoCourse);

        image3DEntityList.clear();

        ////GetSeriesList
        //std::vector<std::string> seriesUIDList = std::move(mDataRepository->GetGalleryImageManager()->GetSeriesUidListViaCourseUid((*itCourse)->get_uid()));
        //TpsImage3DEntity* image3DEntity = nullptr;
        //for (auto itSeriesUID = seriesUIDList.cbegin(); itSeriesUID != seriesUIDList.cend(); ++itSeriesUID) 
        //{
        //    if (!mDataRepository->GetGalleryImageManager()->GetImage3DData(*itSeriesUID, &image3DEntity) || image3DEntity == nullptr) {
        //        TPS_LOG_DEV_ERROR<<"Failed to get image3DData["<<*itSeriesUID<<"] from imageDataManager";
        //        return false;
        //    }

        //    image3DEntityList.push_back(image3DEntity);
        //}

        //for (auto itImage3DEntity = image3DEntityList.cbegin(); itImage3DEntity != image3DEntityList.cend(); ++itImage3DEntity)
        //{
        //    protoSeries = m_patientDataProto->add_serieslist();
        //    ConvertImage3DEntityToProto((*itCourse)->get_uid(), *itImage3DEntity, protoSeries);
        //}

    }

    // GetSeriesGroup
    TpsGalleryImageManager* pGalleryImageManager = mDataRepository->GetGalleryImageManager();
    if (pGalleryImageManager != nullptr)
    {
        std::vector<std::shared_ptr<TpsSeriesGroup4d>> vecSeriesGroup;
        pGalleryImageManager->GetAllSeriesGroup(vecSeriesGroup);
        for (int i = 0; i < vecSeriesGroup.size(); i++)
        {
            protoSeriesGroup = m_patientDataProto->add_seriesgroup();
            ConvertSereisGroupToProto(vecSeriesGroup[i].get(), protoSeriesGroup);
        }
    }

    //RtCourse* pCourse = nullptr;
    //std::vector<std::string> courseUIDList;
    ////std::vector<RtCourse*> courseList;
    //mDataRepository->GetGalleryPatientManager()->GetCourseUIDListByPatientUID(mPatientUid, courseUIDList);
    //for (auto it = courseUIDList.begin(); it != courseUIDList.end(); ++it)
    //{
    //    this->GetCourse(*it , &pCourse);
    //    if (pCourse == nullptr)
    //    {
    //        TPS_LOG_DEV_INFO<<"The Course["<<*it<<"] does not exist.";
    //        return TPS_ER_FAILURE;
    //    }

    //    protoCourse = m_coursesDataProto->add_courses();
    //    protoCourse->set_name(pCourse->get_coursename());
    //    protoCourse->set_uid(pCourse->get_uid());
    //    protoCourse->set_hasbeamgroup(true); // default value;
    //    std::vector<std::string> vBeamGroupUIDs;
    //    if (mDataRepository->GetGalleryPatientManager()->GetBeamGroupUIDsByCourseUID(pCourse->get_uid(), vBeamGroupUIDs))
    //    {
    //        if (vBeamGroupUIDs.size()==0)
    //        {
    //            protoCourse->set_hasbeamgroup(false);
    //        }
    //    }

    //    //std::vector<std::string> vSeriesUIDList;
    //    //mDataRepository->GetGalleryImageManager()->GetSeriesUIDListByCourseUID(pCourse->get_uid(), vSeriesUIDList);

    //    //直接从数据库里读取与原用意不符，应该从patientmanager中读取，但暂无相关接口
    //    std::vector<RtSeries*> vecSeriesList;
    //    mDatabaseWrapper->GetSeriesListByCourseUid(pCourse->get_uid(), vecSeriesList);
    //    
    //    for(auto seriesIt = vecSeriesList.begin(); seriesIt != vecSeriesList.end(); ++seriesIt)
    //    {
    //        protoSeries = protoCourse->add_serieslist();

    //        mDataRepository->GetGalleryImageManager()->GetImage3DData((*seriesIt)->get_uid(), &pImage3DEntity);

    //        //To do need to correct
    //        RtSeries* pSeries = pImage3DEntity->GetRtSeries();

    //        protoSeries->set_seriesuid(pSeries->get_uid()/*pImage3DEntity->GetUID()*/);
    //        protoSeries->set_modality(pSeries->get_modality()/*pImage3DEntity->get()->m_Header.m_cModality*/);
    //        protoSeries->set_patientposition(pSeries->get_patientposition()/*pImage3DEntity->get()->m_Header.m_cPatientPosition*/);
    //        protoSeries->set_description(pSeries->get_seriesdescription()/*pImage3DEntity->GetSeriesDescription()*/);
    //        protoSeries->set_slicethickness(pSeries->get_slicethickness());
    //        protoSeries->set_studyid(pSeries->get_studyid());
    //        protoSeries->set_studydescription(pSeries->get_studydescription());
    //        protoSeries->set_patientname(pSeries->get_patientname());
    //        protoSeries->set_patientid(pSeries->get_patientid());
    //        protoSeries->set_patientbirthday(pSeries->get_patientbirthdate());
    //        protoSeries->set_patientage(pSeries->get_patientage());
    //        protoSeries->set_patientsex(pSeries->get_patientsex());
    //        protoSeries->set_institutionname(pSeries->get_institutionname());
    //        protoSeries->set_manufacturer(pSeries->get_manufacturer());

    //        std::stringstream stream;

    //        DATE_BOOST SeriesDate = pSeries->get_seriesdate();
    //        if(!SeriesDate.is_not_a_date()){
    //            stream<<SeriesDate.month()<<"/"
    //                <<SeriesDate.day()<<"/"
    //                <<SeriesDate.year();
    //            protoSeries->set_seriesdate(stream.str());
    //        }

    //        TIME_BOOST SeriesTime = pSeries->get_seriestime();
    //        if(!SeriesTime.is_not_a_date_time()){
    //            stream.str("");
    //            stream<<SeriesTime.hours()<<":"
    //                <<SeriesTime.minutes();
    //            protoSeries->set_seriestime(stream.str());
    //        }

    //        RtImage3DHeader *header = nullptr;
    //        pImage3DEntity->GetImage3DHeader(header);

    //        protoSeries->set_slicecount(header->m_iSliceCount/*pImage3DEntity->get()->m_Header.m_iSliceCount*/);
    //        protoSeries->set_slicespacing(header->m_fSliceSpacing/*pImage3DEntity->get()->m_Header.m_fSliceSpacing*/);
    //        protoSeries->set_windowingcenter(header->m_iWindowCenter/*pImage3DEntity->get()->m_Header.m_iWindowCenter*/);
    //        protoSeries->set_windowingwidth(header->m_iWindowWidth/*pImage3DEntity->get()->m_Header.m_iWindowWidth*/);
    //        protoSeries->set_xdimension(header->m_iXDim/*pImage3DEntity->get()->m_Header.m_iXDim*/);
    //        protoSeries->set_ydimension(header->m_iYDim/*pImage3DEntity->get()->m_Header.m_iYDim*/);
    //        protoSeries->set_xpixelspacing(header->m_dXSize/*pImage3DEntity->get()->m_Header.m_dXSize*/);
    //        protoSeries->set_ypixelspacing(header->m_dYSize/*pImage3DEntity->get()->m_Header.m_dYSize*/);
    //        protoSeries->set_imageslicebuffer((void*)pImage3DEntity->GetSliceBuffer(), header->m_iXDim * header->m_iYDim /*pImage3DEntity->get()->m_Header.m_iXDim * pImage3DEntity->get()->m_Header.m_iYDim*/);
    //        protoSeries->set_iscopied(pImage3DEntity->GetIsQACopied());

    //        //RtSeries* pSeries = nullptr;
    //        //mDataRepository->GetGalleryImageManager()->GetImageSeries(pImage3DEntity->get()->GetSeriesUID(), &pSeries);
    //        if (nullptr != pSeries) 
    //        {    
    //            protoSeries->set_seriesnumber(pSeries->get_seriesnumber());
    //            protoSeries->set_frameofreferenceuid(pSeries->get_frameofreferenceuid());
    //            protoSeries->set_manufacturermodelname(pSeries->get_manufacturermodelname());
    //            //std::string sCT2DensityUID = pSeries->get_ct2densityuid(); 
    //            //if (!sCT2DensityUID.empty()) 
    //            //{
    //            //    RtCt2density *oCT2Density;
    //            //    //RTFWK::RTFWKDLDOCT2DensityMap oCT2Density(false);
    //            //    //oCT2Density.set_uid(sCT2DensityUID);
    //            //    //if (RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&oCT2Density))
    //            //    //{
    //            //    //    protoSeries->set_ct2densitytablename(oCT2Density.get_tablename());
    //            //    //}

    //            //    //GetCT2DensityTableByUid可能有问题，ToDo byDYX
    //            //    oCT2Density = mDataRepository->GetGalleryImageManager()->GetCT2DensityTableByUid(sCT2DensityUID);
    //            //    if(oCT2Density != nullptr)
    //            //    {
    //            //        protoSeries->set_ct2densitytablename(oCT2Density->get_tablename());
    //            //    }
    //            //}
    //        }

    //        bool hasVOI = true;
    //        std::vector<RtVoi*> vecVoiList;
    //        mDatabaseWrapper->GetVoiListBySeriesUid(pSeries->get_uid(), vecVoiList);
    //        if (vecVoiList.size() == 0)
    //        {
    //            hasVOI = false;
    //        }
    //        protoSeries->set_hasvoi(hasVOI);

    //        //std::vector<std::string> vVOIUIDs = pImage3DEntity->GetVOIUIDList();
    //        //// 如果调用Get方法失败，默认有voi 
    //        //if (vVOIUIDs.size()==0) 
    //        //{
    //        //    hasVOI = false;
    //        //}
    //        //protoSeries->set_hasvoi(hasVOI);

    //        //std::string sectionType ="";
    //        //if (TpsGalleryImageManager::GetInstance()->GetSectionType(*seriesIt, sectionType)){
    //        //    protoSeries->set_sectiontype(sectionType);
    //        //} else {
    //        //    TPS_LOG_DEV_INFO<<"Failed to Get SectionType for Series : "<<*seriesIt;
    //        //}
    //    }
    //    
    //}

    return TPS_ER_SUCCESS;
}

bool GetProtoPatientTreeForGalleryCmd::ConvertCourseToProto(const RtCourse* course, 
    proto::RT_TPS_Gallery_Course* protoCourse) 
{
    if (course==nullptr || protoCourse==nullptr) {
        TPS_LOG_DEV_ERROR<<"course==nullptr || protoCourse==nullptr";
        return false;
    }
    protoCourse->set_courseuid(course->get_uid());
    protoCourse->set_coursename(course->get_coursename());
    protoCourse->set_patientuid(course->get_patientuid());
    protoCourse->set_hasbeamgroup(true); // default value;
    std::vector<std::string> vBeamGroupUIDs;
    if (mDataRepository->GetGalleryPatientManager()->GetBeamGroupUIDsByCourseUID(course->get_uid(), vBeamGroupUIDs))
    {
        if (vBeamGroupUIDs.size()==0)
        {
            protoCourse->set_hasbeamgroup(false);
        }
    }
    return true;
}

bool GetProtoPatientTreeForGalleryCmd::ConvertImage3DEntityToProto(TpsImage3DEntity* image3DEntity, 
    proto::RT_TPS_Gallery_Series* protoSeries) 
{
    if (image3DEntity==nullptr || protoSeries==nullptr) {
        TPS_LOG_DEV_ERROR<<"image3DEntity==nullptr || protoSeries==nullptr";
        return false;
    }

    RtSeries* pSeries = image3DEntity->GetRtSeries();
    if (pSeries == nullptr) {
        TPS_LOG_DEV_ERROR<<"series is null";
        return false;
    }

    protoSeries->set_seriesuid(pSeries->get_uid());
    protoSeries->set_modality(pSeries->get_modality());
    protoSeries->set_patientposition(pSeries->get_patientposition());
    protoSeries->set_seriesdescription(pSeries->get_seriesdescription());
    protoSeries->set_slicethickness(pSeries->get_slicethickness());
    protoSeries->set_seriesnumber(pSeries->get_seriesnumber());
    protoSeries->set_frameofreferenceuid(pSeries->get_frameofreferenceuid());
    protoSeries->set_manufacturermodelname(pSeries->get_manufacturermodelname());

    protoSeries->set_studyid(pSeries->get_studyid());
    protoSeries->set_studydescription(pSeries->get_studydescription());

    protoSeries->set_patientname(pSeries->get_patientname());
    protoSeries->set_patientid(pSeries->get_patientid());
    protoSeries->set_patientbirthday(pSeries->get_patientbirthdate());
    protoSeries->set_patientage(pSeries->get_patientage());
    protoSeries->set_patientsex(pSeries->get_patientsex());
    protoSeries->set_institutionname(pSeries->get_institutionname());
    protoSeries->set_manufacturer(pSeries->get_manufacturer());

    protoSeries->set_groupuid(image3DEntity->GetGroupUid());
    protoSeries->set_iscopied(image3DEntity->GetIsQACopied());

    RtImage3DHeader *header = nullptr;
    image3DEntity->GetImage3DHeader(header);
    if(header != nullptr) {
        protoSeries->set_slicecount(header->m_iSliceCount);
        protoSeries->set_slicespacing(header->m_fSliceSpacing);
        protoSeries->set_windowingcenter(header->m_iWindowCenter);
        protoSeries->set_windowingwidth(header->m_iWindowWidth);
        protoSeries->set_xdimension(header->m_iXDim);
        protoSeries->set_ydimension(header->m_iYDim);
        protoSeries->set_xpixelspacing(header->m_dXSize);
        protoSeries->set_ypixelspacing(header->m_dYSize);
        protoSeries->set_imageslicebuffer(image3DEntity->GetSliceBuffer(), header->m_iXDim * header->m_iYDim);
    }

    std::stringstream streamDate, streamTime;
    DATE_BOOST SeriesDate = pSeries->get_seriesdate();
    if(!SeriesDate.is_not_a_date()){
        streamDate<<SeriesDate.month()<<"/"
            <<SeriesDate.day()<<"/"
            <<SeriesDate.year();
        protoSeries->set_seriesdate(streamDate.str());
    }

    TIME_BOOST SeriesTime = pSeries->get_seriestime();
    if(!SeriesTime.is_not_a_date_time()){
        streamTime.str("");
        streamTime<<SeriesTime.hours()<<":"
            <<SeriesTime.minutes()<<":"
            <<SeriesTime.seconds();
        protoSeries->set_seriestime(streamTime.str());
    }

    bool hasVOI = true;
    std::vector<RtVoi*> vecVoiList;
    mDatabaseWrapper->GetVoiListBySeriesUid(pSeries->get_uid(), vecVoiList);
    if (vecVoiList.size() == 0)
    {
        hasVOI = false;
    }
    protoSeries->set_hasvoi(hasVOI);

    return true;
}

bool GetProtoPatientTreeForGalleryCmd::ConvertSereisGroupToProto(const TpsSeriesGroup4d* pSeriesGroup, 
    proto::RT_TPS_Gallery_SeriesGroup* protoSeriesGroup) 
{
    if (pSeriesGroup == nullptr || protoSeriesGroup == nullptr) {
        TPS_LOG_DEV_ERROR<<"SeriesGroup == nullptr || protoSereisGroup == nullptr";
        return false;
    }
    protoSeriesGroup->set_groupuid(pSeriesGroup->m_strGroupUid);
    protoSeriesGroup->set_groupname(pSeriesGroup->m_strGroupName);
    protoSeriesGroup->set_mainseries(pSeriesGroup->m_strMainSeriesUid);
    for (int i = 0; i < pSeriesGroup->m_vecSeriesUid.size(); i++)
        protoSeriesGroup->add_serieslist(pSeriesGroup->m_vecSeriesUid[i]);
    return true;
}

TPS_END_NAMESPACE
