////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file rt_tps_certifieddelete.cpp
/// 
///  \brief struct CertifiedDelete imaplementation 
/// 
///  \version 1.0
/// 
///  \date    2014/3/07
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifieddelete.h"
#include "RtTpsProtoLib/rt_tps_delete.pb.h"
TPS_BEGIN_NAMESPACE

void CertifiedDelete::Convert( const proto::RT_TPS_Delete &tpsDelete) {
    mObjectType = tpsDelete.objecttype();
    mUid = tpsDelete.uid();
    if(tpsDelete.has_fatheruid()) {
        mFatherUid = tpsDelete.fatheruid();
    }

}

void CertifiedDelete::ConvertBack(proto::RT_TPS_Delete * protoDelete) const{
    protoDelete->set_objecttype(mObjectType);
    protoDelete->set_uid(mUid); 
    protoDelete->set_fatheruid(mFatherUid);

}

TPS_END_NAMESPACE