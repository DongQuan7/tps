////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_fw_graphicobjectbase.h
/// 
///  \brief class TpsGraphicObjectBase declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/01
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_GRAPHICOBJECTBASE_H
#define TPS_FW_GRAPHICOBJECTBASE_H

#include "RtTpsFramework/tps_fw_defs.h"


#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/graphic_object_base.h"

namespace Mcsf {
    namespace MedViewer3D {
        class Quaternion;
        class Point3D;
        class Vector3D;
    }
}
TPS_BEGIN_NAMESPACE
class TpsGraphicObjectBase : public Mcsf::MedViewer3D::GraphicObjBase {
public:
    TpsGraphicObjectBase() : mDirty(false) {}
    virtual ~TpsGraphicObjectBase(){}
    virtual bool Initialize(){return false;};
    virtual bool Finalize(){return false;};

    virtual void SetDirty(bool bFlag) { mDirty = bFlag; }
    virtual bool IsDirty() const { return mDirty; }
private:
    virtual bool HitTest(double ,double ,double ,double* ) {return false;}
    virtual void CenterRotate(const Mcsf::MedViewer3D::Quaternion& ) {};
    virtual void Translate(const Mcsf::MedViewer3D::Vector3D& ) {};
    virtual void Scale(const Mcsf::MedViewer3D::Vector3D& ) {};
    virtual void SetInitializedPostion(const Mcsf::MedViewer3D::Point3D &,
        const Mcsf::MedViewer3D::Vector3D &,
        const Mcsf::MedViewer3D::Quaternion &) {};

    bool mDirty;
};
TPS_END_NAMESPACE
#endif