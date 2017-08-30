//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file beam_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "beam_converter.h"

#include "segment_converter.h"
#include "dosegrid_converter.h"
#include "voi_converter.h"
#include "block_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

ITpsDataRepository* BeamConverter::mDataRepository = nullptr;

bool BeamConverter::ConvertToALG(const RtBeam& tpsBeam, std::shared_ptr<RTFWK::RTFWKDLDOBeam>* algBeam)
{
    algBeam->reset(new RTFWK::RTFWKDLDOBeam(false));
    (*algBeam)->SetUID(tpsBeam.get_uid());
    (*algBeam)->SetDBUID(tpsBeam.get_uid());
    (*algBeam)->SetTrayUID(tpsBeam.get_trayuid());
    (*algBeam)->SetDescription(tpsBeam.get_description());
    (*algBeam)->SetName(tpsBeam.get_name());
    (*algBeam)->SetBeamDeliveryType((RTFWK::BEAM_DELIVERY_TYPE)tpsBeam.get_beamdeliverytype());
    (*algBeam)->SetApertureType((RTFWK::APERTURE_TYPE)tpsBeam.get_aperturetype());
    (*algBeam)->SetBeamType((RTFWK::BEAM_TYPE)tpsBeam.get_beamtype());
    (*algBeam)->SetBeamNumber(tpsBeam.get_beamnumber());
    (*algBeam)->SetBeamWeight(tpsBeam.get_beamweight());
    (*algBeam)->SetBeamMU(tpsBeam.get_beammu());
    (*algBeam)->SetIsVisible(tpsBeam.get_isvisible());
    (*algBeam)->SetBeamMode(tpsBeam.get_beammode());
    (*algBeam)->SetCommissionedunitUID(tpsBeam.get_commissionedunituid());
    (*algBeam)->SetFinalCumulativeMetersetWeight(tpsBeam.get_finalcumulativemetersetweight());
    // (*algBeam)->SetIsJoinCalc(tpsBeam.get_isjoincalc());
    // CX:剂量计算时所有BEAM都强制计算，IsJoinCalc表示参不参与剂量贡献（是否参与组装Dose）
    (*algBeam)->SetIsJoinCalc(true);
    (*algBeam)->SetIsJoinOpt(tpsBeam.get_isjoinopt());
    (*algBeam)->SetISOCenterPOIUID(tpsBeam.get_isocenterpoiuid());
    (*algBeam)->SetNormGroupUID(tpsBeam.get_normgroupuid());
    (*algBeam)->SetIsJawFollowed(tpsBeam.get_jawtype() == RtDbDef::JAWTYPE_Follow);
    (*algBeam)->SetIsAutoShape(tpsBeam.get_isautoshape());
    (*algBeam)->SetJawMarginX(tpsBeam.get_jawmarginx());
    (*algBeam)->SetJawMarginY(tpsBeam.get_jawmarginy());
    (*algBeam)->SetDoseAlgorithmType((RTFWK::DOSEALGORITHM_TYPE)tpsBeam.get_dosealgorithmtype());
    (*algBeam)->SetBolusVOIUID(tpsBeam.get_bolusvoiuid());
    (*algBeam)->SetBolusThickness(tpsBeam.get_bolusthickness());
    (*algBeam)->SetNormPointBeamDose(tpsBeam.get_normpointbeamdose());
    (*algBeam)->SetApplicatorUID(tpsBeam.get_applicatoruid());
    (*algBeam)->SetToleranceUID(tpsBeam.get_toleranceuid());
    (*algBeam)->SetSSD(tpsBeam.get_ssd());
    (*algBeam)->SetDoseRate(tpsBeam.get_doserate());
    (*algBeam)->SetRelatedRTImageUID(tpsBeam.get_relatedrtimageuid());
    (*algBeam)->SetPhysicalWedgeUID(tpsBeam.get_physicalwedgeuid());
    (*algBeam)->SetVirtualWedgeUID(tpsBeam.get_virtualwedgeuid());
    // bolus
    if (!tpsBeam.get_bolusvoiuid().empty())
    {
        std::string bolusVoiUid = tpsBeam.get_bolusvoiuid();
        ROIEntity* pBolusVoi = nullptr;
        if (mDataRepository != nullptr)
        {
            pBolusVoi = mDataRepository->GetVoiManager()->GetROI(bolusVoiUid);
            if (pBolusVoi != nullptr)
            {
                RtVoi* pTpsBolus = pBolusVoi->GetInnerRoi();
                std::shared_ptr<RTFWK::RTFWKDLDOVOI> algBolus;
                VoiConverter::ConvertToALG(*pTpsBolus, &algBolus);
                (*algBeam)->SetBolus(algBolus.get());
            }
        }
    }
    //accessory
    std::vector<std::string> accessoryUIDList;
    if (!tpsBeam.get_physicalwedgeuid().empty())
    {
        accessoryUIDList.push_back(tpsBeam.get_physicalwedgeuid());
    }
    if (!tpsBeam.get_virtualwedgeuid().empty())
    {
        accessoryUIDList.push_back(tpsBeam.get_virtualwedgeuid());
    }
    if (accessoryUIDList.size() > 0)
    {
        (*algBeam)->SetRelateAccessoryUIDList(accessoryUIDList);
    }
    // segment
    std::vector<RTFWK::RTFWKDLDOBeamsegment*> algSegmentList;
    std::vector<RtBeamsegment*> tpsSegmentList = tpsBeam.get_beamsegments();
    if (SegmentConverter::CreateALGSegmentList(tpsSegmentList, algSegmentList))
    {
        (*algBeam)->SetSegments(algSegmentList);
    }

    //dosegrid
    RtDosegrid* tpsDosegrid = tpsBeam.get_dosegrid();
    RTFWK::RTFWKDLDODoseGrid* algDosegrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*tpsDosegrid, &doseGridCarrier);
    algDosegrid = doseGridCarrier.get();
    (*algBeam)->SetDoseGrid(*algDosegrid);

    //block
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBlock>> algBlockCarrier;
    std::vector<RtBlock*> blockList;
    if (tpsBeam.get_aperturetype() == RtDbDef::Aperture_Block)
    {
        RtBlock* apertureBlock = tpsBeam.get_aperture_block();
        blockList.push_back(apertureBlock);
    }
    if (nullptr != tpsBeam.get_shield_block())
    {
        RtBlock* shieldBlock = tpsBeam.get_shield_block();
        blockList.push_back(shieldBlock);
    }
    if (!blockList.empty())
    {
        bool bRet = BlockConverter::CreateALGBlockList(blockList, &algBlockCarrier);
        if (!bRet)
        {
            TPS_LOG_DEV_ERROR << "BlockConverter::CreateALGBlockList failed.";
            return false;
        }
        std::vector<RTFWK::RTFWKDLDOBlock*> algBlockList;
        for (int i = 0; i < algBlockCarrier.size(); ++i) 
        {
            algBlockList.push_back(algBlockCarrier[i].get());
        }
        if (!algBlockList.empty())
        {
            (*algBeam)->SetBlockList(algBlockList);
            //TODO: 等算法组把该接口删除后 需要删除下面代码
            (*algBeam)->SetBlock(algBlockList[0]);
        }
    }

    // beamID,courseuid数据库没有，color暂时没传

    return true;
}

bool BeamConverter::ConvertToTPS(RTFWK::RTFWKDLDOBeam& algBeam, RtBeam* tpsBeam)
{
    tpsBeam->set_uid(algBeam.GetUID());
    tpsBeam->set_beammu(algBeam.GetBeamMU());
    tpsBeam->set_beamweight(algBeam.GetBeamWeight());
    tpsBeam->set_beamtype((int)algBeam.GetBeamType());
    tpsBeam->set_beammode(algBeam.GetBeamMode());
    tpsBeam->set_isautoshape(algBeam.GetIsAutoShape());
    tpsBeam->set_jawmarginx(algBeam.GetJawMarginX());
    tpsBeam->set_jawmarginy(algBeam.GetJawMarginY());
    tpsBeam->set_jawtype(algBeam.GetIsJawFollowed()? RtDbDef::JAWTYPE_Follow : RtDbDef::JAWTYPE_Manual);
    tpsBeam->set_dosealgorithmtype((int)algBeam.GetDoseAlgorithmType());

    std::vector<RtBeamsegment*> tpsSegmentList = tpsBeam->get_beamsegments();
    std::vector<RTFWK::RTFWKDLDOBeamsegment*> algSegmentList = algBeam.GetSegments();
    SegmentConverter::ConvertToTPSSegmentList(&tpsSegmentList, algSegmentList);
    tpsBeam->set_beamsegments(tpsSegmentList);
    // set_beamsegments深拷贝 所以tpsSegmentList需要释放 wzd-2016-11-21
    std::vector<RtBeamsegment*>::iterator itTpsSegment = tpsSegmentList.begin();
    for (; itTpsSegment != tpsSegmentList.end(); ++itTpsSegment)
    {
        DEL_PTR(*itTpsSegment);
    }
    RtDosegrid* tpsDosegrid = tpsBeam->get_dosegrid();
    RTFWK::RTFWKDLDODoseGrid algDosegrid = algBeam.GetDoseGrid();
    DosegridConverter::ConvertToTPS(algDosegrid, tpsDosegrid);

    return true;
}

bool BeamConverter::CreateALGBeamList(
    std::vector<RtBeam*>& tpsBeamList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>>* algBeamList)
{
    std::vector<RtBeam*>::iterator itTpsBeam = tpsBeamList.begin();
    for (; itTpsBeam != tpsBeamList.end(); ++itTpsBeam)
    {
        RtBeam* tpsBeam = *itTpsBeam;
        std::shared_ptr<RTFWK::RTFWKDLDOBeam> algBeam;
        BeamConverter::ConvertToALG(*tpsBeam, &algBeam);
        algBeamList->push_back(algBeam);
    }
    return true;
}

bool BeamConverter::ConvertToTPSBeamList(
    std::vector<RTFWK::RTFWKDLDOBeam*>& algBeamList,
    std::vector<RtBeam*>* tpsBeamList)
{
    std::vector<RtBeam*>::iterator itBeam = tpsBeamList->begin();
    for (; itBeam != tpsBeamList->end(); ++itBeam)
    {
        RtBeam* tpsBeam = *itBeam;
        std::vector<RTFWK::RTFWKDLDOBeam*>::iterator itAlgBeam = algBeamList.begin();
        for (; itAlgBeam != algBeamList.end(); ++itAlgBeam)
        {
            RTFWK::RTFWKDLDOBeam* algBeam = *itAlgBeam;
            if (tpsBeam->get_uid() == algBeam->GetUID())
            {
                BeamConverter::ConvertToTPS(*algBeam, tpsBeam);
                break;
            }
        }
    }
    return true;
}

bool BeamConverter::Convert(RTFWK::RTFWKDLDOBeam* source, RtBeam* target)
{
    if (source == nullptr || target == nullptr)
    {
        return false;
    }

    target->set_beammu(source->GetBeamMU());
    target->set_beamweight(source->GetBeamWeight());

    auto algSegments = source->GetSegments();
    auto segments = target->get_beamsegments();

    if (!SegmentConverter::ConvertRange(algSegments, segments))
    {
        return false;
    }

    auto dosegrid = target->get_dosegrid();
    auto algDosegrid = source->GetDoseGrid();
    if (!DosegridConverter::ConvertToTPS(algDosegrid, dosegrid))
    {
        return false;
    }

    return true;
}

bool BeamConverter::ConvertRange(const std::vector<RTFWK::RTFWKDLDOBeam*>& source, std::vector<RtBeam*>& target)
{
    for (int i = 0; i < source.size(); i++)
    {
        int j = 0;
        for (; j < target.size(); j++)
        {
            if (source[i]->GetUID() == target[j]->get_uid())
            {
                if (!Convert(source[i], target[j]))
                {
                    return false;
                }
                break;
            }
        }
    }

    return true;
}
TPS_END_NAMESPACE