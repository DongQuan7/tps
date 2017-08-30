//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   rt_tps_certifiedvector3d.cpp
///  \brief   vector 3d
///
///  \version 1.0
///  \date  2014/08/15  
///  \
//////////////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedvector3d.h"
#include "RtTpsProtoLib/rt_tps_mesh.pb.h"

TPS_BEGIN_NAMESPACE

void CertifiedVector3D::Convert(const proto::RT_TPS_VECTOR3D &protoVector) {
    mX = protoVector.x();
    mY = protoVector.y();
    mZ = protoVector.z();
}

void CertifiedVector3D::ConvertBack(proto::RT_TPS_VECTOR3D *protoVector) {
    protoVector->set_x(mX);
    protoVector->set_y(mY);
    protoVector->set_z(mZ);
}

TPS_END_NAMESPACE