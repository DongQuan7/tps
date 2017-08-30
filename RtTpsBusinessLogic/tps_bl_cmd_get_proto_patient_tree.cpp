//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  
//
/// \file    
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/08/12
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_get_proto_patient_tree.h"

#include "boost/date_time/posix_time/posix_time.hpp"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
//TPS
#include "RtTpsFramework/tps_fw_commandids.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_constraint.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_drrsetting.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_optgoalmanager.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"

#include "RtTpsProtoLib/rt_tps_proto_patient_tree_data.pb.h"
#include "RtTpsProtoLib/rt_tps_dvhdatalist.pb.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsProtoLib/rt_tps_poigroup.pb.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedunit.h"
#include "RtTpsProtoLib/rt_tps_unitlist.pb.h"


#include "RtTpsProtoLib/rt_tps_optgoals.pb.h"

#include "RtTpsProtoLib/rt_tps_proto_patient.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_course.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_site.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_prescription.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_constraint.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_series.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_series_group.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_plan.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_voi.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_poi.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_normalizationgroup.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_segment.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_block.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_series_table.pb.h"
#include "RtTpsProtoLib/rt_tps_dvhdata.pb.h"

#include "tps_logger.h"

#pragma warning(disable:4702)

TPS_BEGIN_NAMESPACE // begin namespace tps

GetProtoPatientTreeCmd::GetProtoPatientTreeCmd(std::string patientUid) 
: mPatientUid(patientUid)
{
    m_patientDataProto = new proto::RtTpsProtoPatientTreeData();
}

GetProtoPatientTreeCmd::~GetProtoPatientTreeCmd()
{
    if (m_patientDataProto!=nullptr) {
        delete m_patientDataProto;
    }
}

