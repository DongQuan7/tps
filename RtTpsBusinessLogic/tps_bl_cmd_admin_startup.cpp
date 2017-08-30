//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_cca_addvoi_cmd.cpp
///  \brief   addvoi
///
///  \version 1.0
///  \date    Jan. 25, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_admin_startup.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

AdminStartupCmd::AdminStartupCmd(){
}


AdminStartupCmd::~AdminStartupCmd(void) 
{
}

int AdminStartupCmd::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][AdminStartupCmd][Execute]: "<<"AdminStartupCmd execute!";

    int errorCode = LoadPatients();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = LoadSeries();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    return TPS_ER_SUCCESS;
}

int AdminStartupCmd::LoadPatients()
{
    if (mDatabaseWrapper->GetPatientListByCondition(" isinternal = true", mvPatientList) != DB_CODES_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to load internal patients from DB";
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

int AdminStartupCmd::LoadSeries()
{
    std::vector<RtSeries*> seriesList;
    for (auto pPatient = mvPatientList.begin(); pPatient != mvPatientList.end(); ++pPatient)
    {
        seriesList.clear();
        if (mDatabaseWrapper->GetPartialSeriesListByPatientUid((*pPatient)->get_uid(), seriesList) != DB_CODES_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to load partial series from DB with PatientUID:" << (*pPatient)->get_uid();
            return TPS_ER_DB_ERROR;
        }
        std::copy(seriesList.begin(), seriesList.end(), std::back_inserter(mvSeriesList));
    }

    return TPS_ER_SUCCESS;
}

void AdminStartupCmd::GetResult(std::vector<RtPatient*>& vPatientList, std::vector<RtSeries*>& vSeriesList)
{
    vPatientList = mvPatientList;
    vSeriesList = mvSeriesList;
}

TPS_END_NAMESPACE

