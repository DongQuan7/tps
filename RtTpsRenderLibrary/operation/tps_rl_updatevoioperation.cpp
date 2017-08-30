////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatevoioperation.cpp
/// 
///  \brief TpsUpdateVoiOperation  
/// 
///  \version 1.0
/// 
///  \date    2014/6/11
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatevoioperation.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"

//for test
//#include "RtTpsRenderLibrary/tps_rl_surfacevolumerenderinggraphicobject.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsUpdateVoiOperation::TpsUpdateVoiOperation(
    const std::string& seriesUid, const std::string& voiUid)
    : mSeriesUid(seriesUid), mVoiUid(voiUid){
}

TpsUpdateVoiOperation::~TpsUpdateVoiOperation(){

}

bool TpsUpdateVoiOperation::ModifyGraphicObject() const{

    auto voiCollectionGO = mModelWarehouse->GetModelObject(
        mSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOI_COLLECTION));
    auto voiCollection = dynamic_pointer_cast<TpsVOIGOCollection>(voiCollectionGO);
    if(nullptr == voiCollectionGO ) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi collection!";
        return false;
    }

    auto voi = voiCollection->GetVOI(mVoiUid);

    auto convert = mConverterFactory->GetConverter(GO_TYPE_VOI);
    if(nullptr == convert) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi converter!";
        return false;
    }

    GOConverterParameter para;
    para.obj_uid = mVoiUid;
    if(!convert->Update(para,voi)) {
        TPS_LOG_DEV_ERROR<<"update voi go failed !";
        return false;
    }
    
    voiCollection->UpdateVOI(mVoiUid);
    voiCollection->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE