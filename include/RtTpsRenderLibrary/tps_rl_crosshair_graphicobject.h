////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_crosshair_graphicobject.h
/// 
///  \brief class CrosshairGraphicObejct declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////

#ifndef _TPS_RL_CROSSHAIR_GRAPHICOBJECT_H_
#define _TPS_RL_CROSSHAIR_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC CrosshairGraphicObject : public TpsGraphicObjectBase {
public:
    static const float HIT_TEST_MARGIN;// = 0.015f;
    static const float OBLIQUE_ROTATION_REGION;// = 0.1f;
    enum CROSSHAIR_MODE
    {
        CROSSHAIR_ORTHOGONAL = 0,
        CROSSHAIR_OBLIQUE = 1,
        CROSSHAIR_SINGLE_LINE = 2,
    };
    enum HITTED_MODE
    {
        HITTED_NOTHING = 0,
        HITTED_AXIS_Y,
        HITTED_AXIS_Y_Top,
        HITTED_AXIS_Y_Bottom,
        HITTED_AXIS_X,
        HITTED_AXIS_X_Left,
        HITTED_AXIS_X_Right,
        HITTED_CENTER,
    };
public:
    CrosshairGraphicObject();
    virtual ~CrosshairGraphicObject();
    //////////////////////////////////////////////////////////////////////////
    // virtual methods
    virtual bool Initialize();
    virtual bool Finalize();
    virtual bool HitTest(double ,double ,double ,double* ) {return false;}

    virtual void SetInitializedPostion(const Mcsf::MedViewer3D::Point3D& /*myCenterPoint*/,
        const Mcsf::MedViewer3D::Vector3D& /*mySize*/,
        const Mcsf::MedViewer3D::Quaternion& /*myQuaternion*/) { }
    /// \brief Rotate the graphic object
    virtual void CenterRotate(const Mcsf::MedViewer3D::Quaternion& /*myQuaternion*/) { }

    /// \brief Rotate the graphic object
    virtual void Rotate(const Mcsf::MedViewer3D::Quaternion& /*myQuaternion*/){ }

    /// \brief Translate the object
    virtual void Translate(const Mcsf::MedViewer3D::Vector3D& /*myTranslate*/) { }

    /// \brief Scale the object
    virtual void Scale(const Mcsf::MedViewer3D::Vector3D& /*myScale*/) { }

    void GetCrosshairPoint3d(Mcsf::MedViewer3D::Point3D* pCrosshairPt3d) const;
    void GetCrossFourPoints(Mcsf::MedViewer3D::Point3D* point1, Mcsf::MedViewer3D::Point3D* point2, Mcsf::MedViewer3D::Point3D* point3, Mcsf::MedViewer3D::Point3D* point4) const;
    void GetCrosshairPoint2d(Mcsf::MedViewer3D::Point2D* pCrosshairPt2d) const;
    void GetAngles(float *pAngleX, float *pAngleY) const;
    void GetXColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha) const;
    void GetYColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha) const;
    void GetCrosshairMode(CROSSHAIR_MODE* pCrosshairMode) const{ *pCrosshairMode = m_nCrosshairMode;}
    bool Visible() const { return m_bVisible; }
    void GetNormalInOrtho(Mcsf::MedViewer3D::Vector3D* pNormalInOrtho) const {*pNormalInOrtho = m_v3dNormalInOrtho;}
    void GetXInOrtho(Mcsf::MedViewer3D::Vector3D* pXInOrtho) const {*pXInOrtho = m_v3dXInOrtho;}
    int  IsHit();

    void SetCrosshairPoint3d(Mcsf::MedViewer3D::Point3D& pt3dCrosshair);
    void SetCrossFourPoints(Mcsf::MedViewer3D::Point3D& point1, Mcsf::MedViewer3D::Point3D& point2, Mcsf::MedViewer3D::Point3D& point3, Mcsf::MedViewer3D::Point3D& point4);
    void SetCrosshairPoint2d(Mcsf::MedViewer3D::Point2D& pt2dCrosshair);
    void SetAngles(float fAngleX, float fAngleY);
    void SetXColor(float fRed, float fGreen, float fBlue, float fAlpha);
    void SetYColor(float fRed, float fGreen, float fBlue, float fAlpha);
    void SetCrosshairMode(CROSSHAIR_MODE nCrosshairMode) { m_nCrosshairMode = nCrosshairMode;}
    void SetVisible(bool bVisible) { m_bVisible = bVisible; }
    void SetNormalInOrtho(Mcsf::MedViewer3D::Vector3D& v3dNormalInOrtho) { m_v3dNormalInOrtho = v3dNormalInOrtho;}
    void SetXInOrtho(Mcsf::MedViewer3D::Vector3D& v3dXInOrtho) { v3dXInOrtho.GetNormalize(m_v3dXInOrtho);}

    int HitTest(float x, float y);
    void SetHittedMode(HITTED_MODE mode);

    inline void SetIsReady(bool isReady) { m_bReady = isReady; }
    inline bool IsReady() const{ return m_bReady; }
private:
    // in world coordinate
    //                                      (1, 1, 1)
    //
    //                  (0, 0, 0)
    //
    // (-1, -1, -1)

    Mcsf::MedViewer3D::Point3D          m_pt3dCrosshair;
    Mcsf::MedViewer3D::Point3D          m_topPoint,m_bottomPoint,m_leftPoint,m_rightPoint;
    std::vector<Mcsf::MedViewer3D::Point3D>    m_pointX;
    std::vector<Mcsf::MedViewer3D::Point3D>    m_pointY;

    // Plane coordinate:
    //                      ( 1, 1)
    //          ___________________
    //         |   |           |   |
    //         |   |    MPR    |   |
    //         |   |           |   |
    //         |___|___________|___|
    //          ( 0, 0)

    Mcsf::MedViewer3D::Point2D          m_pt2dCrosshair;
    float                               m_fAngleX;
    float                               m_fAngleY;
    float                               m_vecXColor[4];
    float                               m_vecYColor[4];
    int                                 m_nHitXAxis;
    int                                 m_nHitYAxis;
    int                                 m_nRotate;
    CROSSHAIR_MODE                      m_nCrosshairMode;
    bool                                m_bVisible;
    bool                                m_bReady;

    Mcsf::MedViewer3D::Vector3D         m_v3dNormalInOrtho;
    Mcsf::MedViewer3D::Vector3D         m_v3dXInOrtho;
};
TPS_END_NAMESPACE

#endif //_TPS_RL_CROSSHAIR_GRAPHICOBJECT_H_
