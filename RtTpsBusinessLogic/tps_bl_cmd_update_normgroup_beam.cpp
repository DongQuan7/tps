////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Zhednong zhendong.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_update_normgroup_beam.h
/// 
///  \brief class for update normgroup beam
/// 
///  \version 1.0
/// 
///  \date    June.12,2017
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_update_normgroup_beam.h"

#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h" 
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsProtoLib/rt_tps_proto_beam_operate.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"



TPS_BEGIN_NAMESPACE;   // begin namespace tps

UpdateNormgroupBeamCmd::UpdateNormgroupBeamCmd(std::vector<TpsCertifiedBeam*> certifyBeamList,
    proto::RtTpsProtoRepeatedBeamOperate beamOperateList) :
    mCertifiedBeamList(certifyBeamList), mBeamOperateList(beamOperateList)
{

}



UpdateNormgroupBeamCmd::~UpdateNormgroupBeamCmd()
{

}

int UpdateNormgroupBeamCmd::Execute()
{
    if (mCertifiedBeamList.size() != mBeamOperateList.beamoperatelist_size())
    {
        return TPS_ER_FAILURE;
    }

    if (mCertifiedBeamList.size() == 0)
    {
        return TPS_ER_SUCCESS;
    }

    std::string sNormgroupUid = mCertifiedBeamList[0]->mNormGroupUID;

    RtNormgroup* pNg = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(sNormgroupUid, &pNg);

    if (nullptr == pNg)
    {
        TPS_LOG_DEV_ERROR << "normgroup is null.";
        return TPS_ER_FAILURE;
    }

    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(pNg->get_machineuid(), machine))
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine[" << pNg->get_machineuid() << "]";
        return TPS_ER_FAILURE;
    }

    createBeamCmd.SetDataRepository(mDataRepository);
    createBeamCmd.SetRenderProxy(mRenderProxy);
    createBeamCmd.SetDatabaseWrapper(mDatabaseWrapper);
    createBeamCmd.SetSaveObjects(mSaveObjects);
    createBeamCmd.SetLoadObjects(mLoadObjects);
    createBeamCmd.SetDoseProxy(mDoseProxy);

    int size = mCertifiedBeamList.size();

    int iRet;

    for (int i = 0; i < size; i++)
    {
        std::string beamUid = mCertifiedBeamList[i]->mBeamUID;

        RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(beamUid);

        for (int j = 0; j < mBeamOperateList.beamoperatelist_size(); j++)
        {
            proto::RtTpsProtoBeamOperate beamOperate = mBeamOperateList.beamoperatelist(j);
            if (beamOperate.beam_uid() != beamUid) continue;

            if (beamOperate.beam_operate_type() == (int)RtDbDef::BEAM_OPERATE_DELETE)
            {
                iRet = DeleteBeam_i(beamUid);
                if (iRet != TPS_ER_SUCCESS)
                {
                    TPS_LOG_DEV_ERROR << "failed to delete beam:" << beamUid;
                    return iRet;
                }

                mRenderProxy->DeleteBeam(mCertifiedBeamList[i]->mNormGroupUID, beamUid);
            }
            else if (beamOperate.beam_operate_type() == (int)RtDbDef::BEAM_OPERATE_MODIFY)
            {
                bool segmentModifyFlag = false;
                for (int index = 0; index < beamOperate.beam_modify_typelist_size(); index++)
                {
                    int modifyType = beamOperate.beam_modify_typelist(index);

                    if (modifyType == (int)RtDbDef::SEGMENT)
                    {
                        segmentModifyFlag = true;
                    }
                }

                if (!segmentModifyFlag)
                {
                    ClearBeamAttachment_i(beam);
                    UpdateBeamCommissionedUnit_i(machine, pNg->get_radiationtype(), pNg->get_plantype(), beam);
                }
                else
                {
                    RtPoi* isoCenter = nullptr;
                    isoCenter = mDataRepository->GetPoiManager()->GetPOI(beam->get_isocenterpoiuid());
                    if (nullptr == isoCenter)
                    {
                        TPS_LOG_DEV_ERROR << "The iso center does not exist which uid: " << beam->get_isocenterpoiuid();
                        return TPS_ER_FAILURE;
                    }

                    Mcsf::Point3f ptIsoCenter(
                        isoCenter->get_coordinate3dx(),
                        isoCenter->get_coordinate3dy(),
                        isoCenter->get_coordinate3dz());

                    const std::string imageUid = GetImageUidViaBeam_i(beam);

                    ClearCertifyBeamAttachment_i(mCertifiedBeamList[i]);

                    createBeamCmd.SetCertifiedBeam(*mCertifiedBeamList[i]);

                    beam->set_beamtype(TpsCmdHelper::GetBeamTypeViaPlanType((RtDbDef::PLAN_TYPE)pNg->get_plantype()));

                    createBeamCmd.SetJawBoundary(machine, mCertifiedBeamList[i]->mPhysicalWedgeUid, mCertifiedBeamList[i]->mVirtualWedgeUid);

                    ClearBeamAttachment_i(beam);

                    // IMRT and ARC
                    if (!createBeamCmd.CreateBeamSegments(imageUid, beam, (RtDbDef::PLAN_TYPE)pNg->get_plantype(), machine, isoCenter))
                    {
                        TPS_LOG_DEV_ERROR << "Failed to create segments for beam[" << beam->get_uid() << "].";
                        return TPS_ER_FAILURE;
                    }

                    //update FluenceMap
                    if (!mDoseProxy->UpdateFluenceMap(beam))
                    {
                        TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
                        return false;
                    }
                }

                if (TPS_ER_SUCCESS != mSaveObjects->SaveBeam(beamUid))
                {
                    TPS_LOG_DEV_ERROR << "Failed to save beam in database.";
                    return TPS_ER_FAILURE;
                }

                mRenderProxy->UpdateBeam(sNormgroupUid, beamUid);
            }
        }
    }

    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

bool UpdateNormgroupBeamCmd::UpdateBeamCommissionedUnit_i(
    RtMachine* machine,
    int radiationType,
    int techniqueType,
    RtBeam* beam)
{
    RtCommissionedunit* commissionUnit = nullptr;
    if (!mDataRepository->GetMachineManager()->GetCommissionedUnit(beam->get_commissionedunituid(), commissionUnit))
    {
        TPS_LOG_DEV_ERROR << "The commissioned unit does not exist.";
        return false;
    }

    std::vector<RtCommissionedunit*> vCommissionedunitList;
    mDataRepository->GetMachineManager()->GetSpecifiedCommissonedUnit(machine->get_uid(), radiationType, techniqueType, vCommissionedunitList);

    std::vector<RtCommissionedunit*>::iterator itr;
    itr = std::find_if(vCommissionedunitList.begin(), vCommissionedunitList.end(),
        std::bind(TpsCmdHelper::CommissionedUnitComparision, std::placeholders::_1, commissionUnit));

    if (itr == vCommissionedunitList.end())
    {
        return false;
    }
    else
    {
        beam->set_commissionedunituid((*itr)->get_uid());

        std::vector<float> doserateList = (*itr)->get_discretedoserate();
        vector<float>::iterator itDoseRateList;
        itDoseRateList = std::find(doserateList.begin(), doserateList.end(), beam->get_doserate());
        if (itDoseRateList == doserateList.end())
        {
            beam->set_doserate(doserateList[0]);
        }

        std::vector<int> algorithmTypeList;
        if ((*itr)->get_ispencilebeamsupport())
        {
            algorithmTypeList.push_back(1);
        }
        if ((*itr)->get_isconvolutionsupport())
        {
            algorithmTypeList.push_back(2);
        }
        if ((*itr)->get_ismontecarlosupport())
        {
            algorithmTypeList.push_back(3);
        }

        vector<int>::iterator itAlgorithmTypeList;
        itAlgorithmTypeList = std::find(algorithmTypeList.begin(), algorithmTypeList.end(), beam->get_dosealgorithmtype());
        if (itAlgorithmTypeList == algorithmTypeList.end())
        {
            beam->set_dosealgorithmtype(algorithmTypeList[0]);
        }
    }

    return true;

}

