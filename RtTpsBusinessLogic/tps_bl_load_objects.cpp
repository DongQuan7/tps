//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright,(c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ChenXi xi.chen@united-imaging.com
///
///  \file    tps_bl_load_objects.cpp
///  \brief   load data from DB
///
///  \version 1.0
///  \date    Oct. 21, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_load_objects.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"

// c++ header
#include <cctype>                                                   // for string toupper

#include "boost/algorithm/string.hpp"
#include "boost/exception/all.hpp"
#include "boost/bind/bind.hpp"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfDicomConvertor/mcsf_dicom_convertor.h"                // for header declaration
#include "McsfDicomConvertor/mcsf_dicom_convertor_factory.h"
#include "Common\McsfSystemEnvironmentConfig\mcsf_systemenvironment_factory.h"
//Third Part
#include "boost/serialization/access.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/vector.hpp"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>
using namespace std;
using namespace boost::property_tree;

// TPS
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_course.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_template.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_imagetransformation.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_approverecord.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessorychunk.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_constraint.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_drrsetting.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_site.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"
#include "RtTpsDataAccess/tps_da_optgoalmanager.h"
#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"

#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tcpntcpmanager.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedunit.h"

#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include "RtTpsFramework/tps_fw_voistructure.h"
#include "RtTpsFramework/tps_fw_beamstructure.h"
#include "RtTpsFramework/tps_fw_optimizer.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"
#include "tps_logger.h"

#include <process.h>

#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"


#define  POI_TYPE_NUM 6

const static std::string poiType[POI_TYPE_NUM] =
{
    "POI_UNKNOWN",
    "Setup",
    "NormalPoint",
    "IsoCenter",
    "Calculation",
    "Max"
};



TPS_BEGIN_NAMESPACE
    bool SaveToTrueColorBitmap(const std::string& path,
    unsigned char* buffer,
    const int& height,
    const int& width);

TpsLoadObjects::TpsLoadObjects(void)
    :mDatabaseWrapper(nullptr), mDataRepository(nullptr), mDoseCalculation(nullptr),mLastError(TPS_ER_SUCCESS)
{

}


TpsLoadObjects::~TpsLoadObjects(void)
{

}


bool TpsLoadObjects::Initialize()
{
    return true;
}

bool TpsLoadObjects::Finalize()
{
    return true;
}

void TpsLoadObjects::SetDataRepository(ITpsDataRepository* repos)
{
    mDataRepository = repos;
}

bool TpsLoadObjects::LoadPatientTree(const std::string& patientUID)
{
    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;
    mLastError = TPS_ER_SUCCESS;//clear old value

    TPS_LOG_DEV_INFO << "Load patient ENTER! Uid: " << patientUID;
    RtPatient* pPatient = nullptr;
    int iRet = mDatabaseWrapper->GetPatientByUid(patientUID, pPatient);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "No patient in TMS with PatientUID:" << patientUID;
        return false;
    }

    mDataRepository->GetPatientManager()->SetPatient(pPatient);

    // rcc: 目前的加载逻辑，图像是关键（用于构造坐标系），所以如果图像加载失败，也会导致后续Plan的加载失败
    //   所以这里如果图像加载失败，就直接放弃后续数据的加载
    if (!LoadSeriesByPatient_i(*pPatient))
    {
        TPS_LOG_DEV_ERROR << "Failed to load series under patient with PatientUID:" << pPatient->get_uid();
        return false;
    }

    // load series group ( to do )
    std::vector<std::string> loadedCourseUID;
    if (!LoadCourseByPatient_i(*pPatient, loadedCourseUID))
    {
        TPS_LOG_DEV_ERROR << "Failed to load course under patient with PatientUID:" << patientUID;
        return false;
    }

    TPS_LOG_DEV_INFO << "Load patient LEAVE!";
    //TEST_PERFORMANCE_END("BE LoadPatientTree");
    return true;
}

bool TpsLoadObjects::LoadSeriesByPatient_i(RtPatient& aPatient) 
{

    std::vector<RtSeries*> vecRtSeries;
    int iRet = mDatabaseWrapper->GetSeriesListByPatientUid(aPatient.get_uid(), vecRtSeries);

    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to GetSeriesListByPatientUid for patient[" << aPatient.get_uid() << "].";
        return false;
    }

    if (vecRtSeries.empty())
    {
        TPS_LOG_DEV_WARNING << "No series under patient with PatientUID:" << aPatient.get_uid();
        return true;
    }

    TpsImageDataManager& imgMgr = *mDataRepository->GetImageDataManager();
    std::vector<RtSeries*>::iterator itSeries = vecRtSeries.begin();
    for (; itSeries != vecRtSeries.end(); ++itSeries)
    {
        RtSeries& aSeries = *(*itSeries);
        std::string seriesUid = aSeries.get_uid();
        // load image of this series
        if (!LoadImage3D_i(&aSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load image 3d with SeriesUID: " << seriesUid;
            continue;
        }

        if (!LoadVOIListFromSeries_i(aSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load voi under series with SeriesUID:" << seriesUid;
            continue;
        }
        
        if (!LoadPOIListFromSeries_i(seriesUid))
        {
            TPS_LOG_DEV_ERROR << "Failed to load poi under series with SeriesUID:" << seriesUid;
            continue;
        }

        // load float images registered with this series. Even though the transformation is not loaded, the series is still added.
        if (!LoadImageTransformation_i(seriesUid))
        {
            TPS_LOG_DEV_WARNING << "Failed to load image transformation record for series uid:" << seriesUid;
        }
    }

    // Group series if it is in a 4D CT group
    std::vector<RtSeriesgroup*> vecRtSeriesgroups;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetAllSeriesgroups(vecRtSeriesgroups))
    {
        TPS_LOG_DEV_ERROR << "Failed to get all Seriesgroups.";
        return false;
    }
    imgMgr.GroupingSeries(vecRtSeries,vecRtSeriesgroups);
    for (int i = 0; i < vecRtSeriesgroups.size(); ++i)
    {
        delete vecRtSeriesgroups[i];
    }
    vecRtSeriesgroups.clear();

    return true;
}

// 加载TCPNTCP模型计算参数 wzd-2016-08-11
bool TpsLoadObjects::LoadTcpNTcpParameter()
{
    std::vector<RtTcp*> vectRtTcps;
    int iRet = mDatabaseWrapper->GetAllTcp(vectRtTcps);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed: load tcpntcp Paramater";
        return false;
    }

    if (!vectRtTcps.empty())
    {
        mDataRepository->GetTcpNTcpManager()->AddTcpList(vectRtTcps);
    }

    return true;
}

bool TpsLoadObjects::LoadPatientTreeForEasyplan(const std::string& patientUID)
{
    RtPatient* pPatient = nullptr;
    int iRet = mDatabaseWrapper->GetPatientByUid(patientUID, pPatient);
    if (iRet != DB_CODES_SUCCESS) {
        TPS_LOG_DEV_ERROR << "No patient in TMS with PatientUID:" << patientUID;
        return false;
    }

    mDataRepository->GetPatientManager()->SetPatient(pPatient);

    std::vector<std::string> loadedCourseUID;
    if (!LoadCourseByPatientForEasyplan_i(*pPatient, loadedCourseUID)) {
        TPS_LOG_DEV_ERROR << "Failed to load course under patient with PatientUID:" << patientUID;
        return false;
    }

    return true;
}

//用GalleryPatientManager和GalleryImageManager代替
bool TpsLoadObjects::LoadPatientForGallery(const std::string& patientUID)
{
    TPS_LOG_DEV_INFO<<"Load patient for gallery enter! Uid: "<<patientUID;

    //mDataRepository->GetGalleryPatientManager()->ClearManager();
    mDataRepository->GetGalleryPatientManager()->ClearPatientData();

    RtPatient* pPatient = nullptr;
    int iRet = mDatabaseWrapper->GetPatientByUid(patientUID, pPatient);
    if (iRet != DB_CODES_SUCCESS) {
        TPS_LOG_DEV_ERROR<<"No patient in TMS with PatientUID:"<<patientUID;
        return false;
    }

    //if (!mDataRepository->GetGalleryPatientManager()->AddPatient(*pPatient)) {
    //    TPS_LOG_DEV_WARNING<<"The patient already exist:"
    //        <<patientUID;
    //}
    mDataRepository->GetGalleryPatientManager()->SetPatient(pPatient);

    // rcc: 目前的加载逻辑，图像是关键（用于构造坐标系），所以如果图像加载失败，也会导致后续Plan的加载失败
    //      所以这里如果图像加载失败，就直接放弃后续数据的加载
    if (!LoadSeriesByPatientForGallery_i(*pPatient))
    {
        TPS_LOG_DEV_ERROR << "Failed to load series under patient with PatientUID:" << pPatient->get_uid();
        return false;
    }

    std::vector<std::string> loadedCourseUIDList;
    if (!LoadCourseByPatientForGallery_i(*pPatient, loadedCourseUIDList)) {
        TPS_LOG_DEV_ERROR<<"Failed to load course under patient with PatientUID:"<<patientUID;
        return false;
    }

    TPS_LOG_DEV_INFO<<"Load patient for gallery leave!";
    return true;
}

bool TpsLoadObjects::ReloadPatient(const std::string& patientUID)
{
    if (!ClearPatient(patientUID)) {
        TPS_LOG_DEV_ERROR << "Failed to clear patient firstly. Reload Abort!";
        return false;
    }

    return LoadPatientTree(patientUID);
}

bool TpsLoadObjects::ClearPatient(const std::string& /*patientUID*/)
{
    mLastError = TPS_ER_SUCCESS;
    return true;
}

bool TpsLoadObjects::LoadSeriesByPatientForGallery_i(RtPatient& aPatient) {

    std::vector<RtSeries*> vecRtSeries;
    int iRet = mDatabaseWrapper->GetSeriesListByPatientUid(aPatient.get_uid(), vecRtSeries);

    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to GetSeriesListByPatientUid for patient[" << aPatient.get_uid() << "].";
        return false;
    }

    if (vecRtSeries.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No series under patient with PatientUID:" << aPatient.get_uid();
        return true;
    }

    TpsGalleryImageManager& gaImgMgr = *mDataRepository->GetGalleryImageManager();
    std::vector<RtSeries*>::iterator itSeries = vecRtSeries.begin();
    for (; itSeries != vecRtSeries.end(); ++itSeries)
    {
        RtSeries* pSeries = *itSeries;
        std::string seriesUid = pSeries->get_uid();

        if (mDataRepository->GetGalleryImageManager()->GetImageSeries(seriesUid, &pSeries))
        {
            //not load again
            continue;
        }

        if (!mDataRepository->GetGalleryImageManager()->AddImageSeries(pSeries))
        {
            TPS_LOG_DEV_WARNING << "Failed to add series to image3dmanager!";
            continue;
        }

        // load image of this series
        if (!LoadImage3DForGallery_i(pSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load image 3d with SeriesUID: " << seriesUid;
            continue;
        }
    }

    // Group series if it is in a 4D CT group
    gaImgMgr.GroupingSeries(vecRtSeries);
    return true;
}

bool TpsLoadObjects::LoadCourseByPatient_i(RtPatient& aPatient,
    std::vector<std::string>& loadedCourseUID)
{
    std::string patUid = aPatient.get_uid();
    std::vector<RtCourse*> vecRtCourses;
    int iRet = mDatabaseWrapper->GetCourseListByPatientUid(patUid, vecRtCourses);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed: load course object list from database for patient[" << aPatient.get_uid() << "]";
        return false;
    }

    if (vecRtCourses.empty())
    {
        TPS_LOG_DEV_WARNING << "No course under patient with PatientUID:" << aPatient.get_uid();
        return true;
    }

    std::vector<RtCourse*>::iterator itCourse = vecRtCourses.begin();
    for (; itCourse != vecRtCourses.end(); ++itCourse)
    {
        RtCourse* pCourse = *itCourse;
        if (!mDataRepository->GetPatientManager()->AddCourse(pCourse))
        {
            TPS_LOG_DEV_ERROR << "Failed to add course object to patient mgr.";
            continue;
        }

        if (!LoadSiteByCourese_i(pCourse->get_uid()))
        {
            TPS_LOG_DEV_WARNING << "Failed to load site under course with CourseUID:" << pCourse->get_uid();
        }

        // rcc: Presciption不是加载Plan的必要条件，所以这里加载失败（比如空的情况），仍然可以继续加载计划数据
        if (!LoadPrescriptionByCourse_i(*pCourse))
        {
            TPS_LOG_DEV_WARNING << "Failed to load prescription under course with CourseUID:" << pCourse->get_uid();
        }

        // rcc: 计划数据的加载
        std::vector<std::string> loadedPlanUID;
        if (!LoadPlansByCourse_i(*pCourse, loadedPlanUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to load plans under course with CourseUID:" << pCourse->get_uid();
            continue;
        }

        // 至此，表示course完整加载，加入成功的course的列表
        loadedCourseUID.push_back(pCourse->get_uid());
    }

    return true;
}

bool TpsLoadObjects::LoadCourseByPatientForEasyplan_i(RtPatient& aPatient,
    std::vector<std::string>& loadedCourseUID)
{
    std::string patUid = aPatient.get_uid();
    std::vector<RtCourse*> vecRtCourses;
    int iRet = mDatabaseWrapper->GetCourseListByPatientUid(patUid, vecRtCourses);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed: load course object list from database for patient[" << aPatient.get_uid() << "]";
        return false;
    }

    if (vecRtCourses.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No course under patient with PatientUID:" << aPatient.get_uid();
        return true;
    }

    std::vector<RtCourse*>::iterator itCourse = vecRtCourses.begin();
    for (; itCourse != vecRtCourses.end(); ++itCourse)
    {
        RtCourse* pCourse = *itCourse;

        // rcc: 如果往PatientManager添加失败，没必要继续加载下去
        if (!mDataRepository->GetPatientManager()->AddCourse(pCourse))
        {
            TPS_LOG_DEV_ERROR << "Failed to add course object to patient mgr.";
            continue;
        }

        if (!LoadSiteByCourese_i(pCourse->get_uid()))
        {
            TPS_LOG_DEV_WARNING << "Failed to load site under course with CourseUID:" << pCourse->get_uid();
        }

        // rcc: Presciption不是加载Plan的必要条件，所以这里加载失败（比如空的情况），仍然可以继续加载计划数据
        if (!LoadPrescriptionByCourse_i(*pCourse))
        {
            TPS_LOG_DEV_WARNING << "Failed to load prescription under course with CourseUID:" << pCourse->get_uid();
        }


        std::vector<std::string> loadedPlanUID;
        if (!LoadPlansByCourseForEasyplan_i(*pCourse, loadedPlanUID))
        {
            TPS_LOG_DEV_ERROR << "Failed to load plans for easyplan under course with CourseUID:" << pCourse->get_uid();
            continue;
        }

        // 至此，表示course完整加载，加入成功的course的列表
        loadedCourseUID.push_back(pCourse->get_uid());
    }


    return true;
}

bool TpsLoadObjects::LoadCourseByPatientForGallery_i(RtPatient& aPatient, 
    std::vector<std::string>& loadedCourseUID)
{
    std::string patUid = aPatient.get_uid();
    std::vector<RtCourse*> vecRtCourses;
    int iRet = mDatabaseWrapper->GetCourseListByPatientUid(patUid, vecRtCourses);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed: load course object list from database for patient["<<aPatient.get_uid()<<"]";
        return false;
    }

    if (vecRtCourses.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No course under patient with PatientUID:"<<aPatient.get_uid();
        return true;
    }

    std::vector<RtCourse*>::iterator itCourse = vecRtCourses.begin();
    for (; itCourse != vecRtCourses.end(); ++itCourse)
    {
        RtCourse* pCourse = *itCourse;

        //Add By DYX, 20160113
        std::vector<std::string> vBeamGroupUIDs;
        if (!LoadBeamGroupUIDs(*pCourse, vBeamGroupUIDs))
        {
            // log
            TPS_LOG_DEV_ERROR << "Failed to Load BeamGroups for Course :" << pCourse->get_uid();
            continue;
        }
        mDataRepository->GetGalleryPatientManager()->SetBeamGroupUIDsByCourseUID(pCourse->get_uid(), vBeamGroupUIDs);

        if (!mDataRepository->GetGalleryPatientManager()->AddCourse(pCourse))
        {
            TPS_LOG_DEV_ERROR << "Failed to add course object to patient mgr.";
            continue;
        }

        // 至此，表示course完整加载，加入成功的course的列表
        loadedCourseUID.push_back(pCourse->get_uid());
    }

    return true;
}

bool TpsLoadObjects::LoadBeamGroupUIDs(RtCourse& aCourse, std::vector<std::string>& vBeamGroupUIDs)
{
    vBeamGroupUIDs.clear();

    std::string courseUid = aCourse.get_uid();

    //Get PlanUIDs by Course
    std::vector<RtPlan*> vecRtPlanList;
    int iRet = mDatabaseWrapper->GetPlanListByCourseUid(courseUid, vecRtPlanList);
    if (iRet != DB_CODES_SUCCESS) 
    {
        TPS_LOG_DEV_ERROR << "Failed to load plans under course[" << courseUid << "].";
        return false;
    }
    for (auto itPlan = vecRtPlanList.begin(); itPlan != vecRtPlanList.end(); ++itPlan) 
    {
        RtPlan* aPlan = *itPlan;
        if (aPlan == nullptr) 
        {
            TPS_LOG_DEV_ERROR << "Skip one nullptr RtPlan object...";
            continue;
        }
        vBeamGroupUIDs.push_back(aPlan->get_uid());
    }

    return true;
}

bool TpsLoadObjects::LoadSeriesByCourse_i(RtCourse& aCourse,
    std::vector<std::string>& loadedSeriesUID)
{
    std::string courseUid = aCourse.get_uid();
    std::vector<RtSeries*> vecRtSeries;
    int iRet = mDatabaseWrapper->GetSeriesListByCourseUid(courseUid, vecRtSeries);

    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to GetSeriesListByCourseUid for course[" << courseUid << "].";
        return false;
    }

    if (vecRtSeries.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No series under course with CourseUID:" << aCourse.get_uid();
        return true;
    }


    TpsImageDataManager& imgMgr = *mDataRepository->GetImageDataManager();
    std::vector<RtSeries*>::iterator itSeries = vecRtSeries.begin();
    for (; itSeries != vecRtSeries.end(); ++itSeries)
    {
        RtSeries& aSeries = *(*itSeries);
        std::string seriesUid = aSeries.get_uid();
        // load image of this series
        if (!LoadImage3D_i(&aSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load image 3d with SeriesUID: " << seriesUid;
            continue;
        }

        if (!LoadVOIListFromSeries_i(aSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load voi under series with SeriesUID:" << seriesUid;
            continue;
        }

        if (!LoadPOIListFromSeries_i(seriesUid))
        {
            TPS_LOG_DEV_ERROR << "Failed to load poi under series with SeriesUID:" << seriesUid;
            continue;
        }

        // load float images registered with this series. Even though the transformation is not loaded, the series is still added.
        if (!LoadImageTransformation_i(seriesUid))
        {
            TPS_LOG_DEV_WARNING << "Failed to load image transformation record for series uid:" << seriesUid;
        }
        loadedSeriesUID.push_back(seriesUid);
    }

    // Group series if it is in a 4D CT group
    std::vector<RtSeriesgroup*> vecRtSeriesgroups;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetAllSeriesgroups(vecRtSeriesgroups))
    {
        TPS_LOG_DEV_ERROR << "Failed to get all Seriesgroups.";
        return false;
    }
    imgMgr.GroupingSeries(vecRtSeries,vecRtSeriesgroups);
    for (int i = 0; i < vecRtSeriesgroups.size(); ++i)
    {
        delete vecRtSeriesgroups[i];
    }
    vecRtSeriesgroups.clear();
    return true;
}

bool TpsLoadObjects::LoadSeriesByCourseForGallery_i(RtCourse& aCourse,
    std::vector<std::string>& loadedSeriesUID) 
{
    std::string courseUid = aCourse.get_uid();
    std::vector<RtSeries*> vecRtSeries;
    int iRet = mDatabaseWrapper->GetSeriesListByCourseUid(courseUid, vecRtSeries); 
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to GetSeriesListByCourseUid for course["<<courseUid<<"].";
        return false;
    }

    if (vecRtSeries.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No series under course with CourseUID:"<<aCourse.get_uid();
        return true;
    }

    TpsGalleryImageManager& gaImgMgr = *mDataRepository->GetGalleryImageManager();
    std::vector<RtSeries*>::iterator itSeries = vecRtSeries.begin();
    for (; itSeries != vecRtSeries.end(); ++itSeries)
    {
        RtSeries* pSeries = *itSeries;

        if (mDataRepository->GetGalleryImageManager()->GetImageSeries(pSeries->get_uid(), &pSeries))
        {
            //not load again
            continue;
        }

        if (!mDataRepository->GetGalleryImageManager()->AddImageSeries(pSeries))
        {
            TPS_LOG_DEV_WARNING << "Failed to add series to image3dmanager!";
            continue;
        }

        // load image of this series
        if (!LoadImage3DForGallery_i(pSeries))
        {
            TPS_LOG_DEV_ERROR<<"Failed to load image 3d with SeriesUID: "<<pSeries->get_uid();
            continue;
        }

        loadedSeriesUID.push_back(pSeries->get_uid());
    }

    // Group series if it is in a 4D CT group
    gaImgMgr.GroupingSeries(vecRtSeries);
    return true;
}

bool TpsLoadObjects::LoadPhantomSeries(std::vector<std::string>& phantomUidList)
{
    TpsImageDataManager& imgMgr = *mDataRepository->GetImageDataManager();
    std::vector<RtSeries*> pSeriesList;
    int iRet = mDatabaseWrapper->GetAllPhantomSeries(pSeriesList);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to LoadPhantomSeries";
        return false;
    }

    if (pSeriesList.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No Phantom Series";
        return true;
    }
    std::vector<RtSeries*>::iterator itSeries = pSeriesList.begin();
    for (; itSeries != pSeriesList.cend(); ++itSeries)
    {
        //TODO fix bug load TMS series without the relationship between Series and VOIs
        RtSeries& pSeries = *(*itSeries);
        std::string seriesUid = pSeries.get_uid();
        if (imgMgr.CheckUidExist(seriesUid)) continue;
        phantomUidList.push_back(seriesUid);
        // load image of this series
        if (!LoadImage3D_i(&pSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load image 3d with SeriesUID: " << seriesUid;
            continue;
        }

        if (!LoadVOIListFromSeries_i(pSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to load voi under series with SeriesUID:" << seriesUid;
            continue;
        }

        if (!LoadPOIListFromSeries_i(seriesUid))
        {
            TPS_LOG_DEV_ERROR << "Failed to load poi from the SeriesUID." << seriesUid;
            return false;
        }
    }
    return true;
}

bool TpsLoadObjects::LoadSeries(const std::string& patientUID,
    std::vector<std::string>& loadedSeriesUIDList)
{
    int iRet = -1;
    std::vector<RtSeries*> vecRtSeries;
    // find all course of this patient
    iRet = mDatabaseWrapper->GetSeriesListByPatientUid(patientUID, vecRtSeries);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load series by patient uid: " << patientUID;
        return false;
    }

    std::vector<RtSeries*> vecSeriesLoaded;
    std::vector<RtSeries*> vecQASeriesLoaded;
    std::vector<RtSeries*>::iterator itSeries = vecRtSeries.begin();
    for (; itSeries != vecRtSeries.end(); itSeries++)
    {
        RtSeries* series = *itSeries;
        // load image of this series
        if (!LoadImage3D_i(series))
        {
            TPS_LOG_DEV_ERROR << "Failed to load image 3d which uid: " << series->get_uid();
            return false;
        }

        loadedSeriesUIDList.push_back(series->get_uid());
        if(!series->get_isqa()) 
        {
            vecSeriesLoaded.push_back(series);
        }
        else
        {
            vecQASeriesLoaded.push_back(series);
        }

        if (!LoadPOIListFromSeries_i(series->get_uid()))
        {
            TPS_LOG_DEV_ERROR << "Failed to load poi from the SeriesUID." << series->get_uid();
            return false;
        }
    }

    // SetDefaultImage_i(vecSeriesLoaded);

    return true;
}

int TpsLoadObjects::LoadSeries(const std::vector<std::string>& seriesUIDList)
{
    if (seriesUIDList.empty()) return TPS_ER_SUCCESS;

    TPS_LOG_DEV_INFO << "Load series ENTER, series count: " << seriesUIDList.size();
    std::vector<RtSeries*> seriesList;
    int iRet = mDatabaseWrapper->GetSeriesList(seriesUIDList, seriesList);
    if (iRet != DB_CODES_SUCCESS || seriesList.empty())
    {
        TPS_LOG_DEV_ERROR << "Failed to load series list from database";
        return TPS_ER_DB_ERROR;
    }

    TpsImageDataManager& imgMgr = *mDataRepository->GetImageDataManager();
    std::string seriesUid;
    std::map<std::string, int> courseUpdated;
    for (int i = 0; i < seriesList.size(); ++i)
    {
        seriesUid = seriesList[i]->get_uid();

        // load image of this series
        if (!LoadImage3D_i(seriesList[i]))
        {
            TPS_LOG_DEV_ERROR << "Failed to load image 3d with SeriesUID: " << seriesUid;
            return TPS_ER_FAILURE;
        }

        if (!LoadVOIListFromSeries_i(*seriesList[i]))
        {
            TPS_LOG_DEV_ERROR << "Failed to load voi under series with SeriesUID:" << seriesUid;
            return TPS_ER_FAILURE;
        }

        if (!LoadPOIListFromSeries_i(seriesUid))
        {
            TPS_LOG_DEV_ERROR << "Failed to load poi under series with SeriesUID:" << seriesUid;
            return TPS_ER_FAILURE;
        }

        // load float images registered with this series. 
        // Even though the transformation is not loaded, the series is still added.
        if (!LoadImageTransformation_i(seriesUid))
        {
            TPS_LOG_DEV_WARNING << "Failed to load image transformation record for series uid:" << seriesUid;
        }

        courseUpdated[seriesList[i]->get_courseuid()] = 1;
    }
    // Group series if it is in a 4D CT group
    std::vector<RtSeriesgroup*> vecRtSeriesgroups;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetAllSeriesgroups(vecRtSeriesgroups))
    {
        TPS_LOG_DEV_ERROR << "Failed to get all Seriesgroups.";
        return false;
    }

    for (auto it = courseUpdated.begin(); it != courseUpdated.end(); ++it)
    {
        imgMgr.GroupingSeries(it->first,vecRtSeriesgroups);
    }
    TPS_LOG_DEV_INFO << "Load series LEAVE!";

    for (int i = 0; i < vecRtSeriesgroups.size(); ++i)
    {
        delete vecRtSeriesgroups[i];
    }
    vecRtSeriesgroups.clear();

    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadROIs(const std::vector<std::string>& structureSetUIDList)
{
    if (structureSetUIDList.empty()) return TPS_ER_SUCCESS;
    TPS_LOG_DEV_INFO<<"Load rois ENTER, structure set count: " << structureSetUIDList.size();
    std::vector<RtVoi*> voiList;
    if (mDatabaseWrapper->GetVoiList(structureSetUIDList, voiList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load roi from database.";
        return TPS_ER_DB_ERROR;
    }

    TpsImage3DEntity* image = nullptr;
    auto mng = mDataRepository->GetVoiManager();
    for (int i = 0; i < voiList.size(); ++i)
    {
        if (mng->Contains(voiList[i]->get_uid()))
        {
            TPS_LOG_DEV_INFO << "The voi has already loaded.";
            DEL_PTR(voiList[i]);
            continue;
        }

        if (!mDataRepository->GetImageDataManager()->GetImage3DData(
            voiList[i]->get_seriesuid(), &image) || image == nullptr)
        {
                TPS_LOG_DEV_ERROR << "The image 3d data haven't been loaded yet.";
                DEL_PTR(voiList[i]);
                continue;
        }
        LoadVOIFromDatabase_i(voiList[i], image);
    }
    TPS_LOG_DEV_INFO << "Load rois LEAVE!";
    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadPlans(const std::vector<std::string>& planUIDLists)
{
    if (planUIDLists.empty()) return TPS_ER_SUCCESS;
    TPS_LOG_DEV_INFO << "Load plans ENTER, plans count: " << planUIDLists.size();
    std::vector<RtPlan*> planList;
    int iRet = mDatabaseWrapper->GetPlanList(planUIDLists, planList);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load plan list from database";
        return TPS_ER_DB_ERROR;
    }

    std::string planUid;
    RtPlan* plan;
    for (int i = 0; i < planList.size(); ++i)
    {
        plan = planList[i];
        planUid = plan->get_uid();
        TPS_LOG_DEV_INFO << "loading plan[" << planUid << "]...";

        // 这里把EasyPlan的特殊处理暂时去掉，希望以后可以一并解决
        if (!LoadPlanData_i(*plan))
        {
            TPS_LOG_DEV_ERROR << "Failed to load all data of plan object";
            return TPS_ER_FAILURE;
        }
        // add this plan to patient mgr
        // 注意：保证Plan的完整性，有任意子元素加载失败，放弃该Plan的加载
        // 不纳入内存管理
        if (!mDataRepository->GetPatientManager()->AddPlan(plan))
        {
            TPS_LOG_DEV_ERROR << "Failed to add plan into patient mgr.";
            return TPS_ER_FAILURE;
        }

        mDoseCalculation->ComposeDoseForPlanWithoutRendering(plan->get_uid());
        TPS_LOG_DEV_INFO << "add plan[" << planUid << "] to patient manager.";
    }
    TPS_LOG_DEV_INFO << "Load plans LEAVE!";
    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadDoseGrids(const std::vector<std::string>& doseGridUidList, 
    std::vector<std::string>& affectedPlanUidList)
{
    auto ngManager = mDataRepository->GetNormgroupManager();
    auto beamManager = mDataRepository->GetBeamManager();
    std::vector<std::string> unloadedDoseGrid = std::move(
        ngManager->FilterLoadedDoseGridUid(doseGridUidList));
    unloadedDoseGrid = std::move(
        beamManager->FilterLoadedDoseGridUid(unloadedDoseGrid));
    if (unloadedDoseGrid.empty()) return TPS_ER_SUCCESS;
    TPS_LOG_DEV_INFO << "Load dose grids ENTER, dose grid count: " << unloadedDoseGrid.size();
    std::vector<RtDosegrid*> dosegridList;
    int iRet = mDatabaseWrapper->GetDosegridObjects(unloadedDoseGrid, dosegridList);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load dose grid list from database";
        return TPS_ER_DB_ERROR;
    }

    //group dose grids
    std::vector<RtDosegrid*> ngGridList, beamGridList;
    for (int i = 0; i < dosegridList.size(); ++i)
    {
        if (!dosegridList[i]->get_normgroupuid().empty())
        {
            ngGridList.push_back(dosegridList[i]);
        }
        else if (!dosegridList[i]->get_beamuid().empty())
        {
            beamGridList.push_back(dosegridList[i]);
        }
        else
        {
            DEL_PTR(dosegridList[i]);
        }
    }

    std::map<std::string, int> changedPlanUid;
    RtNormgroup* normgroup = nullptr;
    RtBeam* beam = nullptr;
    RtDosegrid* tmpGrid = nullptr;
    for (int i = 0; i < ngGridList.size(); ++i)
    {
        if (!ngManager->GetNormGroup(ngGridList[i]->get_normgroupuid(), &normgroup))
        {
            TPS_LOG_DEV_ERROR << "The normal group which the dose grid related does not exist.";
            continue;
        }
        tmpGrid = normgroup->get_dosegrid();
        if (tmpGrid == nullptr)
        {
            TPS_LOG_DEV_ERROR << "The normal has no dose grid";
            continue;
        }
        if (tmpGrid->get_isdosevalid())
        {
            TPS_LOG_DEV_ERROR << "The dose grid has already loaded.";
            continue;
        }
        *tmpGrid = *ngGridList[i];
        changedPlanUid[normgroup->get_planuid()] = 0;
    }
    for (int i = 0; i < beamGridList.size(); ++i)
    {
        beam = beamManager->GetBeam(beamGridList[i]->get_beamuid());
        if (beam == nullptr)
        {
            TPS_LOG_DEV_ERROR << "The beam which the dose grid related does not exist.";
            continue;
        }
        if (!ngManager->GetNormGroup(beam->get_normgroupuid(), &normgroup))
        {
            TPS_LOG_DEV_ERROR << "The normal group which the dose grid related does not exist.";
            continue;
        }
        tmpGrid = beam->get_dosegrid();
        if (tmpGrid == nullptr)
        {
            TPS_LOG_DEV_ERROR << "The normal has no dose grid";
            continue;
        }
        if (tmpGrid->get_isdosevalid())
        {
            TPS_LOG_DEV_ERROR << "The dose grid has already loaded.";
            continue;
        }
        *tmpGrid = *beamGridList[i];
        changedPlanUid[normgroup->get_planuid()] = 0;
    }
    //delete all dose grids
    for (int i = 0; i < dosegridList.size(); ++i)
    {
        DEL_PTR(dosegridList[i]);
    }
    dosegridList.clear();
    //update plan dose grid & voi,poi,DVH information
    affectedPlanUidList.clear();
    for (auto it = changedPlanUid.begin(); it != changedPlanUid.end(); ++it)
    {
        mDoseCalculation->ComposeDoseForPlanWithoutRendering(it->first);
        affectedPlanUidList.push_back(it->first);
    }
    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::UpdatePlanOrder(const std::string& patientUid)
{
    TPS_LOG_DEV_INFO << "Update plan order ENTER!";
    auto patMng = mDataRepository->GetPatientManager();
    RtPatient* patient = patMng->GetPatient();
    if (patient == nullptr) 
    {
        TPS_LOG_DEV_ERROR << "The current patient is null.";
        return TPS_ER_FAILURE;
    }
    if (patient->get_uid() != patientUid)
    {
        TPS_LOG_DEV_ERROR << "The patient uid is not current one.";
        return TPS_ER_FAILURE;
    }

    //load plan order information firstly
    std::vector<RtCourse*> vecRtCourses;
    int iRet = mDatabaseWrapper->GetCourseListByPatientUid(patientUid, vecRtCourses);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to get course list of Patient: "<<patientUid;
        return TPS_ER_DB_ERROR;
    }

    std::vector<RtPrescription*> vecPrescription;
    std::vector<RtConstraint*> vecConstraint;
    RtCourse* pCourse = nullptr;
    for (int i = 0; i < vecRtCourses.size(); ++i)
    {
        pCourse = vecRtCourses[i];
        std::vector<RtPrescription*> prescriptions;
        iRet = mDatabaseWrapper->GetPrescriptionListByCourseUid(pCourse->get_uid(), prescriptions);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_WARNING << "Failed to load prescription from course[" << pCourse->get_uid() << "].";
            continue;
        }
        vecPrescription.insert(vecPrescription.end(), prescriptions.begin(), prescriptions.end());

        std::vector<std::string> prescriptionUidList(prescriptions.size());
        std::transform(prescriptions.begin(), prescriptions.end(), prescriptionUidList.begin(), [](RtPrescription* prescription)
        {
            return prescription->get_uid();
        });

        std::vector<RtConstraint*> constraints;
        iRet = mDatabaseWrapper->GetConstraintListByPrescriptionUidList(prescriptionUidList, constraints);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_WARNING << "Failed to load prescription from course[" << pCourse->get_uid() << "].";
            continue;
        }
        vecConstraint.insert(vecConstraint.end(), constraints.begin(), constraints.end());
    }

    //clear current plan order information in manager
    patMng->ClearPlanOrderInformation();
    //add new data
    patMng->AddCourseList(vecRtCourses);
    patMng->AddPrescriptionList(vecPrescription);
    patMng->AddConstraintList(vecConstraint);
    TPS_LOG_DEV_INFO<<"Update plan order LEAVE!";
    return TPS_ER_SUCCESS;
}

bool TpsLoadObjects::LoadPOIs(const std::vector<std::string>& seriesUIDList) 
{
    std::vector<std::string>::const_iterator it = seriesUIDList.begin();
    for (; it != seriesUIDList.end(); ++it) {
        //load POI of the plan
        if (!LoadPOIListFromSeries_i(*it)){
            TPS_LOG_DEV_ERROR << "Failed to load poi from the series: " << *it;
            return false;
        }
    }
    return true;
}

bool TpsLoadObjects::LoadSiteByCourese_i(const std::string& courseUid)
{
    std::vector<RtSite*> vectSites;
    int iRet = mDatabaseWrapper->GetSiteListByCourseUid(courseUid, vectSites);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to load site from course[" << courseUid << "].";
        return false;
    }
    if (vectSites.size() > 0)
    {
        mDataRepository->GetPatientManager()->AddSiteList(vectSites);
    }

    return true;
}

bool TpsLoadObjects::LoadPrescriptionByCourse_i(RtCourse& aCourse)
{
    std::vector<RtPrescription*> prescriptions;
    int iRet = mDatabaseWrapper->GetPrescriptionListByCourseUid(aCourse.get_uid(), prescriptions);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to load prescription from course[" << aCourse.get_uid() << "].";
        return false;
    }
    mDataRepository->GetPatientManager()->AddPrescriptionList(prescriptions);

    std::vector<std::string> prescriptionUidList(prescriptions.size());
    std::transform(prescriptions.begin(), prescriptions.end(), prescriptionUidList.begin(), [](RtPrescription* prescription)
    {
        return prescription->get_uid();
    });

    std::vector<RtConstraint*> vRtConstraintList;
    iRet = mDatabaseWrapper->GetConstraintListByPrescriptionUidList(prescriptionUidList, vRtConstraintList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to load prescription from course[" << aCourse.get_uid() << "].";
        return false;
    }

    std::for_each(vRtConstraintList.begin(), vRtConstraintList.end(), [&](RtConstraint* constraint)
    {
        mDataRepository->GetPatientManager()->AddConstraint(constraint);
    });

    return true;
}

bool TpsLoadObjects::LoadPlansByCourse_i(RtCourse& aCourse,
    std::vector<std::string>& loadedPlanUID)
{
    std::string courseUid = aCourse.get_uid();
    // query plans uid from this course
    std::vector<RtPlan*> vecRtPlan;
    int iRet = mDatabaseWrapper->GetPlanListByCourseUid(courseUid, vecRtPlan);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load plans under course[" << courseUid << "].";
        return false;
    }

    if (vecRtPlan.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No plan under this course[" << courseUid << "].";
        return true;
    }

    // iteratively load all plans
    // 加载策略：完整加载成功的Plan，会被加入内存管理;
    //          Plan下任何数据都不允许失败，如果失败，则认为Plan加载失败
    for (auto itPlan = vecRtPlan.begin(); itPlan != vecRtPlan.end(); ++itPlan)
    {
        if (*itPlan == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Skip one nullptr RtPlan object...";
            continue;
        }
        const std::string planUid = (*itPlan)->get_uid();
        TPS_LOG_DEV_INFO << "loading plan[" << planUid << "]...";

        if (!LoadPlanData_i(*(*itPlan)))
        {
            TPS_LOG_DEV_ERROR << "Failed to load all data of plan object";
            DEL_PTR(*itPlan);
            continue;
        }

        // add this plan to patient mgr
        // 注意：保证Plan的完整性，有任意子元素加载失败，放弃该Plan的加载
        // 不纳入内存管理
        if (!mDataRepository->GetPatientManager()->AddPlan(*itPlan)) {
            TPS_LOG_DEV_ERROR << "Failed to add plan into patient mgr.";
            DEL_PTR(*itPlan);
            continue;
        }

        //added by qqzhou@20151126 according to the latest requirement: saving beam's dosegrid only!
        //if(!mDoseProxy->ComposeDoseForPlan(planUid)){
        //    TPS_LOG_DEV_ERROR<<"Failed to ComposeDoseForPlan:"<<planUid;
        //    continue;
        //}

        mDoseCalculation->ComposeDoseForPlanWithoutRendering(planUid);
        loadedPlanUID.push_back(planUid);
        TPS_LOG_DEV_INFO << "add plan[" << planUid << "] to patient manager.";
    }

    return true;
}

bool TpsLoadObjects::LoadPlansByCourseForEasyplan_i(RtCourse& aCourse,
    std::vector<std::string>& loadedPlanUID)
{
    //rcc: 这部分在新架构下争取重写，不再制造额外的分支
    //// query plans uid from this course
    std::vector<RtPlan*> vEasyPlanList;
    vEasyPlanList.clear();
    //rcc: 等QQ完成wrapper相关的接口后，补齐逻辑
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetPlanListByCourseUid(aCourse.get_uid(), vEasyPlanList))
    {
        TPS_LOG_DEV_ERROR << "Failed to GetPlanUIDList";
        return false;
    }
    // no plans under one course is allowed and give warning
    if (vEasyPlanList.size() < 1)
    {
        TPS_LOG_DEV_WARNING << "No plans under this course";
        return true;
    }

    // iteratively load all plans
    // 加载策略：完整加载成功的Plan，会被加入内存管理;
    //          Plan下任何数据都不允许失败，如果失败，则认为Plan加载失败
    for (int n = 0; n < vEasyPlanList.size(); n++)
    {
        TPS_LOG_DEV_INFO << "loading plan[" << vEasyPlanList[n]->get_uid() << "]...";

        if (vEasyPlanList[n]->get_plansourcetype() != RtDbDef::EasyPlan)
        {
            continue;
        }

        if (!LoadEasyPlanData_i(*vEasyPlanList[n]))
        {
            TPS_LOG_DEV_ERROR << "Failed to load all data of easy plan object";
            continue;
        }

        // add this plan to patient mgr
        // 注意：保证Plan的完整性，有任意子元素加载失败，放弃该Plan的加载
        // 不纳入内存管理
        if (!mDataRepository->GetPatientManager()->AddPlan(vEasyPlanList[n])) {
            TPS_LOG_DEV_ERROR << "Failed to add plan into patient mgr.";
            continue;
        }

        loadedPlanUID.push_back(vEasyPlanList[n]->get_uid());
        TPS_LOG_DEV_INFO << "add plan[" << vEasyPlanList[n] << "] to patient manager.";
    }

    return true;
}

// load following data for a plan
// 1. image data for this plan
// 2. create default dose grid for any 3d plan
// 3. POI
// 4. Normgroup 
// 5. dose grid 
bool TpsLoadObjects::LoadPlanData_i(RtPlan& aPlan)
{
    std::string strPlanUid = aPlan.get_uid();

    if (!LoadDoseLineParamsFromPlan_i(&aPlan))
    {
        TPS_LOG_DEV_ERROR << "Failed to load doseline params from the plan.";
        return false;
    }

    // load normgroup of the plan
    if (!LoadNormgroupsByPlan_i(aPlan))
    {
        TPS_LOG_DEV_ERROR << "Failed to load normgroups from the plan.";
        return false;
    }

    // gml: 按照导出的定义，导出的rt_dose对应于我们的Normalgroup dose，
    // 所以，plan的Dose grid无需存储，每次load重新组装，以减少对数据库的操作
    if (!this->GeneratePlanDoseGrid_i(aPlan))
    {
        TPS_LOG_DEV_ERROR << "Failed to generate plan dose grid.";
        return false;
    }

    return true;
}



bool TpsLoadObjects::LoadNormgroupsByPlan_i(RtPlan& aPlan)
{
    using namespace RTFWK;

    std::vector<RtNormgroup*> vNormGroupList;
    std::string planUid = aPlan.get_uid();
    int iRet = mDatabaseWrapper->GetNormgroupListByPlanUid(planUid, vNormGroupList);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load normgroup under plan[" << planUid << "].";
        return false;
    }
    if (vNormGroupList.empty())
    {
        TPS_LOG_DEV_INFO << "No normgroups under this plan";
        return true;
    }

    // 加载Normgroup中的beam时，目前需要Image的信息，所以
    // 这里需要先查到对于的Image 3D对象
    std::string strImageUID = aPlan.get_imageseriesuid();
    TpsImage3DEntity* image3D = nullptr;
    // check whether image3D object already exists in ImageDataManager
    // if no, load FAILED
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(strImageUID, &image3D)
        || nullptr == image3D)
    {
        TPS_LOG_DEV_ERROR << "Failed to load normgroup, can't find Image[: "
            << strImageUID << "] in image data manager.";
        return false;
    }

    // load all normgroups, if one failed, fail all.
    for (auto itrNG = vNormGroupList.begin(); itrNG != vNormGroupList.end(); ++itrNG)
    {
        RtNormgroup* aNormGroup = *itrNG;
        std::string strNgUid = aNormGroup->get_uid();
        TPS_LOG_DEV_INFO << "loading normgroup[" << strNgUid << "]...";
        if (aNormGroup == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Skip one nullptr normgroup object";
            continue;
        }


        // 尝试加载完整的normgroup数据
        if (!LoadNormgroupData_i(*aNormGroup, image3D))
        {
            TPS_LOG_DEV_ERROR << "Failed to load normgroup data from plan";
            return false;
        }

        // add to data manager
        // rcc: 当一个normgroup数据完整加载到内存后，才将normgroup对象加入到内存管理
        if (!AddNormgroupToNGManager_i(aNormGroup, &aPlan))
        {
            TPS_LOG_DEV_ERROR << "Failed to add normgroup to normgroup manager.";
            return false;
        }
    }
    return true;
}

bool TpsLoadObjects::AddNormgroupToNGManager_i(RtNormgroup* aNormGroup, RtPlan* plan)
{
    if (aNormGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR << "aNormGroup is nullptr";
        return false;
    }
    std::string strNgUid = aNormGroup->get_uid();
    if (!mDataRepository->GetNormgroupManager()->AddNormGroup(aNormGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to add this normgroup to normgroup manager.";
        return false;
    }

    RtDbDef::DoseCalculateMode doseCalculateMode = (plan->get_plansourcetype() == (int)RtDbDef::QAPlan) ? RtDbDef::BASE_ON_MU : RtDbDef::BASE_ON_WEIGHT;

    if (!mDataRepository->GetNormgroupManager()->AddNgDoseCalculateMode(aNormGroup->get_uid(), doseCalculateMode))
    {
        TPS_LOG_DEV_ERROR << "Failed to add dose calculate mode.";
        return false;
    }

    TPS_LOG_DEV_INFO << "add normgroup[" << strNgUid << "] to normgroup manager.";

    return true;
}

// load following data:
// 1. all beams
// 2. dose grid for this normgroup if exist. if not exist, give a warning.
bool TpsLoadObjects::LoadNormgroupData_i(RtNormgroup& aNormGroup,
    TpsImage3DEntity* image3D)
{

    const std::string strNgUid = aNormGroup.get_uid();

    //load drrsetting,每个normalgroup只有一个drrsetting，这里接口有误，暂时先用一下
    std::vector<RtDrrsetting*> drrsettings;
    mDatabaseWrapper->GetDrrsettingListByNormgroupUid(strNgUid, drrsettings);
    if (drrsettings.size() == 1)
    {
        mDataRepository->GetNormgroupManager()->AddDrrSetting(drrsettings[0]);
    }

    // 如果没有所谓的“original point”，会导致后面加载Beam的失败，所以这里
    // 是后面继续加载Beam的前提
    if (image3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "image3D is null.";
        return false;
    }

    Mcsf::Point3f orgPt = *image3D->GetSetupPoint();

    RtImage3DHeader* header = nullptr;
    if (!image3D->GetImage3DHeader(header) || nullptr == header)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image 3d header.";
        return false;
    }

    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(aNormGroup.get_machineuid(), machine)
        || machine == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsMachineManager::GetMachine Failed.";
        mLastError = TPS_ER_RELATED_MACHINE_ERROR;
        return false;
    }

    std::vector<RtBeam*> vRtBeamList;
    // 下面的这个接口，会创建出beam的对象列表，如果后面遇到逻辑失败，需要自己释放这些对象
    int iRet = mDatabaseWrapper->GetBeamListByNormgroupUid(strNgUid, vRtBeamList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "GetBeamListByNormgroupUid failed";
        return false;
    }

    RtDoseoptgoalbase* optGoal = nullptr;
    std::vector<RtDoseoptgoalbase*> vDoseOptGoals;
    mDatabaseWrapper->GetDoseoptgoalbaseListByNormgroupUid(strNgUid, vDoseOptGoals);
    for (auto itOptGoal = vDoseOptGoals.begin(); itOptGoal != vDoseOptGoals.end(); ++itOptGoal)
    {
        optGoal = *itOptGoal;
        mDataRepository->GetOptGoalManager()->AddOptGoal(*optGoal);
    }

    // load all beams.
    bool bInterrupted = false;
    std::string invalidBeamUid("");
    RtDosegrid* pDoseGridBeam = nullptr;

    Mcsf::Matrix4f mat;
    image3D->GetVolumeToPatientMatrix(mat);

    for (auto itrBeam = vRtBeamList.begin(); itrBeam != vRtBeamList.end(); ++itrBeam)
    {
        RtBeam* pBeam = *itrBeam;
        //继续维护这个字段
        if (pBeam->get_beamsegments().size() != 0)
        {
            pBeam->set_ssd(pBeam->get_beamsegments().front()->get_startssd());
        }
        const std::string beamUid = pBeam->get_uid();
        TPS_LOG_DEV_INFO << "loading beam[" << beamUid << "]...";

        //TODO should be sorted by SequenceNumber as 300A:0112 Control Point Index
        //bRet = RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(&aBeam);
        //if(!bRet){
        //    TPS_LOG_DEV_ERROR<<"load beam from TMS failed with beam UID:"<< *itrBeam;
        //    return false;
        //}

        //pBeam->set_isjoincalc(true);

        RtSeries* pSeries = image3D->GetRtSeries();
        if (nullptr == pSeries)
        {
            TPS_LOG_DEV_ERROR << "pSeries is null.";
            return false;
        }

        if (!AddBeamIECCS_i(orgPt, pSeries->get_patientposition(), machine->get_sad(), *pBeam))
        {
            TPS_LOG_DEV_ERROR << "Failed to call AddBeamIECCS_i";
            invalidBeamUid = beamUid;
            bInterrupted = true;
            break;
        }

        // 加载Beam的dose grid，如果存在的话
        pDoseGridBeam = pBeam->get_dosegrid();
        if (nullptr == pDoseGridBeam)
        {
            TPS_LOG_DEV_ERROR << "Invalid beams as dosegrid is null for beamuid:[" << invalidBeamUid << "].";
            return false;
        }

        Mcsf::Matrix4f matP2G = TpsArithmeticConverter::ConvertToMatrix4f(pDoseGridBeam->get_grid_to_pat_t()).Inverse();
        mat.Prepend(matP2G);
        Mcsf::Vector3f vecX = mat.Transform(Mcsf::Vector3f(1, 0, 0));
        Mcsf::Vector3f vecY = mat.Transform(Mcsf::Vector3f(0, 1, 0));
        Mcsf::Vector3f vecZ = mat.Transform(Mcsf::Vector3f(0, 0, 1));
        Mcsf::Point3f org = mat.Transform(Mcsf::Point3f(0, 0, 0));

        //当导入一个外部的plan时，可能会出现没有DoseGrid的情况，此时内存中会通过RtBeam的构造函数生成一个默认的DoseGrid，
        //而且此DoseGrid的uid为empty，但是这个DoseGrid的值都是默认值。所以应该按照当前参数给这个beam生成一个合适的DoseGrid
        if (pDoseGridBeam->get_uid().empty())
        {
            if (!HandleEmptyDosegridForBeam_i(pBeam, image3D->GetUID()))
            {
                TPS_LOG_DEV_ERROR << "Failed to handle empty beam dosegrid case";
                invalidBeamUid = beamUid;
                bInterrupted = true;
                break;
            }
        }

        //因为数据库现在支持可空字段，所以load上来的DoseGridBuffer可能是空的，而当前算法不支持buffer为空，所以需要生成一个默认的buffer
        if (nullptr == pDoseGridBeam->get_dosegrid_buffer())
        {
            pDoseGridBeam->create_dosegrid_buffer();
        }

        //创建segment的beam_patient矩阵
        TpsIECCoordinateSystem* iecCS = mDataRepository->GetBeamManager()->GetBeamCS(beamUid);
        Mcsf::Matrix4f matB2P;
        std::vector<RtBeamsegment*> segments = pBeam->get_beamsegments();
        std::vector<RtBeamsegment*>::iterator itSeg = segments.begin();
        for (; itSeg != segments.end(); ++itSeg)
        {
            RtBeamsegment* segment = *itSeg;
            float gantryAngle = segment->get_startgantryangle();
            iecCS->RotateGantry(gantryAngle);
            if (!iecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P))
            {
                TPS_LOG_DEV_ERROR << "Failed to get B2P matrix for segment[" << segment->get_uid() << "].";
                return false;
            }
            float arrB2P[16];
            matB2P.GetElements(arrB2P);
            segment->set_t_beam_to_pat(arrB2P);
        }

#ifdef _DEBUG
        if (!mDoseProxy->CheckValidDosegrid(*pDoseGridBeam))
        {
            TPS_LOG_DEV_ERROR << "Invalid dosegrid or dosegrid buffer under beam[" << pBeam->get_uid() << "].";
        }
#endif

        //for poi dose
        std::vector<RtBeam_poi*> vList;
        int iRet = mDatabaseWrapper->GetPoiDoseListByBeamUid((*itrBeam)->get_uid(), vList);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to get poi dose list by beamuid:" << (*itrBeam)->get_uid();
        }

        for (auto itr = vList.begin(); itr != vList.end(); ++itr)
        {
            RtPoi* poi = mDataRepository->GetPoiManager()->GetPOI((*itr)->get_poiuid());

            // beam dose contribution
            poi->set_poidose((*itr)->get_beamuid(), (*itr)->get_dosecontribution());

            //total plan dose
            poi->set_plandoseinfo(aNormGroup.get_planuid(), poi->get_plandose(aNormGroup.get_planuid()) + (*itr)->get_dosecontribution());
            DEL_PTR(*itr);
        }

        //for 2D image
        if (!LoadImage2DDataByBeamUID(pBeam->get_uid()))
        {
            TPS_LOG_DEV_ERROR << "Failed to load Image2DData from the beam.";
            return false;
        }
    }

    // 如果bInterrupted为真，说明加载的beam中有不合法的情况存在，
    // 那么判定整个加载失败，接着释放所有对象内存
    if (bInterrupted)
    {
        TPS_LOG_DEV_ERROR << "Invalid beams exist in the database:[" << invalidBeamUid << "].";
        ClearBeamList_i(vRtBeamList);
        return false;
    }

    // 加入内存管理，但是如果有beam加入beam managr失败，该如何处理？留待后续统一考虑
    for (auto itrBeam = vRtBeamList.begin(); itrBeam != vRtBeamList.end(); ++itrBeam)
    {
        RtBeam* pBeam = *itrBeam;
        if (!mDataRepository->GetBeamManager()->AddBeam2Map(*itrBeam))
        {
            TPS_LOG_DEV_ERROR << "Failed to add to beam manager. beam[" << pBeam->get_uid() << "].";
            return false;
        }
    }

    // 检查Normgroup的dosegrid对象是否为RtNormgroup构造函数默认生成的
    RtDosegrid* pDoseGridNg = aNormGroup.get_dosegrid();
    if (pDoseGridNg->get_uid().empty())
    {
        pDoseGridNg = aNormGroup.get_dosegrid();
        if (pDoseGridBeam)
        {
            *pDoseGridNg = *pDoseGridBeam;//deep copied from one beam
            pDoseGridNg->clear_dosegrid_buffer();
        }
        else if (!GenerateDoseGrid_i(image3D->GetUID(), pDoseGridNg))
        {
            TPS_LOG_DEV_ERROR<<"Failed to generate dose grid for normal group.";
            return false;
        }
    }

    //检查Normgroup的dosegrid buffer是否为空
    if (nullptr == pDoseGridNg->get_dosegrid_buffer())
    {
        pDoseGridNg->create_dosegrid_buffer();
    }

    // Add fluence map entities.
    FluenceMapEntity* fluenceMap = nullptr;
    for (auto it = vRtBeamList.begin(); it != vRtBeamList.end(); ++it)
    {
        fluenceMap = new FluenceMapEntity();
        if (!mDoseProxy->CalculateFluenceMap(*it, fluenceMap))
        {
            fluenceMap->Clear();
        }
        mDataRepository->GetBeamManager()->AddFluenceMap((*it)->get_uid(), fluenceMap);
    }

    return true;
}

bool TpsLoadObjects::LoadImage3D(const std::string& sImage3DUID)
{
    TpsImage3DEntity* image3DEntity = nullptr;
    if (mDataRepository->GetImageDataManager()->GetImage3DData(sImage3DUID, &image3DEntity))
    {
        TPS_LOG_DEV_INFO << "Image3D which UID: " << sImage3DUID << " has already been loaded.";
        return true;
    }

    RtSeries* pSeries = nullptr;
    pSeries->set_uid(sImage3DUID);
    int iRet = mDatabaseWrapper->GetSeriesByUid(sImage3DUID, pSeries);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load series object from database with uid[" << sImage3DUID << "].";
        DEL_PTR(pSeries);
        return false;
    }

    if (!LoadImage3D_i(pSeries))
    {
        TPS_LOG_DEV_ERROR << "Failed to load Image 3D object uid:" << sImage3DUID;
        return false;
    }

    if (!LoadVOIListFromSeries_i(*pSeries))
    {
        TPS_LOG_DEV_WARNING << "LoadVOIListFromSeries_i failed with series uid: " << sImage3DUID;
        return false;
    }

    if (!LoadPOIListFromSeries_i(sImage3DUID))
    {
        TPS_LOG_DEV_WARNING << "LoadPOIListFromSeries_i failed with series uid: " << sImage3DUID;
        return false;
    }
    return true;
}

int TpsLoadObjects::LoadMachineTree()
{
    TPS_LOG_DEV_INFO << "Load machine tree ENTER!";
    int result = TPS_ER_FAILURE;
    // 读取所有的Approverecord记录。
    std::vector<RtApproverecord*> vApproverecordList;
    result = mDatabaseWrapper->GetApproveRecordList(vApproverecordList);
    if (DB_CODES_SUCCESS != result)
    {
        TPS_LOG_DEV_ERROR << "Failed to get ApproveRecord from database.";
        return TPS_ER_DB_ERROR;
    }

    // 取出所有Approve过的MachineUid。
    std::vector<std::string> vApprovedMachineUidList;
    std::for_each(vApproverecordList.begin(), vApproverecordList.end(),
        [&](RtApproverecord* item)
    {
        if (item->get_objecttype() == RtDbDef::Machine && item->get_isapproved())
        {
            vApprovedMachineUidList.push_back(item->get_objectuid());
        }

        mDataRepository->GetMachineManager()->AddApproveRecord(item);
    });

    result = LoadMachines_i(vApprovedMachineUidList);
    if (TPS_ER_SUCCESS != result)
    {
        TPS_LOG_DEV_ERROR << "Failed to get LoadMachines_i.";
        return result;
    }

    if (!ReadMachineLeafGapOffset_i())
    {
        TPS_LOG_DEV_INFO << "ReadMachineLeafGapOffset_i fail";
    }

    LoadKernelData();

    TPS_LOG_DEV_INFO << "Load machine tree LEAVE!";
    return result;
}

bool TpsLoadObjects::ReadMachineLeafGapOffset_i()
{
    Mcsf::ISystemEnvironmentConfig* systemEnviromentConfig =
        Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
    if (systemEnviromentConfig == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][TpsDisplayDelegate][Initialize]: "
            << "Failed to get system environment.";
        return false;
    }
    std::string cfgPath = systemEnviromentConfig->GetApplicationPath("QMCDataBasePath");

    std::string algConfigFile = cfgPath + "RT-PI-ALGCongfiPara.xml";

    FILE * file = nullptr;
    fopen_s(&file, algConfigFile.c_str(), "r");
    if (file == NULL)
    {
        TPS_LOG_DEV_ERROR << "Failed to open the RT-PI-ALGCongfiPara.xml! Please check the path\n";
        return false;
    }

    fclose(file);

    try
    {
        ptree pt;
        boost::property_tree::read_xml(algConfigFile.c_str(), pt);

        float leafGapOffsetSmallFieldSize = pt.get<float>("Config.MachineConfig.MinLeafGapOffset_InSmallFieldSize_mm.<xmlattr>.Value");
        mDataRepository->GetMachineManager()->SetLeafGapOffsetInSmallField(leafGapOffsetSmallFieldSize);
        float leafGapOffsetLargeFieldSize = pt.get<float>("Config.MachineConfig.MinLeafGapOffset_InLargeFieldSize_mm.<xmlattr>.Value");
        mDataRepository->GetMachineManager()->SetLeafGapOffsetInLargeField(leafGapOffsetLargeFieldSize);
    }
    catch (std::exception& e)
    {
        TPS_LOG_DEV_ERROR << "caught exception in ReadMachineLeafGapOffset_i:" << e.what();
        return false;
    }

    return true;
}


