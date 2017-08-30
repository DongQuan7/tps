//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   rt_tps_certifiedmesh.cpp 
///  \brief   mesh which could represent faces of specific voi
///
///  \version 1.0
///  \date  2014/08/15  
///  \
//////////////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedmesh.h"
#include "RtTpsProtoLib/rt_tps_mesh.pb.h"

#include <algorithm>

TPS_BEGIN_NAMESPACE

void CertifiedTriangleIndex::Convert(const proto::RT_TPS_TRIANGLEINDEX &protoIndex) {
    mIndex1 = protoIndex.index1();
    mIndex2 = protoIndex.index2();
    mIndex3 = protoIndex.index3();
}

void CertifiedTriangleIndex::ConvertBack(proto::RT_TPS_TRIANGLEINDEX *protoIndex) const{
    protoIndex->set_index1(mIndex1);
    protoIndex->set_index2(mIndex2);
    protoIndex->set_index3(mIndex3);
}

void CertifiedMesh::Convert(const proto::RT_TPS_MESH mesh) {
    for (int i = 0; i < mesh.points_size(); ++i) {
        CertifiedPoint3D point;
        point.Convert(mesh.points(i));
        mPoints.push_back(point);
    }

    for(int j = 0; j < mesh.normals_size(); ++j) {
        CertifiedVector3D normalVector;
        normalVector.Convert(mesh.normals(j));
        mNormals.push_back(normalVector);
    }

    for(int k = 0; k < mesh.triangleindices_size(); ++k) {
        CertifiedTriangleIndex triangleIndex;
        triangleIndex.Convert(mesh.triangleindices(k));
        mTriangleIndices.push_back(triangleIndex);
    }
}

void CertifiedMesh::ConvertBack(proto::RT_TPS_MESH *outMesh) const{
     std::for_each(mPoints.begin(), mPoints.end(),
         [&] (CertifiedPoint3D point){
             proto::RT_TPS_POINT3D *newPoint = outMesh->add_points();
             newPoint->set_x(point.mX);
             newPoint->set_y(point.mY);
             newPoint->set_z(point.mZ);
     });
 
     std::for_each(mNormals.begin(), mNormals.end(),
         [&](CertifiedVector3D normal){
             proto::RT_TPS_VECTOR3D *normalVector = outMesh->add_normals();
             normalVector->set_x(normal.mX);
             normalVector->set_y(normal.mY);
             normalVector->set_z(normal.mZ);
     });
 
     std::for_each(mTriangleIndices.begin(), mTriangleIndices.end(),
         [&](CertifiedTriangleIndex triangleIndex){
             proto::RT_TPS_TRIANGLEINDEX *newTriangleIndex = outMesh->add_triangleindices();
             newTriangleIndex->set_index1(triangleIndex.mIndex1);
             newTriangleIndex->set_index2(triangleIndex.mIndex2);
             newTriangleIndex->set_index3(triangleIndex.mIndex3);
 
     });
}
TPS_END_NAMESPACE