////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_ap_mlc_jaw_limitation_helper.cpp
/// 
///  \brief class TpsMlcJawLimitationHelper 
/// 
///  \version 1.0
/// 
///  \date    Jan.6 ,2016
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_mlc_jaw_limitation_helper.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include <string.h>     // for memset, memcpy etc
#include <stdio.h>      // for printf etc

#include <math.h>


TPS_BEGIN_NAMESPACE

void TpsMlcJawLimitationHelper::CheckLeafInterdigitation_i(plunc::LEAF_POSITION_STC* pLeaf, int nLeaf, float fLeafMinGap)
{
    for (int i = 0; i < nLeaf - 1; i++)
    {
        if ((pLeaf[i].pos_2 - pLeaf[i].pos_1) < fLeafMinGap)
        {
            continue;
        }

        if (pLeaf[i + 1].pos_1 > pLeaf[i].pos_2 - fLeafMinGap)
        {
            pLeaf[i + 1].pos_1 = pLeaf[i].pos_2 - fLeafMinGap;
        }

        if (pLeaf[i + 1].pos_2 < pLeaf[i].pos_1 + fLeafMinGap)
        {
            pLeaf[i + 1].pos_2 = pLeaf[i].pos_1 + fLeafMinGap;
        }
    }
}

float TpsMlcJawLimitationHelper::CalculateAverageLeafPosition_i(plunc::LEAF_POSITION_STC* pLeaf, int nLeaf, float fLeafMinGap)
{
    float fAverageX = 0.0;
    int nAverageX = 0;
    for (int i = 0; i < nLeaf; i++)
    {
        if ((pLeaf[i].pos_2 - pLeaf[i].pos_1) < fLeafMinGap)
        {
            continue;
        }
        fAverageX += pLeaf[i].pos_1;
        fAverageX += pLeaf[i].pos_2;
        nAverageX += 2;
    }
    if (nAverageX > 0)
    {
        fAverageX /= nAverageX;
    }

    return fAverageX;
}

float TpsMlcJawLimitationHelper::CalculateAverageLeafPosition_i(std::vector<db_Point2d> vecLeafPos, int nLeaf, float fLeafMinGap)
{
    float fAverageX = 0.0;
    int nAverageX = 0;
    for (int i = 0; i < nLeaf; i++)
    {
        if (std::abs(vecLeafPos[i].y - vecLeafPos[i].x - fLeafMinGap) < 1e-4)
            continue;
        fAverageX += vecLeafPos[i].x;
        fAverageX += vecLeafPos[i].y;
        nAverageX += 2;
    }
    if (nAverageX > 0)
    {
        fAverageX /= nAverageX;
    }

    return fAverageX;
}

void TpsMlcJawLimitationHelper::CorrectMlcGapPosition_i(
    plunc::LEAF_POSITION_STC* pLeaf, 
    int nLeaf,
    float fLeafMinGap,
    float x1Min,
    float x2Max,
    float fLeafSpan,
    float fAverageX,
    float fMinimumOverlap,
    float pJawPosition[4])
{
    for (int i = 0; i < nLeaf; i++)
    {
        if (pLeaf[i].pos_2 - pLeaf[i].pos_1 < fLeafMinGap)
        {
            pLeaf[i].pos_1 = fAverageX - fLeafMinGap * 0.5;
            pLeaf[i].pos_2 = fAverageX + fLeafMinGap * 0.5;

            // overlapped by bottom Jaw or top Jaw
            if (pLeaf[i].max_w <= pJawPosition[2] || pLeaf[i].min_w >= pJawPosition[3])
            {
                continue;
            }

            bool bCanLeft = ((x2Max - fLeafSpan) < (pJawPosition[0] - fMinimumOverlap));
            bool bCanRight = ((x1Min + fLeafSpan) > (pJawPosition[1] + fMinimumOverlap));

            // adjust to left
            if (bCanLeft && !bCanRight)
            {
                pLeaf[i].pos_1 = pJawPosition[0] - fMinimumOverlap - fLeafMinGap;
                pLeaf[i].pos_2 = pJawPosition[0] - fMinimumOverlap;
            }
            // adjust to right
            else if(!bCanRight && bCanRight)
            {
                pLeaf[i].pos_1 = pJawPosition[1] + fMinimumOverlap;
                pLeaf[i].pos_2 = pJawPosition[1] + fMinimumOverlap + fLeafMinGap;
            }
            // both is ok
            else if (bCanLeft && bCanRight)
            {
                if (fAverageX > (pJawPosition[0] + pJawPosition[1]) * 0.5)
                {
                    // adjust to right
                    pLeaf[i].pos_1 = pJawPosition[1] + fMinimumOverlap;
                    pLeaf[i].pos_2 = pJawPosition[1] + fMinimumOverlap + fLeafMinGap;
                }
                else
                {
                    // adjust to left
                    pLeaf[i].pos_1 = pJawPosition[0] - fMinimumOverlap - fLeafMinGap;
                    pLeaf[i].pos_2 = pJawPosition[0] - fMinimumOverlap;
                }
            }
        }
    }
}

void TpsMlcJawLimitationHelper::CorrectMlcGapPosition_i(
    std::vector<db_Point2d> &vecLeafPos,
    std::vector<double> leafBounds,
    int nLeaf,
    float fLeafMinGap,
    float x1Min,
    float x2Max,
    float fLeafSpan,
    float fAverageX,
    float fMinimumOverlap,
    float jawPos[4])
{
    for (int i = 0; i < nLeaf; i++)
    {
        // closed leaf
        if (std::abs(vecLeafPos[i].y - vecLeafPos[i].x - fLeafMinGap) < 1e-4)
        {
            vecLeafPos[i].x = fAverageX - fLeafMinGap * 0.5;
            vecLeafPos[i].y = fAverageX + fLeafMinGap * 0.5;

            // overlapped by bottom Jaw or top jaw
            if (leafBounds[i +1] <= jawPos[2] || leafBounds[i] >= jawPos[3])
                continue;

            bool bCanLeft = ((x2Max - fLeafSpan) < (jawPos[0] - fMinimumOverlap));
            bool bCanRight = ((x1Min + fLeafSpan) > (jawPos[1] + fMinimumOverlap));

            // adjust to left
            if (bCanLeft && !bCanRight)
            {
                vecLeafPos[i].x = jawPos[0] - fMinimumOverlap - fLeafMinGap;
                vecLeafPos[i].y = jawPos[0] - fMinimumOverlap;
            }
            // adjust to right
            else if(!bCanLeft && bCanRight)
            {
                vecLeafPos[i].x = jawPos[1] + fMinimumOverlap;
                vecLeafPos[i].y = jawPos[1] + fMinimumOverlap + fLeafMinGap;
            }
            // both is ok
            else if (bCanLeft && bCanRight)
            {
                if (fAverageX > (jawPos[0] + jawPos[1]) * 0.5)
                {
                    // adjust to right
                    vecLeafPos[i].x = jawPos[1] + fMinimumOverlap;
                    vecLeafPos[i].y = jawPos[1] + fMinimumOverlap + fLeafMinGap;
                }
                else
                {
                    // adjust to left
                    vecLeafPos[i].x = jawPos[0] - fMinimumOverlap - fLeafMinGap;
                    vecLeafPos[i].y = jawPos[0] - fMinimumOverlap;
                }
            }
        }
    }
}

bool TpsMlcJawLimitationHelper::CheckAllLeafClosed_i(plunc::LEAF_POSITION_STC* pLeaf, int nLeaf, float fLeafMinGap)
{
    bool isAllLeafClosed = true;
    for (int i = 0; i < nLeaf; i++)
    {
        if ((pLeaf[i].pos_2 - pLeaf[i].pos_1) < fLeafMinGap)
            continue;
        isAllLeafClosed = false;
    }

    return isAllLeafClosed;
}

bool TpsMlcJawLimitationHelper::CheckAllLeafClosed_i(std::vector<db_Point2d> vecLeafPos, int nLeaf, float fLeafMinGap)
{
    bool isAllLeafClosed = true;

    for (int i = 0; i < nLeaf; i++)
    {
        if (std::abs(vecLeafPos[i].y - vecLeafPos[i].x - fLeafMinGap) < 1e-4)
        {
            continue;
        }
        isAllLeafClosed = false;
    }

    return isAllLeafClosed;
}

void TpsMlcJawLimitationHelper::AdjustMlcJawGivenMachine(
    const RtMachine* pMachine,
    plunc::LEAF_POSITION_STC* pLeaf,
    int nLeaf,
    bool bJawFollow,
    float fJawMarginX,
    float fJawMarginY,
    float pJawPosition[4])
{
    if (nullptr == pMachine || nullptr == pLeaf || 0 == nLeaf) return;

    float fLeafMin = pMachine->get_positiveleafrangemin();
    float fLeafMax = pMachine->get_positiveleafrangemax();
    float fLeafMinGap = pMachine->get_minleafendgap();
    float fLeafSpan = pMachine->get_leafspan();
    float fMinimumOverlap = pMachine->get_minimumjawgap();

    bool bAllowInterdigitation = pMachine->get_isinterdigitation();

    if (pMachine->get_leafnum() != nLeaf) return;

    float x1Min = fLeafMax;
    float x1Max = fLeafMin;
    float x2Min = fLeafMax;
    float x2Max = fLeafMin;
    float yMin = fLeafMax;
    float yMax = fLeafMin;

    bool isAllLeafClosed = CheckAllLeafClosed_i(pLeaf, nLeaf, fLeafMinGap);
    if (isAllLeafClosed)
    {
        x1Min = 0.0f;
        x1Max = 0.0f;
        x2Min = 0.0f;
        x2Max = 0.0f;
        yMin = 0.0f;
        yMax = 0.0f;
    }
    else
    {
        // Check Leaf Min/Max range
        for (int i = 0; i < nLeaf; i++)
        {
            // closed leaf
            if ((pLeaf[i].pos_2 - pLeaf[i].pos_1) < fLeafMinGap)
                continue;
            x1Min = TPS_MIN(pLeaf[i].pos_1, x1Min);
            x1Max = TPS_MAX(pLeaf[i].pos_1, x1Max);
            x2Min = TPS_MIN(pLeaf[i].pos_2, x2Min);
            x2Max = TPS_MAX(pLeaf[i].pos_2, x2Max);
            yMin = TPS_MIN(pLeaf[i].min_w, yMin);
            yMax = TPS_MAX(pLeaf[i].max_w, yMax);
        }
    }


    // Ò¶Æ¬ÔÊÐí½»´í¼ì²é
    if (!bAllowInterdigitation)
    {
        CheckLeafInterdigitation_i(pLeaf, nLeaf, fLeafMinGap);
    }

    // ¼ì²éÒ¶Æ¬µÄ³öÏä¾àÀëÔ¼Êø
    if ((x1Max - x1Min) > fLeafSpan)
    {
        x1Max = x1Min + fLeafSpan;
        for (int i = 0; i < nLeaf; i++)
        {
            // closed leaf
            if ((pLeaf[i].pos_2 - pLeaf[i].pos_1) < fLeafMinGap)
                continue;
            if (pLeaf[i].pos_1 > x1Max)
                pLeaf[i].pos_1 = x1Max;
        }
    }

    if ((x2Max - x2Min) > fLeafSpan)
    {
        x2Min = x2Max - fLeafSpan;
        for (int i = 0; i < nLeaf; i++)
        {
            // closed leaf
            if ((pLeaf[i].pos_2 - pLeaf[i].pos_1) < fLeafMinGap)
                continue;
            if (pLeaf[i].pos_2 < x2Min)
                pLeaf[i].pos_2 = x2Min;
        }
    }


    if (bJawFollow)
    {
        if (isAllLeafClosed)
        {
            pJawPosition[0] = 0.0f;
            pJawPosition[1] = 0.0f;
            pJawPosition[2] = 0.0f;
            pJawPosition[3] = 0.0f;
        }
        else
        {
            pJawPosition[0] = x1Min - fJawMarginX;
            pJawPosition[1] = x2Max + fJawMarginX;
            pJawPosition[2] = yMin - fJawMarginY;
            pJawPosition[3] = yMax + fJawMarginY;
        }
    }

    // Adjust closed leaf
    float fAverageX = CalculateAverageLeafPosition_i(pLeaf, nLeaf, fLeafMinGap);
    CorrectMlcGapPosition_i(pLeaf, nLeaf, fLeafMinGap, x1Min, x2Max, fLeafSpan, fAverageX, fMinimumOverlap, pJawPosition);

}

void TpsMlcJawLimitationHelper::JawFollowMlc(
    const RtMachine* pMachine,
    double gapOffset,
    RtMlcshape* pShape,
    float fJawMarginX,
    float fJawMarginY)
{
    if (nullptr == pMachine || nullptr == pShape) return;

    float fLeafMin = pMachine->get_positiveleafrangemin();
    float fLeafMax = pMachine->get_positiveleafrangemax();
    float fLeafMinGap = pMachine->get_minleafendgap() + gapOffset;
    float fLeafSpan = pMachine->get_leafspan();
    float fMinimumOverlap = pMachine->get_minimumjawgap();
    float x1JawMin = pMachine->get_xmin_1();
    float x1JawMax = pMachine->get_xmax_1();
    float x2JawMin = pMachine->get_xmin_2();
    float x2JawMax = pMachine->get_xmax_2();
    float y1JawMin = pMachine->get_ymin_1();
    float y1JawMax = pMachine->get_ymax_1();
    float y2JawMin = pMachine->get_ymin_2();
    float y2JawMax = pMachine->get_ymax_2();
    std::vector<double> leafBounds = pMachine->get_leafboundaries_list();

    float x1Min = fLeafMax;
    float x1Max = fLeafMin;
    float x2Min = fLeafMax;
    float x2Max = fLeafMin;
    float yMin = fLeafMax;
    float yMax = fLeafMin;
    std::vector<db_Point2d> vecLeafPos = pShape->get_leafpositions();

    if (leafBounds.size() != vecLeafPos.size() + 1) return;

    int nLeaf = vecLeafPos.size();

    bool isAllLeafClosed = CheckAllLeafClosed_i(vecLeafPos, nLeaf, fLeafMinGap);
    if (isAllLeafClosed)
    {
        x1Min = 0.0f;
        x1Max = 0.0f;
        x2Min = 0.0f;
        x2Max = 0.0f;
        yMin = 0.0f;
        yMax = 0.0f;
    }
    else
    {
        for (int i = 0; i < nLeaf; i++)
        {
            if (std::abs(vecLeafPos[i].y - vecLeafPos[i].x - fLeafMinGap) < 1e-4)
            {
                continue;
            }
            x1Min = TPS_MIN(vecLeafPos[i].x, x1Min);
            x1Max = TPS_MAX(vecLeafPos[i].x, x1Max);
            x2Min = TPS_MIN(vecLeafPos[i].y, x2Min);
            x2Max = TPS_MAX(vecLeafPos[i].y, x2Max);
            yMin = TPS_MIN(leafBounds[i], yMin);
            yMax = TPS_MAX(leafBounds[i + 1], yMax);
        }
    }

    float jawPos[4];
    jawPos[0] = x1Min - fJawMarginX;
    jawPos[1] = x2Max + fJawMarginX;
    jawPos[2] = yMin - fJawMarginY;
    jawPos[3] = yMax + fJawMarginY;

    jawPos[0] = TPS_CLAMP(jawPos[0], x1JawMin, x1JawMax);
    jawPos[1] = TPS_CLAMP(jawPos[1], x2JawMin, x2JawMax);
    jawPos[2] = TPS_CLAMP(jawPos[2], y1JawMin, y1JawMax);
    jawPos[3] = TPS_CLAMP(jawPos[3], y2JawMin, y2JawMax);

    pShape->set_xlowerjawpos(jawPos[0]);
    pShape->set_xupperjawpos(jawPos[1]);
    pShape->set_ylowerjawpos(jawPos[2]);
    pShape->set_yupperjawpos(jawPos[3]);

    // Adjust closed leaf
    float fAverageX = CalculateAverageLeafPosition_i(vecLeafPos, nLeaf, fLeafMinGap);
    CorrectMlcGapPosition_i(vecLeafPos, leafBounds, nLeaf, fLeafMinGap, x1Min, x2Max, fLeafSpan, fAverageX, fMinimumOverlap, jawPos);

    pShape->set_leafpositions(vecLeafPos);
}





TPS_END_NAMESPACE
