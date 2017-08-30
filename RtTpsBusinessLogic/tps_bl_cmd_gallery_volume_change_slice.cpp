//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yuxuan.duan@united-imaging.com
///
///  \file    tps_bl_cmd_gallery_volume_change_slice.cpp
///  \brief   Volume Change Slice command
///  \class   VolumeChangeSliceCmd
///
///  \version 1.0
///  \date    Mar 13, 2017
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_gallery_volume_change_slice.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper

#include "RtTpsCore/tps_core_simple_gui_responser.h"

//TPS
#include "tps_logger.h"
//#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprpageturn_operation.h"
//#include "RtTpsRenderLibrary/tps_rl_clearmprrendercacheoperation.h"
//#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"
//#include "RtTpsRenderLibrary/tps_rl_updatecachecollectionoperation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_query_current_mpr_slice_operation.h"


TPS_BEGIN_NAMESPACE

GalleryVolumeChangeSliceCmd::GalleryVolumeChangeSliceCmd(const std::string& strSeriesUid, int nSectionType, int nPageChange, std::shared_ptr<SimpleGuiResponser> spResponser)
    :m_strSeriesUid(strSeriesUid), 
    m_nSectionType(nSectionType), 
    m_nPageChange(nPageChange), 
    m_spResponser(spResponser)
{

}


GalleryVolumeChangeSliceCmd::~GalleryVolumeChangeSliceCmd(void)
{

}

GalleryVolumeChangeSliceCmd *GalleryVolumeChangeSliceCmd::Clone()
{
    GalleryVolumeChangeSliceCmd *cmd = new GalleryVolumeChangeSliceCmd(m_strSeriesUid, m_nSectionType, m_nPageChange, m_spResponser);
    return cmd;
}

int GalleryVolumeChangeSliceCmd::PreExecute()
{
    return TPS_ER_SUCCESS;
}

int GalleryVolumeChangeSliceCmd::Execute()
{
    try
    {
        std::string strSeriesUid = m_strSeriesUid;
        FIRST_POSTFIX_COMPONENT nSection = (FIRST_POSTFIX_COMPONENT)m_nSectionType;

        // 1. execute the MPR scrolling command
        std::shared_ptr<TpsGalleryMPRPageTurnOperation> spPageTurnOp(new TpsGalleryMPRPageTurnOperation(
            m_nPageChange, strSeriesUid, nSection, true));
        mRenderProxy->ProcessOperation(spPageTurnOp);

        // 2. fire to render
        mRenderProxy->Render();

        // send movie progress to FE
        int slice_info[2], start;
        std::shared_ptr<TpsGalleryQueryMprSliceInfoOperation> op(
            new TpsGalleryQueryMprSliceInfoOperation(strSeriesUid, nSection, true));
        mRenderProxy->ProcessOperation(op);
        op->GetSliceInfo(slice_info[0], slice_info[1], start);
        if (m_spResponser != nullptr)
            m_spResponser->Response((char*)slice_info, sizeof(int) * 2);
        //RepositoryDispatcher::GetInstance()->SendMovieProgress(nTarget + 1);
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<ex.what();
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int GalleryVolumeChangeSliceCmd::PostExecute()
{
    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE