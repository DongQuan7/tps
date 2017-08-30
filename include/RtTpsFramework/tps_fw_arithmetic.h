#ifndef TPS_FW_ARITHMETIC_H_
#define TPS_FW_ARITHMETIC_H_

#include "McsfMedViewer3DArithmetic/arithmetic.h"

typedef Mcsf::MedViewer3D::Point2D      PT2;
typedef Mcsf::MedViewer3D::Point3D      PT3;
typedef Mcsf::MedViewer3D::Vector2D     VEC2;
typedef Mcsf::MedViewer3D::Vector3D     VEC3;
typedef Mcsf::MedViewer3D::Quaternion   QUAT;
typedef Mcsf::MedViewer3D::Matrix4x4    MAT4;
typedef Mcsf::MedViewer3D::Plane        PLANE;

template<class T>
double Get(const T& v, int i)
{
    return i == 0 ? v.x : (i == 1 ? v.y : v.z);
}

template<class T>
void Set(T& v, int i, double val)
{
    double& item = (i == 0 ? v.x : (i == 1 ? v.y : v.z));
    item = val;
}

#endif // tps_fw_arithmetic_h__