int GetProtoPatientTreeCmd::Execute()
{
    proto::RtTpsProtoPatient* protoPatient = nullptr;
    proto::RtTpsProtoCourse* protoCourse = nullptr;
    proto::RtTpsProtoSite* protoSite = nullptr;
    proto::RtTpsProtoPrescription* protoPrescription = nullptr; protoPrescription;
    proto::RtTpsProtoSeries* protoSeries = nullptr;
    proto::RtTpsProtoSeriesGroup* protoSeriesGroup = nullptr;
    proto::RtTpsProtoPlan* protoPlan = nullptr;
    proto::RtTpsProtoVoi* protoVoi = nullptr;
    proto::RtTpsProtoPoi* protoPoi = nullptr;
    proto::RtTpsProtoNormalizationGroup* protoNormalizationGroup = nullptr;
    proto::RtTpsProtoBeam* protoBeam = nullptr;
    proto::ReferenceImage* protoRtImage = nullptr;
    proto::RtTpsProtoDoseLine* protoDoseLine = nullptr;
    proto::RT_TPS_Optgoal* protoOptGoal = nullptr; protoOptGoal;
    proto::RtTpsProtoSeriesTable* protoSeriesTable = nullptr;
    proto::RT_TPS_DVHData* protoDvh = nullptr;

    std::vector<RtCourse*> courseList;
    std::vector<TpsImage3DEntity*> image3DEntityList;
    std::vector<RtPlan*> planList;
    std::vector<ROIEntity*> voiEntityList;
    std::vector<RtPoi*> poiList;
    std::vector<RtDoseline*> doseLineList;

    protoPatient = m_patientDataProto->mutable_patient();
    protoPatient->set_patientuid(mPatientUid);
    RtPatient* currentPatient = nullptr;
    currentPatient = mDataRepository->GetPatientManager()->GetPatient();
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
    std::vector<std::string> seriesUIDList = std::move(mDataRepository->GetImageDataManager()->GetSeriesUidListByPatientUid(mPatientUid));
    TpsImage3DEntity* image3DEntity = nullptr;
    for (auto itSeriesUID = seriesUIDList.cbegin(); itSeriesUID != seriesUIDList.cend(); ++itSeriesUID) {
        if (!mDataRepository->GetImageDataManager()->GetImage3DData(*itSeriesUID, &image3DEntity) || image3DEntity == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get image3DData["<<*itSeriesUID<<"] from imageDataManager";
            return false;
        }

        image3DEntityList.push_back(image3DEntity);
    }

    for (auto itImage3DEntity = image3DEntityList.cbegin(); itImage3DEntity != image3DEntityList.cend(); ++itImage3DEntity) {
        protoSeries = m_patientDataProto->add_serieslist();
        ConvertImage3DEntityToProto(*itImage3DEntity, protoSeries);

        protoSeriesTable = protoSeries->mutable_seriestable();
        auto seriesTable = mDataRepository->GetTableManager()->GetSeriesTable((*itImage3DEntity)->GetUID());
        if (seriesTable != nullptr)
        {
            protoSeriesTable->set_seriesuid(seriesTable->GetSeriesUid());
            protoSeriesTable->set_tableuid(seriesTable->GetTableUid());
            double angles[3], offset[3];
            seriesTable->GetRotationAngles(angles);
            seriesTable->GetOffset(offset);
            protoSeriesTable->set_anglex(angles[0]);
            protoSeriesTable->set_angley(angles[1]);
            protoSeriesTable->set_anglez(angles[2]);
            protoSeriesTable->set_offsetx(offset[0]);
            protoSeriesTable->set_offsety(offset[1]);
            protoSeriesTable->set_offsetz(offset[2]);
            protoSeriesTable->set_averagemassdensity(seriesTable->GetDensity());
            protoSeriesTable->set_referenceelectrondensity(seriesTable->GetReferenceEDensity());
        }

        // GetVoi
        if (nullptr != *itImage3DEntity){
            voiEntityList = mDataRepository->GetVoiManager()->GetROIBySeries((*itImage3DEntity)->GetUID());
        }
        for (auto itVoiEntity = voiEntityList.cbegin(); itVoiEntity != voiEntityList.cend(); ++itVoiEntity) {
            protoVoi = m_patientDataProto->add_voilist();
            ConvertVoiEntityToProto(*itVoiEntity, protoVoi);
        }
        voiEntityList.clear();

        // GetPoi
        std::vector<std::string> poiUIDList = mDataRepository->GetPoiManager()->GetPoiUidListBySeriesUid((*itImage3DEntity)->GetRtSeries()->get_uid());

        RtPoi* poi = nullptr;
        for (auto itPoiUID = poiUIDList.cbegin(); itPoiUID != poiUIDList.cend(); ++itPoiUID){
            poi = mDataRepository->GetPoiManager()->GetPOI(*itPoiUID);
            if (poi == nullptr) {
                TPS_LOG_DEV_ERROR<<"Failed to get poi["<<*itPoiUID<<"] from TpsPOIManager";
                return TPS_ER_FAILURE;
            }
            poiList.push_back(poi);
        }

        for (auto itPoi = poiList.cbegin(); itPoi != poiList.cend(); ++itPoi) {
            protoPoi = m_patientDataProto->add_poilist();
            ConvertPoiToProto(*itPoi, protoPoi, (*itImage3DEntity)->GetRtSeries()->get_isphantom());
        }
        poiList.clear();
    }

    // GetCourse
    mDataRepository->GetPatientManager()->GetCourseListByPatientUID(mPatientUid, courseList); 
    for (auto itCourse = courseList.cbegin(); itCourse != courseList.cend(); ++itCourse) {
        protoCourse = m_patientDataProto->add_courselist();
        ConvertCourseToProto(*itCourse, protoCourse);
        //ConvertCourseToProto(*itCourse, protoCourse);

        //Get sites
        std::vector<RtSite*> siteList;
        const std::string courseUid = (*itCourse)->get_uid();
        if (mDataRepository->GetPatientManager()->GetSiteByCourseUid(courseUid, siteList)) 
        {
            for (auto itSite = siteList.cbegin(); itSite != siteList.cend(); ++itSite)
            {
                protoSite = m_patientDataProto->add_sitelist();
                ConvertSiteToProto((*itCourse)->get_uid(), *itSite, protoSite);
            }
        }

        // GetPrescription
        // rcc：这里要等qq增加DB数据类之后，再打开
        std::vector<RtPrescription*> prescriptionList;
        //const std::string courseUid = (*itCourse)->get_uid();
        if (mDataRepository->GetPatientManager()->GetPrescription(courseUid, prescriptionList)) {
            for (auto itPrescription = prescriptionList.cbegin(); itPrescription != prescriptionList.cend(); ++itPrescription) {
                protoPrescription = m_patientDataProto->add_prescriptionlist();
                ConvertPrescriptionToProto((*itCourse)->get_uid(), *itPrescription, protoPrescription);
            }
        }

        // GetPlan (Including easy plan)
        std::vector<std::string> planUIDList = std::move(
            mDataRepository->GetPatientManager()->GetPlanUIDListByCourseUID((*itCourse)->get_uid()));

        RtPlan* plan = nullptr;
        for (auto itPlanUID = planUIDList.cbegin(); itPlanUID != planUIDList.cend(); ++itPlanUID) {
            if (!mDataRepository->GetPatientManager()->GetPlan(*itPlanUID, &plan) || plan == nullptr) {
                TPS_LOG_DEV_ERROR<<"Failed to get plan["<<*itPlanUID<<"] from TpsPatientManager";
                return TPS_ER_FAILURE;
            }

            planList.push_back(plan);
        }

        for (auto itPlan = planList.cbegin(); itPlan != planList.cend(); ++itPlan) {
            // GetPlan
            protoPlan = m_patientDataProto->add_planlist();
            ConvertPlanToProto(*itPlan, protoPlan);

            // GetDoseLineParams
            if (mDataRepository->GetDoseManager()->GetDoseLineList((*itPlan)->get_uid(), doseLineList)) {
                for (auto itDoseLine = doseLineList.cbegin(); itDoseLine != doseLineList.cend(); ++itDoseLine) {
                    protoDoseLine = m_patientDataProto->add_doselinelist();
                    ConvertDoseLineToProto(*itDoseLine, protoDoseLine);
                }
            }

            // GetDVH
            std::vector<RtDvh*> dvhs = mDataRepository->GetDVHManager()->GetPlanDvhs((*itPlan)->get_uid());
            for (auto itDvh = dvhs.cbegin(); itDvh != dvhs.cend(); ++itDvh) {
                protoDvh = m_patientDataProto->add_dvhlist();
                ConvertDVHToProto((*itPlan)->get_uid(), *itDvh, protoDvh);
            }

            // GetNormalizationGroup
            RtNormgroup *normalGroup = nullptr;
            std::vector<std::string> normGroupUidList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan((*itPlan)->get_uid());
            for (auto itNormgroup = normGroupUidList.cbegin(); itNormgroup != normGroupUidList.cend(); ++itNormgroup) {
                mDataRepository->GetNormgroupManager()->GetNormGroup(*itNormgroup, &normalGroup);
                protoNormalizationGroup = m_patientDataProto->add_normalizationgrouplist();
                ConvertNormalizationGroupToProto(normalGroup, protoNormalizationGroup);

                // GetBeam
                RtBeam *beam = nullptr;
                std::vector<std::string> beamUidList = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(*itNormgroup);
                for (auto itBeam = beamUidList.cbegin(); itBeam != beamUidList.cend(); ++itBeam) {
                    beam = mDataRepository->GetBeamManager()->GetBeam(*itBeam);
                    protoBeam = m_patientDataProto->add_beamlist();
                    ConvertBeamToProto(beam, protoBeam);

                    // Get RtImage
                    tps::TpsImage2DEntity* image2DEntity = nullptr;
                    mDataRepository->GetImageDataManager()->GetImage2DData(beam->get_relatedrtimageuid(), &image2DEntity);
                    if(image2DEntity != nullptr){
                        RtRtimage* rtImage = image2DEntity->get();
                        protoRtImage = m_patientDataProto->add_rtimagelist();
                        ConvertRtImageToProto(rtImage, protoRtImage);
                    }
                }
                // Get OptGoals
                // rcc: 这个暂时注掉，等将来再打开
                std::vector<RtDoseoptgoalbase*> vDoseOptGoals;
                if (mDataRepository->GetOptGoalManager()->GetOptGoalListByNormalizationGroupUID(*itNormgroup, vDoseOptGoals)){
                    for(auto itOptGoal = vDoseOptGoals.begin(); itOptGoal != vDoseOptGoals.end(); ++itOptGoal){
                        protoOptGoal = m_patientDataProto->add_optgoallist();
                        ConvertOptGoalToProto(*itOptGoal, protoOptGoal);
                    }
                }
            }
        }
        image3DEntityList.clear();
        planList.clear();
    }

    // GetSeriesGroup
    TpsImageDataManager* pImageManager = mDataRepository->GetImageDataManager();
    if (pImageManager != nullptr)
    {
        std::vector<std::shared_ptr<TpsSeriesGroup4d>> vecSeriesGroup;
        pImageManager->GetAllSeriesGroup(vecSeriesGroup);
        for (int i = 0; i < vecSeriesGroup.size(); i++)
        {
            protoSeriesGroup = m_patientDataProto->add_seriesgroup();
            ConvertSereisGroupToProto(vecSeriesGroup[i].get(), protoSeriesGroup);
        }
    }
    return TPS_ER_SUCCESS;
}

bool GetProtoPatientTreeCmd::ConvertCourseToProto(const RtCourse* course, 
    proto::RtTpsProtoCourse* protoCourse) 
{
    if (course==nullptr || protoCourse==nullptr) {
        TPS_LOG_DEV_ERROR<<"course==nullptr || protoCourse==nullptr";
        return false;
    }
    protoCourse->set_courseuid(course->get_uid());
    protoCourse->set_coursename(course->get_coursename());
    protoCourse->set_patientuid(course->get_patientuid());
    return true;
}

bool GetProtoPatientTreeCmd::ConvertSereisGroupToProto(const TpsSeriesGroup4d* pSeriesGroup, 
    proto::RtTpsProtoSeriesGroup* protoSeriesGroup) 
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

bool GetProtoPatientTreeCmd::ConvertSiteToProto(std::string courseUID, const RtSite* site, 
    proto::RtTpsProtoSite* protoSite)
{
    protoSite->set_sitename(site->get_name());
    protoSite->set_siteuid(site->get_uid());
    protoSite->set_courseuid(courseUID);

    return true;
}

bool GetProtoPatientTreeCmd::ConvertPrescriptionToProto(std::string courseUID, const RtPrescription* prescription, 
    proto::RtTpsProtoPrescription* protoPrescription) 
{
    protoPrescription->set_prescriptionuid(prescription->get_uid());
    protoPrescription->set_prescriptionname(prescription->get_name());
    protoPrescription->set_doseperfraction(prescription->get_doseperfraction());
    protoPrescription->set_fractionsnumber(prescription->get_fractionsnumber());
    protoPrescription->set_courseuid(courseUID);
    protoPrescription->set_isgating(prescription->get_isgating());
    protoPrescription->set_siteuid(prescription->get_siteuid());
    protoPrescription->set_binmode(prescription->get_binmode());
    protoPrescription->set_beamonrangestart(prescription->get_beamonrangestart());
    protoPrescription->set_beamonrangeend(prescription->get_beamonrangeend());

    std::vector<RtConstraint*> constraintList;
    mDataRepository->GetPatientManager()->GetConstraintByPrescriptionUid(prescription->get_uid(), constraintList);

    TpsVOIManager* voiMgr = mDataRepository->GetVoiManager();
    vector<RtVoisetting*> voiSetting = voiMgr->GetVoiSettings();

    proto::RtTpsProtoConstraint* protoConstraint = nullptr;
    for (auto itConstraint = constraintList.begin(); itConstraint != constraintList.end(); ++itConstraint) {
        protoConstraint = protoPrescription->add_constraintlist();
        protoConstraint->set_constraintuid((*itConstraint)->get_uid());
        protoConstraint->set_constraintname((*itConstraint)->get_name());
        protoConstraint->set_tendency((*itConstraint)->get_tendencytype());
        protoConstraint->set_dosetype((*itConstraint)->get_dosetype());
        protoConstraint->set_dose((*itConstraint)->get_dose());
        protoConstraint->set_dosevolume((*itConstraint)->get_dosevolume());
        protoConstraint->set_isachieved((*itConstraint)->get_isachieved());
        protoConstraint->set_comments((*itConstraint)->get_comments());

        std::vector<RtVoisetting*>::iterator iter;
        for (iter = voiSetting.begin();iter!=voiSetting.end();iter++)
        {
            RtVoisetting* rtVoiSetting = *iter;
            if (rtVoiSetting->get_voiname()==(*itConstraint)->get_name())
            {
                protoConstraint->set_constraintplanningrole(rtVoiSetting->get_planningrole());
                protoConstraint->set_constraintbiologicalrole(rtVoiSetting->get_biologyrole());
                protoConstraint->set_comments(rtVoiSetting->get_description());
            }
        }
    }

    return true;
}

bool GetProtoPatientTreeCmd::ConvertImage3DEntityToProto(TpsImage3DEntity* image3DEntity, 
    proto::RtTpsProtoSeries* protoSeries) 
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

    protoSeries->set_studyid(pSeries->get_studyid());
    protoSeries->set_studydescription(pSeries->get_studydescription());

    protoSeries->set_patientname(pSeries->get_patientname());
    protoSeries->set_patientid(pSeries->get_patientid());
    protoSeries->set_patientbirthdate(pSeries->get_patientbirthdate());
    protoSeries->set_patientage(pSeries->get_patientage());
    protoSeries->set_patientsex(pSeries->get_patientsex());
    protoSeries->set_patientuid(pSeries->get_patientuid());

    protoSeries->set_institutionname(pSeries->get_institutionname());
    protoSeries->set_manufacturer(pSeries->get_manufacturer());

    RtImage3DHeader *header = nullptr;
    image3DEntity->GetImage3DHeader(header);
    if(header != nullptr) {
        protoSeries->set_startx(header->m_vScanInfoList[0].m_ImagePosition.x);
        protoSeries->set_starty(header->m_vScanInfoList[0].m_ImagePosition.y);
        protoSeries->set_startz(header->m_vScanInfoList[0].m_ImagePosition.z);
        protoSeries->set_sizex(header->m_dXSize * header->m_iXDim);
        protoSeries->set_sizey(header->m_dYSize * header->m_iYDim);
        protoSeries->set_sizez(header->m_fSliceSpacing * header->m_iSliceCount);
        auto scanInfoList = header->m_vScanInfoList;
        Mcsf::Point3f org = Mcsf::Point3f(scanInfoList[0].m_ImagePosition.x, scanInfoList[0].m_ImagePosition.y, scanInfoList[0].m_ImagePosition.z);
        Mcsf::Point3f last = Mcsf::Point3f(scanInfoList[header->m_iSliceCount - 1].m_ImagePosition.x, 
            scanInfoList[header->m_iSliceCount - 1].m_ImagePosition.y, scanInfoList[header->m_iSliceCount - 1].m_ImagePosition.z);
        Mcsf::Vector3f vecZ = last - org;
        Mcsf::Vector3f axis[3];
        axis[0] = Mcsf::Vector3f(header->m_ImageXOrientation.x, header->m_ImageXOrientation.y, header->m_ImageXOrientation.z);
        axis[1] = Mcsf::Vector3f(header->m_ImageYOrientation.x, header->m_ImageYOrientation.y, header->m_ImageYOrientation.z);
        axis[2] = vecZ.GetNormalize();
        protoSeries->add_orientationx(axis[0].GetX());
        protoSeries->add_orientationx(axis[0].GetY());
        protoSeries->add_orientationx(axis[0].GetZ());
        protoSeries->add_orientationy(axis[1].GetX());
        protoSeries->add_orientationy(axis[1].GetY());
        protoSeries->add_orientationy(axis[1].GetZ());
        protoSeries->add_orientationz(axis[2].GetX());
        protoSeries->add_orientationz(axis[2].GetY());
        protoSeries->add_orientationz(axis[2].GetZ());
    }

    // SGXQ: bug fix for BIMT problem
    std::string c2dUid = pSeries->get_ct2densityuid();
    protoSeries->set_ct2densityuid(c2dUid);

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


    protoSeries->set_slicecount(header->m_iSliceCount);
    protoSeries->set_slicespacing(header->m_fSliceSpacing);
    protoSeries->set_windowingcenter(header->m_iWindowCenter);
    protoSeries->set_windowingwidth(header->m_iWindowWidth);
    protoSeries->set_xdimension(header->m_iXDim);
    protoSeries->set_ydimension(header->m_iYDim);
    protoSeries->set_xpixelspacing(header->m_dXSize);
    protoSeries->set_ypixelspacing(header->m_dYSize);
    protoSeries->set_imageslicebuffer(image3DEntity->GetSliceBuffer(), header->m_iXDim * header->m_iYDim);
    protoSeries->set_isqa(image3DEntity->GetIsQACopied());
    protoSeries->set_slicethickness(pSeries->get_slicethickness());
    protoSeries->set_isphantom(pSeries->get_isphantom());
   // protoSeries->set_acquisitiondatetime(db_img3d->m_Header.m_tAcquisitionDateTime);

    //protoSeries->set_acquisitiondatetime(streamDate.str()+streamTime.str());

    protoSeries->set_manufacturermodelname(pSeries->get_manufacturermodelname());
    protoSeries->set_frameofreference(pSeries->get_frameofreferenceuid());
    protoSeries->set_groupuid(image3DEntity->GetGroupUid());
    protoSeries->set_seriesname(pSeries->get_seriesname());

    std::map<std::string, RtImagetransformation*> tmList = image3DEntity->GetAllImagetransformation();
    for (auto it = tmList.begin(); it != tmList.end(); ++it){
        ::tps::proto::RtTpsProtoImagetransformation* imagetransformation = protoSeries->add_imagetransformationlist();

        imagetransformation->set_uid(it->second->get_uid());
        imagetransformation->set_registrationmethod(it->second->get_registrationmethod());
        imagetransformation->set_description(it->second->get_description());
        imagetransformation->set_originalseriesuid(it->second->get_originalseriesuid());
        imagetransformation->set_transfomationseriesuid(it->second->get_transfomationseriesuid());
    }

    return true;
}

// Convert plans including easy plan
bool GetProtoPatientTreeCmd::ConvertPlanToProto(RtPlan* plan, 
    proto::RtTpsProtoPlan* protoPlan) 
{
    if (plan==nullptr || protoPlan==nullptr) {
        TPS_LOG_DEV_ERROR<<"plan==nullptr || protoPlan==nullptr";
        return false;
    }

    protoPlan->set_planuid(plan->get_uid());
    protoPlan->set_courseuid(plan->get_courseuid());
    protoPlan->set_planname(plan->get_name());
    protoPlan->set_plansourcetype((int)plan->get_plansourcetype());
    protoPlan->set_bodythickness(plan->get_bodythickness());
    //protoPlan->set_isapproved(plan->get_()); //rcc: 这个字段数据库表里面没有
    protoPlan->set_planner(plan->get_planner());
    protoPlan->set_seriesuid(plan->get_imageseriesuid());
	protoPlan->set_patientposition(plan->get_patientposition());
    if (plan->get_dosegrid()!=nullptr) {
        float xinc = plan->get_dosegrid()->get_xcoordinate3dinc();
        float yinc = plan->get_dosegrid()->get_ycoordinate3dinc();
        float zinc = plan->get_dosegrid()->get_zcoordinate3dinc();
        protoPlan->set_dosegridspacingx(xinc);
        protoPlan->set_dosegridspacingy(yinc);
        protoPlan->set_dosegridspacingz(zinc);
        float xstart = plan->get_dosegrid()->get_xcoordinate3dstart();
        float ystart = plan->get_dosegrid()->get_ycoordinate3dstart();
        float zstart = plan->get_dosegrid()->get_zcoordinate3dstart();
        protoPlan->set_dosegridstartx(xstart);
        protoPlan->set_dosegridstarty(ystart);
        protoPlan->set_dosegridstartz(zstart);
        protoPlan->set_dosegridsizex(xinc* plan->get_dosegrid()->get_xcount());
        protoPlan->set_dosegridsizey(yinc* plan->get_dosegrid()->get_ycount());
        protoPlan->set_dosegridsizez(zinc* plan->get_dosegrid()->get_zcount());
        protoPlan->set_isdosevalid(plan->get_dosegrid()->get_isdosevalid());
    }

    protoPlan->set_displaymode(plan->get_displaymode());
    protoPlan->set_hundredpercentdefinition(plan->get_hundredpercentdefinition());
    protoPlan->set_specifieddose(plan->get_specifieddose());
    protoPlan->set_rendermode(plan->get_rendermode());
    protoPlan->set_transparency(plan->get_transparency());
    protoPlan->set_transparency3d(plan->get_transparency3d());
    protoPlan->set_enableskindoseview(plan->get_enableskindoseview());
    protoPlan->set_maxskindose(plan->get_maxskindose());
    protoPlan->set_plandescription(plan->get_description());

    return true;
}

bool GetProtoPatientTreeCmd::ConvertVoiEntityToProto(ROIEntity* voi, 
    proto::RtTpsProtoVoi* protoVoi) 
{
    if (nullptr == voi || nullptr == protoVoi) 
            return false;

    auto innerRoi = voi->GetInnerRoi();
    protoVoi->set_voiuid(innerRoi->get_uid());
    protoVoi->set_seriesuid(innerRoi->get_seriesuid());
    protoVoi->set_voiname(innerRoi->get_name());
    protoVoi->set_planningrole(innerRoi->get_planningrole());
    protoVoi->set_subrole(innerRoi->get_subrole());

    const float* color = innerRoi->get_color();
    protoVoi->clear_voicolor();
    protoVoi->add_voicolor(color[0]);
    protoVoi->add_voicolor(color[1]);
    protoVoi->add_voicolor(color[2]);
    protoVoi->add_voicolor(color[3]);
    protoVoi->add_voicolor(innerRoi->get_alpha3d());
    protoVoi->set_isvisible(innerRoi->get_isvisible());
    protoVoi->set_locktype(innerRoi->get_locktype());
    if (innerRoi->has_field(VOI_AVERAGEDENSITY))
    {
        protoVoi->set_averagedensity(innerRoi->get_averagedensity());
    }
    if (innerRoi->has_field(VOI_MINDENSITY))
    {
        protoVoi->set_mindensity(innerRoi->get_mindensity());
    }
    if (innerRoi->has_field(VOI_MAXDENSITY))
    {
        protoVoi->set_maxdensity(innerRoi->get_maxdensity());
    }
    if (innerRoi->has_field(VOI_CENTROIDX))
    {
        protoVoi->set_centroidx(innerRoi->get_centroidx());
    }
    if (innerRoi->has_field(VOI_CENTROIDY))
    {
        protoVoi->set_centroidy(innerRoi->get_centroidy());
    }
    if (innerRoi->has_field(VOI_CENTROIDZ))
    {
        protoVoi->set_centroidz(innerRoi->get_centroidz());
    }

    protoVoi->set_volume(innerRoi->get_volume());
    protoVoi->set_surfacearea(innerRoi->get_surfacearea());

    protoVoi->clear_slices();
    std::set<int> slices = voi->GetContourSlices();
    for (auto it = slices.begin(); it != slices.end(); ++it)
    {
        protoVoi->add_slices(*it);
    };

    protoVoi->set_mindose(voi->GetMinDose());
    protoVoi->set_maxdose(voi->GetMaxDose());
    protoVoi->set_meandose(voi->GetMeanDose());
    protoVoi->set_standard_deviation(innerRoi->get_standarddeviation());
    protoVoi->set_isshaded(innerRoi->get_isshaded());
    protoVoi->set_description(innerRoi->get_description());
    protoVoi->set_ingrid(voi->GetInGrid());
    protoVoi->set_tcpntcp(innerRoi->get_tcpntcp());
    protoVoi->set_isoverride(innerRoi->get_isoverride());
    if (innerRoi->has_field(VOI_OVERRIDEDENSTIY))
    {
        protoVoi->set_overridemassdensity(innerRoi->get_overridedenstiy());
    }
    if (innerRoi->has_field(VOI_DENSITY))
    {
        protoVoi->set_overridereferenceelectrondensity(innerRoi->get_density());
    }
    return true;
}

