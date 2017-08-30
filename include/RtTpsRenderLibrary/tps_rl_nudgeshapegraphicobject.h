////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_nudgeshapegraphicobject.h
/// 
///  \brief class TpsNudgeShapeGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/03/28
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_NUDGECIRCLEGRAPHICOBJECT_H_
#define TPS_RL_NUDGECIRCLEGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"


#include "McsfMedViewer3DArithmetic/point3d.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsNudgeShapeGraphicObject
/// 
/// \brief the nudge shape graphic object

class TPS_RL_DECLSPEC TpsNudgeShapeGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    TpsNudgeShapeGraphicObject();
    ~TpsNudgeShapeGraphicObject();

    void SetRadius(double radius);
    double GetRadius() const;

    void SetOrigin(const Mcsf::MedViewer3D::Point3D& origin);
    Mcsf::MedViewer3D::Point3D GetOrigin() const;

    void SetColor(float* color);
    void GetColor(float* color) const;

    void SetActive(bool bActive);
    bool IsActive() const;

    void SetShape(int shape);
    int GetShape();

    //set section that the nudge circle belongs to
    FIRST_POSTFIX_COMPONENT GetSection() const { return mSectionType; }
    void SetSection(FIRST_POSTFIX_COMPONENT section) { mSectionType = section; }
private:
    double                          mRadius; //unit: mm
    Mcsf::MedViewer3D::Point3D      mOrigin; //world coordinate
    float                           mColor[4];
    

    FIRST_POSTFIX_COMPONENT         mSectionType;
    bool                            mActive;
    int                             mShape;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsNudgeShapeGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif