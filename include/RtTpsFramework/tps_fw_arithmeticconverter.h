////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_arithmeticconverter.h
/// 
///  \brief class TpsArithmeticConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/27
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_ARITHMETICCONVERTER_H_
#define TPS_FW_ARITHMETICCONVERTER_H_

#include "tps_fw_defs.h"

namespace Mcsf{
    class Point2f;
    class Point3f;
    class Matrix3f;
    class Matrix4f;
    class Vector2f;
    class Vector3f;
    class Vector4f;
    class Quat;

    namespace MedViewer3D{
        class Point2D;
        class Point3D;
        class Matrix4x4;
        class Vector2D;
        class Vector3D;
        class Quaternion;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsArithmeticConverter
/// 
/// \brief static methods converts data 
///  between Mcsf::Arithmetic and Mcsf::MedViewer3D::McsfMedViewer3DArithmetic
class TPS_FW_DECLSPEC TpsArithmeticConverter{

public:
    //convert Mcsf::Matrix4f to Mcsf::MedViewer3D::Matrix4x4
    static Mcsf::MedViewer3D::Matrix4x4 ConvertToMatrix4x4(const Mcsf::Matrix4f& matrix);
    //convert Mcsf::MedViewer3D::Matrix4x4 to Mcsf::Matrix4f
    static Mcsf::Matrix4f ConvertToMatrix4f(const Mcsf::MedViewer3D::Matrix4x4& matrix);
    //convert Mcsf::Point3f to Mcsf::MedViewer3D::Point3D
    static Mcsf::MedViewer3D::Point3D ConvertToPoint3D(const Mcsf::Point3f& point);
    //convert Mcsf::MedViewer3D::Point3D to Mcsf::Point3f
    static Mcsf::Point3f ConvertToPoint3f(const Mcsf::MedViewer3D::Point3D& point);
    static Mcsf::Point2f ConvertToPoint2f(const Mcsf::MedViewer3D::Point2D& point);
    //convert Mcsf::Vector3f to Mcsf::MedViewer3D::Vector3D
    static Mcsf::MedViewer3D::Vector3D ConvertToVector3D(const Mcsf::Vector3f& vector);
    //convert Mcsf::Vector3D to Mcsf::MedViewer3D::Vector3f
    static Mcsf::Vector3f ConvertToVector3f(const Mcsf::MedViewer3D::Vector3D& vector);
    //convert Mcsf::Vector2D to Mcsf::MedViewer3D::Vector2f
    static Mcsf::Vector2f ConvertToVector2f(const Mcsf::MedViewer3D::Vector2D& vector);
    //convert Mcsf::MedViewer3D::Vector2f to Mcsf::MedViewer3D::Point2D
    static Mcsf::MedViewer3D::Point2D ConvertToPoint2D(const Mcsf::Vector2f& vector);
    //convert Mcsf::MedViewer3D::Point2D to Mcsf::MedViewer3D::Vector2f
    static Mcsf::Vector2f Point2DConvertToVector2f(const Mcsf::MedViewer3D::Point2D& vector);

    static Mcsf::Matrix4f ConvertToMatrix4f(const float arrayMatrix[16]);

    static void ConvertToFloat16(const Mcsf::Matrix4f& matrix4f, float arrayMatrix[16]);
};

TPS_END_NAMESPACE  // end namespace tps

#endif