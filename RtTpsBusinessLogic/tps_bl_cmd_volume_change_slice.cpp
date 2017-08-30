//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_bl_cmd_volume_change_slice.cpp
///  \brief   Volume Change Slice command
///  \class   VolumeChangeSliceCmd
///
///  \version 1.0
///  \date    July 30, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_volume_change_slice.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper

#include "RtTpsCore/tps_core_simple_gui_responser.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"
#include "RtTpsRenderLibrary/tps_rl_query_current_mpr_slice_operation.h"


TPS_BEGIN_NAMESPACE

VolumeChangeSliceCmd::VolumeChangeSliceCmd(const std::string& strSeriesUid, const std::string& strPlanUid, int nSectionType, int nPageChange, std::shared_ptr<SimpleGuiResponser> spResponser,bool bIsCyclic)
    :m_strSeriesUid(strSeriesUid), 
     m_strPlanUid(strPlanUid),
     m_nSectionType(nSectionType), 
     m_nPageChange(nPageChange), 
     m_spResponser(spResponser),
     m_bIsCyclic(bIsCyclic){
}


VolumeChangeSliceCmd::~VolumeChangeSliceCmd(void) {
}

VolumeChangeSliceCmd *VolumeChangeSliceCmd::Clone() {
    VolumeChangeSliceCmd *cmd = new VolumeChangeSliceCmd(m_strSeriesUid, m_strPlanUid, m_nSectionType, m_nPageChange, m_spResponser,m_bIsCyclic);
    return cmd;
}

int VolumeChangeSliceCmd::PreExecute() {
    return TPS_ER_SUCCESS;
}

int VolumeChangeSliceCmd::Execute() {

    try
    {
        std::string strSeriesUid = m_strSeriesUid;
        FIRST_POSTFIX_COMPONENT nSection = (FIRST_POSTFIX_COMPONENT)m_nSectionType;

        // 1. execute the MPR scrolling command
        std::shared_ptr<MPRPageTurnOperation> spPageTurnOp(new MPRPageTurnOperation(
            m_nPageChange, strSeriesUid, nSection, EMPTY_COMPONENT, m_bIsCyclic));
        mRenderProxy->ProcessOperation(spPageTurnOp);

        // 3. update dose line buffer
        std::shared_ptr<TpsRefreshDoseOperation> spRefreshDose(new TpsRefreshDoseOperation(m_strPlanUid, nSection));
        mRenderProxy->ProcessOperation(spRefreshDose);

        // 4. fire to render
        mRenderProxy->Render();

        // send movie progress to FE
        int slice_info[2], start;
        std::shared_ptr<QueryMprSliceInfoOperation> op(
            new QueryMprSliceInfoOperation(strSeriesUid, nSection, true));
        mRenderProxy->ProcessOperation(op);
        op->GetSliceInfo(slice_info[0], slice_info[1], start);
        if (m_spResponser != nullptr)
            m_spResponser->Response((char*)slice_info, sizeof(int) * 2);
            //RepositoryDispatcher::GetInstance()->SendMovieProgress(nTarget + 1);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int VolumeChangeSliceCmd::PostExecute() {
    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE