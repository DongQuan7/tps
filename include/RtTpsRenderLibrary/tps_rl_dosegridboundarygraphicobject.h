////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Zuo Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_dosegridboundarygraphicobject.h
/// 
///  \brief class TpsDoseGridGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/11
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_DOSEGRIDBOUNDARYGO_H
#define TPS_RL_DOSEGRIDBOUNDARYGO_H
#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsDoseGridBoundaryGraphicObject : public TpsGraphicObjectBase {
public:
    //the constructor and destructor
    TpsDoseGridBoundaryGraphicObject();
    virtual ~TpsDoseGridBoundaryGraphicObject();

    void SetLeftBottomPoint(const Mcsf::MedViewer3D::Point3D& point);

    Mcsf::MedViewer3D::Point3D GetLeftBottomPoint();

    void SetRightTopPoint(const Mcsf::MedViewer3D::Point3D& point);

    Mcsf::MedViewer3D::Point3D GetRightTopPoint();

    void SetSpacing(const Mcsf::MedViewer3D::Point3D& spacing);

    Mcsf::MedViewer3D::Point3D GetSpacing();

    void SetVisible(bool isVisible);

    bool GetVisible();

    void SetColor(float* color);
    void GetColor(const float*& color) const;

    void SetType(std::string type);
    std::string GeType();

private:
    Mcsf::MedViewer3D::Point3D mLeftBottomPoint;
    Mcsf::MedViewer3D::Point3D mRightTopPoint;
    Mcsf::MedViewer3D::Point3D mSpacing;
    bool                       mIsVisible;
    float                      mColor[3];
    std::string                mType;
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDoseGridBoundaryGraphicObject);
};
TPS_END_NAMESPACE


#endif