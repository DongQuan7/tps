////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_switch_table.cpp
/// 
///  \brief class SwitchTableCommand implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_switch_table.h"

#include "boost/algorithm/string.hpp"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsBusinessLogic/tps_bl_load_objects.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

SwitchTableCommand::SwitchTableCommand(const std::string& seriesUid, 
    const std::string& tableUid) : TpsCommand(), 
    mSeriesUid(seriesUid), mTableUid(tableUid)
{

}

SwitchTableCommand::~SwitchTableCommand()
{

}

int SwitchTableCommand::Execute()
{
    //check image 3d
    TpsImage3DEntity* image3D = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        mSeriesUid, &image3D) || image3D == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The image 3d does not exist;";
        return TPS_ER_FAILURE;
    }
    //check series table
    auto tblMng = mDataRepository->GetTableManager();
    auto seriesTable = tblMng->GetSeriesTable(mSeriesUid);
    if (seriesTable == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The series table does not exist.";
        return TPS_ER_FAILURE;
    }
    //check rt table if table uid is not empty
    auto rtTable = tblMng->GetRtTable(mTableUid);
    if (!mTableUid.empty() && rtTable == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The RtTable does not exist.";
        return TPS_ER_FAILURE;
    }
    seriesTable->SetTableUid(mTableUid);
    seriesTable->ClearTransformation();
    //initialize the location of the new table in the image patient space
    if (!TpsLoadObjects::InitializeTableLocation(image3D, rtTable, seriesTable))
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize the table location.";
        return TPS_ER_FAILURE;
    }

    int iRet = mSaveObjects->SaveSeriesTable(*seriesTable);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"Save series table failed.";
        return iRet;
    }

    //rendering part, update table graphic object & start rendering
    mRenderProxy->UpdateSeriesTable(mSeriesUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

void SwitchTableCommand::ExtractEulerAnglesFromMatrix_i(
    const Mcsf::Matrix4f& matrix, double* angles, Mcsf::Matrix4f& rotationTM) const
{
    float thetaX = atan2(matrix.GetElem(2, 1), matrix.GetElem(2, 2));
    float c2 = sqrt(pow(matrix.GetElem(0, 0), 2) + pow(matrix.GetElem(1, 0), 2));
    float thetaY = atan2(-matrix.GetElem(2, 0), c2);
    float s1(sin(thetaX)), c1(cos(thetaX));
    float thetaZ = atan2(s1 * matrix.GetElem(0, 2) - c1 * matrix.GetElem(0, 1), 
        c1 * matrix.GetElem(1, 1) - s1 * matrix.GetElem(1, 2));
    angles[0] = -thetaX * 180 / PI;
    angles[1] = -thetaY * 180 / PI;
    angles[2] = -thetaZ * 180 / PI;

    rotationTM = rotationTM.RotationX(-thetaX);
    rotationTM.Append(rotationTM.RotationY(-thetaY));
    rotationTM.Append(rotationTM.RotationZ(-thetaZ));
}
TPS_END_NAMESPACE