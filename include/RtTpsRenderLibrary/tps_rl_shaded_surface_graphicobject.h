//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_shaded_surface_graphicobject.h
///  \brief   shaded surface grophic object
///
///  \version 1.0
///  \date    Dec. 22, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHADED_SURFACE_GRAPHICOBJECT_H_
#define TPS_RL_SHADED_SURFACE_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"


TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC ShadedSurfaceGraphicObject : public TpsGraphicObjectBase {
public:
    ShadedSurfaceGraphicObject();

    virtual ~ShadedSurfaceGraphicObject();

    void SetTransParency( const float &);

    float GetTransparency() const;

    void SetLightPosition(const Mcsf::MedViewer3D::Point3D &);

    Mcsf::MedViewer3D::Point3D GetLightPosition() const;

    void SetLightColor(float color[3]);

    float *GetLightColor() const;

    void SetColor(float color[3]);

    float *GetColor() const;

    void SetExtent(const float &);

    float GetExtent() const;

    void SetLightFactor(float factor[4]);

    void GetLightFactor(float factor[4]) const;

    void SetVisible(bool );

    bool GetVisible() const;

    void SetDisplayValue(float minCt, float maxCt);

    void GetDisplayValue(float *minCt, float *maxCt);

//     void SetZoomFactor(float fZoomFator);
//     float GetZoomFactor() const;

private:

    float                                   mTransparency;
    float                                   mMinDisplayValue;
    float                                   mMaxDisplayValue;
    float                                   mExtent;
    bool                                    mIsVisible;
    float                                   mSampleColor[3];
    float                                   mLightColor[3];
    float                                   mLightFactor[4];
    Mcsf::MedViewer3D::Point3D              mLightPosition;
    //float                                   mZoomFactor;
};

TPS_END_NAMESPACE
#endif