////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_drr_graphic_object_converter.cpp
/// 
///  \brief class DRRGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/13
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_drr_graphic_object_converter.h"

//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

DRRGOConverter::DRRGOConverter(const GO_TYPE goType)
    : TpsGOConverterBase(goType){

}

DRRGOConverter::~DRRGOConverter(){

}

GraphicObjectPtr DRRGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<DrrGraphicObejct>(new DrrGraphicObejct());
}

bool DRRGOConverter::PrepareSourceData_i(const GOConverterParameter& /*para*/){
    //mVolume = nullptr;

    //std::string volumeKey = uid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    //auto go = mModelWarehouse->GetModelObject(volumeKey);
    //mVolume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
    //if (mVolume == nullptr){
        //TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][PrepareSourceData_i]: "
            //<<"The volume data with UID: "<<uid<<" does not exist.";
        //return false;
    //}
    return true;
}

bool DRRGOConverter::Update_i(GraphicObjectPtr go){
    
    auto drrGo = std::dynamic_pointer_cast<DrrGraphicObejct>(go);
    //if (mVolume == nullptr || drrGo == nullptr) return false;

//     double dWindowWidth, dWindowCenter;
//     mVolume->GetDefaultWindowing(&dWindowWidth, &dWindowCenter);
//     drrGo->SetWindowing(dWindowWidth, dWindowCenter);

    drrGo->SetWindowing(256.0, 127.0);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
