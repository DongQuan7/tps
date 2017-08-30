////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_hot_spots.cpp
/// 
///  \brief class TpsVoiHotSpotsCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_hot_spots.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy

#include "RtTpsRenderLibrary/tps_rl_hot_spots_operation.h"

TPS_BEGIN_NAMESPACE

TpsVoiHotSpotsCmd::TpsVoiHotSpotsCmd(std::string& strPlanUid, std::string& strVoiUid, float fDoseLimit, int nHotFlag) 
    :m_strPlanUid(strPlanUid), m_strVoiUid(strVoiUid), m_fDoseLimit(fDoseLimit), m_nHotFlag(nHotFlag) {
}

TpsVoiHotSpotsCmd::~TpsVoiHotSpotsCmd() {

}

int TpsVoiHotSpotsCmd::PreExecute() {
    return TPS_ER_SUCCESS;
}

int TpsVoiHotSpotsCmd::Execute() 
{
    std::shared_ptr<TpsHotSpotsOperation> spOpHotSpots(
        new TpsHotSpotsOperation(m_strPlanUid, m_strVoiUid, m_fDoseLimit, m_nHotFlag));
    mRenderProxy->ProcessOperation(spOpHotSpots);
    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

int TpsVoiHotSpotsCmd::PostExecute() {
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE