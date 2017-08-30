////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poigraphicobject.h
/// 
///  \brief class TpsPoiGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/2/12
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_POIGRAPHICOBEJECT
#define TPS_RL_POIGRAPHICOBEJECT

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE
class TPS_RL_DECLSPEC TpsPoiGraphicObject : public TpsGraphicObjectBase {
public:
    TpsPoiGraphicObject();
    virtual ~TpsPoiGraphicObject();

    void SetColor(float color[4]);

    void GetColor(float color[4]) const;

    void SetVisible(bool);

    bool IsVisible();

    void SetCrossHairVisible(bool);

    bool IsCrossHairVisible();

    void SetActiveFlag(bool);

    bool IsActive();

    void SetPosition(const Mcsf::MedViewer3D::Point3D &);

    Mcsf::MedViewer3D::Point3D GetPosition();

    void SetPoiType(RtDbDef::POI_TYPE);

    RtDbDef::POI_TYPE GetPoiType() const;

    void SetPoiName(const std::string &);

    std::string GetPoiName() const;

private:
    float                         mColor[4];
    bool                          mIsVisible;
    bool                          mIsCrossHairVisible;
    bool                          mIsActive;
    Mcsf::MedViewer3D::Point3D    mPosition;
    RtDbDef::POI_TYPE               mPoiType;
    std::string                   mPoiName;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPoiGraphicObject);

};

TPS_END_NAMESPACE
#endif
