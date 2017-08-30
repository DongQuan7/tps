//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   rt_tps_certifiedpoint3d.h 
///  \brief   point3d
///
///  \version 1.0
///  \date  2014/08/15  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef RT_TPS_CERTIFIEDPOINT3D_H_
#define RT_TPS_CERTIFIEDPOINT3D_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_POINT3D;
}

struct CertifiedPoint3D {
    double mX;
    double mY;
    double mZ;

    void Convert(const proto::RT_TPS_POINT3D &protoPoint);

    void ConvertBack(proto::RT_TPS_POINT3D *protoPoint);
};

TPS_END_NAMESPACE

#endif