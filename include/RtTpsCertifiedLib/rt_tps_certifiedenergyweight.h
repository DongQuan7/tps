//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file   rt_ms_certifiedenergyweight.h
///  \brief   CertifiedScpInfo
///
///  \version 1.0
///  \date    7. 02, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDENERGYWEIGHT_H_
#define RT_MS_CERTIFIEDENERGYWEIGHT_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

struct CertifiedEnergyWeight{

    float energy;
    float weight;

};

TPS_END_NAMESPACE


#endif