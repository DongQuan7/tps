////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_change_all_poi_visibility_operation.cpp
/// 
///  \brief class ChangeAllPoiVisibility implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/03/30
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_change_all_poi_visibility_operation.h"

#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

ChangeAllPoiVisibilityOperation::ChangeAllPoiVisibilityOperation(
const std::string& seriesUid, bool visible) : 
mSeriesUid(seriesUid), mVisbile(visible){

}

ChangeAllPoiVisibilityOperation::~ChangeAllPoiVisibilityOperation(){

}

bool ChangeAllPoiVisibilityOperation::ModifyGraphicObject() const{
    std::string key = mSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_POI_COLLECTION);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto poiCollection = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(go);
    if (go == nullptr){
        TPS_LOG_DEV_ERROR<<"The poi collection GO does not exist.";
        return false;
    }

    auto poiMap = poiCollection->GetPoiMap();
    for (auto it = poiMap.begin(); it != poiMap.end(); ++it){
        it->second->SetVisible(mVisbile);
    }
    poiCollection->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps