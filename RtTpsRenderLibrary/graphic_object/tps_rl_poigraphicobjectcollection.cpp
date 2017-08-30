#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"


TPS_BEGIN_NAMESPACE

TpsPoiGraphicObjectCollection::TpsPoiGraphicObjectCollection() {

}

TpsPoiGraphicObjectCollection::~TpsPoiGraphicObjectCollection() {

}

void TpsPoiGraphicObjectCollection::AddPoi(const std::string &uid,
      std::shared_ptr<TpsPoiGraphicObject> poiGo) {
    mPoiMap[uid] = poiGo;
}

bool TpsPoiGraphicObjectCollection::DeletePoi(const std::string &str) {
    std::map<std::string,std::shared_ptr<TpsPoiGraphicObject>>::iterator itr =
        mPoiMap.find(str);
    if(itr == mPoiMap.end()) {
        TPS_LOG_DEV_INFO<<"Poi do not exist!";
        return false;
    }
    mPoiMap.erase(itr);
    return true;
}

void TpsPoiGraphicObjectCollection::RemoveAll() {
    mPoiMap.clear();
}

std::shared_ptr<TpsPoiGraphicObject> TpsPoiGraphicObjectCollection::SelectPoi(
    const std::string &str) {
    std::map<std::string,std::shared_ptr<TpsPoiGraphicObject>>::iterator itr =
        mPoiMap.find(str);
    if(itr != mPoiMap.end()) {
        std::shared_ptr<TpsPoiGraphicObject> poiObj = itr->second;
        return poiObj;
    }
    TPS_LOG_DEV_INFO<<"Poi do not exist!";
    return nullptr;
}

std::map<std::string, std::shared_ptr<TpsPoiGraphicObject>> TpsPoiGraphicObjectCollection::GetPoiMap() const {
    return mPoiMap;
}

TPS_END_NAMESPACE
