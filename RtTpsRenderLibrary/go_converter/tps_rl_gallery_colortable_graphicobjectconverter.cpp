//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Jul. 10, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_colortable_graphicobjectconverter.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr_colortable.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryColorTableGOConverter::TpsGalleryColorTableGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), m_spColorTable(nullptr){

}

TpsGalleryColorTableGOConverter::~TpsGalleryColorTableGOConverter(){

}

GraphicObjectPtr TpsGalleryColorTableGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<GraphicObjectVR_ColorTable>(new GraphicObjectVR_ColorTable());
}

bool TpsGalleryColorTableGOConverter::PrepareSourceData_i(const GOConverterParameter& /*para*/){
    return true;     
}

bool TpsGalleryColorTableGOConverter::Update_i(GraphicObjectPtr go){
    auto colorTable = std::dynamic_pointer_cast<GraphicObjectVR_ColorTable>(go);
    if (colorTable == nullptr) return false;

    colorTable->Initialize();
    // TODO : 如果确定这些配置，目前先暂时从3D中挑选一个配置文件信息使用 => 将来根据部位或者Modality区分？
    // Copy from : UIH/appdata/viewer3d/config/LUT/CT/CHEST/CT_RIB_CAGE.xml
    LUTTYPE lutType = MEDVIEWER3D_HSV_MAP;
    // ColorTransferFunction
    ColorTransferFunction colorTF;
    colorTF.Initialize();
    colorTF.SetTableType(lutType);
    std::multimap<double, Point3D> colorPoints;
    colorPoints.insert(pair<double, Point3D>( 187.01,  Point3D(357.545, 0.862, 1)));
    colorPoints.insert(pair<double, Point3D>( 187.012, Point3D(357.545, 0.862, 1)));
    colorPoints.insert(pair<double, Point3D>( 190.687, Point3D(357.545, 0.862, 1)));
    colorPoints.insert(pair<double, Point3D>( 242.186, Point3D( 35.575, 0.886, 1)));
    colorPoints.insert(pair<double, Point3D>( 469.225, Point3D( 35.575, 0,     1)));
    colorPoints.insert(pair<double, Point3D>(1479.102, Point3D( 35.57,  0,     1)));
    colorPoints.insert(pair<double, Point3D>(2808.358, Point3D( 35.575, 0,     1)));    
    auto itColorPoint = colorPoints.begin();
    while (itColorPoint != colorPoints.end()){
        colorTF.AddPoint(itColorPoint->first, itColorPoint->second.x, itColorPoint->second.y,itColorPoint->second.z);
        itColorPoint ++;
    }
    // Opacity
    PiecewiseFunction opacity;
    opacity.Initialize();
    opacity.SetTableType(lutType);
    std::vector<Point2D> opacityPoints;
    opacityPoints.push_back(Point2D( 187.01,  0));
    opacityPoints.push_back(Point2D( 187.012, 0.864));
    opacityPoints.push_back(Point2D( 190.687, 43.310));
    opacityPoints.push_back(Point2D( 242.186, 107.235));
    opacityPoints.push_back(Point2D( 469.225, 146.349));
    opacityPoints.push_back(Point2D(1479.102, 222.133));
    opacityPoints.push_back(Point2D(2808.358, 224.654));
    auto itOpacityPoint = opacityPoints.begin();
    while (itOpacityPoint != opacityPoints.end()){
        opacity.AddPoint(itOpacityPoint->x, itOpacityPoint->y);
        itOpacityPoint ++;
    }
    // Window
    double aWindow[2] = { 2621.346, 1497.685 };
    // SilhouetteEnhancement
    SilhouetteEnhancement silEnhancement;
    silEnhancement.uiOpacity = 0;
    silEnhancement.uiShading = 0;
    // GMOpacity
    GMOpacity gmOpacity;
    gmOpacity.fValue1 = 0;
    gmOpacity.fValue1Opacity = 0;
    gmOpacity.fValue2 = 0; 
    gmOpacity.fValue2Opacity = 1;
    // BackGroundColorValue
    ColorValue backGroundColorValue(0, 0, 0, 1);
    // LightListActive
    bool bLightListActive = true;   

    //const std::string sColorTableFile = "D:/RT_TMS/UIH/appdata/viewer3d/config/LUT/CT/CHEST/CT_RIB_CAGE.xml";
    ////const std::string sColorTableFile = cfgPath + "/LUT/CT/HEAD_NECK/CT_CAROTIDS_4.xml";
    ////const std::string sColorTableFile = cfgPath + "/LUT/MR/General/MR_AORTA.xml";

    //Mcsf::MedViewer3D::PiecewiseFunction opacity;
    //Mcsf::MedViewer3D::PiecewiseFunction GMOpacityFactor;
    //Mcsf::MedViewer3D::ColorTransferFunction color; 
    //opacity.Initialize(); 
    //color.Initialize();
    //Mcsf::MedViewer3D::SilhouetteEnhancement mySilEnhancement;
    //Mcsf::MedViewer3D::GMOpacity myGMOpacity;
    //Mcsf::MedViewer3D::ColorValue myBackGroundColorValue(0, 0, 0, 0);
    //bool bLightListActive = true;

    //double aWindow[2] = { 0, 0 };
    //Mcsf::MedViewer3D::ImportLUT importer; 
    //importer.Initialize(); 
    //importer.ImportLUTFile(sColorTableFile); 
    //importer.GetColorPoint(color); 
    //importer.GetOpacityPoint(opacity); 
    //importer.GetWindowing(aWindow[0],aWindow[1]);
    //importer.GetSilhouetteEnhancement(mySilEnhancement);
    //importer.GetGMOpacity(myGMOpacity);
    //importer.GetBackGround(myBackGroundColorValue);
    //bLightListActive = importer.GetLightListAcitve();      

    int iMaskLabel = 1;
    colorTable->ClearColorTable();
    colorTable->AddColorTable(colorTF, opacity, iMaskLabel, 512, aWindow[0], aWindow[1]); 
    colorTable->SetBackGroundColor(backGroundColorValue);
    colorTable->SetSilhouetteEnhancement(silEnhancement);
    colorTable->SetGMOpacity(gmOpacity);
    colorTable->SetLightListActive(bLightListActive);  

    return true;    
}
TPS_END_NAMESPACE  // end namespace tps