int TpsLoadObjects::LoadMachines_i(const std::vector<std::string>& approvedMachineUidList)
{
    // 取出所有相关的Machine
    std::vector<RtMachine*> vMachineList;
    int iRet = mDatabaseWrapper->GetMachineList(approvedMachineUidList, vMachineList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to get all machines from database.";
        return TPS_ER_DB_ERROR;
    }
    // 剔除未激活的机器
    std::vector<RtMachine*> validMachineList = vMachineList;
    std::vector<std::string> validMachineUidList = approvedMachineUidList;
    //暂时去除ACTIVE判断，待以后添加
    // 	for (int i = 0; i < vMachineList.size(); ++i)
    // 	{
    // 		//two states now, 1: active, 0: inactive
    // 		if (vMachineList[i]->get_state() == 1)
    // 		{
    // 			validMachineList.push_back(vMachineList[i]);
    // 			validMachineUidList.push_back(vMachineList[i]->get_uid());
    // 		}
    // 	}

    if (validMachineUidList.empty())
    {
        TPS_LOG_DEV_INFO << "There is no machine.";
        return TPS_ER_SUCCESS;
    }

    // 取出所有相关的Machine下所有的Commissionedunit
    std::vector<RtCommissionedunit*> vCommissionedunitList;
    iRet = mDatabaseWrapper->GetCommissionedunitListByMachineUidList(validMachineUidList, vCommissionedunitList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to get all commissioned units from database.";
        return TPS_ER_DB_ERROR;
    }
    if (vCommissionedunitList.empty())
    {
        TPS_LOG_DEV_ERROR << "There is no commissioned unit in the machines.";
        return TPS_ER_SUCCESS;
    }

    //剔除没有unit的machine
    bool hasUnit = false;
    for (auto itrMachine = validMachineList.begin(); itrMachine != validMachineList.end(); ++itrMachine)
    {
        if (nullptr == *itrMachine) continue;

        hasUnit = false;
        for (auto itrUnit = vCommissionedunitList.cbegin(); itrUnit != vCommissionedunitList.cend(); ++itrUnit)
        {
            if (nullptr == *itrUnit) continue;

            if ((*itrUnit)->get_machineuid() == (*itrMachine)->get_uid())
            {
                hasUnit = true;
                break;
            }
        }
        if (hasUnit)
        {
            std::string sMD5("");
            sMD5 = mDatabaseWrapper->CalcHash((const void*)(*itrMachine), RtDbDef::TYPE_MACHINE);
            bool bOK = sMD5 == (*itrMachine)->get_crc();
            (*itrMachine)->set_crc_valid(bOK);
            if (!bOK)
            {
                TPS_LOG_DEV_ERROR<<(*itrMachine)->get_uid()<<"\t DB CRC:"<<(*itrMachine)->get_crc()<<"\t current CRC:"<<sMD5;
            }
            mDataRepository->GetMachineManager()->AddMachine((*itrMachine)->get_uid(), *itrMachine);
        }
        else
        {
            DEL_PTR(*itrMachine);//delete usefulness's machines
        }
    }
    //将unit加入内存管理
    std::vector<std::string> vApprovedCommissionedunitUidList;
    std::for_each(vCommissionedunitList.begin(), vCommissionedunitList.end(),
        [&](RtCommissionedunit* item)
    {
        vApprovedCommissionedunitUidList.push_back(item->get_uid());
        mDataRepository->GetMachineManager()->AddCommissonedUnit(item->get_uid(), item);
    });

    // 取出所有相关的Commissionedunit下所有的Accessorychunk
    std::vector<RtAccessorychunk*> vRtAccessorychunkList;
    iRet = mDatabaseWrapper->GetAccessorychunkListByCommissionedunitUidList(
        vApprovedCommissionedunitUidList, vRtAccessorychunkList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to get all accessory chunk from database.";
        return TPS_ER_DB_ERROR;
    }
    for (auto itAccChunk = vRtAccessorychunkList.begin(); itAccChunk != vRtAccessorychunkList.end(); ++itAccChunk)
    {
        RtContour* profile = nullptr;
        if (DB_CODES_SUCCESS == mDatabaseWrapper->GetContourByAccessorychunkuid((*itAccChunk)->get_uid(), profile))
        {
            (*itAccChunk)->set_contouruid(profile->get_uid());
        }

        //add accessory chunk for physical wedges
        mDataRepository->GetMachineManager()->AddAccessoryChunk(*itAccChunk);
        // this part is hard coded contour for accessory chunk profile.
        if ((*itAccChunk)->get_uid() == "ideal_comp_chunk")
        {
            continue;
        }

        if (nullptr != profile)
        {
            mDataRepository->GetMachineManager()->AddAccessoryChunkProfile(profile);
        }
    }

    // 取出所有相关的Commissionedunit下所有的RtGoldenstt
    std::vector<RtGoldenstt*> vGoldenstt;
    iRet = mDatabaseWrapper->GetGoldensttListByCommissionedunitUidList(vApprovedCommissionedunitUidList, vGoldenstt);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to get all golden stt from database.";
        return TPS_ER_DB_ERROR;
    }

    // 取出所有相关的Accessorychunk中AccessoryUid
    std::vector<std::string> vApprovedAccessoryUidList;
    std::for_each(vRtAccessorychunkList.begin(), vRtAccessorychunkList.end(),
        [&](RtAccessorychunk* item)
    {
        vApprovedAccessoryUidList.push_back(item->get_accessoryuid());
    });

    // 取出所有相关的RtGoldenstt中AccessoryUid
    for (auto itrGldSTT = vGoldenstt.begin(); itrGldSTT != vGoldenstt.end(); ++itrGldSTT)
    {
        vApprovedAccessoryUidList.push_back((*itrGldSTT)->get_accessoryuid());
        mDataRepository->GetMachineManager()->AddGoldenstt(*itrGldSTT);
    }

    //会有重复的AccessoryUid，但是不需要处理。
    //std::unique(vApprovedAccessoryUidList.begin(), vApprovedAccessoryUidList.end());

    // 取出所有相关的Accessory。
    std::vector<RtAccessory*> vRtAccessoryList;
    iRet = mDatabaseWrapper->GetAccessoryListByAccessoryUidList(vApprovedAccessoryUidList, vRtAccessoryList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to get all accessory from database.";
        return TPS_ER_DB_ERROR;
    }

    std::for_each(vRtAccessoryList.begin(), vRtAccessoryList.end(),
        [&](RtAccessory* item)
    {
        mDataRepository->GetMachineManager()->AddAccessory(item);
    });

    // 由于现阶段不支持电子治疗，所以Applicator暂时不读取。
    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadKernelData()
{
    // step 1. kernel data
    std::vector<RtKerneldata*> vKerneldatas;
    mDatabaseWrapper->GetAllKerneldata(vKerneldatas);
    for (std::vector<RtKerneldata*>::iterator it = vKerneldatas.begin(); it != vKerneldatas.end(); ++it)
    {
        mDataRepository->GetMachineManager()->AddKerneldata(*it);
    }
    // step 2. kernel data list
    std::vector<RtKerneldatalist*> vKerneldatalists;
    mDatabaseWrapper->GetAllKerneldatalists(vKerneldatalists);
    for (std::vector<RtKerneldatalist*>::iterator it = vKerneldatalists.begin(); it != vKerneldatalists.end(); ++it)
    {
        mDataRepository->GetMachineManager()->AddKerneldatalist(*it);
    }
    // step 3. kernel weight
    std::vector<RtKernelweight*> vKernelweights;
    mDatabaseWrapper->GetAllKernelWeights(vKernelweights);
    for (std::vector<RtKernelweight*>::iterator it = vKernelweights.begin(); it != vKernelweights.end(); ++it)
    {
        mDataRepository->GetMachineManager()->AddKernelweight(*it);
    }
    // step 4. tray
    std::vector<RtTray*> vTray;
    mDatabaseWrapper->GetAllTrays(vTray);
    for ( std::vector<RtTray*>::iterator it = vTray.begin(); it != vTray.end(); ++it)
    {
        mDataRepository->GetMachineManager()->AddTray(*it);
    }

    // step 5 mcphocommissionedunit and mcphomonoenergyweight
    std::vector<RtMcphocommissionedunit*> vMcphocommissonedunit;
    mDatabaseWrapper->GetAllMcphocommissionedunits(vMcphocommissonedunit);
    for (std::vector<RtMcphocommissionedunit*>::iterator it = vMcphocommissonedunit.begin(); it != vMcphocommissonedunit.end(); ++it)
    {
        mDataRepository->GetMachineManager()->AddMcphocommissionedunit(*it);
        std::vector<RtMcphomonoenergyweight*> phoMonoEnergyWeightList;
        mDatabaseWrapper->GetMcphomonoenergyweightListByMcphocommissionedunituid((*it)->get_uid(), phoMonoEnergyWeightList);
        mDataRepository->GetMachineManager()->AddMcphomonoenergyweight((*it)->get_uid(), phoMonoEnergyWeightList);
    }

    // load voi settings
    std::vector<RtVoisetting*> vVoiSettingList;
    mDatabaseWrapper->GetVoiSettingList(vVoiSettingList);
    mDataRepository->GetVoiManager()->SetVoiSettings(vVoiSettingList);

    // load tables, the couch template, which is common data for all patients
    if (!LoadRtTables_i())
    {
        TPS_LOG_DEV_ERROR << "Failed to load RtTables.";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

bool TpsLoadObjects::LoadPrescription()
{
    return true;
}

bool TpsLoadObjects::LoadImage3D_i(RtSeries* const pSeries)
{
    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;

    TpsImageDataManager* pTmp = mDataRepository->GetImageDataManager();
    if (nullptr == pTmp)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image data manager;";
        return false;
    }

    TpsImage3DEntity* pTpsImage3D = pTmp->CreateImage3D();
    if (pTpsImage3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "pTpsImage3D is nullptr";
        return false;
    }
    if (!pTpsImage3D->SetupImage3D(pSeries))
    { // get Image3D from series
        delete pTpsImage3D;
        pTpsImage3D = nullptr;
        TPS_LOG_DEV_ERROR << "Failed to set up image 3d object.";
        return false;
    }
    // set up a default slice buffer for each Image3d Entity
    if (!pTpsImage3D->InitSliceBuffer())
    {
        TPS_LOG_DEV_ERROR << "Failed to Initialize slice buffer.";
        return false;
    }

    std::string seriesUid = pSeries->get_uid();
    if (!mDataRepository->GetImageDataManager()->AddImage3D(seriesUid, pTpsImage3D))
    {
        delete pTpsImage3D;
        pTpsImage3D = nullptr;
        TPS_LOG_DEV_ERROR << "Failed to add image 3d object(uid: " << seriesUid << ") into ImageDataManager.";
        return false;
    }

    if (pSeries->get_modality() == "PT")
    {
        //load data header
        auto spConvertor = Mcsf::DICOMConvertorFactory::Instance()->CreateSimpleConvertor();
        char* fileBuffer = nullptr;
        size_t size;
        if (!mDatabaseWrapper->GetDICOMImageBySeriesUid(seriesUid, fileBuffer, size) || fileBuffer == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to get DICOM image file buffer.";
            return false;
        }
        Mcsf::IDataHeaderElementMap* dataHeader(Mcsf::IDataHeaderElementMap::CreateDataHeader());
        if (!spConvertor->FastLoadFile(fileBuffer, size, dataHeader))
        {
            TPS_LOG_DEV_ERROR << "Failed to convert image file to data header.";
            delete[] fileBuffer;
            return false;
        }
        delete[] fileBuffer;
        if (!mDataRepository->GetImageDataManager()->AddDataHeader(seriesUid, dataHeader))
        {
            TPS_LOG_DEV_ERROR << "Failed to add series to DataHeaderElement Map";
            delete dataHeader;
            return false;
        }
    } 

    //load series table, if not exist, add an empty series table
    if (!LoadSeriesTable_i(*pTpsImage3D))
    {
        TPS_LOG_DEV_ERROR << "Failed to load series table.";
        return false;
    }

    //TEST_PERFORMANCE_END("BE TpsLoadObjects::LoadImage3D_i");
    return true;
}

bool TpsLoadObjects::LoadImage3DForGallery_i(RtSeries* const pSeries)
{
    TpsGalleryImageManager* pTmp = mDataRepository->GetGalleryImageManager();
    if (nullptr == pTmp)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image data manager;";
        return false;
    }

    TpsImage3DEntity* pTpsImage3D = pTmp->CreateImage3D();
    if (pTpsImage3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "pTpsImage3D is nullptr";
        return false;
    }
    if (!pTpsImage3D->SetupImage3D(pSeries))
    { // get Image3D from series
        delete pTpsImage3D;
        pTpsImage3D = nullptr;
        TPS_LOG_DEV_ERROR << "Failed to set up image 3d object.";
        return false;
    }
    // set up a default slice buffer for each Image3d Entity
    if (!pTpsImage3D->InitSliceBuffer())
    {
        TPS_LOG_DEV_ERROR << "Failed to Initialize slice buffer.";
        return false;
    }

    std::string seriesUid = pSeries->get_uid();
    if (!mDataRepository->GetGalleryImageManager()->AddImage3D(seriesUid, pTpsImage3D))
    {
        delete pTpsImage3D;
        pTpsImage3D = nullptr;
        TPS_LOG_DEV_ERROR << "Failed to add image 3d object(uid: " << seriesUid << ") into ImageDataManager.";
        return false;
    }

    if (pSeries->get_modality() == "PT")
    {
        if (!mDataRepository->GetGalleryImageManager()->AddDataHeader(pSeries))
        {
            TPS_LOG_DEV_ERROR << "Failed to add series to DataHeaderElement Map";
            return false;
        }
    }
    return true;
}

bool TpsLoadObjects::PrepareOneImageTextString_i(FTStringBufferGenerator& bufGen,
    const std::string& strText, const std::string& strKey)
{
        // create string text buffer
        //TEST_PERFORMANCE_INIT;
        //TEST_PERFORMANCE_BEGIN;
        Pixel32* pStringBuf = nullptr;
        int iStringBufWidth = 0;
        int iStringBufHeight = 0;

        if (strText.empty())
        {
            TPS_LOG_DEV_WARNING << "Empty string. Skip creating the buffer for display.";
            return false;
        }

        // Convert string to wstring
        std::wstring wstrText(strText.begin(), strText.end());

        if (!bufGen.GenerateRasterBuffer(wstrText, &pStringBuf,
            iStringBufWidth, iStringBufHeight))
        {
            TPS_LOG_DEV_ERROR << "Failed to generate raster buffer.\n";
            return false;
        }

        // store the buffer into data manager
        if (!mDataRepository->GetImageDataManager()->AddImageTextData(strKey,
            (unsigned char*)pStringBuf, iStringBufWidth, iStringBufHeight))
        {
            TPS_LOG_DEV_ERROR << "Failed to add image text data into data manager";
            return false;
        }

        //TEST_PERFORMANCE_END("End of composing string buffer.");
        return true;
}

//void TpsLoadObjects::SetDefaultImage_i(const std::vector<RtSeries*>& vecSeries)
//{
//    using namespace RTFWK;
//
//    std::string sDefaultSeriesUID("");
//    DATE_BOOST dateLatest(boost::date_time::min_date_time);
//    TIME_BOOST timeLatest(boost::date_time::min_date_time);
//    for (int i = 0; i < vecSeries.size(); ++i)
//    {
//        DATE_BOOST date = vecSeries[i]->get_seriesdate();
//        if (!date.is_not_a_date())
//        {
//            TIME_BOOST time = vecSeries[i]->get_seriestime();
//            if (date > dateLatest)
//            {
//                dateLatest = date;
//                if (!time.is_not_a_date_time())
//                {
//                    timeLatest = time;
//                }
//                sDefaultSeriesUID = vecSeries[i]->get_uid();
//            }
//            else if (date == dateLatest)
//            {
//                if (time >= timeLatest)
//                {
//                    timeLatest = time;
//                    sDefaultSeriesUID = vecSeries[i]->get_uid();
//                }
//            }
//        }
//        else
//        {
//            // Since 4D CT does not have a correct Series Date/Time, 
//            // we need to set the default seriesUid the first one
//            sDefaultSeriesUID = vecSeries[0]->get_uid();
//        }
//    }
//
//    if (!sDefaultSeriesUID.empty())
//    {
//        mDataRepository->GetImageDataManager()->SetDefaultImage3D(sDefaultSeriesUID);
//    }
//}

bool TpsLoadObjects::LoadVOIListFromSeries_i(const RtSeries& tmsSeries)
{
    const std::string sSeriesUid = tmsSeries.get_uid();

    //Load VOI list from TMS
    std::vector<RtVoi*> vecVois;
    if (mDatabaseWrapper->GetVoiListBySeriesUid(sSeriesUid, vecVois) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to get voi objects from database by series uid";
        return false;
    }
    if (vecVois.empty())
    {
        TPS_LOG_DEV_INFO << "None of vois under series[" << sSeriesUid << "].";
        return true;
    }

    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(sSeriesUid, &image) || image == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The image 3d data haven't been loaded yet.";
        //delete vecVois
        return false;
    }

    std::vector<RtVoi*>::iterator itVoi = vecVois.begin();
    std::set<std::string> voiNameExisted;
    for (; itVoi != vecVois.end(); ++itVoi)
    {
        //mBoostThread.create_thread(boost::bind(&TpsLoadObjects::LoadVOIFromDatabase_i, this, *itVoi, image));
        LoadVOIFromDatabase_i(*itVoi, image);
        //DEL_PTR(*itVoi);
    }
    //delete vecVois
    //mBoostThread.join_all();
    return true;
}

void TpsLoadObjects::LoadVOIFromDatabase_i(RtVoi* pVoi, TpsImage3DEntity* image)
{
    //printf("\nenter LoadVOIFromDatabase_i...");
    if (pVoi == nullptr || image == nullptr)
    {
        TPS_LOG_DEV_ERROR << "pVoi is null or image is nullptr.";
        return;
    }
    std::string voiUid = pVoi->get_uid();
    std::string seriesUid = image->GetUID();
    pVoi->set_seriesuid(seriesUid);

    ROIEntity* roi = mDataRepository->GetVoiManager()->CreateROIEntity(pVoi);
    roi->SetIsQA(image->GetIsQACopied());
    if (!LoadROI_i(seriesUid, roi))
    {
        TPS_LOG_DEV_ERROR << "LoadROI_i for VOI(uid" << voiUid << ") failed!";
        TPS_DEL_PTR(roi);
        return;
    }
    mDataRepository->GetVoiManager()->AddROI(roi);

    //UpdateVOIStatisticalInformation

    auto innerRoi = roi->GetInnerRoi();
    if (!innerRoi->has_field(VOI_VOLUME)    && 
        !innerRoi->has_field(VOI_CENTROIDX) && 
        !innerRoi->has_field(VOI_CENTROIDY) && 
        !innerRoi->has_field(VOI_CENTROIDZ))
    {
        //means never do voi statistical, maybe it's imported from other's TPS
        RtCt2density* pCt2Density = mDataRepository->GetImageDataManager()->GetCT2DensityTableByImageUid(seriesUid);
        if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image, roi))
        {
            TPS_LOG_DEV_WARNING << "Failed to UpdateVOI StatisticalInformation VOI:" << voiUid;
        }
        mDatabaseWrapper->UpdateVoiAttributes(*pVoi);
    }
    // record successfully loaded tps VOI
    TPS_LOG_DEV_INFO << "VOI(uid" << voiUid << ") loaded!";
    //printf("\nexit LoadVOIFromDatabase_i...");
    return;
}

bool TpsLoadObjects::LoadPOIListFromSeries_i(const std::string& sSeriesUid)
{
    std::vector<RtPoi*> vPoiList;
    int iRet = mDatabaseWrapper->GetPoiListBySeriesUid(sSeriesUid, vPoiList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "LoadPOIListFromSeries_i failed with seriesuid:" << sSeriesUid;
        return false;
    }

    // 这里需要考虑错误情况下的内存回收和清理工作
    //for poi dose, poi dose move to loadbeam function
    for (auto itrPoi = vPoiList.begin(); itrPoi != vPoiList.end(); ++itrPoi)
    {
        if (!mDataRepository->GetPoiManager()->AddPOI(*itrPoi))
        {
            TPS_LOG_DEV_ERROR << "Add POI Into TpsPOIManager failed! POIUID:" << *itrPoi;
            return false;
        }
    }

    return true;
}

bool TpsLoadObjects::LoadDoseLineParamsFromPlan_i(const RtPlan* plan)
{
    if (nullptr == plan)
    {
        TPS_LOG_DEV_ERROR << "pPlan is null";
        return false;
    }
    const std::string sPlanUid = plan->get_uid();
    std::string planUid = plan->get_uid();

    std::vector<RtDoseline*> vDoselineList;
    int iRet = mDatabaseWrapper->GetDoselineListByPlanUid(sPlanUid, vDoselineList);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "GetDoselineListByPlanUid failed with planuid:" << sPlanUid;
        return false;
    }
    // for imported plan
    if (vDoselineList.size() == 0)
    {
        std::vector<DoseLineTemplate*> doseLineTemplateList;
        if (!mDataRepository->GetConfigurationManager()->GetDoseLineTemplateList(doseLineTemplateList))
        {
            TPS_LOG_DEV_ERROR << "Failed to GetDoseLineTemplateList from TpsConfigurationManager.";
            return false;
        }

        std::list<DoseLineTemplateItem> doseLineTemplateItemList;
        for (auto itDoseLineTemplate = doseLineTemplateList.begin();
            itDoseLineTemplate != doseLineTemplateList.end(); ++itDoseLineTemplate)
        {
            if ((*itDoseLineTemplate)->GetDoseLineTemplateName() == "DEFAULT_DOSELINE_CONFIG")
            {
                (*itDoseLineTemplate)->GetDoseLineTemplateItemList(doseLineTemplateItemList);
                for (auto itDoseLineTemplateItem = doseLineTemplateItemList.begin();
                    itDoseLineTemplateItem != doseLineTemplateItemList.end(); ++itDoseLineTemplateItem)
                {
                    float percent = itDoseLineTemplateItem->GetDosePercent();
                    const float* fColor = itDoseLineTemplateItem->GetColor();
                    RtDoseline* doseline= new RtDoseline(true);
                    doseline->set_planuid(plan->get_uid());
                    doseline->set_red(fColor[0]);
                    doseline->set_green(fColor[1]);
                    doseline->set_blue(fColor[2]);
                    doseline->set_alpha(fColor[3]);
                    doseline->set_doseratio(percent);
                    doseline->set_isvisible(true);
                    vDoselineList.push_back(doseline);
                    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDoselineObject(doseline)) 
                    {
                        TPS_LOG_DEV_ERROR << "Fail to save doseLine into db";
                        return false;
                    }
                }
                break;
            }
        }
    }

    if (!mDataRepository->GetDoseManager()->AddDoseLineList(planUid, vDoselineList))
    {
        TPS_LOG_DEV_ERROR << "Add DoseLine Into TpsDoseManager failed! PlanUID:" << planUid;
        return false;
    }

    return true;
}


bool LessFunc(RtContour* s1, RtContour* s2)
{
    return s1->get_sliceindex() < s2->get_sliceindex();
}

