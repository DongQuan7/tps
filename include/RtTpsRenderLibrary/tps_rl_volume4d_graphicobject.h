////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_volume4d_graphicobject.h
/// 
///  \brief class Volume4dGraphicObejct declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#ifndef _TPS_RL_VOLUME4D_GRAPHICOBJECT_H_
#define _TPS_RL_VOLUME4D_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include <vector>

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC Volume4dGraphicObject : public TpsGraphicObjectBase {
public:
    Volume4dGraphicObject();
    virtual ~Volume4dGraphicObject();
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

public:
    void SetCurrentVolume(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO);
    void SetMainVolume(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO);
    void AddVolume(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO);
	void ClearAllVolumes();

    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> GetMainVolume() const;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> GetCurrentVolume() const;

private:
    std::vector<std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>>   m_vecVolumePtr;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>                m_spMainVolume;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>                m_spCurrentVolume;
};
TPS_END_NAMESPACE

#endif //_TPS_RL_CROSSHAIR_GRAPHICOBJECT_H_
