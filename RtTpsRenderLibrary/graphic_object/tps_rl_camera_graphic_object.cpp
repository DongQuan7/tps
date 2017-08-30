////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_camera_graphic_object.cpp
/// 
///  \brief class CameraGraphicObject implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/01/16
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"

using namespace Mcsf::MedViewer3D;

const double INFINITE_FAR_PLANE_ADJUST = 0.00001;

TPS_BEGIN_NAMESPACE

CameraGraphicObject::CameraGraphicObject() :
    mEye(0.0, 0.0, -10.0), 
    mLookAt(0.0, 0.0, 0.0),
    mUp(0.0, 1.0, 0.0),
    mProjType(Perspective),
    mAspect(1.0),
    mFovY(PI / 4.0),
    mFarDist(100.0),
    mNearDist(1.0),
    mOrthoHeight(2.0),
    mFocalLengthScale(1.0),
    mZoomFactor(1.0),
    mViewMatrixOutOfDate(true),
    mProjMatrixOutOfDate(true),
    mWorldCornersOutOfDate(true)
{
    mFrustumOffset[0] = 0.0;
    mFrustumOffset[1] = 0.0;
}

CameraGraphicObject::~CameraGraphicObject()
{

}

/// Set the position of the camera.
void CameraGraphicObject::SetEye(const Point3D& eye)
{
    mEye = eye;
    mViewMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the position of the camera. 
const Point3D CameraGraphicObject::GetEye() const
{
    return mEye;
}

/// Set the looking direction of the camera.
void CameraGraphicObject::SetLookAt(const Point3D& At)
{
    mLookAt = At;
    mViewMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the looking direction of the camera.
const Point3D CameraGraphicObject::GetLookAt() const
{
    return mLookAt;
}

/// Set the up-direction of the camera.
void CameraGraphicObject::SetUpDirection(const Vector3D& Updirection)
{
    mUp = Updirection;
    mUp.Normalize();
    mViewMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the up-direction of the camera.
const Vector3D CameraGraphicObject::GetUpDirection() const
{
    return mUp;
}

/// Set the Y-dimension Field Of View (FOV) of the frustum.
void CameraGraphicObject::SetFovY(double dFovy)
{
    mFovY = dFovy;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the Y-dimension Field Of View (FOV) of the frustum.
double CameraGraphicObject::GetFovY() const
{
    return mFovY;
}

/// Set the position of the near clipping plane.
void CameraGraphicObject::SetNearClipDistance(double dNearDist)
{
    mNearDist = dNearDist;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the position of the near clipping plane.
double CameraGraphicObject::GetNearClipDistance() const
{
    return mNearDist;
}

/// Set the distance to the far clipping plane.
void CameraGraphicObject::SetFarClipDistance(double dFarDist)
{
    mFarDist = dFarDist;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the distance from the frustum to the far clipping plane.
double CameraGraphicObject::GetFarClipDistance()const
{
    return mFarDist;
}

/// Set the aspect ratio for the frustum viewport.
/// The ratio between the x and y dimensions of the rectangular area 
/// visible through the frustum is known as aspect ratio: aspect = width / height .
void CameraGraphicObject::SetAspectRatio(double dRatio)
{
    mAspect = dRatio;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}
/// Retrieve the current aspect ratio.
double CameraGraphicObject::GetAspectRatio() const
{
    return mAspect;
}

/// Set frustum offsets, used in stereo rendering.
void CameraGraphicObject::SetFrustumOffset(double offsetX, double offsetY)
{
    mFrustumOffset[0] = offsetX;
    mFrustumOffset[1] = offsetY;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}

/// Retrieve the frustum offsets.
void CameraGraphicObject::GetFrustumOffset(double& offsetX, double& offsetY) const
{
    offsetX = mFrustumOffset[0];
    offsetY = mFrustumOffset[1];
}

/// Sets frustum focal length / near clip distance (used in stereo rendering).
/// focalLength The distance to the focal plane from the frustum in world coordinates.
void CameraGraphicObject::SetFocalLengthScale(double focalLength)
{
    mFocalLengthScale = focalLength;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}

/// Returns focal length of frustum.
double CameraGraphicObject::GetFocalLengthScale() const
{
    return mFocalLengthScale;
}

/// Set the orthographic window settings, for use with orthographic rendering only. 
void CameraGraphicObject::SetOrthoWindow(double w, double h)
{
    mOrthoHeight = h;
    mAspect = w / h;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}

/// Gets the orthographic window height, for use with orthographic rendering only. 
double CameraGraphicObject::GetOrthoWindowHeight() const
{
    return mOrthoHeight;
}

/// Gets the orthographic window width, for use with orthographic rendering only. 
double CameraGraphicObject::GetOrthoWindowWidth() const
{
    return mOrthoHeight * mAspect;
}

/// translate the camera
void CameraGraphicObject::Translate(const Vector3D& vTranslate)
{
    //Matrix4x4 viewMatrix = GetViewMatrix();
    //if (!viewMatrix.HasInverse())
    //{
    //    return;
    //}
    //Matrix4x4 matrix = viewMatrix;
    //matrix.Prepend(MakeTranslate(-vTranslate));
    //matrix.Prepend(viewMatrix.Inverse());
    Matrix4x4 matrix(MakeTranslate(-vTranslate));
    mEye = matrix.Transform(mEye);
    mLookAt = matrix.Transform(mLookAt);

    mViewMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}

/// \brief zoom in or zoom out
/// return true if execute the zoom operation, else return false
bool CameraGraphicObject::Zoom(double dfactor)
{
    if (dfactor < 1e-15) return false;

    mZoomFactor = dfactor;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
    return true;
}

double CameraGraphicObject::GetZoomFactor() const
{
    return mZoomFactor;
}

/// \brief rotate camera
void CameraGraphicObject::Rotate(double dAngle,const Vector3D& vAxis)
{
    this->Rotate(Quaternion(dAngle, vAxis));
}
/// \brief rotate camera
void CameraGraphicObject::Rotate(const Quaternion& myQuaternion)
{
    Matrix4x4 matrix = GetViewMatrix();
    if (!matrix.HasInverse())
    {
        return;
    }
    
    Matrix4x4 myMatrix = myQuaternion.ToMatrix();
    matrix.Prepend(MakeTranslate(Point3D(0.0,0.0,0.0) - matrix.Transform(mLookAt)));
    matrix = matrix.Inverse() * myMatrix * matrix;
    mEye = matrix.Transform(mEye);
    mUp = matrix.Transform(mUp);
    mViewMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}

void CameraGraphicObject::AdaptFovViaAspect(double aspect)
{
    if (aspect < 1e-15) return;
    if (fabs(aspect - mAspect) <= 1e-15) return;

    if (mProjType == Perspective)
    {
        double halfH = tan(mFovY * 0.5) * mNearDist;
        double halfAdaptiveLen = mAspect >= 1.0 ? halfH : 
            halfH * mAspect;
        double newHalfH = aspect >= 1.0 ? halfAdaptiveLen : halfAdaptiveLen / aspect;
        if (newHalfH != halfH){
            double fovY = atan(newHalfH / mNearDist) * 2;
            if (fovY > PI) return;
            mFovY = fovY;
        }
    }
    else
    {
        double adaptiveEdgeLen = mAspect >= 1.0 ? mOrthoHeight : 
            mOrthoHeight * mAspect;
        mOrthoHeight = aspect >= 1.0 ? adaptiveEdgeLen : adaptiveEdgeLen / aspect;
    }
    mAspect = aspect;
    mProjMatrixOutOfDate = true;
    mWorldCornersOutOfDate = true;
}

/// \brief set projection type, either Orthographic or Perspective.
void CameraGraphicObject::SetProjectionType(ProjectionType type)
{
   mProjType = type;
   mProjMatrixOutOfDate = true;
   mWorldCornersOutOfDate = true;
}
// \brief Get Projection type
ProjectionType CameraGraphicObject::GetProjectionType() const
{
    return mProjType;
}

/// \brief get the reference of view matrix
Matrix4x4 CameraGraphicObject::GetViewMatrix() const
{
    if (mViewMatrixOutOfDate)
    {
        UpdateViewMatrix_i();
    }
    return mViewMatrix;
}
/// \brief get the reference of projection matrix
Matrix4x4 CameraGraphicObject::GetProjectionMatrix() const
{
    if (mProjMatrixOutOfDate) 
    {
        UpdateProjectionMatrix_i();
    }
    return mProjMatrix;
}

/// \brief get the product of view matrix and projection matrix
Matrix4x4 CameraGraphicObject::GetViewProjectionMatrix() const
{
    return GetProjectionMatrix() * GetViewMatrix();
}

/// \brief get 8 frustum corners in World coordinate
/// The corners are ordered as follows: 
/// bottom-left near, bottom-right near, top-right near, top-left near, 
/// bottom-left far, bottom-right far, top-right far, top-left far.
const Point3D* CameraGraphicObject::GetWorldFrustumCorners() const
{
    if (mWorldCornersOutOfDate)
    {
        UpdateWorldCorners_i();
    }
    return mWorldFrustumCorners;
}

void CameraGraphicObject::UpdateViewMatrix_i() const
{
    Vector3D zaxis = mEye - mLookAt;
    zaxis.Normalize();
    Vector3D xaxis = mUp.CrossProduct(zaxis);
    xaxis.Normalize();
    Vector3D yaxis = zaxis.CrossProduct(xaxis);
    mViewMatrix.SetIdintity();
    mViewMatrix.m[0][0] = xaxis.x;
    mViewMatrix.m[1][0] = xaxis.y;
    mViewMatrix.m[2][0] = xaxis.z;
    mViewMatrix.m[3][0] = -(xaxis.DotProduct(mEye - Point3D(0,0,0)));
    mViewMatrix.m[0][1] = yaxis.x;
    mViewMatrix.m[1][1] = yaxis.y;
    mViewMatrix.m[2][1] = yaxis.z;
    mViewMatrix.m[3][1] = -(yaxis.DotProduct(mEye - Point3D(0,0,0)));
    mViewMatrix.m[0][2] = zaxis.x;
    mViewMatrix.m[1][2] = zaxis.y;
    mViewMatrix.m[2][2] = zaxis.z;
    mViewMatrix.m[3][2] = -(zaxis.DotProduct(mEye - Point3D(0,0,0)));
    mViewMatrix.m[0][3] = 0;
    mViewMatrix.m[1][3] = 0;
    mViewMatrix.m[2][3] = 0;
    mViewMatrix.m[3][3] = 1;
    mViewMatrixOutOfDate = false;
}

void CameraGraphicObject::UpdateProjectionMatrix_i() const
{
    double left, right, bottom, top;
    CalcProjectionParameters_i(left, right, bottom, top);

    double inv_w = 1 / (right - left);
    double inv_h = 1 / (top - bottom);
    double inv_d = 1 / (mFarDist - mNearDist);

    mProjMatrix.SetIdintity();
    if (mProjType == Perspective)
    {
        // Calc matrix elements
        double A = 2 * mNearDist * inv_w;
        double B = 2 * mNearDist * inv_h;
        double C = (right + left) * inv_w;
        double D = (top + bottom) * inv_h;
        double q, qn;
        if (mFarDist == 0)
        {
            // Infinite far plane
            q = INFINITE_FAR_PLANE_ADJUST - 1;
            qn = mNearDist * (INFINITE_FAR_PLANE_ADJUST - 2);
        }
        else
        {
            q = - (mFarDist + mNearDist) * inv_d;
            qn = -2 * (mFarDist * mNearDist) * inv_d;
        }
        // NB: This creates 'uniform' perspective projection matrix,
        // which depth range [-1,1], right-handed rules
        //
        // [ A   0   C   0  ]
        // [ 0   B   D   0  ]
        // [ 0   0   q   qn ]
        // [ 0   0   -1  0  ]
        //
        // A = 2 * near / (right - left)
        // B = 2 * near / (top - bottom)
        // C = (right + left) / (right - left)
        // D = (top + bottom) / (top - bottom)
        // q = - (far + near) / (far - near)
        // qn = - 2 * (far * near) / (far - near)
        mProjMatrix.m[0][0] = A;
        mProjMatrix.m[2][0] = C;
        mProjMatrix.m[1][1] = B;
        mProjMatrix.m[2][1] = D;
        mProjMatrix.m[2][2] = q;
        mProjMatrix.m[3][2] = qn;
        mProjMatrix.m[2][3] = -1;
        mProjMatrix.m[3][3] = 0;
    }
    else
    {
        double A = 2 * inv_w;
        double B = 2 * inv_h;
        double C = - (right + left) * inv_w;
        double D = - (top + bottom) * inv_h;
        double q, qn;
        if (mFarDist == 0)
        {
            // Can not do infinite far plane here, avoid divided zero only
            q = - INFINITE_FAR_PLANE_ADJUST / mNearDist;
            qn = - INFINITE_FAR_PLANE_ADJUST - 1;
        }
        else
        {
            q = - 2 * inv_d;
            qn = - (mFarDist + mNearDist)  * inv_d;
        }
        // NB: This creates 'uniform' orthographic projection matrix,
        // which depth range [-1,1], right-handed rules
        //
        // [ A   0   0   C  ]
        // [ 0   B   0   D  ]
        // [ 0   0   q   qn ]
        // [ 0   0   0   1  ]
        //
        // A = 2 * / (right - left)
        // B = 2 * / (top - bottom)
        // C = - (right + left) / (right - left)
        // D = - (top + bottom) / (top - bottom)
        // q = - 2 / (far - near)
        // qn = - (far + near) / (far - near)
        mProjMatrix.m[0][0] = A;
        mProjMatrix.m[3][0] = C;
        mProjMatrix.m[1][1] = B;
        mProjMatrix.m[3][1] = D;
        mProjMatrix.m[2][2] = q;
        mProjMatrix.m[3][2] = qn;
        mProjMatrix.m[3][3] = 1;
    }
    mProjMatrixOutOfDate = false;
}

void CameraGraphicObject::UpdateWorldCorners_i() const
{
    if (mViewMatrixOutOfDate)
    {
        UpdateViewMatrix_i();
    }

    Matrix4x4 eyeToWorld = mViewMatrix.Inverse();
    double left, right, bottom, top;
    CalcProjectionParameters_i(left, right, bottom, top);

    // Calc far palne corners
    double radio = mProjType == Perspective ? mFarDist / mNearDist : 1;
    double farLeft = left * radio;
    double farRight = right * radio;
    double farBottom = bottom * radio;
    double farTop = top * radio;

    //near
    mWorldFrustumCorners[0] = eyeToWorld.Transform(Point3D(left, bottom, -mNearDist));
    mWorldFrustumCorners[1] = eyeToWorld.Transform(Point3D(right, bottom, -mNearDist));
    mWorldFrustumCorners[2] = eyeToWorld.Transform(Point3D(right, top, -mNearDist));
    mWorldFrustumCorners[3] = eyeToWorld.Transform(Point3D(left, top, -mNearDist));
    //far
    mWorldFrustumCorners[4] = eyeToWorld.Transform(Point3D(farLeft, farBottom, -mFarDist));
    mWorldFrustumCorners[5] = eyeToWorld.Transform(Point3D(farRight, farBottom, -mFarDist));
    mWorldFrustumCorners[6] = eyeToWorld.Transform(Point3D(farRight, farTop, -mFarDist));
    mWorldFrustumCorners[7] = eyeToWorld.Transform(Point3D(farLeft, farTop, -mFarDist));
    mWorldCornersOutOfDate = false;
}

void CameraGraphicObject::CalcProjectionParameters_i(double& left, double& right, double& bottom, double& top) const
{
    if (mProjType == Perspective)
    {
        ////handle zoom factor
        double halfH = tan(mFovY * 0.5) * mNearDist;
        halfH /= mZoomFactor;
        double fovY = atan(halfH / mNearDist) * 2.0;
        if (fovY > PI) return;

        double tanThetaY = tan(fovY * 0.5);
        double tanThetaX = tanThetaY * mAspect;

        double nearOffsetX = mFrustumOffset[0] / mFocalLengthScale;
        double nearOffsetY = mFrustumOffset[1] / mFocalLengthScale;
        double half_w = tanThetaX * mNearDist;
        double half_h = tanThetaY * mNearDist;

        left   = - half_w + nearOffsetX;
        right  = + half_w + nearOffsetX;
        bottom = - half_h + nearOffsetY;
        top    = + half_h + nearOffsetY;
    }
    else
    {
        //handle zoom factor
        double orthoHeight = mOrthoHeight / mZoomFactor;

        // Unknown how to apply frustum offset to orthographic camera, just ignore here
        double half_w = orthoHeight * mAspect * 0.5;
        double half_h = orthoHeight * 0.5;

        left   = - half_w;
        right  = + half_w;
        bottom = - half_h;
        top    = + half_h;
    }
}
TPS_END_NAMESPACE  // end namespace tps