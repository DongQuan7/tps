//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_cornerinfographicobjectconverter.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_graphicobjectconverter.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryCrosshairGOConverter::TpsGalleryCrosshairGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType) {

}

TpsGalleryCrosshairGOConverter::~TpsGalleryCrosshairGOConverter(){

}

GraphicObjectPtr TpsGalleryCrosshairGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<CrosshairGraphicObject>(
        new CrosshairGraphicObject());
}

bool TpsGalleryCrosshairGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    m_strImageUID = it->second;
    return true;
}

bool TpsGalleryCrosshairGOConverter::Update_i(GraphicObjectPtr go){
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairGO == nullptr) return false;

    GO_TYPE mprGoType[3] = {GO_TYPE_GALLERY_MPR_AXIAL, GO_TYPE_GALLERY_MPR_CORONAL, GO_TYPE_GALLERY_MPR_SAGITTAL};
    std::shared_ptr<MPRGraphicObject> spMprGO[3] = {nullptr, nullptr, nullptr};
    int i;
    std::string strKey;
    for (i = 0; i < 3; i++) {
        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(mprGoType[i]);
        go = mModelWarehouse->GetModelObject(strKey);
        spMprGO[i] = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    }

    int index = 0;
    float fColorCoronal[4]  = {0.0f,          0xC6 / 255.0f,  1.0f,          1.0f};      // blue
    float fColorSagittal[4] = {0x5D / 255.0f, 0xD4 / 255.0f,  0x2F / 255.0f, 1.0f};      // green
    float fColorAxial[4]    = {1.0f,          60.0f / 255.0f, 0.0f,          1.0f};      // red
    if (this->mGOType == GO_TYPE_GALLERY_CROSSHAIR_AXIAL) {
        index = 0;
        spCrosshairGO->SetXColor(fColorCoronal[0], fColorCoronal[1], fColorCoronal[2], fColorCoronal[3]);
        spCrosshairGO->SetYColor(fColorSagittal[0], fColorSagittal[1], fColorSagittal[2], fColorSagittal[3]);
        spCrosshairGO->SetDirty(true);
    }
    else if (this->mGOType == GO_TYPE_GALLERY_CROSSHAIR_CORONAL) {
        index = 1;
        spCrosshairGO->SetXColor(fColorAxial[0], fColorAxial[1], fColorAxial[2], fColorAxial[3]);
        spCrosshairGO->SetYColor(fColorSagittal[0], fColorSagittal[1], fColorSagittal[2], fColorSagittal[3]);
        spCrosshairGO->SetDirty(true);
    }
    else if (this->mGOType == GO_TYPE_GALLERY_CROSSHAIR_SAGITTAL) {
        index = 2;
        spCrosshairGO->SetXColor(fColorAxial[0], fColorAxial[1], fColorAxial[2], fColorAxial[3]);
        spCrosshairGO->SetYColor(fColorCoronal[0], fColorCoronal[1], fColorCoronal[2], fColorCoronal[3]);
        spCrosshairGO->SetDirty(true);
    }

    if (spMprGO[index] == nullptr){
        TPS_LOG_DEV_ERROR <<"[ERROR][TpsGalleryCrosshairGOConverter][Convert]: "
            <<"Failed to get the MPR graphic objects.";
        return false;
    }
    Mcsf::MedViewer3D::Point2D pt2d;
    Mcsf::MedViewer3D::Point3D pt3d;
    spMprGO[index]->GetCrosshairPoint(&pt3d);
    spMprGO[index]->GetCrosshairPositionInPlane(&pt2d);
    double dAngleX, dAngleY;
    spMprGO[index]->GetCrosshairAngleXY(&dAngleX, &dAngleY);
    spCrosshairGO->SetCrosshairPoint3d(pt3d);
    spCrosshairGO->SetCrosshairPoint2d(pt2d);
    spCrosshairGO->SetAngles(dAngleX, dAngleY);
    spCrosshairGO->SetVisible(true);
    //spCrosshairGO->SetVisibilty(true);

    Mcsf::MedViewer3D::Point3D vertices[4];
    Mcsf::MedViewer3D::Vector3D vecNormal, vecXAxis;
    spMprGO[index]->GetVertices(vertices);
    spMprGO[index]->GetNormal(&vecNormal);
    vecXAxis = vertices[1] - vertices[0];
    spCrosshairGO->SetNormalInOrtho(vecNormal);
    spCrosshairGO->SetXInOrtho(vecXAxis);

    // recalculate if all 3 mpr is ready
    if (spMprGO[0] != nullptr && spMprGO[1] != nullptr && spMprGO[2] != nullptr) {
        Mcsf::MedViewer3D::Point3D axialCenter = spMprGO[0]->GetCenter();
        Mcsf::MedViewer3D::Point3D coronalCenter = spMprGO[1]->GetCenter();
        Mcsf::MedViewer3D::Point3D sagittalCenter = spMprGO[2]->GetCenter();
        Mcsf::MedViewer3D::Point3D pt3dCenter(sagittalCenter.x, coronalCenter.y, axialCenter.z);
        Mcsf::MedViewer3D::Point2D pt2dCenter;

        spCrosshairGO->SetCrosshairPoint3d(pt3dCenter);
        //spMprGO[index]->SetCrosshairPoint(pt3dCenter); // this function will move mpr plane
        pt2dCenter = spMprGO[index]->GetPlaneCoordinate(pt3dCenter, false);
        spCrosshairGO->SetCrosshairPoint2d(pt2dCenter);
        spCrosshairGO->SetIsReady(true);

        FIRST_POSTFIX_COMPONENT sectionType[3] = {AXIAL, CORONAL, SAGITTAL};
        GO_TYPE goType;
        for (i = 0; i < 3; i++) {
            if (i == index) {
                // if it is this GO
                continue;
            }
            goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, sectionType[i]);
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
            auto go = mModelWarehouse->GetModelObject(strKey);
            auto spLinkedCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
            if (spLinkedCrosshairGO == nullptr) continue;
            spLinkedCrosshairGO->SetCrosshairPoint3d(pt3dCenter);
            pt2dCenter = spMprGO[i]->GetPlaneCoordinate(pt3dCenter, false);
            spLinkedCrosshairGO->SetCrosshairPoint2d(pt2dCenter);
            spLinkedCrosshairGO->SetIsReady(true);
            spLinkedCrosshairGO->SetDirty(true);
        }
    }

    return true;
}

TPS_END_NAMESPACE  // end namespace tps
