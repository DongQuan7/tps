//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file table_converter.h
/// 
///  \version 1.0
/// 
///  \date    2016/04/21
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_TABLE_CONVERTER_H_
#define TPS_DP_TABLE_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"

namespace RTFWK
{
    class RTFWKDLDOVOI;
}

TPS_BEGIN_NAMESPACE

class RtTable;
class SeriesTable;
class TpsImage3DEntity;
class VOIEntity;

class TPS_DP_DECLSPEC TableConverter
{
public:
    static bool ConvertToALG(
        const RtTable& rtTable, 
        const SeriesTable& seriesTable, 
        const TpsImage3DEntity& image3D,
        std::shared_ptr<RTFWK::RTFWKDLDOVOI>* algTableVoi);

    static bool ConvertSeriesTableToVoi(
        const RtTable& rtTable, 
        const TpsImage3DEntity& image3D,
        const SeriesTable& seriesTable);
};
TPS_END_NAMESPACE
#endif