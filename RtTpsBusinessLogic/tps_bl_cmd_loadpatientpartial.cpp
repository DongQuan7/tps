//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_bl_cmd_loadpatientpartial.cpp
///  \brief   load patient partial
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_loadpatientpartial.h"

#include <algorithm>

#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_course.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_site.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "tps_logger.h"
#include "tps_performance.h"

TPS_BEGIN_NAMESPACE

LoadPatientPartialCommand::LoadPatientPartialCommand(std::string patientUID)
: msPatientUID(patientUID), mPatient(nullptr)
{

}

LoadPatientPartialCommand::~LoadPatientPartialCommand()
{

}

void LoadPatientPartialCommand::GetResult(RtPatient*& patient,
    std::vector<RtCourse*>& vCourseList, std::vector<RtSite*>& vSiteList, std::vector<RtPrescription*>& vPrescriptionList,
    std::vector<RtSeries*>& vSeriesList, std::vector<RtPlan*>& vPlanList)
{
    patient = mPatient;
    vCourseList = mvCourseList;
    vSiteList = mvSiteList;
    vPrescriptionList = mvPrescriptionList;

    vSeriesList = mvSeriesList;
    vPlanList = mvPlanList;
}

int LoadPatientPartialCommand::Execute()
{
    TPS_LOG_DEV_INFO<<"LoadPatientPartialCommand::Execute";

    TEST_PERFORMANCE_INIT
    TEST_PERFORMANCE_BEGIN


    if (msPatientUID.empty() || mDatabaseWrapper == nullptr || mDataRepository == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Invalid Input.";
        return TPS_ER_INPUT_INVALID;
    }

    int errorCode = LoadPatient();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = LoadSeries();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = LoadCourse();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = LoadSite();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = LoadPrescription();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = LoadPlan();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    TEST_PERFORMANCE_END("LoadPatientPartialCommand::Execute")

    return TPS_ER_SUCCESS;
}

int LoadPatientPartialCommand::LoadPatient()
{
    if (mDatabaseWrapper->GetPatientByUid(msPatientUID, mPatient) != DB_CODES_SUCCESS
        || mPatient == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to load patient from DB with PatientUID:" << msPatientUID;
        return TPS_ER_DB_ERROR;
    }

    //only real patient need to be set
    //BE dose not manage internal patient
    if(!mPatient->get_isinternal())
    {
        mDataRepository->GetPatientManager()->SetPatient(mPatient);
    }

    return TPS_ER_SUCCESS;
}

int LoadPatientPartialCommand::LoadSeries()
{
    if (mDatabaseWrapper->GetPartialSeriesListByPatientUid(msPatientUID, mvSeriesList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load partial series from DB with PatientUID:" << msPatientUID;
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

int LoadPatientPartialCommand::LoadCourse()
{
    if (mDatabaseWrapper->GetCourseListByPatientUid(msPatientUID, mvCourseList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load course from DB with patientUID:" << msPatientUID;
        return TPS_ER_DB_ERROR;
    }

    for (auto itCourse = mvCourseList.begin(); itCourse != mvCourseList.end(); ++itCourse)
    {
        mDataRepository->GetPatientManager()->AddCourse(*itCourse);
    }

    return TPS_ER_SUCCESS;
}

int LoadPatientPartialCommand::LoadSite()
{
    std::vector<std::string> courseUIDList(mvCourseList.size());
    std::transform(mvCourseList.begin(), mvCourseList.end(),
        courseUIDList.begin(), [](RtCourse* pCourse) { return pCourse->get_uid(); });

    if (mDatabaseWrapper->GetSiteListByCourseUidList(courseUIDList, mvSiteList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load site from DB";
        return TPS_ER_DB_ERROR;
    }

    mDataRepository->GetPatientManager()->AddSiteList(mvSiteList);

    return TPS_ER_SUCCESS;
}

int LoadPatientPartialCommand::LoadPrescription()
{
    std::vector<std::string> siteUIDList(mvSiteList.size());
    std::transform(mvSiteList.begin(), mvSiteList.end(),
        siteUIDList.begin(), [](RtSite* pSite) { return pSite->get_uid(); });

    if (mDatabaseWrapper->GetPrescriptionListBySiteUidList(siteUIDList, mvPrescriptionList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load site from DB";
        return TPS_ER_DB_ERROR;
    }

    mDataRepository->GetPatientManager()->AddPrescriptionList(mvPrescriptionList);

    return TPS_ER_SUCCESS;
}

int LoadPatientPartialCommand::LoadPlan()
{
    std::vector<std::string> courseUIDList(mvCourseList.size());
    std::transform(mvCourseList.begin(), mvCourseList.end(),
        courseUIDList.begin(), [](RtCourse* pCourse) { return pCourse->get_uid(); });

    if (mDatabaseWrapper->GetPartialPlanListByCourseUidList(courseUIDList, mvPlanList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load plan from DB";
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
