////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_ap_mlc_jaw_limitation_helper.h
/// 
///  \brief class TpsMlcJawLimitationHelper declaration 
/// 
///  \version 1.0
/// 
///  \date    Jan.6 ,2016
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_VOI_HELPER_H_
#define TPS_AP_VOI_HELPER_H_

#include "tps_ap_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"

TPS_BEGIN_NAMESPACE  // begin namespace TPS

namespace plunc
{
    struct LEAF_POSITION_STC;
}
class RtMachine;
class RtMlcshape;

class TPS_AP_DECLSPEC TpsMlcJawLimitationHelper
{
public:
    static void AdjustMlcJawGivenMachine(
        const RtMachine* pMachine,
        plunc::LEAF_POSITION_STC* pLeaf,
        int nLeaf,
        bool bJawFollow,
        float fJawMarginX,
        float fJawMarginY,
        float pJawPosition[4]);

    static void JawFollowMlc(
        const RtMachine* pMachine,
        double gapOffset,
        RtMlcshape* pShape,
        float fJawMarginX,
        float fJawMarginY);

private:

    static void TpsMlcJawLimitationHelper::CheckLeafInterdigitation_i(
        plunc::LEAF_POSITION_STC* pLeaf,
        int nLeaf,
        float fLeafMinGap);

    static float TpsMlcJawLimitationHelper::CalculateAverageLeafPosition_i(
        plunc::LEAF_POSITION_STC* pLeaf,
        int nLeaf,
        float fLeafMinGap);

    static float TpsMlcJawLimitationHelper::CalculateAverageLeafPosition_i(
        std::vector<db_Point2d> vecLeafPos,
        int nLeaf,
        float fLeafMinGap);

    static void TpsMlcJawLimitationHelper::CorrectMlcGapPosition_i(
        plunc::LEAF_POSITION_STC* pLeaf,
        int nLeaf,
        float fLeafMinGap,
        float x1Min,
        float x2Max,
        float fLeafSpan,
        float fAverageX,
        float fMinimumOverlap,
        float pJawPosition[4]);

    static void TpsMlcJawLimitationHelper::CorrectMlcGapPosition_i(
        std::vector<db_Point2d> &vecLeafPos,
         std::vector<double> leafBounds,
        int nLeaf,
        float fLeafMinGap,
        float x1Min,
        float x2Max,
        float fLeafSpan,
        float fAverageX,
        float fMinimumOverlap,
        float jawPos[4]);

    static bool CheckAllLeafClosed_i(plunc::LEAF_POSITION_STC* pLeaf, int nLeaf, float fLeafMinGap);

    static bool CheckAllLeafClosed_i(std::vector<db_Point2d> vecLeafPos, int nLeaf, float fLeafMinGap);



};

TPS_END_NAMESPACE  // end namespace TPS

#endif  //TPS_AP_VOI_HELPER_H_
