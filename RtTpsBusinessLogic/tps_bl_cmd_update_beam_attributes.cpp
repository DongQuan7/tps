////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_beam_display.cpp
/// 
///  \brief class for UPDATE_BEAM_VISIBILITY
/// 
///  \version 1.0
/// 
///  \date    Oct.19,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_update_beam_attributes.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamoperation.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE;

UpdateBeamAttributesCmd::UpdateBeamAttributesCmd(const proto::RtTpsProtoBeamUpdate &protoBeamUpdate, int editType)
    : mProtoBeamUpdate(protoBeamUpdate), mEditType(editType)
{
}

UpdateBeamAttributesCmd::~UpdateBeamAttributesCmd(void)
{

}

std::vector<RtBeamsegment*> UpdateBeamAttributesCmd::GetSegmentList()
{
     RtBeam* pBeam = mDataRepository->GetBeamManager()->GetBeam(mProtoBeamUpdate.beam_uid());
     return pBeam->get_beamsegments();
}

int UpdateBeamAttributesCmd::Execute()
{
    TPS_LOG_DEV_INFO << "[INFO][UpdateBeamAttributesCmd][Execute]: UpdateBeamAttributesCmd execute!";
    RtBeam* pBeam = mDataRepository->GetBeamManager()->GetBeam(mProtoBeamUpdate.beam_uid());
    if (nullptr == pBeam)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam from manager:" << mProtoBeamUpdate.beam_uid();
        return TPS_ER_FAILURE;
    }

    bool isNeedUpdateApertureBlock = false;
    bool isNeedUpdateShieldingBlock = false;

    if (mEditType & EDIT_BEAM_NAME)
    {
        if (mProtoBeamUpdate.has_beam_name())
        {
            pBeam->set_name(mProtoBeamUpdate.beam_name());
        }
        else
        {
            pBeam->disable_field(BEAM_NAME);
        }
    }
    if (mEditType & EDIT_BEAM_DESCRIPTION)
    {
        if (mProtoBeamUpdate.has_beam_description())
        {
            pBeam->set_description(mProtoBeamUpdate.beam_description());
        }
        else
        {
            pBeam->disable_field(BEAM_DESCRIPTION);
        }
    }
    if (mEditType & EDIT_BEAM_VISIBILITY)
    {
        if (mProtoBeamUpdate.has_isvisible())
        {
            pBeam->set_isvisible(mProtoBeamUpdate.isvisible());
        }
        else
        {
            pBeam->disable_field(BEAM_ISVISIBLE);
        }
    }
    if (mEditType & EDIT_BEAM_ISAUTOCREATED_DESCRIPTION)
    {
        if (mProtoBeamUpdate.has_isautocreateddescription())
        {
            pBeam->set_isautocreated(mProtoBeamUpdate.isautocreateddescription());
        }
        else
        {
            pBeam->disable_field(BEAM_ISAUTOCREATED);
        }
    }
    if (mEditType & EDIT_BEAM_AUTOTRACKING_SHAPE)
    {
        if (mProtoBeamUpdate.has_isautotrackingshape())
        {
            if (RtDbDef::Aperture_Block == pBeam->get_aperturetype())
            {
                RtBlock* pBlock = pBeam->get_aperture_block();
                if (nullptr == pBlock)
                {
                    TPS_LOG_DEV_ERROR << "no block";
                    return TPS_ER_FAILURE;
                }
                pBlock->set_isautoshape(mProtoBeamUpdate.isautotrackingshape());
                isNeedUpdateApertureBlock = true;
            }
            else
            {
                pBeam->set_isautoshape(mProtoBeamUpdate.isautotrackingshape());
            }
        }
        else
        {
            pBeam->disable_field(BEAM_ISAUTOSHAPE);
        }
    }
    if (mEditType & EDIT_BEAM_SHIELDINGBLOCK_AUTOTRACKINGSHAPE && pBeam->get_shield_block() != nullptr)
    {
        if (mProtoBeamUpdate.has_isshieldingblockautotrackingshape())
        {
            RtBlock* pBlock = pBeam->get_shield_block();
            if (nullptr == pBlock)
            {
                TPS_LOG_DEV_ERROR << "no block";
                return TPS_ER_FAILURE;
            }
            pBlock->set_isautoshape(mProtoBeamUpdate.isshieldingblockautotrackingshape());
            isNeedUpdateShieldingBlock = true;
        }
        else
        {
            pBeam->disable_field(BEAM_ISAUTOSHAPE);
        }
    }
    if (mEditType & EDIT_BEAM_APERTURE_JAWFOLLOW_INACTIVE)
    {
        if (mProtoBeamUpdate.has_isjawfollow())
        {
            pBeam->set_jawtype(mProtoBeamUpdate.isjawfollow() ? RtDbDef::JAWTYPE_Follow : RtDbDef::JAWTYPE_Manual);
        }
        else
        {
            pBeam->disable_field(BEAM_JAWTYPE);
        }
    }
    if (mEditType & EDIT_BEAM_NULL_TARGETVOI)
    {
        pBeam->set_targetvoiuid("");
        pBeam->set_targetmarginx(0.f);
        pBeam->set_targetmarginy(0.f);
        if (RtDbDef::Aperture_Block == pBeam->get_aperturetype())
        {
            RtBlock* pBlock = pBeam->get_aperture_block();
            if (nullptr == pBlock)
            {
                TPS_LOG_DEV_ERROR << "no block";
                return TPS_ER_FAILURE;
            }
            pBlock->set_targetvoiuid("");
            pBlock->set_targetmarginx(0.f);
            pBlock->set_targetmarginy(0.f);
            isNeedUpdateApertureBlock = true;
        }
    }
    if (mEditType & EDIT_BEAM_NULL_AVOIDVOI)
    {
        pBeam->set_avoidvoiuid("");
        pBeam->set_avoidmarginx(0.f);
        pBeam->set_avoidmarginy(0.f);
        if (RtDbDef::Aperture_Block ==pBeam->get_aperturetype())
        {
            RtBlock* pBlock = pBeam->get_aperture_block();
            if (nullptr == pBlock)
            {
                TPS_LOG_DEV_ERROR << "no block";
                return TPS_ER_FAILURE;
            }
            pBlock->set_avoidvoiuid("");
            pBlock->set_avoidmarginx(0.f);
            pBlock->set_avoidmarginy(0.f);
            isNeedUpdateApertureBlock = true;
        }
    }
    if (mEditType & EDIT_BEAM_SHIELDING_BLOCK_NULL_TARGETVOI)
    {
        RtBlock* pBlock = pBeam->get_shield_block();
        if (nullptr == pBlock)
        {
            TPS_LOG_DEV_ERROR << "no block";
            return TPS_ER_FAILURE;
        }
        pBlock->set_targetvoiuid("");
        pBlock->set_targetmarginx(0.f);
        pBlock->set_targetmarginy(0.f);
        isNeedUpdateShieldingBlock = true;
    }
    if (mEditType & EDIT_BEAM_SHIELDING_BLOCK_NULL_AVOIDVOI)
    {
        RtBlock* pBlock = pBeam->get_shield_block();
        if (nullptr == pBlock)
        {
            TPS_LOG_DEV_ERROR<<"no block";
            return TPS_ER_FAILURE;
        }
        pBlock->set_avoidvoiuid("");
        pBlock->set_avoidmarginx(0.f);
        pBlock->set_avoidmarginy(0.f);
        isNeedUpdateShieldingBlock = true;
    }
    if (mEditType & EDIT_BEAM_DOSE_RATE)
    {
        if (mProtoBeamUpdate.has_dose_rate())
        {
            pBeam->set_doserate(mProtoBeamUpdate.dose_rate());
        }
        else
        {
            pBeam->disable_field(BEAM_DOSERATE);
        }
    }
    if (mEditType & EDIT_BEAM_IS_JOINED)
    {
        if (mProtoBeamUpdate.has_isjoined())
        {
            pBeam->set_isjoincalc(mProtoBeamUpdate.isjoined());
            pBeam->set_isjoinopt(mProtoBeamUpdate.isjoined());
        }
        else
        {
            pBeam->disable_field(BEAM_ISJOINCALC);
            pBeam->disable_field(BEAM_ISJOINOPT);
        }
    }
    if (mEditType & EDIT_BEAM_ALGORITHM_TYPE)
    {
        if (mProtoBeamUpdate.has_algorithm_type())
        {
            pBeam->set_dosealgorithmtype(mProtoBeamUpdate.algorithm_type());
        } 
        else
        {
            pBeam->disable_field(BEAM_DOSEALGORITHMTYPE);
        }
    }
    if (mEditType & EDIT_BEAM_COMMISSIONUNIT)
    {
        if (mProtoBeamUpdate.has_commission_uid())
        {
            pBeam->set_commissionedunituid(mProtoBeamUpdate.commission_uid());
        }
        else
        {
            pBeam->disable_field(BEAM_COMMISSIONEDUNITUID);
        }
    }
    if (mEditType & EDIT_BEAM_MU)
    {
        std::vector<RtBeamsegment*> vecSegments;
        vecSegments = pBeam->get_beamsegments();
        float fTotalWeight = 0.f;
        for (size_t i = 0; i < vecSegments.size(); ++i)
        {
            fTotalWeight += vecSegments[i]->get_weight();
        }

        if (fTotalWeight <= 0.f)
        {
            TPS_LOG_DEV_ERROR << "Segment total weight is not equal 1.0" << mProtoBeamUpdate.beam_uid();
            return TPS_ER_FAILURE;
        }

        pBeam->set_beammu(mProtoBeamUpdate.beam_mu());

        for (size_t i = 0; i < vecSegments.size(); ++i)
        {
            vecSegments[i]->set_mu(mProtoBeamUpdate.beam_mu() * vecSegments[i]->get_weight() / fTotalWeight);
        }

    }
    if (mEditType & EDIT_BEAM_BEAMWEIGHT)
    {
        if (mProtoBeamUpdate.has_beam_weight())
        {
            pBeam->set_beamweight(mProtoBeamUpdate.beam_weight());
        }
        else
        {
            pBeam->disable_field(BEAM_BEAMWEIGHT);
        }
    }
    int iRet;
    if (isNeedUpdateApertureBlock)
    {
        RtBlock* pApertureBlock = pBeam->get_aperture_block();
        iRet = mDatabaseWrapper->UpdateBlock(*pApertureBlock);
        if (DB_CODES_SUCCESS  != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to update beam attributes:" << mProtoBeamUpdate.beam_uid();
            return iRet;
        }
    }

    if (isNeedUpdateShieldingBlock)
    {
        RtBlock* pShieldingBlock = pBeam->get_shield_block();
        iRet = mDatabaseWrapper->UpdateBlock(*pShieldingBlock);
        if (DB_CODES_SUCCESS  != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to update beam attributes:" << mProtoBeamUpdate.beam_uid();
            return iRet;
        }
    }

    iRet = mSaveObjects->UpdateBeamAttributes(*pBeam);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to update beam attributes:" << mProtoBeamUpdate.beam_uid();
        return iRet;
    }

    if (mEditType & EDIT_BEAM_MU)
    {
        std::vector<RtBeamsegment*> vecSegments = pBeam->get_beamsegments();
        iRet = mSaveObjects->UpdateBeamSegments(vecSegments);
        if (TPS_ER_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to UpdateBeamSegments:" << mProtoBeamUpdate.beam_uid();
            return iRet;
        }
    }

    // only edit BeamVisibility update render
    if (mEditType & EDIT_BEAM_VISIBILITY
        || mEditType & EDIT_BEAM_MU)
    {
        std::string ngUid = pBeam->get_normgroupuid();
        std::shared_ptr<TpsUpdateBeamOperation> 
            beamVisibilityUpdateOperation(new TpsUpdateBeamOperation(ngUid, mProtoBeamUpdate.beam_uid()));
        mRenderProxy->ProcessOperation(beamVisibilityUpdateOperation);

        std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
        mRenderProxy->ProcessOperation(fire);
    }

    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE
