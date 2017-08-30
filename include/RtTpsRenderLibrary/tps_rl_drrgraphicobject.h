////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_drrgraphicobject.h
/// 
///  \brief class DrrGraphicObejct declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE
class TPS_RL_DECLSPEC DrrGraphicObejct : public TpsGraphicObjectBase {
public:
    DrrGraphicObejct();
    ~DrrGraphicObejct();

    ////////////////////////////////////////////////////////////////////////////
    //// \brief place the camera in world coord;
    //void PlaceCamera(const Mcsf::MedViewer3D::Point3D &ptEye,
    //    const Mcsf::MedViewer3D::Point3D& ptLookAt,
    //    const Mcsf::MedViewer3D::Vector3D& vUp);

    //void SetFov(const double &);

    //Mcsf::MedViewer3D::Point3D GetEye() const;
    //Mcsf::MedViewer3D::Point3D GetLookAt() const;
    //Mcsf::MedViewer3D::Vector3D GetUpOrientation() const;
    //double GetFov() const;

    //void Rotate(const Mcsf::MedViewer3D::Quaternion &);

    //Mcsf::MedViewer3D::Matrix4x4 GetViewProjectionMat(int width , int height);

    void SetWindowing(double windowWidth , double windowCenter);

    void GetWindowing(double *windowWidth, double *windowCenter);
private:
    //Mcsf::MedViewer3D::Point3D           mLookAt;
    //Mcsf::MedViewer3D::Point3D           mEye;
    //Mcsf::MedViewer3D::Point3D           mUp;
    //double                               mFov;
    double                                  mWidowWidth;
    double                                  mWinowCenter;
};
TPS_END_NAMESPACE