////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_hot_spots_graphicobject.h
/// 
///  \brief class HotSpotsGraphicObejct declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/10/15
////////////////////////////////////////////////////////////////

#ifndef _TPS_RL_HOT_SPOTS_GRAPHICOBJECT_H_
#define _TPS_RL_HOT_SPOTS_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"                   // for Mcsf::MedViewer3D::Point3D
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include <vector>

typedef std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>> HotColdContourPointer;

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC HotSpotsGraphicObject : public TpsGraphicObjectBase { 
public:
    HotSpotsGraphicObject();
    virtual ~HotSpotsGraphicObject();
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

    bool Visible() const { return m_bVisible; } 
    void SetVisible(bool bVisible) { m_bVisible = bVisible; }

    HotColdContourPointer GetVecPointsByNameAndPlanUid(const std::string& windowName, const std::string& planUid);

    void HotSpotsGraphicObject::ClearVecPointsByPlanUid(const std::string& planUid);

    std::map<std::string, HotColdContourPointer> m_spVecPoints_Axial_Map;
    std::map<std::string, HotColdContourPointer> m_spVecPoints_Coronal_Map;
    std::map<std::string, HotColdContourPointer> m_spVecPoints_Sagittal_Map;

private:
    // in patient coordinate
    //std::vector<Mcsf::MedViewer3D::Point3D> m_vecPoints;

    bool                                    m_bVisible;
};
TPS_END_NAMESPACE

#endif //_TPS_RL_HOT_SPOTS_GRAPHICOBJECT_H_