bool GetProtoPatientTreeCmd::ConvertPoiToProto(const RtPoi* poi, 
    proto::RtTpsProtoPoi* protoPoi, const bool isEasyPlanPoi) 
{
    if (poi==nullptr || protoPoi==nullptr) {
        TPS_LOG_DEV_ERROR<<"poi==nullptr || protoPoi==nullptr";
        return false;
    }

	protoPoi->set_poiuid(poi->get_uid());
    protoPoi->set_seriesuid(poi->get_seriesuid());
	protoPoi->set_poiname(poi->get_name());
	protoPoi->set_poitype(poi->get_type());
	protoPoi->set_isvisible(poi->get_isvisible());
	protoPoi->set_locktype(poi->get_locktype());

   // float color[4] = {0};
    //poi->GetColor(color); 
    protoPoi->clear_poicolor();
    protoPoi->add_poicolor(poi->get_red());
    protoPoi->add_poicolor(poi->get_green());
    protoPoi->add_poicolor(poi->get_blue());
    protoPoi->add_poicolor(poi->get_alpha());

    if(isEasyPlanPoi){ // only convert points that are not isocenter
        protoPoi->set_xcoordincol(poi->get_coordinate3dx());
        protoPoi->set_ycoordincol(poi->get_coordinate3dy());
        protoPoi->set_pointdepth(poi->get_coordinate3dz());
    }
    else
    {
        protoPoi->set_xcoordinpat(poi->get_coordinate3dx());
        protoPoi->set_ycoordinpat(poi->get_coordinate3dy());
        protoPoi->set_zcoordinpat(poi->get_coordinate3dz());
    }
    //protoPoi->set_dose(poi->get_dose());

    std::map<std::string, RtDbDef::PoiDoseInfo> plandoseMap = poi->get_plandosemap();
    for (auto itPoiDose = plandoseMap.begin(); itPoiDose != plandoseMap.end(); ++itPoiDose)
    {
        tps::proto::RtTpsProtoPoiDoseInPlan* doseInPlan = protoPoi->add_poidoselist();
        doseInPlan->set_planuid(itPoiDose->first);
        doseInPlan->set_poiuid(poi->get_uid());
        doseInPlan->set_dose(itPoiDose->second.dose);
        doseInPlan->set_dosecalcmethod(itPoiDose->second.doseCalcMethod);
    }
    return true;
}

bool GetProtoPatientTreeCmd::ConvertNormalizationGroupToProto(RtNormgroup* normgroup, 
    proto::RtTpsProtoNormalizationGroup* protoNormgroup) 
{
    if (normgroup==nullptr || protoNormgroup==nullptr) {
        TPS_LOG_DEV_ERROR<<"normgroup==nullptr || protoNormgroup==nullptr";
        return false;
    }

    protoNormgroup->set_normalizationgroupuid(normgroup->get_uid());
    protoNormgroup->set_planuid(normgroup->get_planuid());
    protoNormgroup->set_normalizationgroupname(normgroup->get_name());
    protoNormgroup->set_machineuid(normgroup->get_machineuid());
    protoNormgroup->set_techniquetype(normgroup->get_plantype());
    protoNormgroup->set_radiationtype((int)normgroup->get_radiationtype());
    //protoNormgroup->set_algorithmtype((int)normgroup->get_dosealgorithmtype());
    RtDosegrid *doseGrid = normgroup->get_dosegrid();
    if (doseGrid!=nullptr) {
        float xinc = doseGrid->get_xcoordinate3dinc();
        float yinc = doseGrid->get_ycoordinate3dinc();
        float zinc = doseGrid->get_zcoordinate3dinc();
        protoNormgroup->set_dosegridspacingx(xinc);
        protoNormgroup->set_dosegridspacingy(yinc);
        protoNormgroup->set_dosegridspacingz(zinc);
        float xstart = doseGrid->get_xcoordinate3dstart();
        float ystart = doseGrid->get_ycoordinate3dstart();
        float zstart = doseGrid->get_zcoordinate3dstart();
        protoNormgroup->set_dosegridstartx(xstart);
        protoNormgroup->set_dosegridstarty(ystart);
        protoNormgroup->set_dosegridstartz(zstart);
        protoNormgroup->set_dosegridsizex(xinc* doseGrid->get_xcount());
        protoNormgroup->set_dosegridsizey(yinc* doseGrid->get_ycount());
        protoNormgroup->set_dosegridsizez(zinc* doseGrid->get_zcount());
        protoNormgroup->set_isdosevalid(normgroup->get_dosegrid()->get_isdosevalid());
    }
    protoNormgroup->set_normalizationpoiuid(normgroup->get_normalpointpoiuid());
    protoNormgroup->set_targetispoi(!normgroup->get_isnorm2voi());
    protoNormgroup->set_poidosepercent(normgroup->get_percentage() * 100);

    protoNormgroup->set_normalizationvoiuid(normgroup->get_voiuid());
    protoNormgroup->set_voidosepercent(normgroup->get_normvoipercentagevolume() * 100);
    protoNormgroup->set_isaveragevoi(normgroup->get_isnorm2voimeandose());

    protoNormgroup->set_isgating(normgroup->get_isgating());
    protoNormgroup->set_binmode(normgroup->get_binmode());
    protoNormgroup->set_beamonrangestart(normgroup->get_beamonrangestart());
    protoNormgroup->set_beamonrangeend(normgroup->get_beamonrangeend());
    protoNormgroup->set_isjoined(normgroup->get_isjoined());
    //protoNormgroup->set_isapproved(normgroup->GetApprovalStatus()); //rcc: 这个字段数据库表里面没有
    protoNormgroup->set_doseperfraction(normgroup->get_doseperfraction());
    protoNormgroup->set_fractioncount(normgroup->get_fraction());
    protoNormgroup->set_prescriptionuid(normgroup->get_prescriptionuid());
    protoNormgroup->set_description(normgroup->get_description());

    auto normgroupManager = mDataRepository->GetNormgroupManager();
    RtDrrsetting *pDrrsetting = nullptr;
    normgroupManager->GetDrrSetting(normgroup->get_uid(), &pDrrsetting);
    if(pDrrsetting != nullptr)
    {
        protoNormgroup->set_ct2densityuidfordrr(pDrrsetting->get_ct2densityuid());
        protoNormgroup->set_ctthresholdmaxfordrr(pDrrsetting->get_ctthresholdmaxvalue());
        protoNormgroup->set_ctthresholdminfordrr(pDrrsetting->get_ctthresholdminvalue());
        protoNormgroup->set_electronenergyfordrr(pDrrsetting->get_electronenergy());
    }

    return true;
}

bool GetProtoPatientTreeCmd::ConvertOptGoalToProto(RtDoseoptgoalbase* optGoal, proto::RT_TPS_Optgoal* protoOptGoal)
{
    protoOptGoal->set_id(optGoal->get_id());
    protoOptGoal->set_normgroupuid(optGoal->get_normgroupuid());
    protoOptGoal->set_voiuid(optGoal->get_voiuid());
    protoOptGoal->set_fuctiontype(optGoal->get_doseoptgoaltype());
    protoOptGoal->set_weight(optGoal->get_voiweight());
    protoOptGoal->set_optgoaluid(optGoal->get_uid());
    protoOptGoal->set_isjoinopt(optGoal->get_isjoined());
    if (optGoal->has_field(DOSEOPTGOALBASE_EUDRESULT))
    {
        protoOptGoal->set_eudresult(optGoal->get_eudresult());
    }
    if (optGoal->get_ishardlimit() == 1){
        protoOptGoal->set_ishard(true);
    }
    else{
        protoOptGoal->set_ishard(false);
    }
    switch(optGoal->get_doseoptgoaltype()){
    case RtDbDef::EUD: 
        protoOptGoal->set_dose(optGoal->get_eud());
        protoOptGoal->set_eud(optGoal->get_a());
        break;
    case RtDbDef::USER_DVH:
        protoOptGoal->set_dose(optGoal->get_dvhdose());
        protoOptGoal->set_volume(optGoal->get_volume());
        if (optGoal->get_islowerbound()){
            protoOptGoal->set_minmaxflag(1);
        }
        else{
            protoOptGoal->set_minmaxflag(0);
        }
        break;
    case RtDbDef::MIN_DOSE:
    case RtDbDef::MAX_DOSE:
    case RtDbDef::SINGLE_VALUE:
        protoOptGoal->set_dose(optGoal->get_dose());
        break;
    }

    return true;
}

bool GetProtoPatientTreeCmd::ConvertBeamToProto(RtBeam* beam,
    proto::RtTpsProtoBeam* protoBeam) 
{
    if (beam==nullptr || protoBeam==nullptr) {
        TPS_LOG_DEV_ERROR<<"beam==nullptr || protoBeam==nullptr";
        return false;
    }

    protoBeam->set_beam_uid(beam->get_uid());
    protoBeam->set_beam_number(beam->get_beamnumber());
    protoBeam->set_normal_group_uid(beam->get_normgroupuid());
    protoBeam->set_beam_name(beam->get_name());
    protoBeam->set_commissioned_unit_uid(beam->get_commissionedunituid());
    protoBeam->set_isocenter_uid(beam->get_isocenterpoiuid());
    protoBeam->set_is_visible(beam->get_isvisible());
    protoBeam->set_is_joined(beam->get_isjoincalc());//TODO
    protoBeam->set_beam_mu(beam->get_beammu());
    if (beam->has_field(BEAM_BEAMWEIGHT))
    {
        protoBeam->set_beam_weight(beam->get_beamweight());
    }
    protoBeam->set_beam_description(beam->get_description());
    protoBeam->set_is_auto_created(beam->get_isautocreated());
    protoBeam->set_dose_rate(beam->get_doserate());
    protoBeam->set_ssd(beam->get_ssd());
    protoBeam->set_tray_uid(beam->get_trayuid());
    protoBeam->set_is_dose_valid(beam->get_dosegrid()->get_isdosevalid());
    protoBeam->set_mlcedittype(beam->get_mlcedittype());
    protoBeam->set_algorithmtype((int)beam->get_dosealgorithmtype());

    // Geometry
    const std::vector<RtBeamsegment*>& segments = beam->get_beamsegments();
    if (!segments.empty()) {
        RtBeamsegment* segment0 = segments.front();
        protoBeam->set_gantry_start_angle(segment0->get_startgantryangle());
        protoBeam->set_couch_angle(segment0->get_starttablerotationangle());
        protoBeam->set_collimator_angle(segment0->get_startcollimatorangle());

        double arcLength = 0.0, start, end;
        bool isCW = segment0->get_gantryrotationdirection()==RtDbDef::CW;
        int sign = isCW ? 1 : -1;
        for (int i = 0; i < segments.size(); ++i)
        {
            arcLength += segments[i]->get_arclength();
            if (i == segments.size() - 1) break;
            start = segments[i]->get_startgantryangle() + sign * segments[i]->get_arclength();
            end = segments[i+1]->get_startgantryangle();
            arcLength += TpsUtility::ToClockPosition(sign * (end - start));
        }
        protoBeam->set_gantry_arc_length(arcLength);
        protoBeam->set_is_clockwise(segment0->get_gantryrotationdirection()==RtDbDef::CW);
    }

    //Aperture Type
    //0:MLC 1:JawOnly 2:Applicator 3:Aperture Block
    auto apertureType = beam->get_aperturetype();
    protoBeam->set_aperture_type(apertureType);

    //////////////////////////////////////////////////////////////////////////
    //Accessory
    //physical wedge and virtual wedge
    const std::string sPhyUid = beam->get_physicalwedgeuid();
    if (sPhyUid.empty()){
        protoBeam->set_use_physical_wedge(false);
    } 
    else{
        protoBeam->set_physical_wedge_uid(sPhyUid);
        protoBeam->set_use_physical_wedge(true);
    }
    const std::string sVirUid = beam->get_virtualwedgeuid();
    if (sVirUid.empty()){
        protoBeam->set_use_virtual_wedge(false);
    } 
    else{
        protoBeam->set_virtual_wedge_uid(sVirUid);
        protoBeam->set_use_virtual_wedge(true);
    }

    //bolus
    const std::string sBolusUid = beam->get_bolusvoiuid();
    protoBeam->set_bolus_voi_uid(sBolusUid);
    /* protoBeam->set_use_bolus(false);
    if (!sBolusUid.empty()){
    VOIEntity* pBolus = nullptr;
    mDataRepository->GetVoiManager()->GetVOI(sBolusUid, &pBolus);
    if (nullptr != pBolus){
    protoBeam->set_use_bolus(true);
    protoBeam->set_bolus_thickness(beam->get_bolusthickness());
    protoBeam->set_bolus_density(pBolus->GetAverageDensity());
    }
    }*/

    //applicator
    protoBeam->set_applicator_uid(beam->get_applicatoruid());

    std::vector<RtBeamsegment*> beamsegmentList = beam->get_beamsegments();
    proto::RtTpsProtoSegment* protoSegment = nullptr;
    RtBeamsegment* segment;
    RtMlcshape *pStartMlcShape, *pEndMlcShape;
    proto::RtTpsProtoJaw *startJaw, *endJaw;
    RtMlcshape* oneValidMlcShape = nullptr;
    for (auto iter = beamsegmentList.begin(); iter != beamsegmentList.end(); ++iter) {
        segment = *iter;
        protoSegment = protoBeam->add_segment_list();
        protoSegment->set_segmentuid(segment->get_uid());
        protoSegment->set_beamuid(segment->get_beamuid());
        protoSegment->set_segmentmu(segment->get_mu());
        protoSegment->set_segmentweight(segment->get_weight());
        protoSegment->set_sequencenumber(segment->get_sequencenumber());
        protoSegment->set_startgantryangle(segment->get_startgantryangle());
        protoSegment->set_starttablerotationangle(segment->get_starttablerotationangle());
        protoSegment->set_arclength(segment->get_arclength());
        protoSegment->set_endtablerotationangle(segment->get_endtablerotationangle());

        // Init Jaw and Mlc Leaf Position
        pStartMlcShape = segment->get_startmlcshape();
        if (pStartMlcShape != nullptr) {
            startJaw = protoSegment->mutable_startjaw();
            if(RtDbDef::JAWTYPE_Follow == beam->get_jawtype()){
                protoBeam->set_is_jaw_follow(true);
            }
            else{
                protoBeam->set_is_jaw_follow(false);
            }
            startJaw->set_x1(pStartMlcShape->get_xlowerjawpos());
            startJaw->set_x2(pStartMlcShape->get_xupperjawpos());
            startJaw->set_y1(pStartMlcShape->get_ylowerjawpos());
            startJaw->set_y2(pStartMlcShape->get_yupperjawpos());

            std::vector<tps::db_Point2d> leafPosition = pStartMlcShape->get_leafpositions();
            for (auto itLeafPos = leafPosition.begin(); itLeafPos != leafPosition.end(); ++itLeafPos) {
                proto::RtTpsProtoMlcLeaf* mlcLeaf = protoSegment->add_startmlcleaves();
                mlcLeaf->set_x1(itLeafPos->x);
                mlcLeaf->set_x2(itLeafPos->y);
            }
            if (oneValidMlcShape == nullptr) oneValidMlcShape = pStartMlcShape;
        }
        pEndMlcShape = segment->get_endmlcshape();
        if (pEndMlcShape != nullptr) {
            endJaw = protoSegment->mutable_endjaw();
            endJaw->set_x1(pEndMlcShape->get_xlowerjawpos());
            endJaw->set_x2(pEndMlcShape->get_xupperjawpos());
            endJaw->set_y1(pEndMlcShape->get_ylowerjawpos());
            endJaw->set_y2(pEndMlcShape->get_yupperjawpos());

            std::vector<tps::db_Point2d> leafPosition = pEndMlcShape->get_leafpositions();
            for (auto itLeafPos = leafPosition.begin(); itLeafPos != leafPosition.end(); ++itLeafPos) {
                proto::RtTpsProtoMlcLeaf* mlcLeaf = protoSegment->add_endmlcleaves();
                mlcLeaf->set_x1(itLeafPos->x);
                mlcLeaf->set_x2(itLeafPos->y);
            }
        }
    }

    //Aperture block
    RtBlock* pApertureBlock = beam->get_aperture_block();
    if (nullptr != pApertureBlock){
        auto apertureBlock = protoBeam->mutable_aperture_block();
        apertureBlock->set_block_uid(pApertureBlock->get_uid());
        apertureBlock->set_block_type(pApertureBlock->get_blocktype());
        apertureBlock->set_block_name(pApertureBlock->get_blockname());
        apertureBlock->set_thickness(pApertureBlock->get_blockthickness());
        apertureBlock->clear_block_data();
        //add by wzd ---get aperture block Point
        std::vector<db_Point2f> apertureBlockPtList = pApertureBlock->get_points();
        proto::BlockPoint* blockPoint;
        for (auto it = apertureBlockPtList.begin(); it != apertureBlockPtList.end(); ++it) {
            blockPoint = apertureBlock->add_block_data();
            blockPoint->set_x(it->x);
            blockPoint->set_y(it->y);
        }
    }

    //set aperture edit parameters
    if ((apertureType == RtDbDef::Mlc || apertureType == RtDbDef::JawOnly) && oneValidMlcShape != nullptr)
    {
        protoBeam->set_is_auto_tracking_shaped(beam->get_isautoshape());
        protoBeam->set_target_voi_uid(beam->get_targetvoiuid());
        protoBeam->set_target_margin_x(beam->get_targetmarginx());
        protoBeam->set_target_margin_y(beam->get_targetmarginy());
        protoBeam->set_avoid_voi_uid(beam->get_avoidvoiuid());
        protoBeam->set_avoid_margin_x(beam->get_avoidmarginx());
        protoBeam->set_avoid_margin_y(beam->get_avoidmarginy());
        protoBeam->set_is_jaw_follow(beam->get_jawtype() == RtDbDef::JAWTYPE_Follow);
        protoBeam->set_jaw_follow_margin_x(beam->get_jawmarginx());
        protoBeam->set_jaw_follow_margin_y(beam->get_jawmarginy());
        protoBeam->set_is_x_symmetric(oneValidMlcShape->get_jawxissymmetry());
        protoBeam->set_is_y_symmetric(oneValidMlcShape->get_jawyissymmetry());
    }
    else if (apertureType == RtDbDef::Aperture_Block && pApertureBlock != nullptr)
    {
        protoBeam->set_is_auto_tracking_shaped(pApertureBlock->get_isautoshape());
        protoBeam->set_target_voi_uid(pApertureBlock->get_targetvoiuid());
        protoBeam->set_target_margin_x(pApertureBlock->get_targetmarginx());
        protoBeam->set_target_margin_y(pApertureBlock->get_targetmarginy());
        protoBeam->set_avoid_voi_uid(pApertureBlock->get_avoidvoiuid());
        protoBeam->set_avoid_margin_x(pApertureBlock->get_avoidmarginx());
        protoBeam->set_avoid_margin_y(pApertureBlock->get_avoidmarginy());
        if (oneValidMlcShape)
        {
            protoBeam->set_is_jaw_follow(beam->get_jawtype() == RtDbDef::JAWTYPE_Follow);
            protoBeam->set_jaw_follow_margin_x(beam->get_jawmarginx());
            protoBeam->set_jaw_follow_margin_y(beam->get_jawmarginy());
            protoBeam->set_is_x_symmetric(oneValidMlcShape->get_jawxissymmetry());
            protoBeam->set_is_y_symmetric(oneValidMlcShape->get_jawyissymmetry());
        }
    }

    protoBeam->set_tray_uid(beam->get_trayuid());

    //For shielding block and E beam with shielding or aperture block
    //RtBlock* pBlock = beam->get_aperture_block();
    RtBlock* pBlock = beam->get_shield_block();//modify by wzd
    protoBeam->set_use_shielding_block(nullptr != pBlock);
    if (nullptr != pBlock){
        //protoBeam->set_use_block_for_e_beam();TODO
        auto shieldingBlock = protoBeam->mutable_shielding_block();
        shieldingBlock->set_block_type(pBlock->get_blocktype());
        shieldingBlock->set_block_uid(pBlock->get_uid());
        shieldingBlock->set_block_name(pBlock->get_blockname());
        shieldingBlock->set_thickness(pBlock->get_blockthickness());
        protoBeam->set_is_shielding_block_auto_tracking_shape(pBlock->get_isautoshape());
        //protoBeam->set_block_material(pBlock->GetMaterialID());
        protoBeam->set_shielding_block_target_voi_uid(pBlock->get_targetvoiuid());
        protoBeam->set_shielding_block_target_margin_x(pBlock->get_targetmarginx());
        protoBeam->set_shielding_block_target_margin_y(pBlock->get_targetmarginy());
        protoBeam->set_shielding_block_avoid_voi_uid(pBlock->get_avoidvoiuid());
        protoBeam->set_shielding_block_avoid_margin_x(pBlock->get_avoidmarginx());
        protoBeam->set_shielding_block_avoid_margin_y(pBlock->get_avoidmarginy());

        shieldingBlock->clear_block_data();
        //add by wzd ---get shield block Point
        std::vector<db_Point2f> shieldBlockPtList = pBlock->get_points();
        proto::BlockPoint* blockPoint;
        for (auto it = shieldBlockPtList.begin(); it != shieldBlockPtList.end(); ++it) {
            blockPoint = shieldingBlock->add_block_data();
            blockPoint->set_x(it->x);
            blockPoint->set_y(it->y);
        }
    }

    if (nullptr != beam->get_dosegrid()) {
        protoBeam->set_dosegrid_uid(beam->get_dosegrid()->get_uid());
    }

    // Beam Dose Calc
    //float beamDose = 0.0f;
    //RTFWK::RTFWKDLDONormgroup* normGroup = nullptr;
    //if (TpsNormGroupManager::GetInstance()->GetNormGroup(beam->GetNormGroupUID(), &normGroup)
    //    && normGroup != nullptr) {

    //        if (normGroup->GetIsNorm2VOI()) {

    //            VOIEntity* voiEntity = nullptr;
    //            if (TpsVOIManager::GetInstance()->GetVOI(normGroup->GetVOIUID(), &voiEntity)
    //                && voiEntity != nullptr) {

    //                    RTFWK::VoiDose voiDose;
    //                    if (voiEntity->GetTMSVoiObject()->GetBeamDoseContributionPerMU(beam->GetUID(), voiDose)) {
    //                        beamDose = voiDose.meanDose * beam->GetBeamMU();
    //                    }
    //            }
    //        }
    //        else {

    //            RTFWK::RTFWKDLDOPOI* poi = TpsPOIManager::GetInstance()->GetPOI(normGroup->GetNormalPointPoiUID());
    //            if (poi != nullptr) {

    //                float poiDose = 0.0f;
    //                if (poi->GetBeamDoseContributionPerMU(beam->GetUID(), poiDose)) {
    //                    beamDose = poiDose * beam->GetBeamMU();
    //                }
    //            }
    //        }
    //}
    //protoBeam->set_beam_dose(beamDose);

    return true;
}

