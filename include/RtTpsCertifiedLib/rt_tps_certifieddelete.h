////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file rt_tps_certifieddelete.h
/// 
///  \brief struct CertifiedDelete declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/3/07
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDDELETE_H_
#define RT_TPS_CERTIFIEDDELETE_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_Delete;
}

struct CertifiedDelete {
    std::string   mObjectType;
    std::string   mUid;
    std::string   mFatherUid;

    void Convert( const proto::RT_TPS_Delete &tpsDelete);

    void ConvertBack(proto::RT_TPS_Delete * protoDelete) const;
};

TPS_END_NAMESPACE


#endif