void UpdateNormgroupBeamCmd::ClearBeamAttachment_i(RtBeam* beam)
{
    if (nullptr == beam) return;

    // clear block wedge bolus tray
    RtBlock* shieldBlock = beam->get_shield_block();
    if (shieldBlock)
    {
        mSaveObjects->EraseObject(shieldBlock->get_uid(), RtDbDef::TYPE_BLOCK);
        beam->remove_shield_block();
    }

    RtBlock* apertureBlock = beam->get_aperture_block();
    if (apertureBlock && (RtDbDef::APERTURE_TYPE)beam->get_aperturetype() == RtDbDef::Aperture_Block)
    {
        mSaveObjects->EraseObject(apertureBlock->get_uid(), RtDbDef::TYPE_BLOCK);
        beam->remove_aperture_block();
    }

    beam->set_aperturetype((int)RtDbDef::Mlc);
    beam->set_physicalwedgeuid("");
    beam->set_virtualwedgeuid("");
    beam->set_bolusvoiuid("");
    beam->set_trayuid("");
    beam->set_targetvoiuid("");
    beam->set_targetmarginx(0.f);
    beam->set_targetmarginy(0.f);
    beam->set_avoidvoiuid("");
    beam->set_avoidmarginx(0.f);
    beam->set_avoidmarginy(0.f);
}

int UpdateNormgroupBeamCmd::DeleteBeam_i(const std::string& beamUid)
{
    if (beamUid.empty())
    {
        TPS_LOG_DEV_ERROR << "beam uid is empty.";
        return TPS_ER_FAILURE;
    }

    int iRet = mSaveObjects->EraseObject(beamUid, RtDbDef::TYPE_BEAM);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "failed to delete beam in database:" << beamUid;
        return iRet;
    }

    int retCode = (TPS_ERROR_CODES)mDataRepository->GetBeamManager()->DeleteBeam(beamUid);
    if (retCode != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "failed to delete beam:" << beamUid << " from BeamManager";
        return retCode;
    }

    return TPS_ER_SUCCESS;
}

std::string UpdateNormgroupBeamCmd::GetImageUidViaBeam_i(RtBeam* beam)
{
    std::string imageUid = "";

    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(beam->get_normgroupuid(), &normGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to get plan.";
        return imageUid;
    }

    RtPlan* plan = nullptr;
    const std::string sPlanUid = normGroup->get_planuid();
    mDataRepository->GetPatientManager()->GetPlan(sPlanUid, &plan);
    if (nullptr == plan)
    {
        TPS_LOG_DEV_ERROR << "The plan does not exist.";
        return imageUid;
    }

    imageUid = plan->get_imageseriesuid();

    return imageUid;
}

void UpdateNormgroupBeamCmd::ClearCertifyBeamAttachment_i(TpsCertifiedBeam* certifyBeam)
{
    certifyBeam->mApertureType = 0;
    certifyBeam->mUseShieldingBlock = false;
    certifyBeam->mShieldingBlockTargetVoiUid = "";
    certifyBeam->mShieldingBlockAvoidVoiUid = "";
    certifyBeam->mShieldingBlockTargetMarginX = 0;
    certifyBeam->mShieldingBlockTargetMarginY = 0;
    certifyBeam->mShieldingBlockAvoidMarginX = 0;
    certifyBeam->mShieldingBlockAvoidMarginY = 0;
    certifyBeam->mUseVirtualWedge = false;
    certifyBeam->mVirtualWedgeUid = "";
    certifyBeam->mUsePhysicalWedge = false;
    certifyBeam->mPhysicalWedgeUid = "";
    certifyBeam->mTrayUid = "";
    certifyBeam->mBolusVoiUid = "";
    certifyBeam->mApplicatorUid = "";
    certifyBeam->mUseEBeamBlock = false;

    certifyBeam->mTargetVoiUid = "";
    certifyBeam->mTargetMarginX = 0.0;
    certifyBeam->mTargetMarginY = 0.0;
    certifyBeam->mAvoidVoiUid = "";
    certifyBeam->mAvoidMarginX = 0.0;
    certifyBeam->mAvoidMarginY = 0.0;
}





TPS_END_NAMESPACE  // end namespace tps