bool GetProtoPatientTreeCmd::ConvertRtImageToProto(const RtRtimage* rtImage, proto::ReferenceImage* protoReferenceImage)
{
    if (rtImage == nullptr || protoReferenceImage == nullptr) {
        TPS_LOG_DEV_ERROR<<"rtImage == nullptr || protoReferenceImage == nullptr";
        return false;
    }
    protoReferenceImage->set_patientname(rtImage->get_patientname());
    protoReferenceImage->set_patientid(rtImage->get_patientid());
    if(!rtImage->get_patientbirthdate().is_not_a_date_time()) {
        int year = rtImage->get_patientbirthdate().date().year();
        int month = rtImage->get_patientbirthdate().date().month();
        int day = rtImage->get_patientbirthdate().date().day();
        stringstream ss;
        ss<<year<<"-"<<month<<"-"<<day;
        std::string date = ss.str();
        protoReferenceImage->set_patientbirthdate(date);
    }
    protoReferenceImage->set_patientsex(rtImage->get_patientsex());
    protoReferenceImage->set_ssd(rtImage->get_ssd());
    protoReferenceImage->set_patientposition(rtImage->get_patientposition());
    if(!rtImage->get_acquisitiondatetime().is_not_a_date_time()) {
        int year = rtImage->get_acquisitiondatetime().date().year();
        int month = rtImage->get_acquisitiondatetime().date().month();
        int day = rtImage->get_acquisitiondatetime().date().day();
        stringstream ss;
        ss<<year<<"-"<<month<<"-"<<day;
        std::string date = ss.str();
        protoReferenceImage->set_imageacquisitiondate(date);
    }
    /*if(!rtImage->get_acquisitiondatetime().is_not_a_date_time()) {
        int hour = rtImage->get_acquisitiondatetime().time_of_day().hours();
        int minute = rtImage->get_acquisitiondatetime().time_of_day().minutes();
        int second = rtImage->get_acquisitiondatetime().time_of_day().seconds();
        stringstream ss;
        ss<<hour<<":"<<minute<<":"<<second;
        string time = ss.str();
        protoReferenceImage->set_imageacquisitiontime(time);
    }*/

    protoReferenceImage->set_beamuid(rtImage->get_beamuid());
    protoReferenceImage->set_bitsallocated(rtImage->get_bitsallocated());
    protoReferenceImage->set_bitsstored(rtImage->get_bitsstored());
    protoReferenceImage->set_hightbit(rtImage->get_highbit());
    protoReferenceImage->set_imagegantryangle(rtImage->get_angle());
    protoReferenceImage->set_imageheight(rtImage->get_rows());
    protoReferenceImage->set_imagename(rtImage->get_name());
    //protoReferenceImage->set_imagetype((proto::ReferenceImage_IMAGE_TYPE)(rtImage->get_imagetype()));
    protoReferenceImage->set_dicomimagetype(rtImage->get_dicomimagetype());
    protoReferenceImage->set_imageuid(rtImage->get_uid());
    protoReferenceImage->set_imagewidth(rtImage->get_columns());
    protoReferenceImage->set_pixelrepresentation(rtImage->get_pixelrepresentation());
    protoReferenceImage->set_pixelspacingx(rtImage->get_firstimageplanepixelspacing());
    protoReferenceImage->set_pixelspacingy(rtImage->get_secondimageplanepixelspacing());
    protoReferenceImage->set_samplesperpixel(rtImage->get_samplesperpixel());
    protoReferenceImage->set_windowcenter(rtImage->get_windowcenter());
    protoReferenceImage->set_windowwidth(rtImage->get_windowwidth());
    return true;
}

bool GetProtoPatientTreeCmd::ConvertDoseLineToProto(const RtDoseline* doseLine,
    proto::RtTpsProtoDoseLine* protoDoseLine) 
{
    if (doseLine==nullptr || protoDoseLine==nullptr) {
        TPS_LOG_DEV_ERROR<<"doseLine==nullptr || protoDoseLine==nullptr";
        return false;
    }

    protoDoseLine->set_doselineuid(doseLine->get_uid());
    protoDoseLine->set_doseratio(doseLine->get_doseratio());
    protoDoseLine->add_doselinecolor(doseLine->get_red());
    protoDoseLine->add_doselinecolor(doseLine->get_green());
    protoDoseLine->add_doselinecolor(doseLine->get_blue());
    protoDoseLine->add_doselinecolor(doseLine->get_alpha());
    protoDoseLine->set_isvisible(doseLine->get_isvisible());
    protoDoseLine->set_isvisible3d(doseLine->get_isvisible3d());
    protoDoseLine->set_planuid(doseLine->get_planuid());

    return true;
}

bool GetProtoPatientTreeCmd::ConvertDVHToProto(const std::string parentUID, const RtDvh* dvh, proto::RT_TPS_DVHData* protoDvh)
{
	protoDvh->set_parentuid(parentUID);
	protoDvh->set_voiuid(dvh->get_uid());
	protoDvh->set_mindose(dvh->get_mindose());
	protoDvh->set_meandose(dvh->get_meandose());
	protoDvh->set_maxdose(dvh->get_maxdose());
	protoDvh->set_standarddeviation(dvh->get_standarddeviation());
	protoDvh->set_ingrid(dvh->get_ingrid());

	float binWidth = dvh->get_binwidth();
	std::map<int, float> dData = std::move(dvh->get_differential_dvhdata());
	for (std::map<int, float>::iterator cIt = dData.begin(); cIt != dData.end(); ++cIt) {
		auto point = protoDvh->add_point();
		point->set_x(cIt->first * binWidth);
		point->set_y(cIt->second);
	}
	std::map<int, float> cData = std::move(dvh->get_cumulative_dvhdata());
	for (std::map<int, float>::iterator cIt = cData.begin(); cIt != cData.end(); ++cIt) {
		auto point = protoDvh->add_cumulativepoint();
		point->set_x(cIt->first * binWidth);
		point->set_y(cIt->second);
	}

	return true;
}

TPS_END_NAMESPACE
