////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file tps_rl_clear_hot_cold_spots_operation.h
/// 
///  \brief class TpsHotSpotsOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/05/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_clear_hot_cold_spots_operation.h"


#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsClearHotColdSpotsOperation::TpsClearHotColdSpotsOperation(
    const std::string& planUID) :  m_strPlanUID(planUID)
{

}

TpsClearHotColdSpotsOperation::~TpsClearHotColdSpotsOperation()
{

}

bool TpsClearHotColdSpotsOperation::ModifyGraphicObject() const
{
    if (mDataRepository == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get data repository.";
        return false;
    }

    auto goCold = mModelWarehouse->GetModelObject(COMMON_BASE +"|"+ GOTypeHelper::ToString(GO_TYPE_COLD_SPOTS));
    if (nullptr == goCold)
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get ColdSpots go!";
        return false;
    }
    auto spColdSpotsGO = std::dynamic_pointer_cast<HotSpotsGraphicObject>(goCold);
    if (nullptr == spColdSpotsGO)
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to cast ColdSpots go!";
        return false;
    }
    spColdSpotsGO->ClearVecPointsByPlanUid(m_strPlanUID);
    spColdSpotsGO->SetDirty(true);
    
    
    auto goHot = mModelWarehouse->GetModelObject(COMMON_BASE +"|"+ GOTypeHelper::ToString(GO_TYPE_HOT_SPOTS));
    if (nullptr == goHot) 
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to get HotSpots go!";
        return false;
    }
    auto spHotSpotsGO = std::dynamic_pointer_cast<HotSpotsGraphicObject>(goHot);
    if (nullptr == spHotSpotsGO)
    {
        TPS_LOG_DEV_ERROR << "TpsHotSpotsOperation error: failed to cast HotSpots go!";
        return false;
    }
    spHotSpotsGO->ClearVecPointsByPlanUid(m_strPlanUID);
    spHotSpotsGO->SetDirty(true);
    
    return true;
}

TPS_END_NAMESPACE