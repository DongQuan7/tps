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
#include "RtTpsCertifiedLib/rt_tps_certifiedpoint3d.h"
#include "RtTpsProtoLib/rt_tps_mesh.pb.h"

TPS_BEGIN_NAMESPACE

void CertifiedPoint3D::Convert(const proto::RT_TPS_POINT3D &protoPoint) {
    mX = protoPoint.x();
    mY = protoPoint.y();
    mZ = protoPoint.z();
}

void CertifiedPoint3D::ConvertBack(proto::RT_TPS_POINT3D *protoPoint) {
    protoPoint->set_x(mX);
    protoPoint->set_y(mY);
    protoPoint->set_z(mZ);
}

TPS_END_NAMESPACE