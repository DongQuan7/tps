//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  SHI CHUNHUI chunhui.shi@united-imaging.com
///
///  \file    tps_rl_ezcameragraphicobject.h
///  \brief   Camera graphic object in Easy Plan Module
///
///  \version 1.0
///  \date    2015/3/18
///  \{
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEzCameraGraphicObject : public TpsGraphicObjectBase {
public:
    TpsEzCameraGraphicObject();
    virtual ~TpsEzCameraGraphicObject();

    virtual bool Initialize();

    void PlaceCamera(const Mcsf::MedViewer3D::Point3D &ptEye,
        const Mcsf::MedViewer3D::Point3D& ptLookAt,
        const Mcsf::MedViewer3D::Vector3D& vUp);

    Mcsf::MedViewer3D::Point3D GetEye() const;

    Mcsf::MedViewer3D::Point3D GetLookAt() const;

    Mcsf::MedViewer3D::Vector3D GetUpOrientation() const;

    double GetFOV(double aspect) const;

    void SetScale(double dScale);
    double GetScale() const;

    void Rotate(const Mcsf::MedViewer3D::Quaternion &);

    void Translate(const Mcsf::MedViewer3D::Vector3D& vecTranslate);

    Mcsf::MedViewer3D::Matrix4x4 GetViewProjectionMat(int width , int height);

private:
    Mcsf::MedViewer3D::Matrix4x4 GetViewMatrix_i() const;

    double                                  mEyeToLookAtDist;
    double                                  mFov;
    double                                  mZoomFactor;
};

TPS_END_NAMESPACE
