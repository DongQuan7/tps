//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file segment_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/18
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "segment_converter.h"
#include "mlc_shape_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"

TPS_BEGIN_NAMESPACE

bool SegmentConverter::ConvertToALG(const RtBeamsegment* source, RTFWK::RTFWKDLDOBeamsegment* target)
{
    if (source == nullptr || target == nullptr)
    {
        return false;
    }

    target->SetUID(source->get_uid());
    target->SetBeamUID(source->get_beamuid());
    target->SetSequenceNumber(source->get_sequencenumber());
    target->SetGantryRotationDirection((RTFWK::ROTATION_DIRECTION)source->get_gantryrotationdirection());
    target->SetCollimatorRotationDirection((RTFWK::ROTATION_DIRECTION)source->get_collimatorrotationdirection());
    target->SetStartCollimatorAngle(source->get_startcollimatorangle());
    target->SetStartGantryAngle(source->get_startgantryangle());
    //wzd fix dim 44691 打开注释
    target->SetStartTableRotationAngle(source->get_starttablerotationangle()); //rcc: 因为本地TMS较老，暂时注掉
    RTFWK::RTFWKDLDOMLCShape* algStartMlcShape = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDOMLCShape> algStartMlcShapeCarrier;
    MlcShapeConverter::ConvertToALG(*(source->get_startmlcshape()), &algStartMlcShapeCarrier);
    algStartMlcShape = algStartMlcShapeCarrier.get();
    target->SetStartMLCShape(algStartMlcShape);
    target->SetEndCollimatorAngle(source->get_endcollimatorangle());

    //对于超过360度单弧的支持，Simple Arc只存在一个Segment，会出现一个segment超过360度的情况。
    //在算法组数据结构不修改的前提下，如果Arclength超过360，我们将传递超过[0，360]区间的EndAngle给算法组。 GML
    float startGantryAngle = source->get_startgantryangle();
    float arcLength = source->get_arclength();
    bool isCW = source->get_gantryrotationdirection() == RtDbDef::CW;
    int sign = isCW ? 1 : -1;
    float endGantryAngle = startGantryAngle + sign * arcLength;
    if (arcLength < 360)
    {
        endGantryAngle = TpsUtility::ToClockPosition(endGantryAngle);
    }
    target->SetEndGantryAngle(endGantryAngle);

     //wzd fix dim 44691 打开注释
    target->SetEndTableRotationAngle(source->get_endtablerotationangle()); //rcc: 因为本地TMS较老，暂时注掉
    RTFWK::RTFWKDLDOMLCShape* algEndMlcShape = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDOMLCShape> algEndMlcShapeCarrier;
    MlcShapeConverter::ConvertToALG(*(source->get_endmlcshape()), &algEndMlcShapeCarrier);
    algEndMlcShape = algEndMlcShapeCarrier.get();
    target->SetEndMLCShape(algEndMlcShape);
    Mcsf::Matrix4f matB2P = TpsArithmeticConverter::ConvertToMatrix4f(source->get_t_beam_to_pat());
    target->Set_beam_to_pat(matB2P);
    target->SetWeight(source->get_weight());
    target->SetMU(source->get_mu());

    return true;
}

bool SegmentConverter::ConvertToTPS(RtBeamsegment* tpsSegment, RTFWK::RTFWKDLDOBeamsegment& algSegment)
{
    tpsSegment->set_uid(algSegment.GetUID());
    tpsSegment->set_mu(algSegment.GetMU());
    tpsSegment->set_weight(algSegment.GetWeight());
    tpsSegment->set_beamuid(algSegment.GetBeamUID());
    tpsSegment->set_sequencenumber(algSegment.GetSequenceNumber());
    tpsSegment->set_gantryrotationdirection(algSegment.GetGantryRotationDirection());
    tpsSegment->set_collimatorrotationdirection(algSegment.GetCollimatorRotationDirection());
    RTFWK::RTFWKDLDOMachineAngle* startMachineAngle = algSegment.GetStartMachineAngle();
    tpsSegment->set_startcollimatorangle(startMachineAngle->GetCollimatorAngle());
    tpsSegment->set_startgantryangle(startMachineAngle->GetGantryAngle());
    tpsSegment->set_starttablerotationangle(startMachineAngle->GetTableRotationAngle());
    MlcShapeConverter::ConvertToTPS(*algSegment.GetStartMLCShape(), tpsSegment->get_startmlcshape());
    RTFWK::RTFWKDLDOMachineAngle* endMachineAngle = algSegment.GetEndMachineAngle();
    tpsSegment->set_endcollimatorangle(endMachineAngle->GetCollimatorAngle());
    float startGantryAngle = startMachineAngle->GetGantryAngle();
    float endGantryAngle = endMachineAngle->GetGantryAngle();
    bool isCW = tpsSegment->get_gantryrotationdirection() == RtDbDef::CW;
    int sign = isCW ? 1 : -1;
    float arcLength;
    //单弧超过360
    if (endGantryAngle < 0.f || endGantryAngle >= 360.f)
    {
        arcLength = fabs(endGantryAngle - startGantryAngle);
    }
    else
    {
        arcLength = TpsUtility::ToClockPosition(sign * (endGantryAngle - startGantryAngle));
    }
    tpsSegment->set_arclength(arcLength);
    tpsSegment->set_endtablerotationangle(endMachineAngle->GetTableRotationAngle());
    Mcsf::Matrix4f matB2P = algSegment.Get_beam_to_pat();
    float b2p[16];
    TpsArithmeticConverter::ConvertToFloat16(matB2P, b2p);
    tpsSegment->set_t_beam_to_pat(b2p);

    MlcShapeConverter::ConvertToTPS(*algSegment.GetEndMLCShape(), tpsSegment->get_endmlcshape());
    return true;
}

bool SegmentConverter::CreateALGSegmentList(std::vector<RtBeamsegment*>& source, std::vector<RTFWK::RTFWKDLDOBeamsegment*>& target)
{
    RTFWK::RTFWKDLDOBeamsegment* algSegment = nullptr;
    for (auto it = source.begin(); it != source.end(); ++it)
    {
        algSegment = new RTFWK::RTFWKDLDOBeamsegment();
        if (!SegmentConverter::ConvertToALG(*it, algSegment))
        {
            delete algSegment;
            algSegment = nullptr;
            return false;
        }

        target.push_back(algSegment);
    }
    return true;
}

bool SegmentConverter::ConvertToTPSSegmentList(std::vector<RtBeamsegment*>* tpsSegmentList,
    std::vector<RTFWK::RTFWKDLDOBeamsegment*>& algSegmentList)
{

    tpsSegmentList->clear();
    std::vector<RTFWK::RTFWKDLDOBeamsegment*>::iterator itAlgSegment = algSegmentList.begin();
    for (; itAlgSegment != algSegmentList.end(); ++itAlgSegment)
    {
        RtBeamsegment* tpsSegment = new RtBeamsegment();
        RTFWK::RTFWKDLDOBeamsegment* algSegment = *itAlgSegment;
        SegmentConverter::ConvertToTPS(tpsSegment, *algSegment);
        tpsSegmentList->push_back(tpsSegment);
    }
    return true;
}

bool SegmentConverter::Convert(RTFWK::RTFWKDLDOBeamsegment* source, RtBeamsegment* target)
{
    if (source == nullptr || target == nullptr)
    {
        return false;
    }

    target->set_mu(source->GetMU());
    target->set_weight(source->GetWeight());

    return true;
}

bool SegmentConverter::ConvertRange(std::vector<RTFWK::RTFWKDLDOBeamsegment*>& source, std::vector<RtBeamsegment*>& target)
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
