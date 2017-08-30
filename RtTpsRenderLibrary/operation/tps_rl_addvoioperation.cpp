////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_addvoioperation.cpp
/// 
///  \brief class TpsAddVOIOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_addvoioperation.h"

#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsAddVOIOperation::TpsAddVOIOperation(
    const std::string& imageUID, const std::string& voiUID) : 
    mImageUID(imageUID), mVOIUID(voiUID){

}

TpsAddVOIOperation::~TpsAddVOIOperation(){

}

bool TpsAddVOIOperation::ModifyGraphicObject() const{

    if (mImageUID.empty() || mVOIUID.empty()){
        TPS_LOG_DEV_ERROR<<"The Image UID or VOI UID is empty.";
        return false;
    }

    auto go = mModelWarehouse->GetModelObject(
        mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOI_COLLECTION));
    auto voiCollection = dynamic_pointer_cast<TpsVOIGOCollection>(go);
    if (go == nullptr || voiCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get VOI collection of image: "<<mImageUID;
        return false;
    }

    auto converter = mConverterFactory->GetConverter(GO_TYPE_VOI);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get VOI graphic object converter.";
        return false;
    }

    GOConverterParameter para;
    para.obj_uid = mVOIUID;
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> voiGO = converter->Convert(para);
    if (voiGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed convert VOI graphic object.";
        return false;
    }

    voiCollection->AddVOI(mVOIUID, dynamic_pointer_cast<TpsVOIGraphicObject>(voiGO));
    voiGO->SetDirty(true);
    voiCollection->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
