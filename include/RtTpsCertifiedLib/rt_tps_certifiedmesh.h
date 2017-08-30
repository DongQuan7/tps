//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   rt_tps_certifiedmesh.h 
///  \brief   mesh which could represent faces of specific voi
///
///  \version 1.0
///  \date  2014/08/15  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef RT_TPS_CERTIFIEDMESH_H_
#define RT_TPS_CERTIFIEDMESH_H_

#include "tps_defs.h"
#include <vector>

#include "rt_tps_certifiedpoint3d.h"
#include "rt_tps_certifiedvector3d.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_TRIANGLEINDEX;
    class RT_TPS_MESH;
}

struct CertifiedTriangleIndex {
    int mIndex1;
    int mIndex2;
    int mIndex3;

    void Convert(const proto::RT_TPS_TRIANGLEINDEX &protoIndex);
    
    void ConvertBack(proto::RT_TPS_TRIANGLEINDEX *protoIndex) const;
};

struct CertifiedMesh {
    std::vector<CertifiedPoint3D> mPoints;
    std::vector<CertifiedVector3D> mNormals;
    std::vector<CertifiedTriangleIndex> mTriangleIndices;

    void Convert(const proto::RT_TPS_MESH mesh);

    void ConvertBack(proto::RT_TPS_MESH *outMesh) const;
};

TPS_END_NAMESPACE

#endif