////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file rt_tps_certified_poi.cpp
/// 
///  \brief struct CertifiedPoi implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/2/18
////////////////////////////////////////////////////////////////

#include "RtTpsCertifiedLib/rt_tps_certified_poi.h"
#include "RtTpsProtoLib/rt_tps_proto_poi.pb.h"

//#include "RtTpsProtoLib/rt_tps_poiactioninfo.pb.h"

TPS_BEGIN_NAMESPACE

CertifiedPoi::CertifiedPoi(){

}

void CertifiedPoi::Convert(const proto::RtTpsProtoPoi &protoPoi) 
{

    if(protoPoi.has_poiuid()) {
        m_strPoiUID = protoPoi.poiuid();
    }

    if (protoPoi.has_seriesuid()) {
        m_strSeriesUid = protoPoi.seriesuid();
    }

    if(protoPoi.has_poiname()) {
        m_strPoiName = protoPoi.poiname();
    }

    if(protoPoi.has_poitype()) {
        m_nPoiType = protoPoi.poitype();
    }

    if (protoPoi.has_isvisible()) {
        m_bIsVisible = protoPoi.isvisible();
    }

    if (protoPoi.has_locktype()) {
        m_nLockType = protoPoi.locktype();
    }

    if (protoPoi.poicolor_size() > 0) {
        m_fPoiColor[0] = protoPoi.poicolor(0);
        m_fPoiColor[1] = protoPoi.poicolor(1);
        m_fPoiColor[2] = protoPoi.poicolor(2);
        m_fPoiColor[3] = protoPoi.poicolor(3);
    }

    if (protoPoi.has_xcoordinpat()) {
        m_fXCoordInPat = protoPoi.xcoordinpat();
    }
    if (protoPoi.has_ycoordinpat()) {
        m_fYCoordInPat = protoPoi.ycoordinpat();
    }
    if (protoPoi.has_zcoordinpat()) {
        m_fZCoordInPat = protoPoi.zcoordinpat();
    }

    if (protoPoi.has_xcoordincol()) {
        m_fXCoordInCol = protoPoi.xcoordincol();
    }

    if (protoPoi.has_ycoordincol()) {
        m_fYCoordInCol = protoPoi.ycoordincol();
    }

    if (protoPoi.has_pointdepth()) {
        m_fPointDepth = protoPoi.pointdepth();
    }
}

//not recommended, use proto converter
void CertifiedPoi::ConvertBack(proto::RtTpsProtoPoi *protoPoi) const
{
    protoPoi->set_poiuid(m_strPoiUID);
    protoPoi->set_seriesuid(m_strSeriesUid);
    protoPoi->set_poiname(m_strPoiName);
    protoPoi->set_poitype(m_nPoiType);
    protoPoi->set_isvisible(m_bIsVisible);
    protoPoi->set_locktype(m_nLockType);

    protoPoi->clear_poicolor();
    protoPoi->add_poicolor(m_fPoiColor[0]);
    protoPoi->add_poicolor(m_fPoiColor[1]);
    protoPoi->add_poicolor(m_fPoiColor[2]);
    protoPoi->add_poicolor(m_fPoiColor[3]);

    protoPoi->set_xcoordinpat(m_fXCoordInPat);
    protoPoi->set_ycoordinpat(m_fYCoordInPat);
    protoPoi->set_zcoordinpat(m_fZCoordInPat);

    protoPoi->set_xcoordincol(m_fXCoordInCol);
    protoPoi->set_ycoordincol(m_fYCoordInCol);
    protoPoi->set_pointdepth(m_fPointDepth);
}

TPS_END_NAMESPACE