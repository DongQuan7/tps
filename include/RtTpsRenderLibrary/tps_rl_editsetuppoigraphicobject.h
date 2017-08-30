////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_editsetuppoigraphicobject.h
/// 
///  \brief class EditSetupPtGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/03/31
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EDITSETUPPOIGRAPHICOBJECT_H_
#define TPS_RL_EDITSETUPPOIGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC EditSetupPtGraphicObject : public TpsGraphicObjectBase {
public:
    EditSetupPtGraphicObject();
    virtual ~EditSetupPtGraphicObject();

    virtual bool Initialize();
    virtual bool Finalize();

    void SetPosition(const Mcsf::MedViewer3D::Point2D &pt);

    const Mcsf::MedViewer3D::Point2D &GetPosition() const;

    void SetVisible(bool);

    bool GetVisible() const; 

private:
    Mcsf::MedViewer3D::Point2D       mPosition;
    bool                             mVisible;


private:
    TPS_DISALLOW_COPY_AND_ASSIGN(EditSetupPtGraphicObject);


};
TPS_END_NAMESPACE




#endif