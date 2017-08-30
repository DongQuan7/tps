////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Song LI  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_beam_segment_update.cpp
/// 
///  \brief class  BeamSegmentUpdateCmd 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_beam_segment_update.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"

#include "RtTpsCertifiedLib/rt_tps_certified_segment.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "tps_logger.h"


TPS_BEGIN_NAMESPACE;

BeamSegmentUpdateCmd::BeamSegmentUpdateCmd(const std::vector<std::shared_ptr<TpsCertifiedSegment>>& vecSegmentPtr)
    :m_vecCeritifedSegmentPtr(vecSegmentPtr)
{
}

BeamSegmentUpdateCmd::~BeamSegmentUpdateCmd(void)
{

}

static void  CertifiedSegment2RtBeamsegment(TpsCertifiedSegment* pCertifiedSegment, RtBeamsegment* pSegment)
{
    pSegment->set_uid(pCertifiedSegment->segmentUID);
    pSegment->set_mu(pCertifiedSegment->segmentMu);
    pSegment->set_weight(pCertifiedSegment->segmentWeight);
    pSegment->set_sequencenumber(pCertifiedSegment->SequenceNumber);
    pSegment->set_startssd(pCertifiedSegment->startSSD);
    pSegment->set_endssd(pCertifiedSegment->endSSD);

    // Note: for dynamic mlc, should NOT be edited!
    // anyway, the code have to be like this ......
    // start mlc
    RtMlcshape* pMlcStart = pSegment->get_startmlcshape();
    // end mlc
    RtMlcshape* pMlcEnd = pSegment->get_endmlcshape();
    pMlcStart->set_xlowerjawpos(pCertifiedSegment->startJaw.x1);
    pMlcStart->set_xupperjawpos(pCertifiedSegment->startJaw.x2);
    pMlcStart->set_ylowerjawpos(pCertifiedSegment->startJaw.y1);
    pMlcStart->set_yupperjawpos(pCertifiedSegment->startJaw.y2);
    pMlcEnd->set_xlowerjawpos(pCertifiedSegment->startJaw.x1);
    pMlcEnd->set_xupperjawpos(pCertifiedSegment->startJaw.x2);
    pMlcEnd->set_ylowerjawpos(pCertifiedSegment->startJaw.y1);
    pMlcEnd->set_yupperjawpos(pCertifiedSegment->startJaw.y2);

    std::vector<db_Point2d> vecLeafPos;
    db_Point2d leafPos;
    vecLeafPos.clear();
    for (int i = 0; i < pCertifiedSegment->startMlcLeaves.size(); i++) 
    {
        leafPos.x = pCertifiedSegment->startMlcLeaves[i].x1;
        leafPos.y = pCertifiedSegment->startMlcLeaves[i].x2; 
        vecLeafPos.push_back(leafPos);
    }
    pMlcStart->set_leafpositions(vecLeafPos);
    pMlcEnd->set_leafpositions(vecLeafPos);
}

int BeamSegmentUpdateCmd::Execute() 
{
    if (m_vecCeritifedSegmentPtr.size() <= 0)
    {
        return TPS_ER_FAILURE;
    }

    // (1) Check beam
    std::string strBeamUid = m_vecCeritifedSegmentPtr[0]->beamUID;
    if (strBeamUid.empty())
    {
        return TPS_ER_FAILURE;
    }

    TpsBeamManager* pBeamManager = mDataRepository->GetBeamManager();
    if (pBeamManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to find beam manager";
        return TPS_ER_FAILURE;
    }

    RtBeam* pBeam = pBeamManager->GetBeam(strBeamUid);
    if (pBeam == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam: " << strBeamUid;
        return TPS_ER_FAILURE;
    }

    //(2) Check norm group
    std::string strNormGroupUid = pBeam->get_normgroupuid();
    TpsNormGroupManager* pNormGroupManager = mDataRepository->GetNormgroupManager();
    RtNormgroup* pNormGroup = nullptr;
    if (pNormGroupManager == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    if (pNormGroupManager->GetNormGroup(strNormGroupUid, &pNormGroup) != true)
    {
        TPS_LOG_DEV_ERROR << "Failed to find normgroup uid :"<< strNormGroupUid;
        return TPS_ER_FAILURE;
    }
    if (pNormGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normgroup:" << strNormGroupUid;
        return TPS_ER_FAILURE;
    }

    RtDbDef::PLAN_TYPE planType = (RtDbDef::PLAN_TYPE)pNormGroup->get_plantype();

    // the vector contains multiple RtBeamsegment pointer ( created via new )
    std::vector<RtBeamsegment*> vecSegmentOld = pBeam->get_beamsegments();

    if (vecSegmentOld.empty())
    {
        TPS_LOG_DEV_ERROR << "Segment is empty in beam:" << strBeamUid;
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeamsegment*> vecSegmentNew;
    // keep a segment as the copy source
    RtBeamsegment* pModelSegment = vecSegmentOld[0];
    switch (planType)
    {
        // we have add & delete segments here
    case RtDbDef::PLAN_Step_Shoot:
        {
            for (int i = 0; i < m_vecCeritifedSegmentPtr.size(); i++)
            {
                std::string strSegmentUid = m_vecCeritifedSegmentPtr[i]->segmentUID;
                RtBeamsegment* pSegment = nullptr;
                // find the segment origin
                for (int j = 0; j < vecSegmentOld.size(); j++)
                {
                    if (vecSegmentOld[j] == nullptr)
                        continue;
                    if (vecSegmentOld[j]->get_uid() == strSegmentUid)
                    {
                        //pSegment = vecSegmentOld[j];
                        pSegment = new RtBeamsegment(*vecSegmentOld[j]);
                        vecSegmentOld[j] = nullptr;
                        break;
                    }
                }
                // for new comer, copy construct from the model segment
                if (pSegment == nullptr)
                {
                    pSegment = new RtBeamsegment(*pModelSegment);
                    // start mlc 
                    RtMlcshape* pMlcStart = pSegment->get_startmlcshape();
                    pMlcStart->set_uid(RtUidGenerater::GeneraterUid());

                    // end mlc
                    RtMlcshape* pMlcEnd = pSegment->get_endmlcshape();
                    pMlcEnd->set_uid(RtUidGenerater::GeneraterUid());
                }

                // Modify RtSegment from Certified input
                CertifiedSegment2RtBeamsegment(m_vecCeritifedSegmentPtr[i].get(), pSegment);
                vecSegmentNew.push_back(pSegment);
            }
            pBeam->set_beamsegments(vecSegmentNew);
            break;
        }
    case RtDbDef::PLAN_3D_CRT:
    case RtDbDef::PLAN_2D:
    case RtDbDef::PLAN_Sliding_Window:
    case RtDbDef::PLAN_Simple_Arc:
    case RtDbDef::PLAN_Conformal_Arc:
    case RtDbDef::PLAN_VMAT_Arc:
    case RtDbDef::PLAN_Burst_Arc: 
        for (int i = 0; i < vecSegmentOld.size(); i++) 
        {
            std::string strSegmentUid = vecSegmentOld[i]->get_uid();
            for (int j = 0; j < m_vecCeritifedSegmentPtr.size(); j++)
            {
                if (strSegmentUid != m_vecCeritifedSegmentPtr[j]->segmentUID)
                    continue;
                //RtBeamsegment* pSegment = new RtBeamsegment(*pModelSegment);

                // Modify RtSegment from Certified input
                CertifiedSegment2RtBeamsegment(m_vecCeritifedSegmentPtr[j].get(), vecSegmentOld[i]);
                //vecSegmentNew.push_back(pSegment);
                break;
            }
        }
        // pBeam->set_beamsegments(vecSegmentNew);
        break;
    default:
        TPS_LOG_DEV_ERROR << "Unrecognized plan type:" << planType;
        return TPS_ER_FAILURE;
    }

    for (auto itr = vecSegmentNew.begin(); itr != vecSegmentNew.end(); ++itr) DEL_PTR(*itr);

    if (!mDoseProxy->UpdateFluenceMap(pBeam))
    {
        TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
        return false;
    }

    int iRet = mSaveObjects->SaveBeam(strBeamUid);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to save beam.";
        return iRet;
    }

    mRenderProxy->UpdateSegment(pBeam->get_normgroupuid(), strBeamUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
