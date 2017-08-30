////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_update_table.cpp
/// 
///  \brief class UpdateTableCommand implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/07/21
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_update_table.h"

#include "boost/algorithm/string.hpp"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

UpdateTableCommand::UpdateTableCommand(
    const std::string& seriesUid, 
    const std::string& tableUid,
    double density,
    double referenceEDensity,
    double offsetX,
    double offsetY,
    double offsetZ, 
    double angleX,
    double angleY,
    double angleZ) : TpsCommand(), 
    mSeriesUid(seriesUid), mTableUid(tableUid), mDensity(density), mReferenceEDensity(referenceEDensity)
{
    mOffsets[0] = offsetX;
    mOffsets[1] = offsetY;
    mOffsets[2] = offsetZ;
    mRotationAngles[0] = angleX;
    mRotationAngles[1] = angleY;
    mRotationAngles[2] = angleZ;
}

UpdateTableCommand::~UpdateTableCommand()
{

}

int UpdateTableCommand::Execute()
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
    if (seriesTable->GetTableUid() != mTableUid)
    {
        TPS_LOG_DEV_ERROR<<"The current table uid is not coincident with specified one.";
        return TPS_ER_FAILURE;
    }
    //check rt table if table uid is not empty
    auto rtTable = tblMng->GetRtTable(mTableUid);
    if (!mTableUid.empty() && rtTable == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The RtTable does not exist.";
        return TPS_ER_FAILURE;
    }
    seriesTable->SetDensity(mDensity);
    seriesTable->SetReferenceEDensity(mReferenceEDensity);
    seriesTable->SetOffset(mOffsets);
    seriesTable->SetRotationAngles(mRotationAngles);

    int iRet = mSaveObjects->SaveSeriesTable(*seriesTable);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Save series table failed.";
        return iRet;
    }

    //to be improved:if density changed only, need not render
    mRenderProxy->UpdateSeriesTable(mSeriesUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE