////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_applicator_graphic_object.h
/// 
///  \brief class ApplicatorGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_APPLICATOR_GRAPHIC_OBJECT_H_
#define TPS_RL_APPLICATOR_GRAPHIC_OBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class ApplicatorGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC ApplicatorGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    ApplicatorGraphicObject();
    virtual ~ApplicatorGraphicObject();

    std::string GetName() const;
    void SetName(const std::string& name);

    RtDbDef::APPLICATOR_APERTURE_SHAPE GetShapeType() const;
    void SetShapeType(RtDbDef::APPLICATOR_APERTURE_SHAPE shapeType);

    double GetOpening() const;
    void SetOpening(double val);

    double GetOpeningX() const;
    void SetOpeningX(double val);

    double GetOpeningY() const;
    void SetOpeningY(double val);
private:
    std::string                     mName;
    RtDbDef::APPLICATOR_APERTURE_SHAPE       mShapeType;
    double                          mOpening; //FOR SYM_SQUARE || SYM_CIRCULAR
    double                          mOpeningX;//FOR SYM_RECTANGLE
    double                          mOpeningY;//FOR SYM_RECTANGLE
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ApplicatorGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif