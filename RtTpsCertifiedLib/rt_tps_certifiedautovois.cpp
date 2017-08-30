////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedautovois.cpp
/// 
///  \brief struct CertifiedAutoVois implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/03/14
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedautovois.h"
#include "RtTpsProtoLib/rt_tps_proto_voi.pb.h"

TPS_BEGIN_NAMESPACE

void CertifiedAutoVois::Convert(const proto::RtTpsProtoRepeatedVoi& protoVoiList) {
    int size = protoVoiList.voilist_size();
    for(int i = 0; i <size; ++i) {
        proto::RtTpsProtoVoi protoVoi = protoVoiList.voilist(i);
        CertifiedVOI certifiedVoi;
        certifiedVoi.Convert(protoVoi);
        VOIList.push_back(certifiedVoi);
    }
}

void CertifiedAutoVois::ConvertBack(proto::RtTpsProtoRepeatedVoi *protoVoiList) const {
    protoVoiList->Clear();
    for(auto itr = VOIList.begin(); itr != VOIList.end(); ++itr) {
        auto pVoi = protoVoiList->add_voilist();
        itr->ConvertBack(pVoi);
    }
}

TPS_END_NAMESPACE
