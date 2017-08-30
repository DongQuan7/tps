//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
///
///  \file    RtTpsBusinessLogic.cpp
///  \brief   
///
///  \version 1.0
///  \date    Mar. 25, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_change_ct2densitytable.h"

//TPS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

ChangeCT2DensityTableCmd::ChangeCT2DensityTableCmd(
    const std::string& imageUid,
    const std::string& ct2DensityUid)
    :mImageUid(imageUid), 
    mCt2DensityUid(ct2DensityUid) 
{

}

ChangeCT2DensityTableCmd::~ChangeCT2DensityTableCmd(void) 
{
}

int ChangeCT2DensityTableCmd::Execute() 
{

    TPS_LOG_DEV_INFO << "[INFO][ChangeCT2DensityTableCmd][Execute]: " 
        << "ChangeCT2DensityTableCmd execute!";

    RtSeries* series = nullptr;
    mDataRepository->GetImageDataManager()->GetImageSeries(mImageUid, &series);
    series->set_ct2densityuid(mCt2DensityUid);

    //do not save seriesaprgroupid to DB 
    //seriesaprgroupid is varchar(16) in DB, NOT enough. by WJ20170315
    series->disable_field(SERIES_SERIESAPRGROUPID);
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateSeriesObject(*series))
    {
        TPS_LOG_DEV_ERROR<<"failed to UpdateSeriesObject";
        return TPS_ER_DB_ERROR;
    } 

    TpsImage3DEntity *image3D = nullptr;
    TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    if (!imageMng->GetImage3DData(mImageUid, &image3D) || image3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image 3d data.";
        return TPS_ER_FAILURE;
    }

    RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(mImageUid);
    // update voi min/max/mean density wzd 2016-06-29
    std::vector<ROIEntity*> voiList = mDataRepository->GetVoiManager()->GetROIBySeries(mImageUid);
    for (int i = 0; i < voiList.size(); ++i)
    {
        // update voi statistical information
        if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, voiList[i]))
        {
            TPS_LOG_DEV_ERROR << "Failed to update statistical information.";
            return TPS_ER_FAILURE;
        }

        if (mSaveObjects->SaveRoi(*voiList[i]) != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "SaveVOItoTMS failed VOI UID:" << voiList[i]->GetUid();
            return TPS_ER_DB_ERROR;
        }
    }

    TPS_LOG_DEV_INFO << "[INFO][ChangeCT2DensityTableCmd][Execute]: "
        << "ChangeCT2DensityTableCmd leave!";

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
