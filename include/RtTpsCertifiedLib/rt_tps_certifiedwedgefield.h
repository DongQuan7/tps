//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file   rt_ms_certifiedUID.h
///  \brief   CertifiedUID
///
///  \version 1.0
///  \date    8. 21, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDWEDGEFIELD_H_
#define RT_MS_CERTIFIEDWEDGEFIELD_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE


typedef struct CommonContourVerter
{
    std::string uid;
    float contourVerterx;
    float contourVertery;
}ContourVerter;


typedef struct CommonPhysicalWedgeField
{
    std::string wedgeUID;
    std::string wedgeType;
    std::string wedgeId;
    float  mu;
    float dmudx;
    float dmudr;
    float dmuda;
    float hvlSlope;
    std::string carrierUID;
    float carriermu;
    float carrierdmudx;
    float carrierdmudr;
    float carrierdmuda;
    float carrierhvlSlope;
    float density;

    float wedgeThick;
    float carrierThick;
    std::string contourUID1;
    std::string contourUID2;

    std::vector< std::tr1::shared_ptr<CommonContourVerter>> wedgeContourVerterList;
    std::vector< std::tr1::shared_ptr<CommonContourVerter>> carrierContourVerterList;

}PhysicalWedgeField;

typedef struct CertifiedVirtualWedgeField
{
    std::string  uid;
    float  collimatorX1Position;
    float collimatorX2Position;
    float relativeMU;
    float correctionFactor;
}VirtualWedgeField;

typedef struct CertifiedVirtualWedgeInfo
{
    std::string virtualUID;
    std::string goldenUID;
    std::vector< std::tr1::shared_ptr<CertifiedVirtualWedgeField> >virtualWedgeList;
}VirtualWedgeInfo;

TPS_END_NAMESPACE


#endif
