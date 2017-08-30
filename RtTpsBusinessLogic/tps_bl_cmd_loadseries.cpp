//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_bl_cmd_loadseries.cpp
///  \brief   load series
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_loadseries.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess\tps_da_poimanager.h"

#include "RtTpsBusinessLogic/tps_bl_load_objects.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_table.h"

#include "tps_logger.h"
#include "tps_performance.h"

TPS_BEGIN_NAMESPACE

LoadSeriesCommand::LoadSeriesCommand(const std::string sSeriesUID)
: msSeriesUID(sSeriesUID)
{

}

LoadSeriesCommand::~LoadSeriesCommand()
{

}


void LoadSeriesCommand::GetResult(TpsImage3DEntity*& image3DEntity,
    std::vector<ROIEntity*>& vVoiEntityList, std::vector<RtPoi*>& vRtPoiList)
{
    image3DEntity = mpImage3DEntity;
    vVoiEntityList = mvVoiEntityList;
    vRtPoiList = mvRtPoiList;
}

int LoadSeriesCommand::Execute()
{
    TPS_LOG_DEV_INFO<<"LoadSeriesCommand::Execute";

    TEST_PERFORMANCE_INIT
    TEST_PERFORMANCE_BEGIN

    std::vector<std::string> seriesUIDList(1, msSeriesUID);
    int errorCode = mLoadObjects->LoadSeries(seriesUIDList);
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeSeries();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializeVoi();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    errorCode = InitializePoi();
    if (errorCode != TPS_ER_SUCCESS)
    {
        return errorCode;
    }

    TEST_PERFORMANCE_END("LoadSeriesCommand::Execute")

    return TPS_ER_SUCCESS;
}

int LoadSeriesCommand::InitializeSeries()
{
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(msSeriesUID, &mpImage3DEntity)
        || mpImage3DEntity == nullptr)
    {
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int LoadSeriesCommand::InitializeVoi()
{
    mvVoiEntityList = mDataRepository->GetVoiManager()->GetROIBySeries(msSeriesUID);

    return TPS_ER_SUCCESS;
}

int LoadSeriesCommand::InitializePoi()
{
    mvRtPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(msSeriesUID);

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
