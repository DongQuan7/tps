////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_mpr_show_fusion_cmd.cpp
/// 
///  \brief class MprShowFusionCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/10/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_mpr_show_fusion.h"

#include "boost/lexical_cast.hpp"

#include "RtTpsDataAccess/tps_da_idatarepository.h"             // for ITpsDataRepository
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"            // for TpsImageDataManager
#include "RtTpsFramework/tps_fw_image3dentity.h"               // for TpsImage3DEntity
#include "RtTpsRenderController/tps_rc_renderproxy.h"           // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_datasourcegroup.h"              // for TpsDataSourceGroup

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

MprShowFusionCmd::MprShowFusionCmd(const std::string& strFixedSeriesUid, const std::string& strFloatSeriesUid, bool bVisible)
    :m_strFixedSeriesUid(strFixedSeriesUid), m_strFloatSeriesUid(strFloatSeriesUid), m_bVisible(bVisible)
{
}

MprShowFusionCmd::~MprShowFusionCmd()
{
}

int MprShowFusionCmd::Execute()
{
    //check if the float image has registered with current image
    //if(mDataRepository->GetImageDataManager()->GetCurrentImage3DUID() != m_strFixedSeriesUid) 
    //{
    //    TPS_LOG_DEV_ERROR<<"Current image differ from registration record.";
    //    return TPS_ER_FAILURE;
    //}

    auto group = mRenderProxy->GetDataSourceGroup(SOURCE_GROUP_COMMON);
    if (group == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The common data source group does not exist.";
        return TPS_ER_FAILURE;
    }

    if (m_bVisible) 
    {
        if (!mDataRepository->GetImageDataManager()->IsSeriesExists(m_strFloatSeriesUid))
        {
            TPS_LOG_DEV_ERROR<<"Image3D does not exist.";
            return TPS_ER_FAILURE;
        }
        group->SetDataSource(FLOAT_IMAGE, m_strFloatSeriesUid);
    }
    else 
    {
        group->RemoveDataSource(FLOAT_IMAGE);
    }

    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE  // end namespace tps