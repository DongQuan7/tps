////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_drrgraphicobject.cpp
/// 
///  \brief class DrrGraphicObejct implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/camera.h"


TPS_BEGIN_NAMESPACE
DrrGraphicObejct::DrrGraphicObejct() : mWidowWidth(0), mWinowCenter(0) {
    //mEye = Mcsf::MedViewer3D::Point3D(0.0,0.0,1);
    //mLookAt = Mcsf::MedViewer3D::Point3D(0.0,0.0,0.0);
    //mUp = Mcsf::MedViewer3D::Point3D(0.0,1.0,0.0);

}

DrrGraphicObejct::~DrrGraphicObejct() {

}

//void DrrGraphicObejct::PlaceCamera(const Mcsf::MedViewer3D::Point3D &ptEye,
//    const Mcsf::MedViewer3D::Point3D& ptLookAt,
//    const Mcsf::MedViewer3D::Vector3D& vUp) {
//        using namespace Mcsf::MedViewer3D;
//        const double dDistance = (ptEye - ptLookAt).Magnitude();
//
//        Vector3D vCameraUp = vUp;
//        vCameraUp.Normalize();
//        Point3D ptUp = ptLookAt + vCameraUp * dDistance;
//
//        Vector3D vCameraRight = vCameraUp.CrossProduct(ptEye - ptLookAt);
//        vCameraRight.Normalize();
//        Point3D ptRight = ptLookAt + vCameraRight * dDistance;
//
//        Matrix4x4 matModel(0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1);
//        Matrix4x4 matWorld(ptLookAt.x, ptLookAt.y, ptLookAt.z, 1.0,
//            ptRight.x, ptRight.y, ptRight.z, 1.0,
//            ptUp.x, ptUp.y, ptUp.z, 1.0,
//            ptEye.x, ptEye.y, ptEye.z, 1.0);
//        this->SetModel2World(matWorld * matModel.Inverse());
//        this->SetFov(std::atan(1.0 / dDistance) * 2);
//}
//
//void DrrGraphicObejct::SetFov(const double &angle) {
//    mFov = angle;
//}
//
//
//double DrrGraphicObejct::GetFov() const{
//    return mFov;
//}
//
//Mcsf::MedViewer3D::Point3D DrrGraphicObejct::GetEye() const {
//    return m_Model2WorldMatrixTransform.Transform(mEye);
//}
//
//Mcsf::MedViewer3D::Point3D DrrGraphicObejct::GetLookAt() const {
//    return m_Model2WorldMatrixTransform.Transform(mLookAt);
//}
//
//Mcsf::MedViewer3D::Vector3D DrrGraphicObejct::GetUpOrientation() const {
//    Mcsf::MedViewer3D::Vector3D vec = m_Model2WorldMatrixTransform.Transform(mUp - mLookAt);
//    if(!vec.Normalize()) {
//        //log; error
//        return vec;
//    }
//    return vec;
//
//}
//
//void DrrGraphicObejct::Rotate(const Mcsf::MedViewer3D::Quaternion &quat) {
//    m_Model2WorldMatrixTransform.Append(quat.ToMatrix());
//}
//
//Mcsf::MedViewer3D::Matrix4x4 DrrGraphicObejct::GetViewProjectionMat(int width, int height) {
//    Mcsf::MedViewer3D::ProjectionCamera *camera = new Mcsf::MedViewer3D::ProjectionCamera();
//    double distance = (GetLookAt() - GetEye()).Magnitude();
//    camera->SetProjectionType(Mcsf::MedViewer3D::ProjectionCamera::PT_PERSPECTIVE);
//    camera->SetLookAt(GetLookAt());
//    camera->SetEye(GetEye());
//    camera->SetUpDirection(GetUpOrientation());
//    camera->SetFarClipDistance(distance * 2.0 - 2.0);
//    camera->SetAspectRatio(static_cast<float>(width) / height);
//    camera->SetFovY(std::atan(1.0 / distance) * 2);
//    camera->SetNearClipDistance(1.0);
//    Mcsf::MedViewer3D::Matrix4x4 mat = camera->GetViewProjectionMatrix();
//    delete camera;
//    camera = nullptr;
//    return mat;
//
//}

void DrrGraphicObejct::SetWindowing(double windowWidth , double windowCenter) {
    mWidowWidth = windowWidth;
    mWinowCenter = windowCenter;
    
}

void DrrGraphicObejct::GetWindowing(double *windowWidth, double *windowCenter) {
  
    *windowWidth = mWidowWidth;
    *windowCenter = mWinowCenter;
}

TPS_END_NAMESPACE
