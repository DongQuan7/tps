//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xiaoqing.Shaangguan Xiaoqing.Shaangguan@united-imaging.com
//
/// \file    tps_load_patient_tree_cmd.cpp
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/07/08
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_load_patient_tree.h"
#include "RtTpsBusinessLogic/tps_bl_load_objects.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

LoadPatientTreeCmd::LoadPatientTreeCmd(std::string patientUid) 
: mPatientUid(patientUid)
{

}

LoadPatientTreeCmd::~LoadPatientTreeCmd()
{

}

int LoadPatientTreeCmd::Execute()
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    int iRet = TPS_ER_FAILURE;

    iRet = mLoadObjects->LoadPatientTree(mPatientUid) ? TPS_ER_SUCCESS : TPS_ER_FAILURE;
    if (TPS_ER_SUCCESS != mLoadObjects->GetLastError())
    {
        iRet = mLoadObjects->GetLastError();
    }
    TEST_PERFORMANCE_END("BE LoadPatientTreeCmd::Execute");
    return iRet;
}

TPS_END_NAMESPACE