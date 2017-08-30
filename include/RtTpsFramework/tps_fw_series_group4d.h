////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song    song.li@united-imaging.com
/// 
///  \file tps_da_series_group4d.h
/// 
///  \brief class TpsSeriesGroup4d declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_SERIES_GROUP4D_H_
#define TPS_DA_SERIES_GROUP4D_H_

#include "tps_fw_defs.h"
//#include "tps_da_arithmeticdefines.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class RtSeries;

struct TpsSeriesGroup4d
{
    bool                        m_bValid;
    bool                        m_bIsPlaying;
    std::string                 m_strMainSeriesUid;         // main series, all VOI for display/edit is associated with this
    std::string                 m_strCurrentSeriesUid;      // current selected series, either switch manually or via movie playing

    std::string                 m_strFirstSeriesUid;        // when movie stops, return to First Series
                                                            // it is set when manually switching the series.
                                                            // and it is the home series when stops 4D movie
    std::vector<std::string>    m_vecSeriesUid;
    std::vector<RtSeries*>      m_vecSeries;                // for temp use now
    std::string                 m_strGroupUid;
    std::string                 m_strGroupName;
    std::string                 m_strGroupingRule;          //the grouping rule: frame of reference uid + acquisition time + manufacturer model name

    void Init()
    {
        m_bValid = false;
        m_bIsPlaying = false;
        m_vecSeriesUid.clear();
        m_vecSeries.clear();
    }

    bool Contains(std::string strSeriesUid)
    {
        if(!m_bValid)
            return false;
        for (int i = 0; i < m_vecSeriesUid.size(); i++)
        {
            if (m_vecSeriesUid[i] == strSeriesUid)
            {
                return true;
            }
        }
        return false;
    }
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_DA_SERIES_GROUP4D_H_
