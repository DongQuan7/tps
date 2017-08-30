////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  xiaowei.li@united-imaging.com
/// 
///  \file tps_bl_cmd_create_internal_patient.cpp
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    July 27,2017
////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_internal_patient.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsCreatePatientCmd::TpsCreatePatientCmd(const proto::RtTpsProtoPatient &protoPatient)
: mProtoPatient(protoPatient), mPatient(new RtPatient(false)) 
{

}

TpsCreatePatientCmd::~TpsCreatePatientCmd(void) {
    delete mPatient;
    mPatient = nullptr;
}


int TpsCreatePatientCmd::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][TpsCreatePatientCmd][Execute]: "<<"TpsCreatePatientCmd execute!";
    mPatient->set_uid(mProtoPatient.patientuid());
    mPatient->set_patientid(mProtoPatient.patientuid());
    mPatient->set_isinternal(mProtoPatient.isinternal());
    mPatient->set_lastname(mProtoPatient.patientlastname());
    mPatient->set_patientsex(mProtoPatient.patientsex());
    mPatient->set_isactive(true);
    if (TPS_ER_SUCCESS != mSaveObjects->SavePatient(mPatient))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save series:"<<mPatient->get_uid();
        return TPS_ER_FAILURE;
    }

    //patient table need crc
    mPatient->setdirty_all_fields(false);
    std::string sMD5 = mDatabaseWrapper->CalcHash((const void*)mPatient, RtDbDef::TYPE_PATIENT);
    if (sMD5.empty())
    {
        TPS_LOG_DEV_ERROR<<"TpsCreatePatientCmd: Failed to CalcHash crc with patient uid:" << mPatient->get_uid();
        return TPS_ER_FAILURE;
    }

    mPatient->set_crc(sMD5);
    if (TPS_ER_SUCCESS != mSaveObjects->UpdatePatient(mPatient))
    {
        TPS_LOG_DEV_ERROR<<"TpsCreatePatientCmd: Failed to update crc with patient uid:" << mPatient->get_uid();
        return TPS_ER_FAILURE;
    }

    TPS_LOG_DEV_INFO<<"[INFO][TpsCreatePatientCmd][Execute]: "<<"TpsCreatePatientCmd leave!";
    return TPS_ER_SUCCESS;
}

void TpsCreatePatientCmd::GetPatient(const RtPatient*& pPatient){
    pPatient = mPatient;
}
TPS_END_NAMESPACE