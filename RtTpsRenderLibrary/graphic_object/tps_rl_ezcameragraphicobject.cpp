//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  SHI CHUNHUI chunhui.shi@united-imaging.com
///
///  \file    tps_rl_ezcameragraphicobject.cpp
///  \brief   Camera graphic object in Easy Plan Module
///
///  \version 1.0
///  \date    2015/3/18
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_ezcameragraphicobject.h"
#include "McsfMedViewer3DArithmetic/camera.h"

TPS_BEGIN_NAMESPACE

TpsEzCameraGraphicObject::TpsEzCameraGraphicObject() :mZoomFactor(1.0) {
}

TpsEzCameraGraphicObject::~TpsEzCameraGraphicObject() {

}

bool TpsEzCameraGraphicObject::Initialize() {

    PlaceCamera(Mcsf::MedViewer3D::Point3D(0,-100,0),
        Mcsf::MedViewer3D::Point3D(0,0,0),
        Mcsf::MedViewer3D::Vector3D(0,0,1));
    return true;
}

void TpsEzCameraGraphicObject::PlaceCamera(const Mcsf::MedViewer3D::Point3D &ptEye, 
    const Mcsf::MedViewer3D::Point3D& ptLookAt, 
    const Mcsf::MedViewer3D::Vector3D& vUp) {
    using namespace Mcsf::MedViewer3D;

    Vector3D vecZ = ptEye - ptLookAt;
    mEyeToLookAtDist = vecZ.Magnitude();
    vecZ.Normalize();
    Vector3D vecX = vUp.CrossProduct(vecZ);
    vecX.Normalize();
    Vector3D vecY = vecZ.CrossProduct(vecX);

    Matrix4x4 matInCamera(1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1);

    Matrix4x4 matInWorld(
        vecX.x, vecX.y, vecX.z, 0, 
        vecY.x, vecY.y, vecY.z, 0, 
        vecZ.x, vecZ.y, vecZ.z, 0, 
        ptLookAt.x, ptLookAt.y, ptLookAt.z, 1);
    m_Model2WorldMatrixTransform = matInWorld * matInCamera.Inverse();
    if (mZoomFactor != 1.0){
        m_Model2WorldMatrixTransform.Append(
            MakeScale(Mcsf::MedViewer3D::Vector3D(
            1.0 / mZoomFactor, 1.0 / mZoomFactor, 1.0 / mZoomFactor)));
    }
}

double TpsEzCameraGraphicObject::GetFOV(double aspect) const {

    Mcsf::MedViewer3D::Point3D eye = this->GetEye();
    Mcsf::MedViewer3D::Point3D lookAt = this->GetLookAt();
    Mcsf::MedViewer3D::Matrix4x4 matView = this->GetViewMatrix_i();
    double distance = matView.Transform(lookAt - eye).Magnitude();
    double angle = std::atan(1.0 / distance) * 2;
    return (aspect > 1.0) ? angle : (angle / aspect);
}

void TpsEzCameraGraphicObject::SetScale(double dScale) {
    if (dScale < 1e-6) return;
    double scale = dScale / mZoomFactor;
    mZoomFactor = dScale;
    m_Model2WorldMatrixTransform.Append(
        MakeScale(Mcsf::MedViewer3D::Vector3D(1.0 / scale, 1.0 / scale, 1.0 / scale)));
}

double TpsEzCameraGraphicObject::GetScale() const {
    return mZoomFactor;
}

Mcsf::MedViewer3D::Point3D TpsEzCameraGraphicObject::GetEye() const {
    Mcsf::MedViewer3D::Point3D lookAt = m_Model2WorldMatrixTransform.Transform(
        Mcsf::MedViewer3D::Point3D(0, 0, 0));
    Mcsf::MedViewer3D::Vector3D vecLookAtToEye = m_Model2WorldMatrixTransform.Transform(
        Mcsf::MedViewer3D::Vector3D(0, 0, 1));
    vecLookAtToEye.Normalize();
    return lookAt + mEyeToLookAtDist * vecLookAtToEye;
}

