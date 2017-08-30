////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_camera_graphic_object.h
/// 
///  \brief class CameraGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/01/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CAMERA_GRAPHIC_OBJECT_H__
#define TPS_RL_CAMERA_GRAPHIC_OBJECT_H__

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE

enum ProjectionType
{
    Orthographic,
    Perspective
};

// A tps graphic object simple version of Camera
class TPS_RL_DECLSPEC CameraGraphicObject : public TpsGraphicObjectBase
{
public:
    //the constructor and destructor
    CameraGraphicObject();
    virtual ~CameraGraphicObject();

    /// Set the position of the camera.
    void SetEye(const Mcsf::MedViewer3D::Point3D& eye);
    /// Retrieve the position of the camera. 
    const Mcsf::MedViewer3D::Point3D GetEye() const;

    /// Set the looking direction of the camera.
    void SetLookAt(const Mcsf::MedViewer3D::Point3D& At);
    /// Retrieve the looking direction of the camera.
    const Mcsf::MedViewer3D::Point3D GetLookAt() const;

    /// Set the up-direction of the camera.
    void SetUpDirection(const Mcsf::MedViewer3D::Vector3D& Updirection);
    /// Retrieve the up-direction of the camera.
    const Mcsf::MedViewer3D::Vector3D GetUpDirection() const;

    /// Set the Y-dimension Field Of View (FOV) of the frustum,
    /// for use with perspective rendering only.
    /// note that the real fov will be affected by zoom scale, 
    /// the dFovy is the value when zoom factor equals to 1.0.
    void SetFovY(double dFovy);
    /// Retrieve the Y-dimension Field Of View (FOV) of the frustum,
    /// for use with perspective rendering only.
    /// note that the real fov will be affected by zoom scale, 
    /// the Fovy to be retrieved is the value when zoom factor equals to 1.0.
    double GetFovY() const;

    /// Set the position of the near clipping plane.
    void SetNearClipDistance(double dNearDist);
    /// Retrieve the position of the near clipping plane.
    double GetNearClipDistance() const;

    /// Set the distance to the far clipping plane.
    void SetFarClipDistance(double dFarDist);
    /// Retrieve the distance from the frustum to the far clipping plane.
    double GetFarClipDistance()const;

    /// Set the aspect ratio for the frustum viewport.
    /// The ratio between the x and y dimensions of the rectangular area 
    /// visible through the frustum is known as aspect ratio: aspect = width / height .
    void SetAspectRatio(double dRatio);
    /// Retrieve the current aspect ratio.
    double GetAspectRatio() const;

    /// Set frustum offsets, used in stereo rendering.
    void SetFrustumOffset(double offsetX, double offsetY);

    /// Retrieve the frustum offsets.
    void GetFrustumOffset(double& offsetX, double& offsetY) const;

    /// Sets frustum focal length / near clip distance (used in stereo rendering).
    /// focalLength The distance to the focal plane from the frustum in world coordinates.
    void SetFocalLengthScale(double focalLength = 1.0);

    /// Returns focal length of frustum / near clip distance.
    double GetFocalLengthScale() const;

    /// Set the orthographic window settings, for use with orthographic rendering only. 
    /// note that the real window size will be affected by zoom scale, 
    /// the w/h to be set is the value when zoom factor equals to 1.0.
    void SetOrthoWindow(double w, double h);
    /// Get the orthographic window height, for use with orthographic rendering only. 
    /// note that the real window size will be affected by zoom scale, 
    /// the height to be retrieved is the value when zoom factor equals to 1.0.
    double GetOrthoWindowHeight() const;
    /// Get the orthographic window width, for use with orthographic rendering only. 
    /// note that the real window size will be affected by zoom scale, 
    /// the width to be retrieved is the value when zoom factor equals to 1.0.
    double GetOrthoWindowWidth() const;

    /// \brief rotate camera
    void Rotate(double dAngle,const Mcsf::MedViewer3D::Vector3D& vAxis);
    /// \brief rotate camera
    void Rotate(const Mcsf::MedViewer3D::Quaternion& myQuaternion);

    /// translate the camera
    void Translate(const Mcsf::MedViewer3D::Vector3D& vTranslate);

    /// \brief zoom in or zoom out
    /// return true if execute the zoom operation, else return false
    bool Zoom(double dfactor);
    /// \brief get zoom factor
    double GetZoomFactor() const;

    /// \brief adapt FOV by aspect
    /// to be commented by GML
    void AdaptFovViaAspect(double aspect);

    /// \brief set projection type, either Orthographic or Perspective.
    void SetProjectionType(ProjectionType type);
    // \brief Get Projection type
    ProjectionType GetProjectionType() const;

    /// \brief get the reference of view matrix
    Mcsf::MedViewer3D::Matrix4x4 GetViewMatrix() const;
    /// \brief get the reference of projection matrix
    Mcsf::MedViewer3D::Matrix4x4 GetProjectionMatrix() const;
    /// \brief get the product of view matrix and projection matrix
    Mcsf::MedViewer3D::Matrix4x4 GetViewProjectionMatrix() const;
    /// \brief get 8 frustum corners in World coordinate
    /// The corners are ordered as follows: 
    /// bottom-left near, bottom-right near, top-right near, top-left near, 
    /// bottom-left far, bottom-right far, top-right far, top-left far.
    const Mcsf::MedViewer3D::Point3D* GetWorldFrustumCorners() const;
private:
    void UpdateViewMatrix_i() const;
    void UpdateProjectionMatrix_i() const;
    void UpdateWorldCorners_i() const;
    void CalcProjectionParameters_i(double& left, double& right, double& bottom, double& top) const;

private:
    /// The camera position in world coordinate
    Mcsf::MedViewer3D::Point3D mEye;
    /// Look at point in world coordinate
    Mcsf::MedViewer3D::Point3D mLookAt;
    /// The up direction in world coordinate
    Mcsf::MedViewer3D::Vector3D mUp;

    /// Orthographic or perspective
    ProjectionType mProjType;
    /// y-direction field-of-view (default 45), radian
    double mFovY;
    /// Far clip distance - default 10000
    double mFarDist;
    /// Near clip distance - default 100
    double mNearDist;
    /// x/y viewport ratio - default 1.3333
    double mAspect;
    /// Ortho height size (world units)
    double mOrthoHeight;
    /// Off-axis frustum center offset - default (0.0, 0.0)
    double mFrustumOffset[2];
    /// Focal length of frustum / near clip distance(for stereo rendering, defaults to 1.0)
    double mFocalLengthScale;

    /// 8 frustum corners in World coordinate
    /// The corners are ordered as follows: 
    /// bottom-left near, bottom-right near, top-right near, top-left near, 
    /// bottom-left far, bottom-right far, top-right far, top-left far.
    mutable Mcsf::MedViewer3D::Point3D mWorldFrustumCorners[8];
    /// view matrix
    mutable Mcsf::MedViewer3D::Matrix4x4 mViewMatrix;
    /// projection matrix
    mutable Mcsf::MedViewer3D::Matrix4x4 mProjMatrix;
    ///Flags
    mutable bool mViewMatrixOutOfDate;
    mutable bool mProjMatrixOutOfDate;
    mutable bool mWorldCornersOutOfDate;

    // zoom scale
    double mZoomFactor;
};

TPS_END_NAMESPACE
#endif // tps_rl_camera_graphic_object_h__