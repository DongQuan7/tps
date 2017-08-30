////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_arithmeticconverter.cpp
/// 
///  \brief class TpsArithmeticConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/27
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

//convert Mcsf::Matrix4f to Mcsf::MedViewer3D::Matrix4x4
Mcsf::MedViewer3D::Matrix4x4 TpsArithmeticConverter::ConvertToMatrix4x4(
    const Mcsf::Matrix4f& matrix){
    
    Mcsf::Vector4f col0 = matrix.GetCol0();
    Mcsf::Vector4f col1 = matrix.GetCol1();
    Mcsf::Vector4f col2 = matrix.GetCol2();
    Mcsf::Vector4f col3 = matrix.GetCol3();
    return Mcsf::MedViewer3D::Matrix4x4(
        col0.GetX(), col0.GetY(), col0.GetZ(), col0.GetW(), 
        col1.GetX(), col1.GetY(), col1.GetZ(), col1.GetW(), 
        col2.GetX(), col2.GetY(), col2.GetZ(), col2.GetW(), 
        col3.GetX(), col3.GetY(), col3.GetZ(), col3.GetW());
}

//convert Mcsf::MedViewer3D::Matrix4x4 to Mcsf::Matrix4f
Mcsf::Matrix4f TpsArithmeticConverter::ConvertToMatrix4f(
    const Mcsf::MedViewer3D::Matrix4x4& matrix){

    Mcsf::Vector4f col0(matrix._m[0], matrix._m[1], matrix._m[2], matrix._m[3]);
    Mcsf::Vector4f col1(matrix._m[4], matrix._m[5], matrix._m[6], matrix._m[7]);
    Mcsf::Vector4f col2(matrix._m[8], matrix._m[9], matrix._m[10], matrix._m[11]);
    Mcsf::Vector4f col3(matrix._m[12], matrix._m[13], matrix._m[14], matrix._m[15]);
    return Mcsf::Matrix4f(col0, col1, col2, col3);
}

//convert Mcsf::Point3f to Mcsf::MedViewer3D::Point3D
Mcsf::MedViewer3D::Point3D TpsArithmeticConverter::ConvertToPoint3D(
    const Mcsf::Point3f& point){

    return Mcsf::MedViewer3D::Point3D(
        point.GetX(), point.GetY(), point.GetZ());
}

Mcsf::Point2f TpsArithmeticConverter::ConvertToPoint2f(
    const Mcsf::MedViewer3D::Point2D& point)
{
    return Mcsf::Point2f(point.x, point.y);
}

//convert Mcsf::Point3f to Mcsf::MedViewer3D::Point3D
Mcsf::Point3f TpsArithmeticConverter::ConvertToPoint3f(
    const Mcsf::MedViewer3D::Point3D& point){

    return Mcsf::Point3f((float)point.x,(float)point.y,(float)point.z);
}

//convert Mcsf::Vector3f to Mcsf::MedViewer3D::Vector3D
Mcsf::MedViewer3D::Vector3D TpsArithmeticConverter::ConvertToVector3D(
    const Mcsf::Vector3f& vector){

    return Mcsf::MedViewer3D::Vector3D(
        vector.GetX(), vector.GetY(), vector.GetZ());
}
//convert Mcsf::Vector3D to Mcsf::MedViewer3D::Vector3f
Mcsf::Vector3f TpsArithmeticConverter::ConvertToVector3f(
    const Mcsf::MedViewer3D::Vector3D& vector) {
        return Mcsf::Vector3f(
        vector.x, vector.y, vector.z);
}
//convert Mcsf::Vector2D to Mcsf::MedViewer3D::Vector2f
Mcsf::Vector2f TpsArithmeticConverter::ConvertToVector2f(
    const Mcsf::MedViewer3D::Vector2D& vector) {
        return Mcsf::Vector2f(
        vector.x, vector.y);
}
//convert Mcsf::MedViewer3D::Vector2f to Mcsf::MedViewer3D::Point2D
Mcsf::MedViewer3D::Point2D TpsArithmeticConverter::ConvertToPoint2D(
    const Mcsf::Vector2f& vector) {
        return Mcsf::MedViewer3D::Point2D(vector.GetX(),vector.GetY());
}

//convert Mcsf::MedViewer3D::Point2D to Mcsf::MedViewer3D::Vector2f
Mcsf::Vector2f TpsArithmeticConverter::Point2DConvertToVector2f(
    const Mcsf::MedViewer3D::Point2D& vector) {
        return Mcsf::Vector2f(vector.x,vector.y);
}

//rcc: 这里要求array里面的排列顺序是列优先
Mcsf::Matrix4f TpsArithmeticConverter::ConvertToMatrix4f(const float arrayMatrix[16])
{
    Mcsf::Matrix4f matrix;
    for (int n=0; n<4; n++) {
        for (int m=0; m<4; m++) {
            matrix.SetElem(n,m,arrayMatrix[n*4+m]);
        }
    }
    return matrix;
}

void TpsArithmeticConverter::ConvertToFloat16(const Mcsf::Matrix4f& matrix4f, float arrayMatrix[16])
{
    for (int n=0; n<4; n++) {
        for (int m=0; m<4; m++) {
            arrayMatrix[n*4+m] = matrix4f.GetElem(n,m);
        }
    }
}

TPS_END_NAMESPACE  // end namespace tps