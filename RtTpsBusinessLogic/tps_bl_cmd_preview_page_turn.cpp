//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_bl_cmd_voi_preview_page_turn.cpp
///  \brief   VoiPreviewPageTurnCmd command
///
///  \version 1.0
///  \date    Oct. 21, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_preview_page_turn.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper


//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

//TPS
#include "tps_logger.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"

#include "RtTpsFramework/tps_fw_common_enums.h"

#include "RtTpsRenderController/tps_rc_voi_preview_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_beam_preview_rendercontroller.h"

TPS_BEGIN_NAMESPACE


PreviewPageTurnCmd::PreviewPageTurnCmd(
    const std::string& strWindowUid, int nWindowType, 
    int nPageTurn, PREVIEW_TYPE nPreViewType)
    :m_strWindowUid(strWindowUid), m_nWindowType(nWindowType),
     m_nPageTurn(nPageTurn), m_nPreViewType(nPreViewType)
{

}


PreviewPageTurnCmd::~PreviewPageTurnCmd(void)
{
}

PreviewPageTurnCmd *PreviewPageTurnCmd::Clone()
{
    PreviewPageTurnCmd *cmd = new PreviewPageTurnCmd(m_strWindowUid, m_nWindowType, m_nPageTurn, m_nPreViewType);
    return cmd;
}

int PreviewPageTurnCmd::PreExecute() 
{
    return TPS_ER_SUCCESS;
}

int PreviewPageTurnCmd::Execute() 
{
    try
    {
         // 1. Check information
        if (mRenderProxy == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to get related render proxy.";
            return TPS_ER_FAILURE;
        }

        std::string strSeriesUid;
        if (!mRenderProxy->GetDataSource((WINDOW_TYPE)m_nWindowType, IMAGE, strSeriesUid)) 
        {
            TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
            return TPS_ER_FAILURE;
        }

        auto renderController = mRenderProxy->GetRenderController((WINDOW_TYPE)m_nWindowType, m_strWindowUid);

        FIRST_POSTFIX_COMPONENT section = EMPTY_COMPONENT;
        SECOND_POSTFIX_COMPONENT sectionExtend = EMPTY_COMPONENT;
        if (m_nPreViewType == PREVIEW_VOI)
        {
            auto voiPreviewRenderController = dynamic_cast<TpsVoiPreviewRenderController*>(renderController);
            if (voiPreviewRenderController == nullptr)
            {
                TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
                return TPS_ER_FAILURE;
            }
            section = voiPreviewRenderController->GetSectionType();
            sectionExtend = voiPreviewRenderController->GetSectionExtendType();
        }
        else if (m_nPreViewType == PREVIEW_BEAM)
        {
            auto beamPreviewRenderController = dynamic_cast<TpsBeamPreviewRenderController*>(renderController);
            if (beamPreviewRenderController == nullptr)
            {
                TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
                return TPS_ER_FAILURE;
            }
             section = beamPreviewRenderController->GetSectionType();
             sectionExtend = beamPreviewRenderController->GetSectionExtendType();
        }
        else
        {
            TPS_LOG_DEV_ERROR << "Unknown Preview type.";
            return TPS_ER_FAILURE;
        }

        // 2. execute the MPR pageturn operation
        std::shared_ptr<MPRPageTurnOperation> pageTurnoperation(
            new MPRPageTurnOperation(m_nPageTurn, strSeriesUid, section, sectionExtend));
        mRenderProxy->ProcessOperation(pageTurnoperation);

        mRenderProxy->Render();

        return TPS_ER_SUCCESS;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << ex.what();
        return TPS_ER_FAILURE;
    }
}

int PreviewPageTurnCmd::PostExecute() 
{
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
