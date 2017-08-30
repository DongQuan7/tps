//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  wenqiao.jiang@united-imaging.com
///
///  \file    tps_rl_crosshair_update_operation.cpp
///  \brief   Crosshair movement implementation
///
///  \version 1.0
///  \date    2016.04.09
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_crosshair_update_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_rotate_operation.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps


CrosshairUpdateOperation::CrosshairUpdateOperation(ModelWarehouse* modelWarehouse, const std::string& imageUID)
:m_strImageUID(imageUID),
mModelWarehouse(modelWarehouse){
}

CrosshairUpdateOperation::~CrosshairUpdateOperation(){

}
bool CrosshairUpdateOperation::ModifyGraphicObject() const{

    if (nullptr == mModelWarehouse)
        return false;

    Mcsf::MedViewer3D::Point2D pt2dPosPoint(0, 0);
    Mcsf::MedViewer3D::Vector3D normalCoronal;
    Mcsf::MedViewer3D::Vector3D normalSagittal;
    Mcsf::MedViewer3D::Vector3D normalAxial;
    Mcsf::MedViewer3D::Point3D pt3dnormalCoronal;
    Mcsf::MedViewer3D::Point3D pt3dnormalSagittal;
    Mcsf::MedViewer3D::Point3D pt3dnormalAxial;

    auto goType = GOTypeHelper::ComposeAsGOType(MPR, CORONAL);
    std::string strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(strKey);
    auto spCoronalMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (spCoronalMprGO == nullptr){
        TPS_LOG_DEV_WARNING << "[WARNING][CrosshairUpdateOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
    }

    goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, CORONAL);
    strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairCORONALGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairCORONALGO == nullptr){
        TPS_LOG_DEV_WARNING << "[WARNING][CrosshairUpdateOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
    }

    goType = GOTypeHelper::ComposeAsGOType(MPR, SAGITTAL);
    strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetModelObject(strKey);
    auto spSagittalMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (spSagittalMprGO == nullptr){
        TPS_LOG_DEV_WARNING << "[WARNING][CrosshairUpdateOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
    }

    goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, SAGITTAL);
    strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairSAGITTALGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairSAGITTALGO == nullptr){
        TPS_LOG_DEV_WARNING << "[WARNING][CrosshairUpdateOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
    }

    goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL);
    strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetModelObject(strKey);
    auto spAxialMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (spAxialMprGO == nullptr){
        TPS_LOG_DEV_WARNING << "[WARNING][CrosshairUpdateOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
    }

    goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, AXIAL);
    strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairAXIALGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairAXIALGO == nullptr){
        TPS_LOG_DEV_WARNING << "[WARNING][CrosshairUpdateOperation][SetCrossPointsForOtherPlane]: "
            << "Failed to dynamic cast IGraphicObjects.";
    }


    if (spAxialMprGO != nullptr)
    {
        spAxialMprGO->GetNormal(&normalAxial);
        normalAxial.Normalize();
        pt3dnormalAxial = spAxialMprGO->GetWorldCoordinate(pt2dPosPoint);
    }

    if (spCoronalMprGO != nullptr)
    {
        spCoronalMprGO->GetNormal(&normalCoronal);
        normalCoronal.Normalize();
        pt3dnormalCoronal = spCoronalMprGO->GetWorldCoordinate(pt2dPosPoint);
    }

    if (spSagittalMprGO != nullptr)
    {
        spSagittalMprGO->GetNormal(&normalSagittal);
        normalSagittal.Normalize();
        pt3dnormalSagittal = spSagittalMprGO->GetWorldCoordinate(pt2dPosPoint);
    }

    Mcsf::MedViewer3D::Point3D Axial_Sagittal1(0, 0, 0), Axial_Sagittal2(0, 0, 0);
    Mcsf::MedViewer3D::Point3D Axial_Coronal1(0, 0, 0), Axial_Coronal2(0, 0, 0);
    Mcsf::MedViewer3D::Point3D Sagittal_Coronal1(0, 0, 0), Sagittal_Coronal2(0, 0, 0);
    if (spAxialMprGO != nullptr&&spSagittalMprGO != nullptr)
    {
        GetTwoPointFromLine(normalAxial, pt3dnormalAxial, normalSagittal, pt3dnormalSagittal, Axial_Sagittal1, Axial_Sagittal2);
    }

    if (spAxialMprGO != nullptr&&spCoronalMprGO != nullptr)
    {
        GetTwoPointFromLine(normalAxial, pt3dnormalAxial, normalCoronal, pt3dnormalCoronal, Axial_Coronal1, Axial_Coronal2);
    }

    if (spSagittalMprGO != nullptr&&spCoronalMprGO != nullptr)
    {
        GetTwoPointFromLine(normalSagittal, pt3dnormalSagittal, normalCoronal, pt3dnormalCoronal, Sagittal_Coronal1, Sagittal_Coronal2);
    }
    Mcsf::MedViewer3D::Point3D point1, point2, point3, point4;
    if (spCoronalMprGO != nullptr && spCrosshairCORONALGO != nullptr)
    {
        point1 = Sagittal_Coronal1;
        point2 = Sagittal_Coronal2;
        point3 = Axial_Coronal1;
        point4 = Axial_Coronal2;
        spCrosshairCORONALGO->SetCrossFourPoints(point1, point2, point3, point4);
    }
    if (spSagittalMprGO != nullptr && spCrosshairSAGITTALGO != nullptr)
    {
        point1 = Sagittal_Coronal1;
        point2 = Sagittal_Coronal2;
        point3 = Axial_Sagittal1;
        point4 = Axial_Sagittal2;
        spCrosshairSAGITTALGO->SetCrossFourPoints(point1, point2, point3, point4);
    }
    if (spAxialMprGO != nullptr && spCrosshairAXIALGO != nullptr)
    {
        point1 = Axial_Sagittal1;
        point2 = Axial_Sagittal2;
        point3 = Axial_Coronal1;
        point4 = Axial_Coronal2;
        spCrosshairAXIALGO->SetCrossFourPoints(point1, point2, point3, point4);
    }
    return true;
}

bool CrosshairUpdateOperation::GetTwoPointFromLine(Mcsf::MedViewer3D::Vector3D &normal1, Mcsf::MedViewer3D::Point3D &pt1, Mcsf::MedViewer3D::Vector3D &normal2, Mcsf::MedViewer3D::Point3D &pt2, Mcsf::MedViewer3D::Point3D &outPoint1, Mcsf::MedViewer3D::Point3D &outPoint2) const
{
    double d1 = normal1.x*pt1.x + normal1.y*pt1.y + normal1.z*pt1.z;
    double d2 = normal2.x*pt2.x + normal2.y*pt2.y + normal2.z*pt2.z;

    Mcsf::MedViewer3D::Vector3D crossprod = normal1.CrossProduct(normal2);

    if (crossprod.x == 0 && crossprod.y == 0 && crossprod.z == 0)
        return false;

    double val1 = -1, val2 = 1;
    Mcsf::MedViewer3D::Point3D Vex2(d1, d2, val1), Vex1(d1, d2, val2);
    Mcsf::MedViewer3D::Matrix4x4 modelToVolumea(
        normal1.x, normal2.x, crossprod.x, 0,
        normal1.y, normal2.y, crossprod.y, 0,
        normal1.z, normal2.z, crossprod.z, 0,
        0, 0, 0, 1);

    if (modelToVolumea.HasInverse())
    {
        outPoint1 = modelToVolumea.Inverse()*Vex1;
        outPoint2 = modelToVolumea.Inverse()*Vex2;
        return true;
    }

    /*
    Mcsf::MedViewer3D::Matrix4x4 modelToVolumeb(
    normal1.x, normal2.x, 0, 0,
    normal1.y, normal2.y, 1, 0,
    normal1.z, normal2.z, 0, 0,
    0, 0, 0, 1);
    if (modelToVolumeb.HasInverse())
    {
    outPoint1 = modelToVolumeb.Inverse()*Vex1;
    outPoint2 = modelToVolumeb.Inverse()*Vex2;
    return true;
    }


    Mcsf::MedViewer3D::Matrix4x4 modelToVolumec(
    normal1.x, normal2.x, 1, 0,
    normal1.y, normal2.y, 0, 0,
    normal1.z, normal2.z, 0, 0,
    0, 0, 0, 1);

    if (modelToVolumec.HasInverse())
    {
    outPoint1 = modelToVolumec.Inverse()*Vex1;
    outPoint2 = modelToVolumec.Inverse()*Vex2;
    return true;
    }*/

    return false;

}

TPS_END_NAMESPACE  // end namespace tps
