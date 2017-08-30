//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_bl_cmd_volume4d_change_current.cpp
///  \brief   Volume4dChangeCurrentCmd
///  \class   Volume4dChangeCurrentCmd
///
///  \version 1.0
///  \date    July 30, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_volume4d_change_current.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper

#include "RtTpsCore/tps_core_simple_gui_responser.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"

#include "RtTpsRenderLibrary/tps_rl_volume4d_change_current_operation.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"

TPS_BEGIN_NAMESPACE

Volume4dChangeCurrentCmd::Volume4dChangeCurrentCmd(const std::string& strSeriesUid, int nControl, std::shared_ptr<SimpleGuiResponser> spResponser,bool bIsCyclic)
:m_strSeriesUid(strSeriesUid), m_nControl(nControl), m_spResponser(spResponser), m_bIsCyclic(bIsCyclic){
}


Volume4dChangeCurrentCmd::~Volume4dChangeCurrentCmd(void) {
}

Volume4dChangeCurrentCmd *Volume4dChangeCurrentCmd::Clone() {
    Volume4dChangeCurrentCmd *cmd = new Volume4dChangeCurrentCmd(m_strSeriesUid, m_nControl, m_spResponser, m_bIsCyclic);
    return cmd;
}

int Volume4dChangeCurrentCmd::PreExecute() {
    return TPS_ER_SUCCESS;
}

int Volume4dChangeCurrentCmd::Execute() {

    try
    {
        if (mDataRepository == nullptr)
            return TPS_ER_FAILURE;
        TpsImageDataManager* pImageManager = mDataRepository->GetImageDataManager();
        if(pImageManager == nullptr)
            return TPS_ER_FAILURE;
        // Note: m_strSeriesUid is the main phase series
        std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = pImageManager->GetSeriesGroup4d(m_strSeriesUid);
        if (spSeriesGroup4d == nullptr)
            return TPS_ER_FAILURE;

        if (spSeriesGroup4d->m_bValid)
        {
            if (m_nControl == 0)      // play
            {
                int nCurrentIndex = 0;
                for (int i = 0; i < spSeriesGroup4d->m_vecSeriesUid.size(); i++)
                {
                    if (spSeriesGroup4d->m_vecSeriesUid[i] == spSeriesGroup4d->m_strCurrentSeriesUid)
                    {
                        nCurrentIndex = i;
                        break;
                    }
                }
                if (m_bIsCyclic)
                {
                    nCurrentIndex = (nCurrentIndex + 1) % spSeriesGroup4d->m_vecSeriesUid.size();
                }
                else
                {
                    nCurrentIndex = TPS_CLAMP(nCurrentIndex + 1, 1, spSeriesGroup4d->m_vecSeriesUid.size()-1);
                }
                if (spSeriesGroup4d->m_strCurrentSeriesUid == spSeriesGroup4d->m_vecSeriesUid[nCurrentIndex])
                    return TPS_ER_SUCCESS;
                spSeriesGroup4d->m_strCurrentSeriesUid = spSeriesGroup4d->m_vecSeriesUid[nCurrentIndex];
            }
            else if(m_nControl == 1)  // stop
            {
                // When stop 4D movie, return to FirstSeries. if already there, ignore
                if (spSeriesGroup4d->m_strCurrentSeriesUid == spSeriesGroup4d->m_strFirstSeriesUid)
                    return TPS_ER_SUCCESS;
                spSeriesGroup4d->m_strCurrentSeriesUid = spSeriesGroup4d->m_strFirstSeriesUid;
            }
        }

        std::shared_ptr<Volume4dChangeCurrentOperation> spVolume4dOp(new Volume4dChangeCurrentOperation(spSeriesGroup4d->m_strMainSeriesUid, spSeriesGroup4d->m_strCurrentSeriesUid));
        mRenderProxy->ProcessOperation(spVolume4dOp);

        // 5. fire to render
        mRenderProxy->Render();

        if (m_spResponser != nullptr)
            m_spResponser->Response(spSeriesGroup4d->m_strCurrentSeriesUid.c_str(), spSeriesGroup4d->m_strCurrentSeriesUid.length());
        //RepositoryDispatcher::GetInstance()->Send4DMovieProgress(spVolume4dEntity->m_strCurrentSeriesUid);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int Volume4dChangeCurrentCmd::PostExecute() {
    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE
