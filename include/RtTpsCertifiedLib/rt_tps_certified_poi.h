////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file rt_tps_certified_poi.cpp
/// 
///  \brief struct CertifiedPoi declaration
/// 
///  \version 1.0
/// 
///  \date    2013/2/18
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIED_POI_H_
#define RT_TPS_CERTIFIED_POI_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RtTpsProtoPoi;
}

struct CertifiedPoi 
{
    std::string         m_strPoiUID;
    std::string         m_strPoiName;
    std::string         m_strSeriesUid;
    int                 m_nPoiType;
    float               m_fPoiColor[4];
    bool                m_bIsVisible;
    int                 m_nLockType;

    float             m_fXCoordInPat;
    float             m_fYCoordInPat;
    float             m_fZCoordInPat;

    float             m_fXCoordInCol;
    float             m_fYCoordInCol;
    float             m_fPointDepth;

    CertifiedPoi();

    void Convert(const proto::RtTpsProtoPoi &protoPoi);

    void ConvertBack(proto::RtTpsProtoPoi *protoPoi) const;

};
TPS_END_NAMESPACE


#endif