Mcsf::MedViewer3D::Point3D TpsEzCameraGraphicObject::GetLookAt() const {
    return m_Model2WorldMatrixTransform.Transform(
        Mcsf::MedViewer3D::Point3D(0, 0, 0));
}

Mcsf::MedViewer3D::Vector3D TpsEzCameraGraphicObject::GetUpOrientation() const {
    Mcsf::MedViewer3D::Vector3D vec = m_Model2WorldMatrixTransform.Transform(
        Mcsf::MedViewer3D::Vector3D(0, 1, 0));
    if(!vec.Normalize()) { // error
        return vec;
    }
    return vec;
}

void TpsEzCameraGraphicObject::Rotate(const Mcsf::MedViewer3D::Quaternion &quat) {
    m_Model2WorldMatrixTransform.Append(quat.ToMatrix());
}

void TpsEzCameraGraphicObject::Translate(const Mcsf::MedViewer3D::Vector3D& vecTranslate){
    m_Model2WorldMatrixTransform.Append(MakeTranslate(vecTranslate));
}

Mcsf::MedViewer3D::Matrix4x4 TpsEzCameraGraphicObject::GetViewMatrix_i() const{

    Mcsf::MedViewer3D::Matrix4x4 mat = m_Model2WorldMatrixTransform.Inverse();
    Mcsf::MedViewer3D::Vector3D vecLookAtToEye = m_Model2WorldMatrixTransform.Transform(
        Mcsf::MedViewer3D::Vector3D(0, 0, 1));
    double offsetZ = mEyeToLookAtDist / vecLookAtToEye.Magnitude();
    mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 
        0, 0, 1, 0, 0, 0, -1 * offsetZ, 1));
    return mat;
}

Mcsf::MedViewer3D::Matrix4x4 TpsEzCameraGraphicObject::GetViewProjectionMat(int width, int height) {
    Mcsf::MedViewer3D::Point3D eye = this->GetEye();
    Mcsf::MedViewer3D::Point3D lookAt = this->GetLookAt();
    Mcsf::MedViewer3D::Matrix4x4 matView = this->GetViewMatrix_i();

    Mcsf::MedViewer3D::Camera *camera = new Mcsf::MedViewer3D::Camera();
    camera->SetProjectionType(Mcsf::MedViewer3D::Camera::PT_PERSPECTIVE);
    double aspect = double(width) / height;
    double distance = matView.Transform(lookAt - eye).Magnitude();
    double angle = std::atan(1.0 / distance) * 2;
    camera->SetAspectRatio(aspect);
    camera->SetFovY((aspect > 1.0) ? angle : (angle / aspect));
    camera->SetNearClipDistance(0.1 * distance);
    camera->SetFarClipDistance(2 * distance);
    //calculate near clip plane & far clip plane
    /*double minZ(10.0), maxZ(0.0);
    Mcsf::MedViewer3D::Point3D vertices[8] = {
        Mcsf::MedViewer3D::Point3D(-1.0, -1.0, -1.0),
        Mcsf::MedViewer3D::Point3D(1.0, -1.0, -1.0),
        Mcsf::MedViewer3D::Point3D(1.0, 1.0, -1.0),
        Mcsf::MedViewer3D::Point3D(-1.0, 1.0, -1.0),
        Mcsf::MedViewer3D::Point3D(-1.0, -1.0, 1.0),
        Mcsf::MedViewer3D::Point3D(1.0, -1.0, 1.0),
        Mcsf::MedViewer3D::Point3D(1.0, 1.0, 1.0),
        Mcsf::MedViewer3D::Point3D(-1.0, 1.0, 1.0)};
    Mcsf::MedViewer3D::Point3D tmp;
    for (int i = 0; i < 8; ++i){
        tmp = matView.Transform(vertices[i]);
        minZ = TPS_MIN(minZ, -1 * tmp.z);
        maxZ = TPS_MAX(maxZ, -1 * tmp.z);
    }
    camera->SetNearClipDistance(minZ);
    camera->SetFarClipDistance(maxZ);*/
    Mcsf::MedViewer3D::Matrix4x4 vp = camera->GetProjectionMatrix() * matView;
    delete camera;
    camera = nullptr;
    return vp;
}

TPS_END_NAMESPACE