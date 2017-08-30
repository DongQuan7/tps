////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Zhendong WANG  zhendong.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_beam_block_update.cpp
/// 
///  \brief class  BeamBlockUpdateCmd 
/// 
///  \version 1.0
/// 
///  \date    January.15,2016
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_beam_block_update.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"         // for RtBeam
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"        // for RtBlock

#include "RtTpsProtoLib/rt_tps_proto.pb.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "tps_logger.h"


TPS_BEGIN_NAMESPACE;

BeamBlockUpdateCmd::BeamBlockUpdateCmd(const std::string &beamUid,
    const std::shared_ptr<TpsCertifiedBlock>& block, int blockType)
:m_beamUid(beamUid), m_block(block), m_blockType(blockType)
{
}

BeamBlockUpdateCmd::~BeamBlockUpdateCmd(void)
{
}


int BeamBlockUpdateCmd::Execute()
{
    if (m_beamUid.empty())
    {
        return TPS_ER_FAILURE;
    }
    TpsBeamManager* pBeamManager = mDataRepository->GetBeamManager();
    if (pBeamManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to find beam manager";
        return TPS_ER_FAILURE;
    }
    RtBeam* pBeam = pBeamManager->GetBeam(m_beamUid);
    if (pBeam == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam: "<< m_beamUid;
        return TPS_ER_FAILURE;
    }

    RtBlock* pBlock = nullptr;

    if (m_blockType == 0)
    {
        pBlock = pBeam->get_aperture_block();
        pBlock->set_blocktype(RtDbDef::APERTURE);
    }
    else
    {
        pBlock = pBeam->get_shield_block();
        pBlock->set_blocktype(RtDbDef::SHIELDING);
    }

    pBlock->set_uid(m_block->mBlockUid);
    pBlock->set_blockname(m_block->mBlockName);
    std::string sMaterial = "";
    if (m_block->mMaterial == 0) sMaterial = "WOLFRAM";
    else sMaterial = "LEAD";
    pBlock->set_materialid(sMaterial);
    pBlock->set_blockthickness(m_block->mThickness);
    std::vector<db_Point2f> blockPtList;
    tps::db_Point2f blockPt;
    for (int i = 0; i < m_block->mBlockData.size(); i++)
    {
        blockPt = tps::db_Point2f(m_block->mBlockData[i].GetX(), m_block->mBlockData[i].GetY());
        blockPtList.push_back(blockPt);
    }
    pBlock->set_points(blockPtList);

    if (!mDoseProxy->UpdateFluenceMap(pBeam))
    {
        TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
        return false;
    }

    if (nullptr != pBlock)
    {
        int iRet = mSaveObjects->SaveBeamBlock(*pBlock);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to update beam block.";
            return iRet;
        }
    }

    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
