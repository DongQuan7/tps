////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedautovois.h
/// 
///  \brief struct CertifiedAutoVois declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/03/14
////////////////////////////////////////////////////////////////


#ifndef RT_TPS_CERTIFIEDAUTOVOIS_H_
#define RT_TPS_CERTIFIEDAUTOVOIS_H_

#include "tps_defs.h"
#include <vector>
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_AutoVOIs;
    class RtTpsProtoRepeatedVoi;
}

struct CertifiedAutoVois {
    std::vector<CertifiedVOI>   VOIList;

    void Convert(const proto::RtTpsProtoRepeatedVoi& protoVoiList);

    void ConvertBack(proto::RtTpsProtoRepeatedVoi *protoVoiList) const;
};

TPS_END_NAMESPACE

#endif