bool TpsLoadObjects::LoadROI_i(const std::string& imgUID, ROIEntity* roi)
{
    TPS_LOG_DEV_INFO << "[INFO][TpsLoadObjects][LoadROI_i]: "
        << "Load ROI execute.";

    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;

    RtVoi* innerRoi = roi->GetInnerRoi();
    if (innerRoi == nullptr)
    {
        //log
        return false;
    }
    std::string voiUid = innerRoi->get_uid();

    TpsImage3DEntity* img3D = nullptr;
    mDataRepository->GetImageDataManager()->GetImage3DData(imgUID, &img3D);
    if (nullptr == img3D)
    {
        TPS_LOG_DEV_ERROR << "img3D is null with ImageDataUID:" << imgUID;
        return false;
    }

    RtImage3DHeader* header = nullptr;
    if (!img3D->GetImage3DHeader(header) || header == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image header of image: " << imgUID;
        return false;
    }

    Mcsf::Matrix4f matPatientToVolume;
    if (!img3D->GetVolumeToPatientMatrix(matPatientToVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix.";
        return false;
    }
    matPatientToVolume = matPatientToVolume.Inverse();
    float matP2V[16];
    matPatientToVolume.GetElements(matP2V);
    innerRoi->set_pat2volumematrix(matP2V);

    std::vector<RtContour*> rtcontours = innerRoi->get_contours();
    SignedDistanceField* sdf = roi->GetSDF();
    if (rtcontours.empty())
    {
        sdf->Clear();
        TPS_LOG_DEV_INFO << "No contours in this voi[" << voiUid << "]";
        return true;
    }

    std::vector<std::vector<PT3>> contours;
    for (int i = 0; i < rtcontours.size(); ++i)
    {
        auto db_contour = rtcontours[i]->get_contour_points();
        if (db_contour.size() < 3) continue;

        std::vector<PT3> contour(db_contour.size());
        std::transform(db_contour.begin(), db_contour.end(), contour.begin(), 
            [](const db_Point3f& db_pt)->PT3
            {
                return PT3(db_pt.x, db_pt.y, db_pt.z);
            });
        contours.push_back(contour);
    }
    if (!SubVolumeHelperAPI::SetupContourSet(img3D, contours, roi))
    {
        TPS_LOG_DEV_ERROR << "Failed to setup ana-struct.";
        return false;
    }

    printf("\nprocess roi: %s", innerRoi->get_name().c_str());

    ContourSet* cs = roi->GetContourSet();
    if (!ROIShapeConverter::ContourSetToSDF(*cs, *sdf, false))
    {
        TPS_LOG_DEV_ERROR<<"Failed to convert contour set to SDF.";
        return false;
    }
    TPS_LOG_DEV_INFO << "[INFO][TpsLoadObjects][LoadROI_i]: "
        << "Load ROI leave!";
    return true;
}

bool TpsLoadObjects::AddBeamIECCS_i(const Mcsf::Point3f &orgPt,
    std::string patientPosition,
    float sad,
    RtBeam &beam)
{
    const std::string sBeamUid = beam.get_uid();
    std::vector<RtBeamsegment*> segments = beam.get_beamsegments();
    if (segments.size() == 0)
    {
        TPS_LOG_DEV_ERROR << "No segments in the Beam[" << sBeamUid << "].";
        return false;
    }

    RtBeamsegment* segment = segments.front();
    if (segment == nullptr)
    {
        TPS_LOG_DEV_ERROR << "No first segment in the Beam[" << sBeamUid << "].";
        return false;
    }


    PatientPosition pp = TpsCmdHelper::GetPatientPosition(patientPosition);

    TpsIECCoordinateSystem* iecCS = new TpsIECCoordinateSystem();
    if (!iecCS->Initialize(orgPt, pp, sad, segment->get_startgantryangle(),
        segment->get_startcollimatorangle(),
        segment->get_starttablerotationangle()))
    {
        TPS_LOG_DEV_ERROR << "Failed to initialize IEC coordinate system.";
        delete iecCS; 
        iecCS = nullptr;
        return false;
    }

    std::string isoCenterUID = beam.get_isocenterpoiuid();
    auto isoCenter = mDataRepository->GetPoiManager()->GetPOI(isoCenterUID);
    if (isoCenter == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The iso center does not exist which uid: " << isoCenterUID;
        delete iecCS;
        iecCS = nullptr;
        return false;
    }

    iecCS->SetISOCenter(Mcsf::Point3f(
        isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(),
        isoCenter->get_coordinate3dz()));

    if (mDataRepository->GetBeamManager()->AddBeamCS(sBeamUid, iecCS) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to AddBeamCS with BeamUid:" << isoCenterUID;
        delete iecCS; return false;
    }

    return true;
}

int TpsLoadObjects::LoadVOITemplate(std::vector<VOITemplate*>& voiTemplateList)
{
    auto manager = mDataRepository->GetConfigurationManager();
    if (manager == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    manager->GetVOITemplateList(&voiTemplateList);

    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadBeamTemplate(std::vector<BeamTemplate*>& beamTemplateList)
{
    auto manager = mDataRepository->GetConfigurationManager();
    if (manager == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    manager->GetBeamTemplateList(&beamTemplateList);

    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadOptimizerTemplateList(std::vector<OptimizerTemplate*>& optimizerTemplateList)
{
    auto manager = mDataRepository->GetConfigurationManager();
    if (manager == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    manager->GetOptimizerTemplateList(&optimizerTemplateList);

    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadDoseLineTemplate(std::vector<DoseLineTemplate*>& doseLineTemplateList)
{
    auto manager = mDataRepository->GetConfigurationManager();
    if (manager == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    manager->GetDoseLineTemplateList(doseLineTemplateList);

    return TPS_ER_SUCCESS;
}

bool TpsLoadObjects::LoadImage2DData(const std::string& sImage2DUid)
{
    bool ret = true;

    if (sImage2DUid.empty())
    {
        TPS_LOG_DEV_INFO << "Image2D of UID is empty.";
        return ret;
    }

    TpsImage2DEntity *image2D = nullptr;
    ret = mDataRepository->GetImageDataManager()->GetImage2DData(sImage2DUid, &image2D);
    if (true == ret)
    {
        TPS_LOG_DEV_INFO << "Image2D of UID: " << sImage2DUid << " already been loaded.";
        return ret;
    }

    RtRtimage* rtImage = nullptr;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetRtImageByUid(sImage2DUid, rtImage))
    {
        TPS_LOG_DEV_ERROR << "Load rt image failed.";
        return false;
    }

    TpsImage2DEntity *pTpsImage_DRR = new TpsImage2DEntity();
    std::string sImageUID = rtImage->get_uid();
    pTpsImage_DRR->SetRTImage(rtImage);
    ret = mDataRepository->GetImageDataManager()->AddImage2D(sImageUID, pTpsImage_DRR);
    return ret;
}

bool TpsLoadObjects::LoadImage2DDataByBeamUID(const std::string& beamUID)
{
    std::vector<RtRtimage*> rtImagesList;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetRtImageListByBeamUid(beamUID, rtImagesList))
    {
        TPS_LOG_DEV_ERROR << "Load rt image failed.";
        return false;
    }
    //beam没有关联drr是正常情况
    if (rtImagesList.size() == 0) return true;

    for (auto itrRtImage = rtImagesList.begin(); itrRtImage != rtImagesList.end(); ++itrRtImage)
    {
        RtRtimage* pRtImage = *itrRtImage;
        TpsImage2DEntity *pTpsImage_DRR = new TpsImage2DEntity();
        std::string sImageUID = pRtImage->get_uid();
        pTpsImage_DRR->SetRTImage(pRtImage);
        if (!mDataRepository->GetImageDataManager()->AddImage2D(sImageUID, pTpsImage_DRR))
        {
            TPS_LOG_DEV_ERROR << "AddImage2D failed";
            return false;
        }
    }
    return true;
}

bool TpsLoadObjects::LoadImageTransformation_i(const std::string &fixSeriesUID)
{
    TpsImageDataManager *imgMgr = mDataRepository->GetImageDataManager();
    //get fixed image entity
    TpsImage3DEntity* fixedImage = nullptr;
    if (!imgMgr->GetImage3DData(fixSeriesUID, &fixedImage) || fixedImage == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The fixed image does not exist which uid: " << fixSeriesUID;
        return false;
    }

    std::vector<RtImagetransformation*> vecFloatList;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetImagetransformationListBySeriesUid(
        fixSeriesUID, vecFloatList))
    {  
        TPS_LOG_DEV_ERROR << "Failed to load image transformation list.";  
        return false;
    }

    for (auto it = vecFloatList.begin(); it != vecFloatList.end(); ++it)
    {
        fixedImage->SetImagetransformation((*it));
    }

    return true;
}

bool TpsLoadObjects::LoadSystemCT2DensityTables()
{
    std::vector<RtCt2density*> vecCt2DensityMapList;
    if (mDatabaseWrapper->GetAllCt2densityObjects(vecCt2DensityMapList))
    {
        TPS_LOG_DEV_ERROR << "Failed to get all ct2density maps from database";
        return false;
    }

    if (vecCt2DensityMapList.size() < 1)
    {
        TPS_LOG_DEV_ERROR << "None of ct2density maps loaded! Exception.";
        return false;
    }

    for (auto iter = vecCt2DensityMapList.begin(); iter != vecCt2DensityMapList.end(); ++iter)
    {
        RtCt2density* p = *iter;
        if (!mDataRepository->GetImageDataManager()->AddCT2DensityMap(*p))
        {
            TPS_LOG_DEV_ERROR << "Failed to add ct2density map into image data manager. uid["
                << p->get_uid() << "].";
            continue;
        }
    }

    return true;
}

bool TpsLoadObjects::LoadEasyPlanData_i(RtPlan &easyPlan)
{
    // rcc: 暂时注掉，等合适的时候再打开
    // 2D image
    //  TODO get rtimageuid from plan's beam
    /*if (!LoadImage2DData(easyPlan.GetRTImageUID())) {
    TPS_LOG_DEV_ERROR<<"Failed to LoadImage2DData";
    return false;
    }*/

    // load normgroup of the plan
    if (!LoadEasyNormgroupsByPlan_i(easyPlan))
    {
        TPS_LOG_DEV_ERROR << "Failed to load normgroups from the easy plan.";
        return false;
    }

    return true;
}

bool TpsLoadObjects::LoadEasyNormgroupsByPlan_i(RtPlan &easyPlan)
{
    using namespace RTFWK;

    std::vector<RtNormgroup*> vNormGroupList;
    std::string planUid = easyPlan.get_uid();
    int iRet = mDatabaseWrapper->GetNormgroupListByPlanUid(planUid, vNormGroupList);
    if (iRet != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load normgroup under plan[" << planUid << "].";
        return false;
    }
    if (vNormGroupList.empty())
    {
        TPS_LOG_DEV_INFO << "No normgroups under this plan";
        return true;
    }

    // load all normgroups, if one failed, fail all.
    for (auto itrNG = vNormGroupList.begin(); itrNG != vNormGroupList.end(); ++itrNG)
    {
        if (nullptr == *itrNG)
        {
            TPS_LOG_DEV_ERROR << "Failed to load normgroup under plan[" << planUid << "].";
            return false;
        }

        const std::string strNgUid = (*itrNG)->get_uid();
        TPS_LOG_DEV_INFO << "loading normgroup[" << strNgUid << "]...";

        RtMachine* machine = nullptr;
        if (!mDataRepository->GetMachineManager()->GetMachine((*itrNG)->get_machineuid(), machine)
            || machine == nullptr)
        {
            TPS_LOG_DEV_ERROR << "TpsMachineManager::GetMachine Failed.";
            return false;
        }

        std::vector<RtBeam*> vBeamList;
        int iRet = mDatabaseWrapper->GetBeamListByNormgroupUid(strNgUid, vBeamList);
        if (iRet != DB_CODES_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "GetBeamListByNormgroupUid failed with normgroup uid[" << strNgUid << "].";
            return false;
        }

        // no beam under a normgroup is allowed.
        if (vBeamList.empty())
        {
            TPS_LOG_DEV_WARNING << "No beams under this normgroup.";
            //return true;
        }
        // load all beams.
        for (auto itrBeam = vBeamList.begin(); itrBeam != vBeamList.end(); ++itrBeam)
        {
            if (nullptr == *itrBeam)
            {
                TPS_LOG_DEV_ERROR << "Failed to load beam under normgroup " << strNgUid << "].";
                return false;
            }

            TPS_LOG_DEV_INFO << "loading beam[" << *itrBeam << "]...";

            const std::string sBeamUid = (*itrBeam)->get_uid();
            //TODO should be sorted by SequenceNumber as 300A:0112 Control Point Index
            //aBeam.SetIsJoined(true);

            // 加载Beam的dose grid，如果存在的话
            RtDosegrid* pDoseGrid = (*itrBeam)->get_dosegrid();
            if (nullptr == pDoseGrid || nullptr == pDoseGrid->get_dosegrid_buffer())
            {
                TPS_LOG_DEV_ERROR << "Invalid dosegrid or dosegrid buffer under beam[" << sBeamUid << "].";
                return false;
            }

            // 最后成功了才会加入内存管理
            if (!mDataRepository->GetBeamManager()->AddBeam2Map(*itrBeam))
            {
                TPS_LOG_DEV_ERROR << "Failed to add to beam manager.";
                return false;
            }

            // 将Beam 对应(2D Image)RtImage Load到内存里
            std::string beamRelatedRtImgUID = (*itrBeam)->get_relatedrtimageuid();
            if (!LoadImage2DData(beamRelatedRtImgUID))
            {
                TPS_LOG_DEV_ERROR<<"Failed to LoadImage2DData";
                return false;
            }

            // 重新建立EasyPlan Image 与 Beam 的对应关系
            if((*itrBeam)->get_beamnumber() == 1)
            {
                mDataRepository->GetImageDataManager()->SetEasyPlanImageUID( planUid, beamRelatedRtImgUID);
            }
            else
            {
                mDataRepository->GetImageDataManager()->SetEasyPlanOpposedImageUID(planUid, beamRelatedRtImgUID);
            }

            TPS_LOG_DEV_INFO << "add beam[" << *itrBeam << "] to beam manager.";
        }

        // add to data manager
        // rcc: 当一个normgroup数据完整加载到内存后，才将normgroup对象加入到内存管理
        if (!AddNormgroupToNGManager_i(*itrNG, &easyPlan))
        {
            TPS_LOG_DEV_ERROR << "Failed to add normgroup to normgroup manager.";
            return false;
        }
    }
    return true;
}

bool TpsLoadObjects::LoadPlanReportData(const std::string& planUID, const std::string& courseUID)
{
    RtPlan *plan = new RtPlan;
    plan->set_uid(planUID);

    //Get PlanUIDs by Course
    std::vector<RtPlan*> vecRtPlanList;
    if (DB_CODES_SUCCESS == mDatabaseWrapper->GetPlanListByCourseUid(courseUID,vecRtPlanList))
    {
        for (auto itPlan = vecRtPlanList.begin(); itPlan != vecRtPlanList.end(); ++itPlan) 
        {
            RtPlan* aPlan = *itPlan;
            if (aPlan != nullptr && planUID == aPlan->get_uid())
            {
                plan = aPlan;
            }
        }
    }

    RtSeries *series = new RtSeries;
    int iRet = mDatabaseWrapper->GetSeriesByUid(plan->get_imageseriesuid(), series);
    if (iRet!=DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load series object from DB";
        return false;
    }

    if (!LoadImage3D_i(series))
    {
        TPS_LOG_DEV_ERROR << "Failed to load image 3d with SeriesUID: "<<series->get_uid();
        return false;
    }

    if (!LoadVOIListFromSeries_i(*series))
    {
        TPS_LOG_DEV_ERROR << "Failed to load voi under series with SeriesUID:"<<series->get_uid();
        return false;
    }

    if (!LoadPOIListFromSeries_i(series->get_uid()))
    {
        TPS_LOG_DEV_ERROR << "Failed to load poi under series with SeriesUID:"<<series->get_uid();
        return false;
    }

    if (!LoadPlanData_i(*plan))
    {
        TPS_LOG_DEV_ERROR << "Failed to load plan object from DB";
        return false;
    }

    if (!mDataRepository->GetPatientManager()->AddPlan(plan))
    {
        TPS_LOG_DEV_ERROR << "Failed to add plan into patient mgr.";
        return false;
    }

    if (!mDoseCalculation->ComposeDoseForPlanWithoutRendering(planUID)) return false;
    return true;
}

bool TpsLoadObjects::ComposeDoseForPlanReport(const std::string& planUID)
{
    if(mDoseCalculation->ComposeDoseForPlanWithoutRendering(planUID))
    {
        return true;
    }

    return false;
}

int TpsLoadObjects::LoadTemplate()
{
    std::vector<RtTemplate*> vRtSystemconfigList;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetSystemconfigList(vRtSystemconfigList))
    {
        TPS_LOG_DEV_WARNING << "Failed to get voi template!";
        return TPS_ER_DB_ERROR;
    }

    auto manager = mDataRepository->GetConfigurationManager();
    if (manager == nullptr)
    {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return TPS_ER_FAILURE;
    }

    for (auto itSystemconfig = vRtSystemconfigList.begin();
        itSystemconfig != vRtSystemconfigList.end(); ++itSystemconfig)
    {
        if (!mDataRepository->GetConfigurationManager()->AddSystemConfig(**itSystemconfig))
        {
            TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
            return TPS_ER_FAILURE;
        }

        int dataType = (*itSystemconfig)->get_datatype();

        if (dataType == WINDOWLEVEL_WINDOWWIDTH_TEMPLATE_TYPE)
        {
            //当前窗宽窗位的template和其他的存储有些不同
            continue;
        }

        try
        {
            std::istringstream buffer((*itSystemconfig)->get_value());
            boost::archive::text_iarchive ia(buffer);
            switch (dataType)
            {
            case DEFAULT_DOSELINE_TEMPLATE:
            case DOSELINE_TEMPLATE: 
                {
                    DoseLineTemplate doseLineInfoGroup;
                    ia >> doseLineInfoGroup;
                    if (!manager->AddDoseLineTemplate(doseLineInfoGroup))
                    {
                        TPS_LOG_DEV_ERROR << "Failed to add doseline template to manager!";
                        continue;
                    }
                }
                break;
            case VOI_TEMPLATE_TYPE: 
                {
                    VOITemplate* voiTemplate = new VOITemplate();
                    ia >> *voiTemplate;
                    if (!manager->AddVOITemplate(voiTemplate)) {
                        TPS_LOG_DEV_ERROR << "Failed to add voi template to manager!";
                        continue;
                    }
                }
                break;
            case BEAM_TEMPLATE_TYPE:
                {
                    BeamTemplate* beamTemplate = new BeamTemplate();
                    ia >> *beamTemplate;
                    if (!manager->AddBeamTemplate(beamTemplate)) {
                        TPS_LOG_DEV_ERROR << "Failed to add beam template to manager!";
                        continue;
                    }
                }
                break;
            case OPTIMIZER_TEMPLATE_TYPE: 
                {
                    OptimizerTemplate* optimizerTemplate = new OptimizerTemplate();
                    ia >> *optimizerTemplate;
                    if (!manager->AddOptimizerTemplate(optimizerTemplate)) {
                        TPS_LOG_DEV_ERROR << "Failed to add beam template to manager!";
                        continue;
                    }
                }
                break;
            default:
                break;
            }
        }
        catch (boost::exception& ex)
        {
            typedef boost::error_info<struct tag_err_no, int> err_no;
            typedef boost::error_info<struct tag_err_str, std::string> err_str;
            std::cout << *boost::get_error_info<err_str>(ex) << std::endl;
            continue;
        }
    }

    return TPS_ER_SUCCESS;
}

int TpsLoadObjects::LoadWindowingTemplateList(std::vector<RtTemplate*>& windowingTemplateList)
{
    //std::vector<RtTemplate*> allSystemConfig;
    //mDataRepository->GetConfigurationManager()->GetAllSystemConfig(allSystemConfig);


    //for (auto itSystemconfig = allSystemConfig.begin();
    //    itSystemconfig != allSystemConfig.end(); ++itSystemconfig)
    //{
    //    int dataType = (*itSystemconfig)->get_datatype();
    //    if (dataType == WINDOWLEVEL_WINDOWWIDTH_TEMPLATE_TYPE)
    //    {
    //        windowingTemplateList.push_back(*itSystemconfig);
    //    }
    //}

    mDataRepository->GetConfigurationManager()
        ->GetSystemConfigFromDatatype(WINDOWLEVEL_WINDOWWIDTH_TEMPLATE_TYPE, windowingTemplateList); 


    return TPS_ER_SUCCESS;
}


bool TpsLoadObjects::GeneratePlanDoseGrid_i(RtPlan& aPlan)
{
    RtDosegrid* doseGrid = aPlan.get_dosegrid();

    std::vector<std::string> vecNGUIDs =
        mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(aPlan.get_uid());
    if (vecNGUIDs.empty())
    {
        //if there is no normal group in the plan, create default dose grid
        DosegridInfo info;
        info.spacingx = DEFAULT_DOSE_GRID_SPACING;
        info.spacingy = DEFAULT_DOSE_GRID_SPACING;
        info.spacingz = DEFAULT_DOSE_GRID_SPACING;

        TpsImage3DEntity* image3DEntity = nullptr;
        if (!mDataRepository->GetImageDataManager()->GetImage3DData(aPlan.get_imageseriesuid(), &image3DEntity)){
            TPS_LOG_DEV_ERROR<<"Failed to get the image data.";
            return TPS_ER_FAILURE;
        }

        ROIEntity* skinVoi = mDataRepository->GetVoiManager()->GetExternal(aPlan.get_imageseriesuid());

        if (!mDataRepository->GetDoseManager()->InitializeDosegrid(info, image3DEntity, skinVoi, *doseGrid))
        {
            TPS_LOG_DEV_ERROR << "Failed to initialize default empty plan dosegrid.";
            return false;
        }
    }
    else
    {
        //clone an empty dose grid from its first child
        RtNormgroup* ng = nullptr;
        if (!mDataRepository->GetNormgroupManager()->GetNormGroup(vecNGUIDs[0], &ng))
        {
            TPS_LOG_DEV_ERROR << "Failed to get normal group.";
            return false;
        }

        RtDosegrid* ngDosegrid = ng->get_dosegrid();
        doseGrid->set_xcoordinate3dstart(ngDosegrid->get_xcoordinate3dstart());
        doseGrid->set_ycoordinate3dstart(ngDosegrid->get_ycoordinate3dstart());
        doseGrid->set_zcoordinate3dstart(ngDosegrid->get_zcoordinate3dstart());
        doseGrid->set_xcoordinate3dinc(ngDosegrid->get_xcoordinate3dinc());
        doseGrid->set_ycoordinate3dinc(ngDosegrid->get_ycoordinate3dinc());
        doseGrid->set_zcoordinate3dinc(ngDosegrid->get_zcoordinate3dinc());
        doseGrid->set_grid_to_pat_t(ngDosegrid->get_grid_to_pat_t());
        doseGrid->set_xcount(ngDosegrid->get_xcount());
        doseGrid->set_ycount(ngDosegrid->get_ycount());
        doseGrid->set_zcount(ngDosegrid->get_zcount());
        doseGrid->set_accuracy(ngDosegrid->get_accuracy());
        doseGrid->set_isdosevalid(ngDosegrid->get_isdosevalid());
        doseGrid->create_dosegrid_buffer();
    }


    return true;
}

bool TpsLoadObjects::GenerateDoseGrid_i(const std::string& seriesUid,
    RtDosegrid*& dosegridOut)
{
    dosegridOut->set_isdosevalid(false);
    dosegridOut->set_bitsallocated(TPS_BITSALLOCATED);
    dosegridOut->set_bitsstored(TPS_BITSALLOCATED);
    dosegridOut->set_highbits(TPS_BITSALLOCATED -1);

    // fix bug for third part plan withou dosegrid
    dosegridOut->set_uid(mDatabaseWrapper->GeneraterUid());

    DosegridInfo info;
    info.spacingx = DEFAULT_DOSE_GRID_SPACING;
    info.spacingy = DEFAULT_DOSE_GRID_SPACING;
    info.spacingz = DEFAULT_DOSE_GRID_SPACING;

    TpsImage3DEntity* image3DEntity = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image3DEntity)){
        TPS_LOG_DEV_ERROR<<"Failed to get the image data.";
        return TPS_ER_FAILURE;
    }

    ROIEntity* skinVoi = mDataRepository->GetVoiManager()->GetExternal(seriesUid);

    if (!mDataRepository->GetDoseManager()->InitializeDosegrid(info, image3DEntity, skinVoi, *dosegridOut)) {
        TPS_LOG_DEV_ERROR << "Failed to initialize default empty plan dosegrid.";
        return false;
    }
    return true;
}

void TpsLoadObjects::ClearBeamList_i(std::vector<RtBeam*>& beamList)
{
    auto itBeam = beamList.begin();
    for (; itBeam != beamList.end(); ++itBeam)
    {
        RtBeam* pBeam = *itBeam;
        delete pBeam;
        pBeam = nullptr;
    }

    beamList.clear();
}

bool TpsLoadObjects::HandleEmptyDosegridForBeam_i(RtBeam* pBeam,
    const std::string& seriesUid)
{
    if (pBeam == nullptr)
    { 
        TPS_LOG_DEV_ERROR << "pBeam==nullptr";
        return false;
    }
    std::string beamUid = pBeam->get_uid();
    //pBeam->create_dosegrid();
    RtDosegrid* pDoseGrid = pBeam->get_dosegrid();
    if (!GenerateDoseGrid_i(seriesUid, pDoseGrid))
    {
        TPS_LOG_DEV_ERROR << "Failed to generate dose grid for beam.";
        return false;
    }

    /*  TpsIECCoordinateSystem* iecCS = mDataRepository->GetBeamManager()->GetBeamCS(beamUid);
    Mcsf::Matrix4f matB2P;
    std::vector<RtBeamsegment*> segments = pBeam->get_beamsegments();
    std::vector<RtBeamsegment*>::iterator itSeg = segments.begin();
    for (; itSeg != segments.end(); ++itSeg) {
    RtBeamsegment* segment = *itSeg;
    float gantryAngle = segment->get_startgantryangle();
    iecCS->RotateGantry(gantryAngle);
    if (!iecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P)){
    TPS_LOG_DEV_ERROR << "Failed to get B2P matrix for segment[" << segment->get_uid() << "].";
    return false;
    }
    float arrB2P[16];
    matB2P.GetElements(arrB2P);
    segment->set_t_beam_to_pat(arrB2P);
    }*/
    return true;
}

bool TpsLoadObjects::LoadMachineSetting()
{

    return true;
}

bool TpsLoadObjects::LoadRtTables_i()
{
    std::vector<RtTablecontour*> tableContours;
    int retCode = mDatabaseWrapper->GetAllTablecontourList(tableContours);
    if (retCode != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"DB errors when loading RtTables.";
        return false;
    }

    //现在的Table按照HFS下的Patient C.S.存储，将其转到IEC Patient Support坐标系下
    //以后Table可以按照Table坐标系存储，则无需再做这个转换
    ConvertToIECTableSpace_i(tableContours);

    auto tblMgr = mDataRepository->GetTableManager();
    RtTable* table = nullptr;
    std::string tableUid;
    for (int i = 0; i < tableContours.size(); ++i)
    {
        if (tableContours[i] == nullptr) continue;
        tableUid = tableContours[i]->get_tableuid();
        table = tblMgr->GetRtTable(tableUid);
        if (table == nullptr)
        {
            table = new RtTable();
            table->SetUid(tableUid);
            table->SetName(tableContours[i]->get_tablename());
            tblMgr->AddRtTable(table);
        }
        if (!table->AddTableContour(tableContours[i]))
        {
            TPS_LOG_DEV_ERROR<<"Failed to add table contour.";
            return false;
        }
    }

    std::vector<RtTable*> vecTables(std::move(tblMgr->GetAllRtTables()));
    for (int i = 0; i < vecTables.size(); ++i)
    {
        if (!this->SetupTableVolume_i(vecTables[i]))
        {
            TPS_LOG_DEV_ERROR<<"Failed to setup table volume, ignore the table.";
            tblMgr->RemoveRtTable(vecTables[i]->GetUid());
            continue;
        }
    }
    return true;
}

void TpsLoadObjects::ConvertToIECTableSpace_i(std::vector<RtTablecontour*>& contours)
{
    std::vector<db_Point3d> points;
    for (int i = 0; i < contours.size(); ++i)
    {
        points = contours[i]->get_points();
        for (int j = 0; j < points.size(); ++j)
        {
            swap(points[j].y, points[j].z);
            points[j].z *= -1;
        }
        contours[i]->set_points(points);
    }
}

bool TpsLoadObjects::SetupTableVolume_i(RtTable* table)
{
    //get the bounding box of contours in patient c.s.
    double boundary[6] = 
    {
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min()
    };

    auto contours = table->GetTableContours();
    std::vector<db_Point3d> points;
    int count = 0;
    for (int i = 0; i < contours.size(); ++i)
    {
        points = std::move(contours[i]->get_points());
        for (int j = 0; j < points.size(); ++j)
        {
            boundary[0] = TPS_MIN(points[j].x, boundary[0]);
            boundary[1] = TPS_MAX(points[j].x, boundary[1]);
            boundary[2] = TPS_MIN(points[j].y, boundary[2]);
            boundary[3] = TPS_MAX(points[j].y, boundary[3]);
            boundary[4] = TPS_MIN(points[j].z, boundary[4]);
            boundary[5] = TPS_MAX(points[j].z, boundary[5]);
            count++;
        }
    }
    //handle empty points case
    if (count == 0)
    {
        table->ClearTableMask();
        return true;
    }
    //spacing, using thickness/3 as the pixel spacing in lateral & vertical direction 
    double thickness = contours[0]->get_thickness();
    double spacing[3] = { thickness / 3, thickness / 3, thickness };
    //table contour按照longitudinal方向存储，每个contour里所有point.y相同
    //为使每层contour转为mask之后在volume内连续，建立table volume：
    //vecVolumeX = vecTableX（lateral）， vecVolumeY = vecTableZ(vertical), 
    //vecVolumeZ = vecTableY(longitudinal), 原点为bounding box的最小点
    Mcsf::Matrix4f matTable2Volume(
        Mcsf::Vector4f(spacing[0], 0, 0, 0), 
        Mcsf::Vector4f(0, 0, spacing[1], 0),
        Mcsf::Vector4f(0, spacing[2], 0, 0),
        Mcsf::Vector4f(boundary[0], boundary[2], boundary[4], 1));
    matTable2Volume = matTable2Volume.Inverse();
    table->SetTableToMaskVolume(matTable2Volume);

    //create mask buffer
    unsigned int dim[3] = 
    {
        ceil((boundary[1] - boundary[0]) / spacing[0]) + 1,
        ceil((boundary[5] - boundary[4]) / spacing[1]) + 1,
        ceil((boundary[3] - boundary[2]) / spacing[2]) + 1
    };

    table->SetMaskDim(dim);
    auto tableMask = table->GetMask();
    //convert contour to mask & update the volume mask
    Mcsf::Point3f vertices[4] = 
    {
        Mcsf::Point3f(0, 0, 0),
        Mcsf::Point3f(dim[0] - 1, 0, 0),
        Mcsf::Point3f(dim[0] - 1, dim[1] - 1, 0),
        Mcsf::Point3f(0, dim[1] - 1, 0)
    };

    int slice;
    Mcsf::Point3f point;
    std::vector<std::vector<Mcsf::Point2f>> vecContour;
    std::vector<Mcsf::Point2f> contour;
    unsigned char* buffer = new unsigned char[dim[0] * dim[1]];
    for (int i = 0; i < contours.size(); ++i)
    {
        vecContour.clear();
        points = std::move(contours[i]->get_points());
        if (points.empty()) continue;
        point = matTable2Volume.Transform(Mcsf::Point3f(points[0].x, points[0].y, points[0].z));
        slice = point.GetZ();
        for (int j = 0; j < 4; ++j)
        {
            vertices[j].SetZ(slice);
        }
        contour.clear();
        contour.reserve(points.size());
        for (int j = 0; j < points.size(); ++j)
        {
            point = matTable2Volume.Transform(Mcsf::Point3f(points[j].x, points[j].y, points[j].z));
            contour.push_back(std::move(Mcsf::Point2f(point.GetX(), point.GetY())));
        }
        vecContour.push_back(contour);

        memset(buffer, 0, dim[0] * dim[1] * sizeof(unsigned char));
        if (TPS_ER_SUCCESS != SubVolumeHelperAPI::GetMaskFromContours(vecContour, dim[0], dim[1], buffer))
        {
            TPS_DEL_ARRAY(buffer);
            table->ClearTableMask();
            TPS_LOG_DEV_ERROR<<"GetMaskFromContours error!";
            return false;
        }

        /*char fileName[256];
        sprintf_s(fileName, "d:\\temp\\contour_mask\\mask_%d_slice_%d.bmp",i,slice);
        SubVolumeHelperAPI::SaveToGrayBitmap(fileName, buffer,dim[1], dim[0]);*/

        //直接对slice层进行或操作，避免调用update subvolume算法
        for (int j = 0; j < dim[0] * dim[1]; ++j)
        {
            tableMask[slice * dim[0] * dim[1] + j] |= buffer[j];
        }
    }
    TPS_DEL_ARRAY(buffer);
    return true;
}

bool TpsLoadObjects::LoadSeriesTable_i(const TpsImage3DEntity& image3D)
{
    std::string seriesUID = image3D.GetUID();

    SeriesTable* srsTbl = new SeriesTable();
    srsTbl->SetSeriesUid(seriesUID);
    RtSeriestable* innerTable = nullptr;
    if (DB_CODES_SUCCESS == mDatabaseWrapper->GetSeriestable(
        seriesUID, innerTable) && innerTable != nullptr)
    {
        srsTbl->CopyFrom(innerTable);
        TPS_DEL_PTR(innerTable);
    }

    auto tblMng = mDataRepository->GetTableManager();
    std::string tblUID = srsTbl->GetTableUid();
   // RtTable* tbl = tblMng->GetRtTable(tblUID);

	RtTable* tbl = nullptr;
	srsTbl->SetTableUid("");
	if (!InitializeTableLocation(&image3D, tbl, srsTbl))
	{
	    TPS_LOG_DEV_ERROR<<"Failed to initialize table location";
	    TPS_DEL_PTR(srsTbl);
	    return false;
	}

    tblMng->AddSeriesTable(seriesUID, srsTbl);
    return true;
}

bool TpsLoadObjects::InitializeTableLocation(
    const TpsImage3DEntity* image3d, 
    const RtTable* rtTable, 
    SeriesTable* seriesTable)
{
    if (rtTable == nullptr)
    {
        seriesTable->ClearTransformation();
        return true;
    }

    Mcsf::Matrix4f matV2P;
    if (!image3d->GetVolumeToPatientMatrix(matV2P))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix.";
        return false;
    }

    RtImage3DHeader* header = nullptr;
    if (!image3d->GetImage3DHeader(header) || header == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d header.";
        return false;
    }

    std::string strPP = header->m_cPatientPosition;
    boost::to_upper(strPP);
    PatientPosition pp;
    if (strPP == "HFS") pp = HFS;
    else if (strPP == "FFS") pp = FFS;
    else if (strPP == "HFP") pp = HFP;
    else if (strPP == "FFP") pp = FFP;
    else if (strPP == "HFDL") pp = HFDL;
    else if (strPP == "FFDL") pp = FFDL;
    else if (strPP == "HFDR") pp = HFDR;
    else if (strPP == "FFDR") pp = FFDR;
    else
    {
        TPS_LOG_DEV_ERROR<<"Unrecognized patient position.";
        return false;
    }

    int dim[3] = 
    {
        header->m_iXDim, header->m_iYDim, header->m_iSliceCount 
    };
    Mcsf::Point3f volumeVertices[8] = 
    {
        Mcsf::Point3f(0.f, 0.f, 0.f),
        Mcsf::Point3f(dim[0] - 1, 0.f, 0.f),
        Mcsf::Point3f(dim[0] - 1, dim[1] - 1, 0.f),
        Mcsf::Point3f(0.f, dim[1] - 1, 0.f),
        Mcsf::Point3f(0.f, 0.f, dim[2] - 1),
        Mcsf::Point3f(dim[0] - 1, 0.f, dim[2] - 1),
        Mcsf::Point3f(dim[0] - 1, dim[1] - 1, dim[2] - 1),
        Mcsf::Point3f(0.f, dim[1] - 1, dim[2] - 1)
    };
    float srsBdry[6] = 
    {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest()
    };
    for (int i = 0; i < 8; ++i)
    {
        volumeVertices[i] = matV2P.Transform(volumeVertices[i]);
        srsBdry[0] = TPS_MIN(volumeVertices[i].GetX(), srsBdry[0]);
        srsBdry[1] = TPS_MAX(volumeVertices[i].GetX(), srsBdry[1]);
        srsBdry[2] = TPS_MIN(volumeVertices[i].GetY(), srsBdry[2]);
        srsBdry[3] = TPS_MAX(volumeVertices[i].GetY(), srsBdry[3]);
        srsBdry[4] = TPS_MIN(volumeVertices[i].GetZ(), srsBdry[4]);
        srsBdry[5] = TPS_MAX(volumeVertices[i].GetZ(), srsBdry[5]);
    }

    unsigned int tblDim[3];
    rtTable->GetMaskDim(tblDim);
    Mcsf::Matrix4f matVol2Tbl = rtTable->GetTableToMaskVolume().Inverse();

    volumeVertices[0] = Mcsf::Point3f(0.f, 0.f, 0.f);
    volumeVertices[1] = Mcsf::Point3f(tblDim[0] - 1, 0.f, 0.f);
    volumeVertices[2] = Mcsf::Point3f(tblDim[0] - 1, tblDim[1] - 1, 0.f);
    volumeVertices[3] = Mcsf::Point3f(0.f, tblDim[1] - 1, 0.f);
    volumeVertices[4] = Mcsf::Point3f(0.f, 0.f, tblDim[2] - 1);
    volumeVertices[5] = Mcsf::Point3f(tblDim[0] - 1, 0.f, tblDim[2] - 1);
    volumeVertices[6] = Mcsf::Point3f(tblDim[0] - 1, tblDim[1] - 1, tblDim[2] - 1);
    volumeVertices[7] = Mcsf::Point3f(0.f, tblDim[1] - 1, tblDim[2] - 1);
    float tblBdry[6] = 
    {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest()
    };
    for (int i = 0; i < 8; ++i)
    {
        volumeVertices[i] = matVol2Tbl.Transform(volumeVertices[i]);
        tblBdry[0] = TPS_MIN(volumeVertices[i].GetX(), tblBdry[0]);
        tblBdry[1] = TPS_MAX(volumeVertices[i].GetX(), tblBdry[1]);
        tblBdry[2] = TPS_MIN(volumeVertices[i].GetY(), tblBdry[2]);
        tblBdry[3] = TPS_MAX(volumeVertices[i].GetY(), tblBdry[3]);
        tblBdry[4] = TPS_MIN(volumeVertices[i].GetZ(), tblBdry[4]);
        tblBdry[5] = TPS_MAX(volumeVertices[i].GetZ(), tblBdry[5]);
    }

    Mcsf::Matrix4f matTbl2Pat = TpsLoadObjects::CalcTableToPatientViaPatientPosition_i(
        pp, srsBdry, tblBdry);
    seriesTable->SetInitLocation(matTbl2Pat);
    return true;
}

TPS_ERROR_CODES TpsLoadObjects::GetLastError() const
{
    return mLastError;
}

Mcsf::Matrix4f TpsLoadObjects::CalcTableToPatientViaPatientPosition_i(
    int patientPosition, const float* srsBdry, const float* tblBdry)
{
    //IEC病床坐标系各轴方向，面向病床时：X从右向左，Y指向加速器，Z从下向上
    //根据当前病人的(Patient Position)，可以得到病床的一个较为合理的初始摆位。
    //偏移，将床板放在病人相应方位的体数据最底沿，病床的其他两个方向取中心对应。
    Mcsf::Vector3f vecTblXInSrs(1.f, 0.f, 0.f);
    Mcsf::Vector3f vecTblYInSrs(0.f, 1.f, 0.f); 
    Mcsf::Vector3f vecTblZInSrs(0.f, 0.f, 1.f);
    Mcsf::Point3f ptTbl(
        (tblBdry[0] + tblBdry[1]) * 0.5,
        (tblBdry[2] + tblBdry[3]) * 0.5, tblBdry[4]);
    Mcsf::Point3f ptSrs;
    ptSrs.SetZ((srsBdry[4] + srsBdry[5]) * 0.5);
    switch(patientPosition)
    {
    case HFS:
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, 1.f);
        vecTblZInSrs = Mcsf::Vector3f(0.f, -1.f, 0.f);
        ptSrs.SetX((srsBdry[0] + srsBdry[1]) * 0.5);
        ptSrs.SetY(srsBdry[3]);
        break;
    case FFS:
        vecTblXInSrs *= -1;
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, -1.f);
        vecTblZInSrs = Mcsf::Vector3f(0.f, -1.f, 0.f);
        ptSrs.SetX((srsBdry[0] + srsBdry[1]) * 0.5);
        ptSrs.SetY(srsBdry[3]);
        break;
    case HFP:
        vecTblXInSrs *= -1;
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, 1.f);
        vecTblZInSrs = Mcsf::Vector3f(0.f, 1.f, 0.f);
        ptSrs.SetX((srsBdry[0] + srsBdry[1]) * 0.5);
        ptSrs.SetY(srsBdry[2]);
        break;
    case FFP:
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, -1.f);
        vecTblZInSrs = Mcsf::Vector3f(0.f, 1.f, 0.f);
        ptSrs.SetX((srsBdry[0] + srsBdry[1]) * 0.5);
        ptSrs.SetY(srsBdry[2]);
        break;
    case FFDL:
        vecTblXInSrs = Mcsf::Vector3f(0.f, 1.f, 0.f);
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, -1.f);
        vecTblZInSrs = Mcsf::Vector3f(-1.f, 0.f, 0.f);
        ptSrs.SetX(srsBdry[1]);
        ptSrs.SetY((srsBdry[2] + srsBdry[3]) * 0.5);
        break;
    case HFDL:
        vecTblXInSrs = Mcsf::Vector3f(0.f, -1.f, 0.f);
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, 1.f);
        vecTblZInSrs = Mcsf::Vector3f(-1.f, 0.f, 0.f);
        ptSrs.SetX(srsBdry[1]);
        ptSrs.SetY((srsBdry[2] + srsBdry[3]) * 0.5);
        break;
    case FFDR:
        vecTblXInSrs = Mcsf::Vector3f(0.f, -1.f, 0.f);
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, -1.f);
        vecTblZInSrs = Mcsf::Vector3f(1.f, 0.f, 0.f);
        ptSrs.SetX(srsBdry[0]);
        ptSrs.SetY((srsBdry[2] + srsBdry[3]) * 0.5);
        break;
    case HFDR:
        vecTblXInSrs = Mcsf::Vector3f(0.f, 1.f, 0.f);
        vecTblYInSrs = Mcsf::Vector3f(0.f, 0.f, 1.f);
        vecTblZInSrs = Mcsf::Vector3f(1.f, 0.f, 0.f);
        ptSrs.SetX(srsBdry[0]);
        ptSrs.SetY((srsBdry[2] + srsBdry[3]) * 0.5);
        break;
    }
    Mcsf::Matrix4f matTbl(
        Mcsf::Vector4f(1.f, 0.f, 0.f, 0.f), 
        Mcsf::Vector4f(0.f, 1.f, 0.f, 0.f), 
        Mcsf::Vector4f(0.f, 0.f, 1.f, 0.f), 
        Mcsf::Vector4f(ptTbl));
    Mcsf::Vector4f col0(vecTblXInSrs);
    Mcsf::Vector4f col1(vecTblYInSrs);
    Mcsf::Vector4f col2(vecTblZInSrs);
    Mcsf::Vector4f col3(ptSrs);
    Mcsf::Matrix4f matTbl2Pat(col0, col1, col2, col3);
    matTbl2Pat.Append(matTbl.Inverse());
    return matTbl2Pat;
}
TPS_END_NAMESPACE

