//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file beam_converter.h
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_BEAM_CONVERTER_H_
#define TPS_DP_BEAM_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC BeamConverter
{
public:
    static bool ConvertToALG(const RtBeam& tpsBeam, std::shared_ptr<RTFWK::RTFWKDLDOBeam>* algBeam);

    static bool ConvertToTPS(RTFWK::RTFWKDLDOBeam& algBeam, RtBeam* tpsBeam);

    static bool CreateALGBeamList(
        std::vector<RtBeam*>& tpsBeamList,
        std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>>* algBeamList);

    static bool ConvertToTPSBeamList(
        std::vector<RTFWK::RTFWKDLDOBeam*>& algBeamList,
        std::vector<RtBeam*>* tpsBeamList);

    static bool Convert(RTFWK::RTFWKDLDOBeam* source, RtBeam* target);
    static bool ConvertRange(const std::vector<RTFWK::RTFWKDLDOBeam*>& source, std::vector<RtBeam*>& target);

    static ITpsDataRepository          *mDataRepository;
};
TPS_END_NAMESPACE
#endif